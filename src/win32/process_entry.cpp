/**
 * @file win32/process_entry.cpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Process entry implementation for Windows. 
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

// C++ Standard Library:
#include <string>
#include <system_error>

// Boost Library:
#include <boost/optional.hpp>

// System:
#include <windows.h>
#include <tlhelp32.h>

// Berry:
#include <berry/process.hpp>
#include <berry/process_entry.hpp>
#include <berry/error.hpp>

berry::process_entry::process_entry()
  : pid(0), parent_pid(0), name()
{ }

berry::process_snapshot berry::create_process_snapshot()
{
    ::HANDLE snap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(snap == INVALID_HANDLE_VALUE)
    {
        std::error_code error(::GetLastError(), std::system_category());
        throw std::system_error(error,
            "berry::create_process_snapshot : "
            "::CreateToolhelp32Snapshot failed");
    }
    
    return berry::process_snapshot(snap, &::CloseHandle);
}
   
berry::process_entry
    berry::extract_first_process(berry::process_snapshot& snap)
{
    ::PROCESSENTRY32 entry; 
    entry.dwSize = sizeof(entry);
    if(!::Process32First(snap, &entry))
    {
        std::error_code error(::GetLastError(), std::system_category());
        throw std::system_error(error,
            "berry::extract_first_process : "
            "::Process32First failed");
    }
    
    berry::process_entry result = { entry.th32ProcessID,
                                    entry.th32ParentProcessID,
                                    entry.szExeFile };
    return result;
}
   
boost::optional<berry::process_entry> berry::extract_next_process(
    berry::process_snapshot& snap)
{
    ::PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);
    if(::Process32Next(snap, &entry))
    {
        berry::process_entry result = { entry.th32ProcessID,
                                        entry.th32ParentProcessID,
                                        entry.szExeFile };
        return result;
    }
   
    return boost::optional<berry::process_entry>();
}