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
#endif

// Berry:
#include <berry/process.hpp>
#include <berry/process_entry.hpp>
#include <berry/error.hpp>

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
   std::getline(stat, result.command, ')');
   char dummy; stat >> dummy >> result.parent_pid;
   
   return result;
}
#endif


berry::process_snapshot_type berry::create_process_snapshot()
{
#ifdef BERRY_LINUX
   return berry::process_snapshot_type(new snapshot(), &destroy_snapshot);
#endif
}
   
berry::process_entry
   berry::extract_first_process(berry::process_snapshot_type& snap)
{
   snapshot* ss = static_cast<snapshot*>(snap.get());
   boost::filesystem::path dir(ss->data.gl_pathv[ss->data_ptr++]);
   return make_entry_from_path(dir);
}
   
boost::optional<berry::process_entry> berry::extract_next_process(
   berry::process_snapshot_type& snap)
{
   snapshot* ss = static_cast<snapshot*>(snap.get());
   if(ss->data_ptr < ss->data.gl_pathc)
   {
      boost::filesystem::path dir(ss->data.gl_pathv[ss->data_ptr++]);
      return boost::optional<berry::process_entry>(make_entry_from_path(dir));
   }
   
   return boost::optional<berry::process_entry>();
}