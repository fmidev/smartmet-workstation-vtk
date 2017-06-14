cmake_minimum_required(VERSION 3.7.1)

project (VTKSLN)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(
    -D_WIN64
    -D_UNICODE 
    -DUNICODE 
    )

add_compile_options(
    $<$<CXX_COMPILER_ID:MSVC>:/MP>
    )


include(
    UsefulVariables.cmake
    )
add_subdirectory(
	modules/newbase
)

add_subdirectory(
	fmivisbase
)


add_subdirectory(
	testbed
)

add_subdirectory(
	volumetest
)

add_subdirectory(
	surfacevis
)


