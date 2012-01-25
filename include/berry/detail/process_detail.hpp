/**
 * @file process_detail.hpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Non-public API used by the public process API.
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

#ifndef __BERRY_DETAIL_PROCESSDETAIL_HPP__
#define __BERRY_DETAIL_PROCESSDETAIL_HPP__ 1

// Berry:
#include <berry/detail/system.hpp>

namespace berry
{
    namespace detail
    {
        namespace process
        {
#ifdef BERRY_LINUX
            typedef int pid_type;
            unsigned int const max_comm_len = 15;
            
            struct process_data
            {
                explicit inline process_data(pid_type pid = 0)
                    : pid(pid)
                { }
                
                pid_type pid;
            };

#           define BERRY_PROCESS_AS_PARAM process
#endif
        
#ifdef BERRY_WINDOWS
            typedef unsigned long pid_type;
            typedef void* handle_type;
            unsigned int const max_comm_len = 254;
            
            struct process_data
            {
                inline process_data(pid_type pid = 0, handle_type handle = 0)
                    : pid(pid), handle(0)
                { }
                
                pid_type pid;
                handle_type handle;
            };
#           define BERRY_PROCESS_AS_PARAM process const&
#endif
        }
    }
}

#endif // __BERRY_DETAIL_PROCESSDETAIL_HPP__