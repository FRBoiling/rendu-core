#|--------------------------------
#| RD Application Project
#|--------------------------------
cmake_minimum_required(VERSION 3.14)

set(RENDU_APP example_plugins)
#set(LIB_LUA_DIR ${RENDU_PROJECT_DIR}/Dependencies/lua)

project(${RENDU_APP})

file(GLOB_RECURSE RENDU_APP_SOURCE *.cpp *.cc *.c)
file(GLOB_RECURSE RENDU_APP_HEADERS *.h *.hpp *.ipp)

#file(GLOB_RECURSE LIB_LUA_SOURCE ${LIB_LUA_DIR}/*.cpp ${LIB_LUA_DIR}/*.cc ${LIB_LUA_DIR}/*.c)
#file(GLOB_RECURSE LIB_LUA_HEADERS ${LIB_LUA_DIR}/*.h ${LIB_LUA_DIR}/*.hpp ${LIB_LUA_DIR}/*.ipp)

#list(REMOVE_ITEM LIB_LUA_SOURCE ${LIB_LUA_DIR}/lua.c ${LIB_LUA_DIR}/luac.c)

add_executable(${RENDU_APP} ${RENDU_APP_SOURCE} ${RENDU_APP_HEADERS})
#target_sources(${RENDU_APP} PRIVATE ${LIB_LUA_SOURCE} ${LIB_LUA_HEADERS})
#include_directories(${LIB_LUA_DIR})

source_group("Source" FILES ${RENDU_APP_SOURCE})
source_group("Headers" FILES ${RENDU_APP_HEADERS})

#source_group("Lua" FILES ${LIB_LUA_SOURCE})
#source_group("Lua" FILES ${LIB_LUA_HEADERS})
set_target_properties(${RENDU_APP} PROPERTIES LINKER_LANGUAGE CXX)
if(WIN32)
  set_target_properties(${RENDU_APP} PROPERTIES LINK_FLAGS "/DEBUG /PDBSTRIPPED:${RENDU_APP}.pdb")
endif()
target_link_libraries(${RENDU_APP} rendu::rendu)
