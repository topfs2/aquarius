cmake_minimum_required(VERSION 2.6)
project(Project)
add_definitions(-std=c++11)
add_definitions(-Os)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})

pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

pkg_search_module(GLEW REQUIRED glew)
include_directories(${GLEW_INCLUDE_DIRS})

pkg_search_module(GLU REQUIRED glu)
include_directories(${GLU_INCLUDE_DIRS})

pkg_search_module(GL REQUIRED gl)
include_directories(${GL_INCLUDE_DIRS})

pkg_search_module(ENET REQUIRED libenet)
include_directories(${ENET_INCLUDE_DIRS})

pkg_search_module(OPENAL REQUIRED openal)
include_directories(${OPENAL_INCLUDE_DIRS})

pkg_search_module(FREEALUT REQUIRED freealut)
include_directories(${FREEALUT_INCLUDE_DIRS})

pkg_search_module(VORBISFILE REQUIRED vorbisfile)
include_directories(${VORBISFILE_INCLUDE_DIRS})

pkg_search_module(LUA52 REQUIRED lua5.2)
include_directories(${LUA52_INCLUDE_DIRS})

pkg_search_module(EIGEN3 REQUIRED eigen3)
include_directories(${EIGEN3_INCLUDE_DIRS})

pkg_search_module(LUABIND REQUIRED luabind)
include_directories(${LUABIND_INCLUDE_DIRS})

pkg_search_module(BOX2D REQUIRED box2d)
include_directories(${BOX2D_INCLUDE_DIRS})

FIND_PACKAGE(Threads)

include_directories ("${PROJECT_SOURCE_DIR}/entityx")
add_subdirectory (entityx)

set(LIBRARIES ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${GLEW_LIBRARIES} ${GLU_LIBRARIES} ${GL_LIBRARIES} ${BOX2D_LIBRARIES} ${ENET_LIBRARIES} ${OPENAL_LIBRARIES} ${FREEALUT_LIBRARIES} ${VORBISFILE_LIBRARIES} ${VORBISFILE_LIBRARIES} ${LUA52_LIBRARIES} ${EIGEN3_LIBRARIES} ${LUABIND_LIBRARIES} ${BOX2D_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

set(SOURCES polypartition.cpp Thread.cpp Locks.cpp LogStdOut.cpp Texture.cpp RenderProgram.cpp DebugSystems.cpp ENetNetworkSystem.cpp InputSystem.cpp PlayerSystem.cpp MoldSystem.cpp LuaSystem.cpp Box2dPhysicsSystem.cpp SDLPlatformSystem.cpp SDLOpenGLSystem.cpp VorbisFileDecoder.cpp OpenALSoundSystem.cpp)

add_executable(sandbox ${SOURCES} sandbox.cpp)
target_link_libraries(sandbox ${LIBRARIES})
target_link_libraries(sandbox entityx)
add_dependencies(sandbox entityx)

add_executable(client ${SOURCES} client.cpp)
target_link_libraries(client ${LIBRARIES})
target_link_libraries(client entityx)
add_dependencies(client entityx)

add_executable(server ${SOURCES} server.cpp)
target_link_libraries(server ${LIBRARIES})
target_link_libraries(server entityx)
add_dependencies(server entityx)
