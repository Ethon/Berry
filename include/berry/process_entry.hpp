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
#include <string>
#include <memory>

// Boost Library:
#include <boost/optional.hpp>

// Berry:
#include <berry/detail/process_detail.hpp>

namespace berry
{
   /**
    * @brief Class representing a process entry on the system.
    **/
   struct process_entry
   {
      process_entry();
      
      detail::process::pid_type pid;
      detail::process::pid_type parent_pid;
      std::string name;
   };
  
   /**
    * @brief Type used to store snapshots of processes.
    **/
   typedef std::shared_ptr<void> process_snapshot_type;
  
   /**
    * @brief Creates a snapshot of all running processes on the system.
    *
    * @return process_snapshot_type The created snapshot.
    **/
   process_snapshot_type create_process_snapshot();
   
   /**
    * @brief Extracts the first process entry from the snapshot.
    *
    * @param snap The snapshot to extract from.
    * @return :process_entry The first process.
    **/
   process_entry extract_first_process(process_snapshot_type& snap);
   
   /**
    * @brief Extracts the next process entry from the snapshot.
    *
    * @param snap The snapshot to extract from.
    * @return :optional< berry::process_entry > The next process .
    **/
   boost::optional<process_entry> extract_next_process(
      process_snapshot_type& snap);
   
   /**
    * @brief Retrieves a process entry by its identifier.
    *
    * @param pid The pid to look for.
    * @return :optional< berry::process_entry > The matching entry.
    **/
   boost::optional<process_entry> get_entry_by_pid(
      detail::process::pid_type pid);
   
   /**
    * @brief Retrieves a process entry by its name.
    * Many systems limit this to a few characters, so this function may yield
    * false results on those systems. Use get_executable_path to be sure to
    * determine the full name.
    * @param name The name to look for.
    * @param case_sensitive Decide if the search is case sensitive or not.
    * @return :optional< berry::process_entry > The matching entry.
    **/
   boost::optional<process_entry> get_entry_by_name(std::string name,
      bool case_sensitive = true);
}

#endif // __BERRY_PROCESSENTRY_HPP__