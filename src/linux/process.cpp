/**
 * @file linux/process.cpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011, 2012
 * @version 1.0a
 * @brief Public process API for Linux. 
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

#include <berry/detail/system.hpp>
#ifndef BERRY_LINUX
#   error "Attempt to compile source file on a wrong system"
#endif

// System:
#include <elf.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

// C++ Standard Library:
#include <cassert>
#include <string>
#include <stdexcept>
#include <array>
#include <system_error>

// Boost:
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

// Berry:
#include <berry/process.hpp>
#include <berry/detail/process_detail.hpp>
#include <berry/process_entry.hpp>

#define ASSERT_PROCESS() assert(*this != berry::not_a_process)

/******** Free helper functions ********/
static boost::filesystem::path extract_link(
   boost::filesystem::path const& link)
{
    std::size_t const buffer_size = 1024;
    std::array<char, buffer_size> buffer;
    ::ssize_t result = ::readlink(link.c_str(), buffer.data(), buffer.size());
    if(result == -1)
    {
        std::error_code error(errno, std::system_category());
        throw std::system_error(error, "::extract_link : ::readlink failed");
    }
   
    return boost::filesystem::path(buffer.begin(), buffer.begin() + result);
}

/******** Constructors and Destructor ********/
berry::process::process()
    : m_data()
{ }

berry::process::process(pid_type pid)
    : m_data(pid)
{ }

berry::process::process(std::string const& name, bool case_sensitive)
    : m_data()
{
   boost::optional<process_entry> entry(
       berry::get_entry_by_name(name, case_sensitive));
   if(entry)
       m_data.pid = entry->pid;
   else
       throw std::runtime_error("berry::process::process : process not found");
}
        
berry::process::process(process const& other)
    : m_data(other.m_data.pid)
{ }
        
berry::process::process(process&& other)
    : m_data(other.m_data.pid)
{ }

berry::process::~process()
{ }

/******** Member functions ********/
berry::pid_type berry::process::pid() const
{
    return m_data.pid;
}

std::string berry::process::name() const
{
    ASSERT_PROCESS();
    
    std::string result;
    {
        boost::filesystem::ifstream comm(
            berry::unix_like::get_procfs_dir(*this) / "comm");
        
        // Provide fallback.
        if(!comm)
            return executable_path().filename().string();
        
        std::getline(comm, result);
    }
    return result;
}
        
boost::filesystem::path berry::process::executable_path() const
{
    ASSERT_PROCESS();
    
    return ::extract_link(berry::unix_like::get_procfs_dir(*this) / "exe");
}
        
int berry::process::bitness() const
{
    ASSERT_PROCESS();
    
    // Open the process' executable and read the elf ident.
    std::array<char, EI_NIDENT> ident;
    {
        boost::filesystem::ifstream exe(executable_path());
        if(!exe)
            throw std::runtime_error(
                "berry::process::bitness : exe not readable");
        exe.read(&ident[0], ident.size());
    }
   
    // Assert if it is a valid elf file.
    static std::array<char, 4> const elf_magic = { {0x7F, 'E', 'L', 'F'} };
    if(!std::equal(elf_magic.begin(), elf_magic.end(), ident.begin()))
        throw std::runtime_error(
            "berry::process::bitness : no valid elf binary");
   
    // Return the bitness of the elf class field.
    switch(ident[EI_CLASS])
    {
    case ELFCLASS32:
      return 32;
      
    case ELFCLASS64:
      return 64;
   
    default:
        throw std::runtime_error(
            "berry::process::bitness : no valid elf class");
    }
}

void berry::process::terminate(bool force)
{
    ASSERT_PROCESS();
    
    // Send a sigterm to the process.
    if(::kill(pid(), force ? SIGKILL : SIGTERM) == -1)
    {
        std::error_code error(errno, std::system_category());
        throw std::system_error(error,
            "berry::process::terminate : ::kill failed");
    }
}
        
bool berry::process::still_exists() const
{
    if(pid() == 0)
        return false;
    
    return boost::filesystem::exists(berry::unix_like::get_procfs_dir(*this));
}

/******** Member operator overloads ********/
berry::process& berry::process::operator=(berry::process const& other)
{
    m_data.pid = other.m_data.pid;
    return *this;
}
        
berry::process& berry::process::operator=(berry::process&& other)
{
    m_data.pid = other.m_data.pid;
    return *this;
}

/******** Free functions ********/
berry::process const& berry::get_current_process()
{
    static berry::process const current_process(::getpid());
    return current_process;
}

static boost::filesystem::path g_procfs_base("/proc/");

void berry::unix_like::set_procfs_base(boost::filesystem::path const& base_dir)
{
    assert(boost::filesystem::exists(base_dir));
    g_procfs_base = base_dir;
}

boost::filesystem::path berry::unix_like::get_procfs_dir(berry::process proc)
{
    return g_procfs_base / std::to_string(proc.pid());
}
