cmake_minimum_required (VERSION 3.13)

project(RayTracing)

find_package(Vulkan REQUIRED)

# TODO: Figure out how to allow later standards
if(NOT DEFINED CMAKE_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD 17)
endif()

add_subdirectory(Core)
add_subdirectory(Dependencies)
add_subdirectory(RayTracing)
