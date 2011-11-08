/**
 * @file process.cpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Process implementation.
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
#include <string>
#include <array>
#include <algorithm>
#include <vector>
#include <cassert>

// Boost Library:
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

// System:
#include <berry/detail/system.hpp>
#ifdef BERRY_LINUX
#  include <unistd.h>
#  include <elf.h>
#  include <sys/types.h>
#  include <signal.h>
#elif defined BERRY_WINDOWS
#  include <Windows.h>
#endif

// Berry:
#include <berry/process.hpp>
#include <berry/process_entry.hpp>
#include <berry/error.hpp>

// Returns the procfs root directory.
static boost::filesystem::path const& get_procfs_root()
{
#ifdef BERRY_LINUX
   // We simply assume that it is default mounted.
   static boost::filesystem::path root("/proc");
#else
   // Else we assume there is no procfs.
   static boost::filesystem::path root();
#endif
   
   return root;
}

// Determines if the there is a mounted procfs.
static bool has_procfs_mounted()
{
   if(   get_procfs_root().empty() ||
         !boost::filesystem::exists(get_procfs_root()))
      return false;
   
   return true;
}

#ifdef BERRY_LINUX
// Performs readlink.
static boost::filesystem::path extract_link(
   boost::filesystem::path const& link)
{
   size_t const buffer_size = 1024;
   std::array<char, buffer_size> buffer;
   ssize_t result = readlink(link.c_str(), buffer.data(), buffer.size());
   if(result == -1)
   {
      int error_code = errno;
      throw berry::system_error("readlink() failed", error_code);
   }
   
   return boost::filesystem::path(
      std::string(buffer.begin(), buffer.begin() + result));
}
#endif

berry::process::process()
  : m_pid(0)
{ }

berry::process::process(berry::process::pid_type pid)
  : m_pid(pid)
{ }

berry::process::pid_type berry::process::get_pid() const
{
  return m_pid;
}

std::string berry::get_name(berry::process proc)
{
   std::string result;
   
#ifdef BERRY_LINUX
   boost::filesystem::ifstream comm(
      berry::unix_like::get_procfs_dir(proc) / "comm");
   std::getline(comm, result);

#elif defined BERRY_WINDOWS
   return berry::get_executable_path(proc).filename().string();
   
#endif

   return result;
}

bool berry::still_exists(berry::process proc)
{
   if(proc.get_pid() == berry::get_current_process().get_pid())
      return true;
   
#ifdef BERRY_LINUX
   return boost::filesystem::exists(berry::unix_like::get_procfs_dir(proc));
   
#elif BERRY_WINDOWS
   // Open a handle to the process.
   HANDLE target = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
      false, proc.get_pid());
   if(!target)
      return false;
   
   CloseHandle(target);
   return true;
#endif
}

boost::filesystem::path
   berry::unix_like::get_procfs_dir(berry::process proc)
{
   static bool const mounted = has_procfs_mounted();
   if(!mounted) return boost::filesystem::path();
   
   return boost::filesystem::path(get_procfs_root() /
      boost::lexical_cast<std::string>(proc.get_pid()));
}

boost::filesystem::path berry::get_working_dir(berry::process proc)
{
#ifdef BERRY_LINUX
   return extract_link(berry::unix_like::get_procfs_dir(proc) / "cwd");
#endif
}
    
boost::filesystem::path berry::get_executable_path(berry::process proc)
{
#ifdef BERRY_LINUX
   return extract_link(berry::unix_like::get_procfs_dir(proc) / "exe");
   
#elif BERRY_WINDOWS
   // Open a handle to the process.
   HANDLE target = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
      false, proc.get_pid());
   if(!target)
   {
      int error_code = GetLastError();
      throw berry::system_error("OpenProcess() failed", error_code);
   }
   
   // Query it's process image path.
   std::array<char, 512> buffer;
   if(!QueryFullProcessImageNameA(target, 0, &buffer[0], buffer.size()))
   {
      int error_code = GetLastError();
      CloseHandle(target);
      throw berry::system_error("QueryFullProcessImageNameA() failed",
         error_code);
   }
   
   CloseHandle(target);
   return boost::filesystem::path(&buffer[0]);
#endif
}

int berry::get_bitness(berry::process proc)
{
#ifdef BERRY_LINUX
   // Open the process' executable and read the elf ident.
   boost::filesystem::ifstream exe(get_executable_path(proc));
   if(!exe)
      throw berry::error("Could not get bitness of exe file: Not readable");
   std::array<char, EI_NIDENT> ident;
   exe.read(&ident[0], ident.size());
   
   // Assert if it's a valid elf file.
   static std::array<char, 4> const elf_magic = { 0x7F, 'E', 'L', 'F'};
   if(!std::equal(elf_magic.begin(), elf_magic.end(), ident.begin()))
      throw berry::error("Could not get bitness of exe file: No valid ELF");
   
   // Return the bitness of the elf class field.
   switch(ident[EI_CLASS])
   {
   case ELFCLASS32:
      return 32;
      
   case ELFCLASS64:
      return 64;
   
   default:
      throw berry::error("Could not get bitness of exe file: No valid class");
   }
   
#elif defined BERRY_WINDOWS
   // First check if we are on a 64bit Windows, else its always 32bit.
   if(!GetSystemWow64Directory(nullptr, 0))
   {
      if(GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
         return 32;
   }
   
   // Open a handle to the process.
   HANDLE target = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
      false, proc.get_pid());
   if(!target)
   {
      int error_code = GetLastError();
      throw berry::system_error("OpenProcess() failed", error_code);
   }
   
   // Check the Wow64 state.
   BOOL is_wow64;
   if(!IsWow64Process(target, &is_wow64);
   {
      int error_code = GetLastError();
      CloseHandle(target);
      throw berry::system_error("IsWow64Process() failed", error_code);
   }
   CloseHandle(target);
   
   // If the process is running under Wow64, it's 32bit for sure.
   return is_wow64 ? 32 : 64;
#endif
}

berry::process berry::get_current_process()
{
#ifdef BERRY_LINUX
   static berry::process const current_process(getpid());
   
#elif defined BERRY_WINDOWS
   static berry::process const current_process(GetCurrentProcessId());
#endif
   
   return current_process;
}

berry::process berry::simple_create_process(
   std::vector<std::string> const& arguments)
{
   assert(arguments.size() > 0);
   
#ifdef BERRY_LINUX
   // Fork.
   berry::process::pid_type result = fork();
   if(result == -1)
   {
      int error_code = errno;
      throw berry::system_error("fork() failed", error_code);
   }
   else if(result == 0)
   {
      // Create a new array of raw pointers and execvp.
      std::vector<char*> args;
      args.reserve(arguments.size() + 1);
      for(std::string const& cur : arguments)
         args.push_back(const_cast<char*>(cur.c_str()));
      args.push_back(nullptr);
      execvp(arguments[0].c_str(), &args[0]);
      
      // Exit in case exec failed.
      _exit(-1);
   }
   else
   {
      return berry::process(result);
   }

#elif BERRY_WINDOWS
   // Initialize data.
    PROCESS_INFORMATION process_information;
    STARTUPINFO startup_info;
    memset(&process_information, 0, sizeof(process_information));
    memset(&startup_info, 0, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    
   // Construct the command line.
   std::vector<char> cmd_line;
   
   // First set the application's name.
   cmd_line.push_back('"');
   cmd_line.append(arguments[0].begin, arguments[0].end());
   cmd_line.push_back('"');
   
   // Append arguments.
   for(std::string const& cur : arguments)
   {
      cmd_line.push_back(' ');
      cmd_line.push_back('"');
      cmd_line.append(cur.begin, cur.end());
      cmd_line.push_back('"');
   }
   
   // Create the process with that commandline.
   BOOL result = CreateProcessA( nullptr,
                                 &cmd_line[0],
                                 nullptr,
                                 nullptr,
                                 false,
                                 0,
                                 nullptr,
                                 nullptr,
                                 &startup_info,
                                 &process_information);
   if(!result)
   {
      int error_code = GetLastError();
      throw berry::system_error("CreateProcess() failed", error_code);
   }
   
   CloseHandle(process_information.hProcess);
   CloseHandle(process_information.hThread);                            
   return berry::process(process_information.dwProcessId);
#endif
}

void berry::terminate_process(berry::process proc, int exit_code)
{
   if(proc.get_pid() == 0)
      return;
      
#ifdef BERRY_LINUX
   // Send a sigterm to the process.
   if(kill(proc.get_pid(), SIGTERM) == -1)
   {
      int error_code = errno;
      throw berry::system_error("kill() failed", error_code);
   }
   
#elif defined BERRY_WINDOWS
   // Open a handle to the process.
   HANDLE target = OpenProcess(PROCESS_TERMINATE, false, proc.get_pid());
   if(!target)
   {
      int error_code = GetLastError();
      throw berry::system_error("OpenProcess() failed", error_code);
   }
   
   // Terminate that process.
   if(!TerminateProcess(target, exit_code))
   {
      int error_code = GetLastError();
      CloseHandle(target);
      throw berry::system_error("TerminateProcess() failed", error_code);
   }
   CloseHandle(target);
#endif
}