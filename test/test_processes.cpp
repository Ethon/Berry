// C++ Standard Library:
#include <limits>
#include <string>
#include <iostream>

// Boost Library:
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE processes test
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

// Berry:
#include <berry/process.hpp>

using berry::process;

BOOST_AUTO_TEST_SUITE(BerryProcessAPI)

// Test berry::get_name
BOOST_AUTO_TEST_CASE(BerryProcessGetName)
{
   process self(berry::get_current_process());
   std::string name(berry::get_name(self));
   
   std::cout << "Current process name: " << name << '\n';
   BOOST_CHECK(!name.empty());
}

// Test berry::still_exists
BOOST_AUTO_TEST_CASE(BerryProcessStillExists)
{
   process existing(berry::get_current_process());
   process non_existing(std::numeric_limits<int>::max() - 317);
      
   BOOST_CHECK(berry::still_exists(existing));
   BOOST_CHECK(!berry::still_exists(non_existing));
}

// Test berry::unix_like::get_procfs_dir
BOOST_AUTO_TEST_CASE(BerryProcessGetProcfsDir)
{
   process self(berry::get_current_process());
   boost::filesystem::path proc_dir(berry::unix_like::get_procfs_dir(self));
     
   std::cout << "Current process procfs directory: "
      << proc_dir.string() << '\n';
   BOOST_CHECK(boost::filesystem::exists(proc_dir) || proc_dir.empty());
}

// Test berry::get_working_dir
BOOST_AUTO_TEST_CASE(BerryProcessGetWorkingDir)
{
   process self(berry::get_current_process());
   boost::filesystem::path cwd(berry::get_working_dir(self));
      
   std::cout << "Current process working directory: " << cwd.string() << '\n';
   BOOST_CHECK(boost::filesystem::exists(cwd));
}

// Test berry::get_exexcutable_path
BOOST_AUTO_TEST_CASE(BerryProcessGetExecutablePath)
{
   process self(berry::get_current_process());
   boost::filesystem::path exe(berry::get_executable_path(self));
      
   std::cout << "Current process executable path: " << exe.string() << '\n';
   BOOST_CHECK(boost::filesystem::exists(exe));
}

// Test berry::get_bitness
BOOST_AUTO_TEST_CASE(BerryProcessGetBitness)
{
   process self(berry::get_current_process());
   int bitness = berry::get_bitness(self);
   int real_bitness = sizeof(void*) == 8 ? 64 : 32;
      
   std::cout << "Current process bitness: " << bitness << '\n';
   BOOST_CHECK_EQUAL(bitness, real_bitness);
}

BOOST_AUTO_TEST_SUITE_END()

