project (surfacevis)

find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

# Configure the template file
SET(USER_FILE ${PROJECT_NAME}.vcxproj.user)
SET(OUTPUT_PATH ${PROJECT_BINARY_DIR}/${USER_FILE})
SET(COMMANDLINE_ARGUMENTS "")
SET(WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration))
CONFIGURE_FILE(UserTemplate.user ${USER_FILE} @ONLY)


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

add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${PROJECT_SOURCE_DIR}/res
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${PROJECT_SOURCE_DIR}/res/icons.png
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/icons.png
)