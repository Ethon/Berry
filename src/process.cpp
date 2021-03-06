/**
 * @file process.cpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Public platform-independent process API.
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

#include <berry/process.hpp>

berry::process const berry::not_a_process;

bool berry::operator<(berry::process_param_type lhs,
    berry::process_param_type rhs)
{
    return lhs.pid() < rhs.pid();
}
    
bool berry::operator==(berry::process_param_type lhs,
    berry::process_param_type rhs)
{
    return lhs.pid() == rhs.pid();
}

bool berry::operator!=(berry::process_param_type lhs,
    berry::process_param_type rhs)
{
    return lhs.pid() != rhs.pid();
}