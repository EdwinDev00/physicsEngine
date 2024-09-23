#--------------------------------------------------------------------------
# GraphicV2
#--------------------------------------------------------------------------

PROJECT(GraphicV2)
FILE(GLOB GraphicV2_headers code/*.h)
FILE(GLOB GraphicV2_sources code/*.cc)
FILE(GLOB GraphicV2_shaders code/*.glsl)

SET(files_GraphicV2 ${GraphicV2_headers} ${GraphicV2_sources} ${GraphicV2_shaders})
SOURCE_GROUP("GraphicV2" FILES ${files_GraphicV2})

ADD_EXECUTABLE(GraphicV2 ${files_GraphicV2})
TARGET_LINK_LIBRARIES(GraphicV2 core render)
ADD_DEPENDENCIES(GraphicV2 core render)

if (MSVC)
    set_property(TARGET GraphicV2 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()