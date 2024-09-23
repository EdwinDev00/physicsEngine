#--------------------------------------------------------------------------
# myProject project
#--------------------------------------------------------------------------

PROJECT(myProject)
FILE(GLOB myProject_headers code/*.h)
FILE(GLOB myProject_sources code/*.cc)
FILE(GLOB myProject_shaderFile code/*.glsl)

SET(files_myProject ${myProject_headers} ${myProject_sources} ${myProject_shaderFile})
SOURCE_GROUP("myProject" FILES ${files_myProject})

ADD_EXECUTABLE(myProject ${files_myProject})
TARGET_LINK_LIBRARIES(myProject core render)
ADD_DEPENDENCIES(myProject core render)