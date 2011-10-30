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
#include <type_traits>

// Boost Library:
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

// System:
#ifdef BERRY_LINUX
#  include <unistd.h>
#  include <elf.h>
#endif

// Berry:
#include <berry/process.hpp>
#include <berry/process_entry.hpp>
#include <berry/detail/system.hpp>
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
static boost::filesystem::path extract_link(boost::filesystem::path link)
{
   size_t const buffer_size = 1024;
   std::array<char, buffer_size> buffer;
   ssize_t result = readlink(link.c_str(), buffer.data(), buffer.size() - 1);
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
  : m_pid(0), m_name()
{ }

berry::process::process(berry::process::pid_type pid)
  : m_pid(pid), m_name()
{ }

berry::process::process(berry::process_entry const& entry)
  : m_pid(entry.pid), m_name(&entry.command[0])
{ }

berry::process::pid_type berry::process::get_pid() const
{
  return m_pid;
}

std::string const& berry::process::get_name() const
{
  if(m_name.empty()) m_name = get_process_name(m_pid);
  return m_name;
}

bool berry::still_exists(berry::process const& proc)
{
   if(proc.get_pid() == berry::get_current_process().get_pid())
      return true;
   
#ifdef BERRY_LINUX
   return boost::filesystem::exists(berry::unix_like::get_procfs_dir(proc));
#endif
}

boost::filesystem::path
   berry::unix_like::get_procfs_dir(berry::process const& proc)
{
   if(!berry::detail::process::impl::has_procfs_mounted())
      return boost::filesystem::path();
   
   return boost::filesystem::path(
      berry::detail::process::impl::get_procfs_root() /
      boost::lexical_cast<std::string>(proc.get_pid()));
}

boost::filesystem::path berry::get_working_dir(berry::process const& proc)
{
#ifdef BERRY_LINUX
   return extract_link(berry::unix_like::get_procfs_dir(proc) / "cwd");
#endif
}
    
boost::filesystem::path berry::get_executable_path(berry::process const& proc)
{
#ifdef BERRY_LINUX
   return extract_link(berry::unix_like::get_procfs_dir(proc) / "exe");
#endif
}

int berry::get_bitness(berry::process const& proc)
{
#ifdef BERRY_LINUX
   try
   {
      // Open the process' executable and read the elf ident.
      boost::filesystem::ifstream exe(get_executable_path(proc));
      if(!exe)
         throw 
      
      std::array<char, EI_NIDENT> ident;
      exe.read(&ident[0], ident.size());
   }
   catch(berry::system_error const& e)
   {
      
   }
   
   // Assert if it's a valid elf file.
   static std::array<char, 4> const elf_magic = { 0x7F, 'E', 'L', 'F'};
   if(!std::equal(elf_magic.begin(), elf_magic.end(), ident.begin()))
      return 0;
   
   // Return the bitness of the elf class field.
   switch(ident[EI_CLASS])
   {
   case ELFCLASS32:
      return 32;
      
   case ELFCLASS64:
      return 64;
   
   default:
      return 0;
   }
#endif
}
    
  /**
  * @brief Returns the currently active process calling this function.
  *
  * @return :process& The current process.
  **/
  process const& get_current_process();
    
  /**
  * @brief Creates a new process which runs as the current process' child.
  *
  * @param arguments The arguments to the process where the command to run
  * is passed as the first argument.
  * @return :process The created child process.
  **/
  berry::process create_process(std::vector<std::string> const& arguments);
  
  void kill_process(process const& proc);