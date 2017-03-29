cmake_minimum_required(VERSION 3.7.2)

project (VTKSLN)

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
	step1
)

add_subdirectory(
	volumetest
)

