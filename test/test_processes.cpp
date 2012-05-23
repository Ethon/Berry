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
#include <berry/process_entry.hpp>
#include <berry/process_iterator.hpp>

using berry::process;

BOOST_AUTO_TEST_SUITE(BerryProcessAPI)

// Test berry::get_name
BOOST_AUTO_TEST_CASE(BerryProcessGetName)
{
   process self(berry::get_current_process());
   std::string name(self.name());
   
   std::cout << "Current process name: " << name << '\n';
   BOOST_CHECK(!name.empty());
}

// Test berry::still_exists
BOOST_AUTO_TEST_CASE(BerryProcessStillExists)
{
   process existing(berry::get_current_process());
   process non_existing(std::numeric_limits<int>::max() - 317);
      
   BOOST_CHECK(existing.still_exists());
   BOOST_CHECK(!non_existing.still_exists());
}

#if BERRY_HAS_PROCFS
// Test berry::unix_like::get_procfs_dir
BOOST_AUTO_TEST_CASE(BerryProcessGetProcfsDir)
{
   process self(berry::get_current_process());
   boost::filesystem::path proc_dir(berry::unix_like::get_procfs_dir(self));
     
   std::cout << "Current process procfs directory: "
      << proc_dir.string() << '\n';
   BOOST_CHECK(boost::filesystem::exists(proc_dir));
}
#endif

// Test berry::get_exexcutable_path
BOOST_AUTO_TEST_CASE(BerryProcessGetExecutablePath)
{
   process self(berry::get_current_process());
   boost::filesystem::path exe(self.executable_path());
      
   std::cout << "Current process executable path: " << exe.string() << '\n';
   BOOST_CHECK(boost::filesystem::exists(exe));
}

// Test berry::get_bitness
BOOST_AUTO_TEST_CASE(BerryProcessGetBitness)
{
   process self(berry::get_current_process());
   int bitness = self.bitness();
   int real_bitness = sizeof(void*) == 8 ? 64 : 32;
      
   std::cout << "Current process bitness: " << bitness << '\n';
   BOOST_CHECK_EQUAL(bitness, real_bitness);
}

// Test berry::get_entry_by_pid.
BOOST_AUTO_TEST_CASE(BerryGetEntryByPid)
{
   process self(berry::get_current_process());
   
   boost::optional<berry::process_entry> by_pid(
      berry::get_entry_by_pid(self.pid()));
   BOOST_REQUIRE(by_pid);
   BOOST_CHECK_EQUAL(self.name(), by_pid->name);
}

// Test berry::get_entry_by_name 1
BOOST_AUTO_TEST_CASE(BerryGetEntryByName1)
{
   process self(berry::get_current_process());
   std::string self_name(self.name());
   
   boost::optional<berry::process_entry> by_name(
      berry::get_entry_by_name(self_name));
   BOOST_REQUIRE(by_name);
   BOOST_CHECK_EQUAL(self_name, by_name->name);
}

// Test berry::get_entry_by_name 2
BOOST_AUTO_TEST_CASE(BerryGetEntryByName2)
{
   process self(berry::get_current_process());
   std::string self_name(self.name());
   
   boost::optional<berry::process_entry> by_name1(
      berry::get_entry_by_name(self_name, true));
   BOOST_REQUIRE(by_name1);
   
   std::string upper_case_name(self_name);
   std::transform(self_name.begin(), self_name.end(),
      upper_case_name.begin(), &toupper);
   boost::optional<berry::process_entry> by_name2(
      berry::get_entry_by_name(self_name, false));
   BOOST_REQUIRE(by_name2);  
   
   BOOST_CHECK_EQUAL(by_name1->pid, by_name2->pid);
}

BOOST_AUTO_TEST_SUITE_END()

