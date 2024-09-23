#--------------------------------------------------------------------------
# Obj_lightingProject
#--------------------------------------------------------------------------

PROJECT(Obj_lightingProject)
FILE(GLOB Obj_lightingProject_headers code/*.h)
FILE(GLOB Obj_lightingProject_sources code/*.cc)
FILE(GLOB Obj_lightingProject_shaders code/*.glsl)

SET(files_Obj_lightingProject ${Obj_lightingProject_headers} ${Obj_lightingProject_sources} ${Obj_lightingProject_shaders})
SOURCE_GROUP("Obj_lightingProject" FILES ${files_Obj_lightingProject})

ADD_EXECUTABLE(Obj_lightingProject ${files_Obj_lightingProject})
TARGET_LINK_LIBRARIES(Obj_lightingProject core render)
ADD_DEPENDENCIES(Obj_lightingProject core render)