ADD_EXECUTABLE(headerify headerify.c)
ADD_CUSTOM_COMMAND(OUTPUT mus.libxm COMMAND libxmize ${CMAKE_CURRENT_SOURCE_DIR}/mus.xm mus.libxm DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mus.xm)
ADD_CUSTOM_COMMAND(OUTPUT mus.h COMMAND headerify mus mus.libxm DEPENDS headerify mus.libxm)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(demo demo.h mus.h main.c scene1.c scene2.c)
TARGET_LINK_LIBRARIES(demo ${DEMOS_LINK_LIBS})
