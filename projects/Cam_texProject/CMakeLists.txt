#--------------------------------------------------------------------------
# Cam_texProject project
#--------------------------------------------------------------------------

PROJECT(Cam_texProject)
FILE(GLOB Cam_texProject_headers code/*.h)
FILE(GLOB Cam_texProject_sources code/*.cc)
FILE(GLOB Cam_texProject_shaders code/*.glsl)

SET(files_Cam_texProject ${Cam_texProject_headers} ${Cam_texProject_sources} ${Cam_texProject_shaders})
SOURCE_GROUP("Cam_texProject" FILES ${files_Cam_texProject})

ADD_EXECUTABLE(Cam_texProject ${files_Cam_texProject})
TARGET_LINK_LIBRARIES(Cam_texProject core render)
ADD_DEPENDENCIES(Cam_texProject core render)