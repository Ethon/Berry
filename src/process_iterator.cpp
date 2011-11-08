/**
 * @file process_iterator.cpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Process Iterator implementation.
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
#include <berry/process_entry.hpp>
#include <berry/process_iterator.hpp>

/*** process_iterator implementation ***/

void berry::process_iterator::increment()
{
   assert(m_snap && m_entry);
   m_entry = berry::extract_next_process(m_snap);
}

bool berry::process_iterator::equal(berry::process_iterator const& other) const
{
   return static_cast<bool>(m_entry) == static_cast<bool>(other.m_entry);
}

berry::process_entry const& berry::process_iterator::dereference() const
{
   return *m_entry;
}
      
berry::process_iterator::process_iterator()
   : m_snap(nullptr), m_entry()
{ }    

berry::process_iterator::process_iterator(int)
   :  m_snap(berry::create_process_snapshot()),
      m_entry(berry::extract_first_process(m_snap))
{ }

/*** process_list implementation ***/

berry::process_iterator berry::process_list::begin() const
{
   return berry::process_iterator(0);
}
      
berry::process_iterator berry::process_list::end() const
{
   return berry::process_iterator();
}