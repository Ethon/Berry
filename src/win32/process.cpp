/**
 * @file win32/process.cpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Public process API for Win32. 
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
#ifndef BERRY_WINDOWS
#   error "Attempt to compile source file on a wrong system"
#endif

// System:
#include <windows.h>

// C++ Standard Library:
#include <string>
#include <stdexcept>
#include <array>
#include <system_error>
#include <memory>

// Berry:
#include <berry/process.hpp>
#include <berry/detail/process_detail.hpp>
#include <berry/process_entry.hpp>


/******** Free helper functions ********/
static ::HANDLE copy_handle(::HANDLE in)
{
    ::HANDLE out;
    ::HANDLE const self = ::GetCurrentProcess();
    if(in == self)
        return in;
    
    ::BOOL const result = ::DuplicateHandle(self, in, self, &out, 0, FALSE,
                                                DUPLICATE_SAME_ACCESS);
    if(!result)
    {
        std::error_code error(::GetLastError(), std::system_category());
        throw std::system_error(error,
            "::copy_handle : ::DuplicateHandle failed");
    }
    
    return out;
}

static ::HANDLE open_handle(berry::pid_type pid)
{
    if(pid == berry::get_current_process().pid())
        return ::GetCurrentProcess();
    
    ::DWORD const access =  SYNCHRONIZE |
                            PROCESS_DUP_HANDLE |
                            PROCESS_QUERY_LIMITED_INFORMATION;
                            
    ::HANDLE const result = ::OpenProcess(access, FALSE, pid);
    if(!result)
    {
        std::error_code error(::GetLastError(), std::system_category());
        throw std::system_error(error,
            "::open_handle : ::OpenProcess failed");
    }
    
    return result;
}

static void close_handle(::HANDLE& in)
{
    if(in && in != ::GetCurrentProcess())
    {
        if(!::CloseHandle(in))
        {
            std::error_code error(::GetLastError(), std::system_category());
            throw std::system_error(error,
                "::close_handle : ::CloseHandle failed"); 
        }
        
        in = 0;
    }
    
}

/******** Constructors and Destructor ********/
berry::process::process()
    : m_data()
{ }

berry::process::process(pid_type pid)
    : m_data(pid, ::open_handle(pid))
{ }

berry::process::process(std::string const& name, bool case_sensitive)
    : m_data()
{
   boost::optional<process_entry> entry(
       get_entry_by_name(name, case_sensitive));
   if(!entry)
       throw std::runtime_error("berry::process::process : process not found");
   
   m_data.pid = entry->pid;
   m_data.handle = ::open_handle(entry->pid);
}
        
berry::process::process(process const& other)
    : m_data(other.m_data.pid, ::copy_handle(other.m_data.handle))
{ }
        
berry::process::process(process&& other)
    : m_data(other.m_data.pid, other.m_data.handle)
{
    other.m_data.handle = 0;
}

berry::process::~process()
{
    ::close_handle(m_data.handle);
}

/******** Member functions ********/
berry::pid_type berry::process::pid() const
{
    return m_data.pid;
}

std::string berry::process::name() const
{
    return executable_path().filename().string();
}
        
boost::filesystem::path berry::process::executable_path() const
{
    std::array<wchar_t, MAX_PATH> buffer;
    ::DWORD len = buffer.size();
    
    ::BOOL const result = ::QueryFullProcessImageNameW(m_data.handle, 0,
        buffer.data(), &len);
    if(!result)
    {
        std::error_code error(::GetLastError(), std::system_category());
        throw std::system_error(error,
            "berry::process::executable_path : "
            "::QueryFullProcessImageNameW failed");
    }
   
    return boost::filesystem::path(buffer.begin(), buffer.begin() + len);
}
        
int berry::process::bitness() const
{
    // First check if we are on a 64bit Windows, else its always 32bit.
    if(!::GetSystemWow64DirectoryW(nullptr, 0))
    {
        if(GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
            return 32;
    }
   
    // Check the Wow64 state.
    ::BOOL is_wow64;
    if(!::IsWow64Process(m_data.handle, &is_wow64))
    {
        std::error_code error(::GetLastError(), std::system_category());
        throw std::system_error(error,
            "berry::process::bitness : ::IsWow64Process failed");
    }
   
    // If the process is running under Wow64, it's 32bit for sure.
    return is_wow64 ? 32 : 64;
}

void berry::process::terminate(bool)
{
    ::HANDLE terminate = ::OpenProcess(PROCESS_TERMINATE, FALSE, 0);
    if(!terminate)
    {
        std::error_code error(::GetLastError(), std::system_category());
        throw std::system_error(error,
            "berry::process::terminate : ::OpenProcess failed");
    }
    
    if(!::TerminateProcess(terminate, EXIT_FAILURE))
    {
        ::close_handle(terminate);
        std::error_code error(::GetLastError(), std::system_category());
        throw std::system_error(error,
            "berry::process::terminate : ::TerminateProcess failed");
    }
    
    ::close_handle(terminate);
}
        
bool berry::process::still_exists() const
{
    ::DWORD exit_code;
    if(!::GetExitCodeProcess(m_data.handle, &exit_code))
    {
        std::error_code error(::GetLastError(), std::system_category());
        throw std::system_error(error,
            "berry::process::still_exists : ::GetExitCodeProcess failed");
    }
    
    return exit_code == STILL_ACTIVE;
}

/******** Member operator overloads ********/
berry::process& berry::process::operator=(berry::process const& other)
{
    ::close_handle(m_data.handle);
    
    m_data.pid = other.m_data.pid;
    m_data.handle = ::copy_handle(other.m_data.handle);
    return *this;
}
        
berry::process& berry::process::operator=(berry::process&& other)
{
    ::close_handle(m_data.handle);
    
    m_data.pid = other.m_data.pid;
    m_data.handle = other.m_data.handle;
    other.m_data.handle = 0;
    return *this;
}

/******** Free functions ********/
berry::process const& berry::get_current_process()
{
    static berry::process const current_process(GetCurrentProcessId());
    return current_process;
}