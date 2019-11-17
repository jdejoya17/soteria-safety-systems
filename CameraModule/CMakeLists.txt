# Set CMake
cmake_minimum_required( VERSION 3.0 )
cmake_policy( SET CMP0012 NEW )


# Name Project
project( BS_DETECTOR )


# Flags
set(THREADS_PREFER_PTHREAD_FLAG ON)
# Enable C++11
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


# Files
# Source Files
set( 	
	SRC 
	./src/bs_detector.cpp 
	./src/frame_processor.cpp
	./src/depth_stream_visualizer.cpp
	./src/bt_transmitter.cpp
)


# Find Packages
# OpenCV
find_package( OpenCV 4.0.0 REQUIRED )
# librealsense2
find_package( realsense2 REQUIRED )
# pthreads
find_package(Threads REQUIRED)


# Include files
include_directories(
	./include 
	./src 
	${OpenCV_INCLUDE_DIRS}
	${realsense_INCLUDE_DIR}
)


# Target files
add_executable( ${PROJECT_NAME} ${SRC})


# Link depencies (OpenCV) to our executable
target_link_libraries( 
	${PROJECT_NAME} 
	${OpenCV_LIBS} 
	${realsense2_LIBRARY} 
	pthread
)

