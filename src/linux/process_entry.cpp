/**
 * @file linux/process_entry.cpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Process entry implementation for Linux. 
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

#include <berry/detail/system.hpp>
#ifndef BERRY_LINUX
#   error "Attempt to compile source file on a wrong system"
#endif

// System:
#include <glob.h>

// C++ Standard Library:
#include <limits>
#include <string>

// Boost Library:
#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

// Berry:
#include <berry/process.hpp>
#include <berry/process_entry.hpp>

/******** Helper classes ********/
struct snapshot
{
    snapshot()
        : data_ptr(0), data()
    {
        int result =
            ::glob("/proc/[0-9]*", GLOB_ONLYDIR, 0, &data);
        if(result == GLOB_NOSPACE)
            throw std::runtime_error("snapshot::snapshot : out of memory");
        if(result == GLOB_ABORTED || result == GLOB_NOMATCH)
            throw std::runtime_error(   "snapshot::snapshot : "
                                        "procfs not correctly mounted");
    }
   
    ~snapshot()
    {
        ::globfree(&data);
    }
   
    std::size_t data_ptr;
    ::glob_t data;
};

/******** Free helper functions ********/
static void destroy_snapshot(void* snap)
{
    delete static_cast< ::snapshot*>(snap);
}

static berry::process_entry make_entry_from_path(boost::filesystem::path& dir)
{
    dir /= "stat";
    boost::filesystem::ifstream stat(dir);
   
    berry::process_entry result;
    stat >> result.pid;
    stat.ignore(std::numeric_limits<std::streamsize>::max(), '(');
    std::getline(stat, result.name, ')');
    char state; stat >> state >> result.parent_pid;
   
    return result;
}

/******** Constructors and Destructor ********/
berry::process_entry::process_entry()
    : pid(0), parent_pid(0), name()
{ }

/******** Free functions ********/
berry::process_snapshot berry::create_process_snapshot()
{
    return berry::process_snapshot(new ::snapshot(), &::destroy_snapshot);
}
   
berry::process_entry
    berry::extract_first_process(berry::process_snapshot& snap)
{
    ::snapshot* ss = static_cast< ::snapshot*>(snap.get());
    boost::filesystem::path dir(ss->data.gl_pathv[0]);
    return ::make_entry_from_path(dir);
}
   
boost::optional<berry::process_entry> berry::extract_next_process(
    berry::process_snapshot& snap)
{
    ::snapshot* ss = static_cast< ::snapshot*>(snap.get());
    if(ss->data_ptr < ss->data.gl_pathc)
    {
        boost::filesystem::path dir(ss->data.gl_pathv[ss->data_ptr++]);
        return boost::optional<berry::process_entry>(
            ::make_entry_from_path(dir));
    }
   
    return boost::optional<berry::process_entry>();
}