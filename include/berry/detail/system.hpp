/**
 * @file system.hpp
 * @author Florian Erler aka Ethon <ethon [a-t] ethon.cc>
 * @date 2011
 * @brief Collects info about the target cpu and os.
 * 
 * This file is a part of of Berry.
 * Berry is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Berry is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Berry.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __BERRY_DETAIL_SYSTEM_HPP__
#define __BERRY_DETAIL_SYSTEM_HPP__ 1

// Determine operating system.
#if defined linux || defined __linux__
#   define BERRY_VALID_OS 1
#   define BERRY_LINUX 1
#   define BERRY_OS_STRING "Linux"
#endif

// Determine CPU.
#if defined __i386__ || defined i386 || defined _M_IX86
#   define BERRY_VALID_CPU 1
#   define BERRY_X86 1
#   if defined __x86_64__  || defined _M_X64
#       define BERRY_64BIT 1
#       define BERRY_BITS 64
#       define BERRY_CPU_STRING "AMD x86-64 compatible CPU"
#   else
#       define BERRY_32BIT 1
#       define BERRY_BITS 32
#       define BERRY_CPU_STRING "i386+ compatible CPU"
#   endif
#endif

// Assert if we can compile for this platform.
#ifndef BERRY_VALID_OS
#   error "This operating system is currently not supported"
#endif

#ifndef BERRY_VALID_CPU
#   error "This platform is currently not supported"
#endif

#endif // __BERRY_DETAIL_SYSTEM_HPP__