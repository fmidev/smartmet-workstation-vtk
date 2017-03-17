cmake_minimum_required(VERSION 3.7.2)

project (volumetest)

find_package(VTK REQUIRED)

include(${VTK_USE_FILE})


file( 
GLOB SOURCE_FILES
	*.cpp
	*.h
)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
 
if(VTK_LIBRARIES)
  target_link_libraries(${PROJECT_NAME}  ${VTK_LIBRARIES})
else()
  target_link_libraries(${PROJECT_NAME}  vtkHybrid vtkWidgets)
endif()