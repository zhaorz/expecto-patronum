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

set(CMAKE_MODULE_PATH "/usr/local/Cellar/sfml/2.4.2_1/share/SFML/cmake/Modules" ${CMAKE_MODULE_PATH})

find_package(SFML 2 REQUIRED COMPONENTS network audio graphics window system)

message(STATUS "SFML library status:")
message(STATUS "    libraries: ${SFML_LIBRARIES}")
message(STATUS "    dependencies: ${SFML_DEPENDENCIES}")

add_executable( Main Main.C )
target_link_libraries(Main ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
