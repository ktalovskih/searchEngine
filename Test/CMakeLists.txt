add_executable("Test" testing.cpp)

include(FetchContent)

FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)


target_link_libraries("Test" PRIVATE gtest_main nlohmann_json::nlohmann_json)


enable_testing()

include(GoogleTest)
gtest_discover_tests(SearchEngine DISCOVERY_MODE PRE_TEST)