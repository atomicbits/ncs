PROJECT(synapse_flat)
AUX_SOURCE_DIRECTORY(source "source")
INCLUDE_DIRECTORIES("include" ${NCS_INCLUDES})

FIND_PACKAGE(CUDA)
IF(CUDA_FOUND AND NCS_CUDA)
	INCLUDE_DIRECTORIES(${CUDA_INCLUDE_DIRS})
	CUDA_COMPILE(cuda_source 
               "source/Flat.cu" 
               OPTIONS ${CUDA_OPTIONS} 
               --compiler-options '-fPIC')
	SET(source ${source} ${cuda_source})
ENDIF(CUDA_FOUND AND NCS_CUDA)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")
ADD_LIBRARY(${PROJECT_NAME} MODULE ${source})
ADD_DEPENDENCIES(${PROJECT_NAME} ncs_sim ncs_spec)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES 
	LINKER_LANGUAGE CXX)
INSTALL(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION "${NCS_PLUGIN_PATH}/synapse")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ncs_sim ncs_spec)

