project (surfacevis)

find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

file( 
GLOB SOURCE_FILES
	*.cpp
	*.h
)

message(${BOOST_LIB_DIR})
link_directories(
	${BOOST_LIB_DIR}
	)
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
 
if(VTK_LIBRARIES)
  target_link_libraries(${PROJECT_NAME} 
  ${VTK_LIBRARIES}
  fmivisbase
  )
else()
  target_link_libraries(${PROJECT_NAME} 
  vtkHybrid
  vtkWidgets 
  fmivisbase
  )
endif()

target_include_directories(${PROJECT_NAME}
	PUBLIC ${fmivisbase_INCLUDE_DIRS}
	)
