/**
 * @file process_iterator.hpp
 * @author Ethon aka Florian Erler (ethon [a-t] ethon.cc)
 * @date 2011
 * @version 1.0a
 * @brief Public process_iterator API. 
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

#ifndef __BERRY_PROCESSITERATOR_HPP__
#define __BERRY_PROCESSITERATOR_HPP__ 1

// Boost Library:
#include <boost/iterator/iterator_facade.hpp>
#include <boost/optional.hpp>

// Berry:
#include <berry/process_entry.hpp>

namespace berry
{
   /**
    * @brief An incrementable iterator to walk the process list.
    **/
   class process_iterator
      : public boost::iterator_facade< process_iterator,
                                       process_entry const,
                                       boost::incrementable_traversal_tag>
   {
      friend class boost::iterator_core_access;
      
   private:
      process_snapshot m_snap;
      boost::optional<process_entry> m_entry;

      void increment();
      bool equal(process_iterator const& other) const;
      process_entry const& dereference() const;
      
   public:
      /**
       * @brief Default constructor creating an invalid iterator.
       **/
      process_iterator();
      
      /**
       * @brief Constructor creating a valid iterator.
       *
       * @param dummy Pass any value to create a valid iterator.
       **/
      process_iterator(int dummy);
   };
   
   /**
    * @brief A begin/end interface to walk the process list.
    **/
   class process_list
   {
   public:
      /**
       * @brief Returns an iterator to the begin of the list.
       *
       * @return :process_iterator An iterator to the begin of the list.
       **/
      process_iterator begin() const;
      
      /**
       * @brief Returns an iterator to the end of the list.
       *
       * @return :process_iterator An iterator to the end of the list.
       **/
      process_iterator end() const;
   };
}

#endif // __BERRY_PROCESSITERATOR_HPP__