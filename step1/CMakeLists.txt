project (Step1)

find_package(VTK REQUIRED)
vtk_module_config(VTK
  vtkCommonCore
  vtkFiltersSources
  vtkFiltersSources
  vtkInteractionStyle
  vtkRendering${VTK_RENDERING_BACKEND}
)
include(${VTK_USE_FILE})


file( 
GLOB SOURCE_FILES
	*.cpp
	*.h
)

add_executable(
	${PROJECT_NAME} MACOSX_BUNDLE 
	${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})