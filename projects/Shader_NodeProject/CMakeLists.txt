#--------------------------------------------------------------------------
# Shader_NodeProjectproject
#--------------------------------------------------------------------------

PROJECT(Shader_NodeProject)
FILE(GLOB Shader_NodeProject_headers code/*.h)
FILE(GLOB Shader_NodeProject_sources code/*.cc)
FILE(GLOB Shader_NodeProject_shaders code/*.glsl)

SET(files_Shader_NodeProject ${Shader_NodeProject_headers} ${Shader_NodeProject_sources} ${Shader_NodeProject_shaders})
SOURCE_GROUP("Shader_NodeProject" FILES ${files_Shader_NodeProject})

ADD_EXECUTABLE(Shader_NodeProject ${files_Shader_NodeProject})
TARGET_LINK_LIBRARIES(Shader_NodeProject core render)
ADD_DEPENDENCIES(Shader_NodeProject core render)