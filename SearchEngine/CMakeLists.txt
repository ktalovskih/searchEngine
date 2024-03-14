add_executable (SearchEngine "main.cpp" "Converter.h" "Index.h" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SearchEngine PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries("SearchEngine" PRIVATE nlohmann_json::nlohmann_json)


