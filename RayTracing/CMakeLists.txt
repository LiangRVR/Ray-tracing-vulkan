file(GLOB_RECURSE RayTracing_SRC LIST_DIRECTORIES false src/*.cpp)

add_executable(RayTracing ${RayTracing_SRC})
target_include_directories(RayTracing PRIVATE src)
target_link_libraries(RayTracing PRIVATE Walnut)

# setup internal project compile definition
if(WIN32)
    target_compile_definitions(Walnut PRIVATE WL_PLATFORM_WINDOWS)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(RayTracing PRIVATE WL_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_definitions(RayTracing PRIVATE WL_RELEASE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(RayTracing PRIVATE WL_DIST)
endif()

install(TARGETS RayTracing DESTINATION bin)
