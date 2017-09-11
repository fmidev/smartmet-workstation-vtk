project (tests)

find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

file( 
GLOB SOURCE_FILES
	*.cpp
	*.h
)

link_directories(
	${BOOST_LIB_DIR}
	)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
 
if(VTK_LIBRARIES)
  target_link_libraries(${PROJECT_NAME} 
  ${VTK_LIBRARIES}
  fmivisbase
  gtest
  gmock_main
  )
else()
  target_link_libraries(${PROJECT_NAME} 
  vtkHybrid
  vtkWidgets 
  fmivisbase
  gtest
  gmock_main
  )
endif()

target_include_directories(${PROJECT_NAME}
	PUBLIC ${fmivisbase_INCLUDE_DIRS}
	)

