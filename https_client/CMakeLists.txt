cmake_minimum_required(VERSION 3.12)
project(https_client VERSION 1.0.0 DESCRIPTION "header-only HTTPS client")

# Find Boost components required by Boost.Beast (Boost.Asio and Boost.System)
find_package(Boost REQUIRED)

# Find OpenSSL package
find_package(OpenSSL REQUIRED)

# Header-only libraries don't have a build step, just declare the interface
add_library(https_client INTERFACE)

# Specify the include directories for Boost, OpenSSL, and your own headers
target_include_directories(https_client INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${Boost_INCLUDE_DIRS}  # Include Boost headers
    ${OPENSSL_INCLUDE_DIR}  # Include OpenSSL headers
)

# Link Boost and OpenSSL libraries (not necessary for header-only Boost and OpenSSL)
# target_link_libraries(https_client INTERFACE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

# Optionally, you can set versioning for your library
set_target_properties(https_client PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Install the library headers
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Optionally, you can export the library so it can be imported by other projects
export(TARGETS https_client FILE https_clientConfig.cmake)
