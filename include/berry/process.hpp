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

// Boost Library:
#include <boost/filesystem/path.hpp>

// Berry:
#include <berry/detail/process_detail.hpp>

namespace berry
{
    /**
     * @brief Defines a type used to store arbitrary process identifiers.
     **/
    typedef detail::process::pid_type pid_type;
        
    /**
     * @brief Represents a process on the system.
     **/
    class process
    {
    private:
        detail::process::process_data m_data;
        
    public:
        /**
         * @brief Constructs a process object which doesn't represent a process.
         **/
        process();

        /**
         * @brief Constructs a process object from a process id.
         * 
         * @param pid The process' id.
         **/
        explicit process(pid_type pid);

        /**
         * @brief Constructs a process object from a process name.
         *
         * @param name The process' name.
         * @param case_sensitive Pass true to search case sensitive.
         **/
        process(std::string const& name, bool case_sensitive = true);

        /**
         * @brief Constructs a process object from copying another process.
         * 
         * @param other Another process to copy.
         **/
        process(process const& other);

        /**
         * @brief Constructs a process object from moving another process.
         * 
         * @param other Another process to move.
         **/
        process(process&& other);
 
        /**
         * @brief Destructs the process object.
         **/
        ~process();

        /**
         * @brief Copies another process object.
         *
         * @param other Another process to copy.
         * @return :process& *this
         **/
        process& operator=(process const& other);
    
        /**
         * @brief Moves another process object.
         * 
         * @param other Another process to move.
         * @return :process& *this
         **/
        process& operator=(process&& other);

        /**
         * @brief Returns the process' pid.
         *
         * @return :pid_type The process' pid.
         **/
        pid_type pid() const;

        /**
         * @brief Retrieves the process' name.
         * Many systems limit this to a few characters, so it may be incomplete
         * on those systems. Use executable_path to be sure to determine the
         * full name.
         * @return :string The process' namespace
         * @see executable_path
         **/
        std::string name() const;
        
        /**
         * @brief Retrieves the path to the process' executable file.
         *
         * @return :filesystem3::path The path to the binary file.
         **/
        boost::filesystem::path executable_path() const;
        
        /**
         * @brief Retrieves the process' bitness. (32/64 bit)
         *
         * @return int The result as integer.
         **/
        int bitness() const;

        /**
         * @brief Terminates the process. 
         *
         * @param force Pass true to force the process' termination.
         **/
        void terminate(bool force = false);
        
        /**
         * @brief Makes simple checks to determine if the process still exists.
         *
         * @return bool True if the process still exists, false otherwise.
         **/
        bool still_exists() const;
    };
    
    /**
     * @brief Type to use in function expecting a const process object.
     **/
    typedef BERRY_PROCESS_AS_PARAM process_param_type;
    
    /**
     * @brief Less-than comparison operator.
     *
     * @param lhs Process object on the left.
     * @param rhs Process object on the right.
     * @return bool True if the left is less than the right.
     **/
    bool operator<(process_param_type lhs, process_param_type rhs);
    
    /**
     * @brief Equality comparison operator.
     *
     * @param lhs Process object on the left.
     * @param rhs Process object on the right.
     * @return bool True if both processes specify the same process.
     **/
    bool operator==(process_param_type lhs, process_param_type rhs);

    /**
     * @brief Inequality comparison operator.
     *
     * @param lhs Process object on the left.
     * @param rhs Process object on the right.
     * @return bool True if both processes don't specify the same process.
     **/
    bool operator!=(process_param_type lhs, process_param_type rhs);
    
    /**
     * @brief Returns the currently active process calling this function.
     *
     * @return :process& The current process.
     **/
    process const& get_current_process();
    
    /**
     * @brief Process type which doesn't represent a real process.
     **/
    extern process const not_a_process;
    
#ifdef BERRY_HAS_PROCFS 
    namespace unix_like
    {
        /**
         * @brief Sets the ProcFS base directory.
         * Per default the ProcFS base directory is set to the most common
         * path on the system (e.g. /proc on Linux). Call this function if
         * the ProcFS is mounted somewhere else.
         * @param base_dir The base of the ProcFS.
         **/
        void set_procfs_base(boost::filesystem::path const& base_dir);
        
        /**
         * @brief Returns the ProcFS directory of the specified process.
         * Some systems implement the Proc filesystem. For those systems, the
         * path to the process' directory inside the ProcFS is returned.
         * @param proc The target process.
         * @return :filesystem3::path The path to the ProcFS directory.
         **/
        boost::filesystem::path get_procfs_dir(process proc);
    }
#endif
}

#endif // __BERRY_PROCESS_HPP__