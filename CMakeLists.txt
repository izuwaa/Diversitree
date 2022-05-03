cmake_minimum_required(VERSION 3.3)

set(PROJECT_NAME nearOptimal)
project(${PROJECT_NAME})

if(TARGET SCIP::SCIP)
  # find package by SCIP PATH
  find_package(SCIP CONFIG PATHS ${SCIP_BINARY_DIR} REQUIRED)
else()
  find_package(SCIP REQUIRED)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
#find_package(GMP REQUIRED)

include_directories(${GMP_INCLUDE_DIR})
include_directories(${SCIP_INCLUDE_DIRS})

# set variables
set(EXECUTABLE_NAME "diversitree")
set(DIVERSITREE_VERSION_MAJOR 1)
set(DIVERSITREE_VERSION_MINOR 0)
set(SRC src)
set(DOC doc)
set(CMAKE cmake)
set(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -Wno-unused-but-set-variable -Wno-unused-variable")



set(HEADERS_FILES
        ${SRC}/branchCountScip.h
        ${SRC}/currentStateVariables.h
        ${SRC}/diversitreenode.h
        ${SRC}/diversityCalculator.h
        ${SRC}/oneTreeGams.h
        ${SRC}/optionsReader.h
        ${SRC}/resultWriter.h
        ${SRC}/solutionsGenerator.h
        ${SRC}/solutionStoreMap.h
	${SRC}/solutionStoreVector.h
	${SRC}/utilities.h
        )

set(SOURCE_FILES
        ${SRC}/branchCountScip.cpp
	${SRC}/main.cpp
        ${SRC}/currentStateVariables.cpp
        ${SRC}/diversitreenode.cpp
        ${SRC}/diversityCalculator.cpp
        ${SRC}/oneTreeGams.cpp
        ${SRC}/optionsReader.cpp
        ${SRC}/resultWriter.cpp
        ${SRC}/solutionsGenerator.cpp
        ${SRC}/solutionStoreMap.cpp
	${SRC}/solutionStoreVector.cpp
	${SRC}/utilities.cpp
        )

include_directories(${PROJECT_SOURCE_DIR}/${SRC})

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})
target_link_libraries(${EXECUTABLE_NAME} ${SCIP_LIBRARIES})
if( TARGET applications )
    add_dependencies( applications ${EXECUTABLE_NAME} )
endif()

#include(CTest)

#
# add tests to build the application and run on some easy instances
#
#add_test(NAME applications-${EXECUTABLE_NAME}-build
#        COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --config $<CONFIG> --target "${EXECUTABLE_NAME}"
#        )