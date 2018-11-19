project (fmivisbase)

set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(VTK REQUIRED NO_MODULE)

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
  SmartTools
  JsonSpirit
  )
else()
  target_link_libraries(${PROJECT_NAME} 
  vtkHybrid
  vtkWidgets 
  Newbase
  SmartTools
  JsonSpirit
  )
endif()

target_include_directories(${PROJECT_NAME}
	PUBLIC ${Newbase_INCLUDE_DIRS}
	PUBLIC ${SmartTools_INCLUDE_DIRS}
	)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
                                 ${Newbase_INCLUDE_DIRS}
								 ${SmartTools_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
	
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)