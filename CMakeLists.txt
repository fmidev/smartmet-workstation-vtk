cmake_minimum_required(VERSION 3.7.2)

project (VTKSLN)

include(
    UsefulVariables.cmake
    )

include(
    modules/dependencies/install_deps.cmake
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

