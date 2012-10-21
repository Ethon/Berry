/**
 * @file memory_protection.cpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011, 2012
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
#include <cassert>

// Berry:
#include <berry/memory_protection.hpp>

/******** Free helper functions ********/
namespace
{
    enum index
    {
        index_readable = 0,
        index_writable = 1,
        index_executable = 2,
        index_shared = 3
    };
    
    bool is_readable(const char* str)
    {
        assert(str);
        assert(str[index_readable] == 'r' || str[index_readable] == '-');
        return str[index_readable] == 'r';
    }
    
    bool is_writable(const char* str)
    {
        assert(str);
        assert(str[index_writable] == 'w' || str[index_writable] == '-');
        return str[index_writable] == 'w';
    }
    
    bool is_executable(const char* str)
    {
        assert(str);
        assert(str[index_executable] == 'x' || str[index_executable] == '-');
        return str[index_executable] == 'x';
    }
    
    bool is_shared(const char* str)
    {
        assert(str);
        assert(str[index_shared] == 's' || str[index_shared] == 'p');
        return str[index_shared] == 's';
    }
}

/******** Constructors ********/

berry::memory_protection::memory_protection()
    : m_readable(false), m_writable(false), m_executable(false), m_shared(false)
{ }
        
berry::memory_protection::memory_protection(char const* string)
    :   m_readable(::is_readable(string)),
        m_writable(::is_writable(string)),
        m_executable(::is_executable(string)),
        m_shared(::is_shared(string))
{ }
        
berry::memory_protection::memory_protection(std::string const& string)
    :   m_readable(::is_readable(string.c_str())),
        m_writable(::is_writable(string.c_str())),
        m_executable(::is_executable(string.c_str())),
        m_shared(::is_shared(string.c_str()))
{
    assert(string.size() > ::index_shared);
}
        
berry::memory_protection::memory_protection(bool readable, bool writable,
    bool executable, bool shared)
    :   m_readable(readable), m_writable(writable),
        m_executable(executable), m_shared(shared)
{ }
            
bool berry::memory_protection::readable() const
{
    return m_readable;
}
        
bool berry::memory_protection::writable() const
{
    return m_writable;
}

bool berry::memory_protection::executable() const
{
    return m_executable;
}
        
bool berry::memory_protection::shared() const
{
    return m_shared;
}
        
bool berry::memory_protection::is_private() const
{
    return !shared();
}
