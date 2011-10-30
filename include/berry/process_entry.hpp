/**
 * @file process_entry.hpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Public process_entry API. 
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

#ifndef __BERRY_PROCESSENTRY_HPP__
#define __BERRY_PROCESSENTRY_HPP__ 1

// C++ Standard Library:
#include <array>

// Berry:
#include <berry/detail/process_detail.hpp>

namespace berry
{
  struct process_entry
  {
    detail::process::pid_type pid;
    detail::process::pid_type parent_pid;
    std::array<char, detail::process::max_comm_len + 1> command;
  };
}

#endif // __BERRY_PROCESSENTRY_HPP__