#--------------------------------------------------------------------------
# testProject project
#--------------------------------------------------------------------------

PROJECT(testProject)
FILE(GLOB testProject_headers code/*.h)
FILE(GLOB testProject_sources code/*.cc)
FILE(GLOB testProject_shaders code/*.glsl)

SET(files_testProject ${testProject_headers} ${testProject_sources} ${testProject_shaders})
SOURCE_GROUP("testProject" FILES ${files_testProject})

ADD_EXECUTABLE(testProject ${files_testProject})
TARGET_LINK_LIBRARIES(testProject core render)
ADD_DEPENDENCIES(testProject core render)