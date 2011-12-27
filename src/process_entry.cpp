/**
 * @file process_entry.cpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Process entry implementation. 
 * 
 * This file is part of Berry.
 * 
 * Berry is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Berry is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Berry. If not, see <http://www.gnu.org/licenses/>.
 */

// C++ Standard Library:
#include <limits>

// Boost Library:
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

// System:
#include <berry/detail/system.hpp>
#ifdef BERRY_LINUX
#  include <glob.h>
#elif defined BERRY_WINDOWS
#  include <Windows.h>
#  include <Tlhelp32.h>
#endif

// Berry:
#include <berry/process.hpp>
#include <berry/process_entry.hpp>
#include <berry/error.hpp>

berry::process_entry::process_entry()
  : pid(0), parent_pid(0), name()
{ }

#ifdef BERRY_LINUX
struct snapshot
{
   snapshot()
      : data_ptr(0), data()
   {
      int result =
         glob("/proc/[0-9]*", GLOB_ONLYDIR, NULL, &data);
      if(result == GLOB_NOSPACE)
         throw berry::unexpected_error("Out of memory");
      if(result == GLOB_ABORTED || result == GLOB_NOMATCH)
         throw berry::unexpected_error("procfs not correctly mounted");
   }
   
   ~snapshot()
   {
      globfree(&data);
   }
   
   std::size_t data_ptr;
   glob_t data;
};
   
static void destroy_snapshot(void* snap)
{
   delete static_cast<snapshot*>(snap);
}

static berry::process_entry make_entry_from_path(boost::filesystem::path& dir)
{
   dir /= "stat";
   boost::filesystem::ifstream stat(dir);
   
   berry::process_entry result;
   (stat >> result.pid).ignore(
      std::numeric_limits<std::streamsize>::max(), '(');
   std::getline(stat, result.name, ')');
   char dummy; stat >> dummy >> result.parent_pid;
   
   return result;
}
#endif


berry::process_snapshot_type berry::create_process_snapshot()
{
#ifdef BERRY_LINUX
   return berry::process_snapshot_type(new snapshot(), &destroy_snapshot);
   
#elif defined BERRY_WINDOWS
   HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
   if(snap == INVALID_HANDLE_VALUE)
   {
      int error = GetLastError();
      throw berry::system_error("CreateToolhelp32Snapshot() failed", error);
   }
   return berry::process_snapshot_type(snap, &CloseHandle);
#endif
}
   
berry::process_entry
   berry::extract_first_process(berry::process_snapshot_type& snap)
{
#ifdef BERRY_LINUX
   snapshot* ss = static_cast<snapshot*>(snap.get());
   boost::filesystem::path dir(ss->data.gl_pathv[ss->data_ptr++]);
   return make_entry_from_path(dir);
   
#elif defined BERRY_WINDOWS
   PROCESSENTRY32A entry;
   entry.dwSize = sizeof(entry);
   if(!Process32FirstA(snap, &entry))
   {
      int error = GetLastError();
      throw berry::system_error("Process32FirstA() failed", error);
   }
   return { entry.th32ProcessID, entry.th32ParentProcessID, entry.szExeFile };
#endif
}
   
boost::optional<berry::process_entry> berry::extract_next_process(
   berry::process_snapshot_type& snap)
{
#ifdef BERRY_LINUX
   snapshot* ss = static_cast<snapshot*>(snap.get());
   if(ss->data_ptr < ss->data.gl_pathc)
   {
      boost::filesystem::path dir(ss->data.gl_pathv[ss->data_ptr++]);
      return boost::optional<berry::process_entry>(make_entry_from_path(dir));
   }
   
#elif defined BERRY_WINDOWS
   PROCESSENTRY32A entry;
   entry.dwSize = sizeof(entry);
   if(Process32NextA(snap, &entry))
   {
      return boost::optional<berry::process_entry>(
         { entry.th32ProcessID, entry.th32ParentProcessID, entry.szExeFile });
   }
#endif
   
   return boost::optional<berry::process_entry>();
}

boost::optional<berry::process_entry> berry::get_entry_by_pid(
   berry::detail::process::pid_type pid)
{
   // Iterate all processes.
   auto snapshot = berry::create_process_snapshot();
   boost::optional<berry::process_entry> entry =
      berry::extract_first_process(snapshot);
   do
   {
      // Check if we have a match.
      if(entry->pid == pid)
         return entry;
   }
   while( (entry = berry::extract_next_process(snapshot)) );
   
   // We have no match.
   return boost::optional<berry::process_entry>();
}
   
boost::optional<berry::process_entry> berry::get_entry_by_name(
   std::string name, bool case_sensitive)
{
#ifdef BERRY_LINUX
   // Adjust name length if we're on a system which limits this.
   if(name.length() >= berry::detail::process::max_comm_len)
      name.resize(berry::detail::process::max_comm_len - 1);
#endif
      
   // Convert to lower case if we don't compare case sensitive.
   if(!case_sensitive)
      std::transform(name.begin(), name.end(), name.begin(), &tolower);
   
   // Iterate all processes.
   auto snapshot = berry::create_process_snapshot();
   boost::optional<berry::process_entry> entry =
      berry::extract_first_process(snapshot);
   do
   {
      // Convert to lower case if we don't compare case sensitive.
      if(!case_sensitive)
         std::transform(entry->name.begin(), entry->name.end(),
            entry->name.begin(), &tolower);

      // Check if we have a match.
      if(name == entry->name)
         return entry;
   }
   while( (entry = berry::extract_next_process(snapshot)) );
   
   // We have no match.
   return boost::optional<berry::process_entry>();
}