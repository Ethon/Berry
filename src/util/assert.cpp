/**
 * @file util/assert.cpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2012
 * @version 1.0a
 * @brief Implements the assertion handler.
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
#include <sstream>
#include <stdexcept>

// Boost:
#include <boost/assert.hpp>

void boost::assertion_failed(char const* expr, char const* function,
    char const* file, long line)
{
    std::ostringstream msg;
    msg << "In " << function << " (" << file << ':' << line << ") :"
        << "Expression " << expr << " failed";
    throw std::logic_error(msg.str());
}