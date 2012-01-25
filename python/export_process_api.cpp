/**
 * @file export_process_api.cpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Exports the process API to python.
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

// Boost:
#include <boost/python/class.hpp>
#include <boost/python/args.hpp>
#include <boost/python/str.hpp>
#include <boost/python/def.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/copy_const_reference.hpp>

// Berry:
#include <berry/process.hpp>
#include <berry/process_entry.hpp>
#include <berry/process_iterator.hpp>

namespace bp = boost::python;

static std::string wrap_executable_path(berry::process const* obj)
{
    return obj->executable_path().string();
}

#ifdef BERRY_HAS_PROCFS
static void wrap_set_procfs_base(std::string const& base)
{
    berry::unix_like::set_procfs_base(boost::filesystem::path(base));
}
#endif

void export_process_hpp()
{
    //::berry::process
    typedef bp::class_<berry::process> process_exposer_t;
    process_exposer_t process_exposer =
        process_exposer_t( "process", bp::init<>() );
    bp::scope process_scope(process_exposer);
    
    process_exposer.def(bp::init<berry::pid_type>((bp::arg("pid"))));
    bp::implicitly_convertible<berry::pid_type, berry::process>();
    
    process_exposer.def(bp::init<std::string const &, bp::optional<bool> >(
        (bp::arg("name"), bp::arg("case_sensitive")=(bool)(true))));
    bp::implicitly_convertible<std::string const &, berry::process>();
    
    { //::berry::process::bitness
        typedef int (::berry::process::*bitness_function_type)() const;
        process_exposer.def( 
            "bitness",
            bitness_function_type(&::berry::process::bitness));
    }
    
    { //::berry::process::executable_path
        typedef ::std::string (*executable_path_function_type)
            (berry::process const*);
        process_exposer.def( 
            "executable_path",
            executable_path_function_type(&::wrap_executable_path));
        
    }
    
    { //::berry::process::name
        typedef ::std::string (::berry::process::*name_function_type)() const;
        process_exposer.def( 
            "name",
            name_function_type(&::berry::process::name));
        
    }
    
    { //::berry::process::pid
        typedef ::berry::pid_type (::berry::process::*pid_function_type)() const;
        process_exposer.def( 
            "pid",
            pid_function_type(&::berry::process::pid));
        
    }

    { //::berry::process::still_exists
        typedef bool (::berry::process::*still_exists_function_type)() const;
        process_exposer.def( 
            "still_exists",
            still_exists_function_type(&::berry::process::still_exists));
        
    }
    
    { //::berry::process::terminate
        typedef void (::berry::process::*terminate_function_type)(bool);
        process_exposer.def( 
            "terminate",
            terminate_function_type(&::berry::process::terminate),
            ( bp::arg("force")=(bool)(false)));
    }
    
    process_exposer.def(bp::self != bp::self);
    process_exposer.def(bp::self < bp::self);
    process_exposer.def(bp::self == bp::self);

    { //::berry::get_current_process
        typedef ::berry::process const& (*get_current_process_function_type)();
        bp::def( 
            "get_current_process",
            get_current_process_function_type(&::berry::get_current_process),
            bp::return_value_policy<bp::copy_const_reference>());
    
    }

#ifdef BERRY_HAS_PROCFS
    { //::berry::unix_like::get_procfs_dir
        typedef ::boost::filesystem3::path (*get_procfs_dir_function_type)
            (::berry::process);
        
        bp::def( 
            "get_procfs_dir",
            get_procfs_dir_function_type(&::berry::unix_like::get_procfs_dir),
            (bp::arg("proc")));
    
    }

    { //::berry::unix_like::set_procfs_base
        typedef void (*set_procfs_base_function_type)(std::string const&);
        
        bp::def( 
            "set_procfs_base",
            set_procfs_base_function_type(&::wrap_set_procfs_base),
            (bp::arg("base_dir")));
    
    }
#endif

    bp::scope().attr("not_a_process") = berry::not_a_process;
}