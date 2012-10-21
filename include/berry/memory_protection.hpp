/**
 * @file memory_protection.hpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2012
 * @version 1.0a
 * @brief Utility to store and access the protection of a memory range.
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

namespace berry
{
    /**
     * @brief Represents the protection settings of a page of memory.
     **/
    class memory_protection
    {
    private:
        bool m_readable;
        bool m_writable;
        bool m_executable;
        bool m_shared;
        
    public:
        /**
         * @brief Constructs a dummy object with all settings set to false. 
        **/
        memory_protection();
        
        /**
         * @brief Read protections from a Unix style string.
         * Examples:
         * "rw-s" means that the memory is readable, writable, NOT executable
         *  and shared.
         * "-wxp" means that the memory is NOT readable, writable, executable
         *  and private.
         * 
         * @param string The string to parse.
         **/
        explicit memory_protection(char const* string);
        
        /**
         * @brief Read protections from a Unix style string.
         * Examples:
         * "rw-s" means that the memory is readable, writable, NOT executable
         *  and shared.
         * "-wxp" means that the memory is NOT readable, writable, executable
         *  and private.
         * 
         * @param string The string to parse.
         **/
        explicit memory_protection(std::string const& string);
        
        /**
         * @brief Manually set all protection settings.
         * 
         * @param readable Set to true if the memory can be read from.
         * @param readable Set to true if the memory can be written to.
         * @param readable Set to true if the memory can be executed.
         * @param readable Set to true if the memory is shared.
         **/
        memory_protection(bool readable, bool writable, bool executable,
            bool shared);
            
        /**
         * @brief Returns whether the memory can be read from.
         *
         * @return bool True if readable, false otherwise.
         **/
        bool readable() const;
        
        /**
         * @brief Returns whether the memory can be written to.
         *
         * @return bool True if writable, false otherwise.
         **/
        bool writable() const;
        
        /**
         * @brief Returns whether the memory can be executed.
         *
         * @return bool True if executable, false otherwise.
         **/
        bool executable() const;
        
        /**
         * @brief Returns whether the memory is shared.
         *
         * @return bool True if shared, false otherwise.
         **/
        bool shared() const;
        
        /**
         * @brief Returns whether the memory is private.
         *
         * @return bool True if private, false otherwise.
         **/
        bool is_private() const;
    };
}
