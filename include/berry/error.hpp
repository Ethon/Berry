/**
 * @file error.hpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Public error API. 
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

#ifndef __BERRY_ERROR_HPP__
#define __BERRY_ERROR_HPP__ 1

// C++ Standard Library:
#include <string>
#include <stdexcept>

namespace berry
{  
   /**
   * @brief Base exception class for berry.
   **/
   class error : public std::runtime_error
   {
   public:
      inline error(std::string const& msg)
         : std::runtime_error(msg)
      { }
   };
   
   /**
    * @brief Exception which will be thrown in case of system api errors.
    **/
   struct system_error : public error
   {
   public:
      typedef int error_code_type;
      
   private:
      error_code_type m_error_code;
      
   public:
      inline system_error(std::string const& msg, error_code_type error_code)
         : error(msg), m_error_code(error_code)
      { }
      
      inline error_code_type get_error_code() const
      {
         return m_error_code;
      }
   };
   
   /**
    * @brief Exception which will be thrown if a 
    **/
   struct unexpected_error : public error
   {
   public:
      inline unexpected_error(std::string const& msg)
         : error(msg)
      { }
   };
}

#endif // __BERRY_ERROR_HPP__