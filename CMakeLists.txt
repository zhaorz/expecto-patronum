cmake_minimum_required(VERSION 3.9)
set (CMAKE_CXX_STANDARD 11)
project (patronus)

message(STATUS "Architecture: ${ARCH}")

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBRARIES}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

add_executable( Patronus Patronus.C )
target_link_libraries( Patronus ${OpenCV_LIBS} )

# Main graphics
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(SFML 2 REQUIRED COMPONENTS network audio graphics window system)

message(STATUS "SFML library status:")
message(STATUS "    libraries: ${SFML_LIBRARIES}")
message(STATUS "    include: ${SFML_INCLUDE_DIR}")
message(STATUS "    dependencies: ${SFML_DEPENDENCIES}")

add_executable( Main Main.C )
include_directories(${SFML_INCLUDE_DIR})

target_link_libraries(Main ${OpenCV_LIBS} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
