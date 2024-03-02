

add_executable (SearchEngine "main.cpp" "Converter.h" "Index.h" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SearchEngine PROPERTY CXX_STANDARD 20)
endif()
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
target_link_libraries("SearchEngine" PRIVATE nlohmann_json::nlohmann_json gtest_main)
target_link_libraries("SearchEngine" PRIVATE gtest_main)

include(GoogleTest)

gtest_discover_tests(SearchEngine DISCOVERY_MODE PRE_TEST)