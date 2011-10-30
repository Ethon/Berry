/**
 * @file process.hpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Public process API. 
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

#ifndef __BERRY_PROCESS_HPP__
#define __BERRY_PROCESS_HPP__ 1

// C++ Standard Library:
#include <string>
#include <vector>

// Boost Library:
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

// Berry:
#include <berry/detail/process_detail.hpp>
#include <berry/process_entry.hpp>

namespace berry
{
   /**
   * @brief Represents a process on the system.
   */
   class process
   {
   public:
      /**
      * @brief Defines a type used to store arbitrary process identifiers.
      */
      typedef detail::process::pid_type pid_type;
      
   private:
      pid_type m_pid;
      
   public:
      /**
       * @brief Constructs a process object which doesn't represent a process.
       */
      process();
    
      /**
       * @brief Constructs a process object from a process id.
       * 
       * @param pid The target pid.
       */
      explicit process(pid_type pid);
      
      /**
       * @brief Returns the pid of the process.
       *
       * @return pid_type The pid of the process.
       */
      pid_type get_pid() const;
   };
   
   /**
    * @brief Returns the name of the specified process.
    * Many systems limit this to a few characters, so it may be incomplete
    * on those systems. Use get_executable_path to be sure to determine the
    * full name.
    * @param proc The target process.
    * @return :string& The process' name
    * @see get_executable_path
    */
   std::string get_name(process proc);
    
   /**
    * @brief Makes simple checks to determine if the process still exists.
    *
    * @param proc The target process.
    * @return bool True if the process still exists, false otherwise.
    */
   bool still_exists(process proc);
    
   namespace unix_like
   {
      /**
       * @brief Returns the ProcFS directory of the specified process.
       * Some systems implement the Proc filesystem. For those systems, the
       * path to the process' directory inside the ProcFS is returned.
       * @param proc The target process.
       * @return :filesystem3::path The path to the ProcFS directory or an
       * empty path, if not implemented.
       */
      boost::filesystem::path get_procfs_dir(process proc);
   }
    
   /**
    * @brief Returns the working directory of the specified process.
    *
    * @param proc The target process.
    * @return :filesystem3::path The path to the working directory.
    */
   boost::filesystem::path get_working_dir(process proc);
    
   /**
   * @brief Returns the path to the binary file of the specified process.
   * On some systems you need to have administrative rights to query
   * this value from processes run by other users.
   * @param proc The target process.
   * @return :filesystem3::path The path to the binary file.
   */
   boost::filesystem::path get_executable_path(process proc);

   /**
   * @brief Returns the bitness of the specified process (32/64 bit)
   *
   * @param proc The target process.
   * @return int The result as integer, 0 if the value cant be determined.
   */
   int get_bitness(process proc);
    
   /**
   * @brief Returns the currently active process calling this function.
   *
   * @return :process& The current process.
   */
   process get_current_process();
    
   /**
   * @brief Creates a new process which runs as the current process' child.
   *
   * @param arguments The arguments to the process where the command to run
   * is passed as the first argument.
   * @return :process The created child process.
   */
   process simple_create_process(std::vector<std::string> const& arguments);
  
   /**
    * @brief Terminates the specified process. 
    * Some systems allow to pass a exit code which is returned as the process'
    * exit code, other systems will ignore this parameter.
    * @param proc The target process.
    * @param exit_code An optional exit code value.
    */
   void terminate_process(process proc, int exit_code = 0);
}

#endif // __BERRY_PROCESS_HPP__