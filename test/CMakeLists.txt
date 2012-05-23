# Boost is required to build the tests.
find_package(Boost 1.42.0 COMPONENTS
	system filesystem unit_test_framework REQUIRED)

# Compile and link tests for the process API.
add_executable(test_processes test_processes.cpp)
target_link_libraries(test_processes
	${BERRY_LIBRARIES}
	${Boost_LIBRARIES}
)
