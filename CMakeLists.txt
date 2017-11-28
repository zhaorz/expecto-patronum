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
