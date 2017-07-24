project (fmivisbase)

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

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
 
if(VTK_LIBRARIES)
  target_link_libraries(${PROJECT_NAME} 
  ${VTK_LIBRARIES}
  Newbase
  )
else()
  target_link_libraries(${PROJECT_NAME} 
  vtkHybrid
  vtkWidgets 
  Newbase
  )
endif()

target_include_directories(${PROJECT_NAME}
	PUBLIC ${Newbase_INCLUDE_DIRS}
	)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
                                 ${Newbase_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)