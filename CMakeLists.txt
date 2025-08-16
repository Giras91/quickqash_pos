cmake_minimum_required(VERSION 3.16)
project(QuickQash VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

# Put build artifacts in build/ by default
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Find Qt components (include Test here so test target can link to Qt6::Test)
find_package(Qt6 COMPONENTS Widgets Sql UiTools Test REQUIRED)

file(GLOB_RECURSE SOURCES
  ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(QuickQash ${SOURCES})

target_link_libraries(QuickQash PRIVATE Qt6::Widgets Qt6::Sql Qt6::UiTools)

# Public include directory for the main target
target_include_directories(QuickQash PUBLIC ${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE TEST_SOURCES
  ${CMAKE_SOURCE_DIR}/tests/*.cpp
)

if (TEST_SOURCES)
  add_executable(QuickQashTests ${TEST_SOURCES})
  target_link_libraries(QuickQashTests PRIVATE Qt6::Test Qt6::Sql)
  # Ensure tests can see the project's public headers
  target_include_directories(QuickQashTests PUBLIC ${CMAKE_SOURCE_DIR}/include)
  enable_testing()
  add_test(NAME QuickQashTests COMMAND QuickQashTests)
endif()

# Set output directories
set_target_properties(QuickQash PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Ensure the ui/ directory is available at runtime (copy to build dir)
add_custom_command(TARGET QuickQash POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/ui $<TARGET_FILE_DIR:QuickQash>/ui
)

message(STATUS "Config: ${CMAKE_BUILD_TYPE}")

# Simple install rules
install(TARGETS QuickQash
  RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/ui
  DESTINATION share/quickqash/ui
)

install(FILES ${CMAKE_SOURCE_DIR}/data/schema.sql
  DESTINATION share/quickqash/data
)

# Basic CPack configuration (ZIP package by default)
set(CPACK_PACKAGE_NAME "QuickQash")
set(CPACK_PACKAGE_VENDOR "QuickQash")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "ZIP")
include(InstallRequiredSystemLibraries)
include(CPack)

# Add license and resource for packaging
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "QuickQash POS - lightweight Windows point-of-sale")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/icon.ico")

# Custom commands for building with Visual Studio 17 2022
if (MSVC)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp PROPERTY FOLDER "Source Files")
  set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/another_source_file.cpp PROPERTY FOLDER "Source Files")
  # Add more source files as needed
endif()
