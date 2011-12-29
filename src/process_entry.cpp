/**
 * @file process_entry.cpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Process entry implementation. 
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
#include <algorithm>

// Boost Library:
#include <boost/optional.hpp>
#include <boost/algorithm/string/compare.hpp>

// Berry:
#include <berry/process.hpp>
#include <berry/process_entry.hpp>

boost::optional<berry::process_entry> berry::get_entry_by_pid(
    berry::detail::process::pid_type pid)
{
    // Iterate all processes.
    berry::process_snapshot snapshot = berry::create_process_snapshot();
    boost::optional<berry::process_entry> entry =
        berry::extract_first_process(snapshot);
    do
    {
        // Check if we have a match.
        if(entry->pid == pid)
            return entry;
    }
    while( (entry = berry::extract_next_process(snapshot)) );
   
    // We have no match.
    return boost::optional<berry::process_entry>();
}
   
boost::optional<berry::process_entry> berry::get_entry_by_name(
    std::string const& name, bool case_sensitive)
{
    std::string::const_iterator name_begin = name.begin();
    std::string::const_iterator name_end = name.end();
    
    // Adjust name length if we're on a system which limits this.
    if(name.length() >= berry::detail::process::max_comm_len)
        name_end = name_begin + berry::detail::process::max_comm_len;
    
    // Iterate all processes.
    berry::process_snapshot snapshot = berry::create_process_snapshot();
    boost::optional<berry::process_entry> entry =
        berry::extract_first_process(snapshot);
    do
    {
        // Check if we have a match.
        if(case_sensitive)
        {
            if(std::equal(name_begin, name_end, entry->name.begin(),
                boost::algorithm::is_equal()))
            {
                return entry;
            }
        }
        else
        {
            if(std::equal(name_begin, name_end, entry->name.begin(),
                boost::algorithm::is_iequal()))
            {
                return entry;
            }
        }
    }
    while( (entry = berry::extract_next_process(snapshot)) );
   
    // We have no match.
    return boost::optional<berry::process_entry>();
}