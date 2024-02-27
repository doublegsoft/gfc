##
##           .d888
##          d88P"
##          888
##  .d88b.  888888 .d8888b
## d88P"88b 888   d88P"
## 888  888 888   888
## Y88b 888 888   Y88b.
##  "Y88888 888    "Y8888P
##      888
## Y8b d88P
##  "Y88P"
##
## Copyright (C) 2023 doublegsoft.open
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

cmake_minimum_required(VERSION 3.6)

project(gfc VERSION 1.0.0 DESCRIPTION "g foundation class")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_BSD_SOURCE -Wno-format -Wno-deprecated-declarations")

set(LIB_LUAJIT		"libluajit")
set(LUAJIT				"LuaJIT-2.1")
set(LIB_LUA				"liblua")
set(LUA						"lua-5.4.6")

option(WITH_GFC_JVM 			"gfc jvm" ON)
option(WITH_GFC_LUA 			"gfc lua" ON)
option(WITH_GFC_CRYPTO 		"gfc crypto" ON)

set(GFC_SOURCES 
	src/gfc_base64.c
  src/gfc_hex.c
  src/gfc_list.c
  src/gfc_log.c
  src/gfc_lru.c
  src/gfc_map.c
  src/gfc_ring.c
  src/gfc_sha1.c
  src/gfc_string.c
  src/gfc_bignum.c
  src/gfc_tcp.c
  src/gfc_utf8.c
  src/gfc_uuid.c
  src/gfc_date.c
  src/gfc_btree.c
  src/gfc_mt.c
  src/gfc_ws.c
  src/gfc_gc.c
  src/gfc_fs.c
  src/gfc_cli.c
  src/gfc_id.c
)

####################
##
## WITH OPTIONS
##
####################
if(WITH_GFC_JVM)
	if (APPLE)
		list(APPEND GFC_SOURCES src/gfc_jvm.c)
	endif (APPLE)
endif(WITH_GFC_JVM)

if(WITH_GFC_CRYPTO) 
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGFC_USE_OPENSSL")
	list(APPEND GFC_SOURCES src/gfc_crypto.c)
endif(WITH_GFC_CRYPTO)

if(WITH_GFC_LUA)
	list(APPEND GFC_SOURCES src/gfc_lua.c)
endif(WITH_GFC_LUA)

####################
##
## CUSTOM BUILD
##
####################
if(WITH_GFC_LUA)
	add_custom_command(OUTPUT ${LIB_LUAJIT}_OUT
	  COMMAND mkdir -p ${CMAKE_BINARY_DIR}/3rd/${LUAJIT}/
	  COMMAND cp -rf ${CMAKE_SOURCE_DIR}/3rd/${LUAJIT}/ ${CMAKE_BINARY_DIR}/3rd/${LUAJIT}
	  COMMAND export MACOSX_DEPLOYMENT_TARGET=10.16 && make -C ${CMAKE_BINARY_DIR}/3rd/${LUAJIT} 
	)
	add_custom_target(${LIB_LUAJIT} ALL
	  DEPENDS ${LIB_LUAJIT}_OUT
	)
	
	add_custom_command(OUTPUT ${LIB_LUA}_OUT
	  COMMAND mkdir -p ${CMAKE_BINARY_DIR}/3rd/${LUA}/
	  COMMAND cp -rf ${CMAKE_SOURCE_DIR}/3rd/${LUA}/ ${CMAKE_BINARY_DIR}/3rd/${LUA}
	  COMMAND make -C ${CMAKE_BINARY_DIR}/3rd/${LUA} 
	)
	add_custom_target(${LIB_LUA} ALL
	  DEPENDS ${LIB_LUA}_OUT
	)
endif(WITH_GFC_LUA)

####################
##
## LINK
##
####################
if(UNIX)
  link_directories(
	  /opt/local/lib
	)
endif(UNIX)

####################
##
## INCLUDE
##
####################
if (WIN32)
	include_directories(
		"include"
	)
else ()  
	include_directories(
		"/usr/local/include"
		"/opt/local/include"
		"include"
	)
	if(WITH_GFC_JVM)	
		include_directories(
			"/Library/Java/JavaVirtualMachines/jdk1.8.0_212.jdk/Contents/Home/include"
			"/Library/Java/JavaVirtualMachines/jdk1.8.0_212.jdk/Contents/Home/include/darwin"
		)
	endif()
	if(WITH_GFC_LUA)	
		include_directories(
			"3rd/${LUA}"
			"3rd/${LUAJIT}"
		)
	endif()
endif(WIN32)

add_library(gfc STATIC ${GFC_SOURCES})
add_library(gfc_shared SHARED ${GFC_SOURCES})
set_target_properties(gfc_shared PROPERTIES OUTPUT_NAME gfc)

if (WIN32)
	link_directories(
		"/opt/local/x86_64-w64-mingw32/lib"
	)
	target_link_libraries(gfc PRIVATE
	  iconv
	  crypto
	  ws2_32
	  Rpcrt4
	)
	target_link_libraries(gfc_shared PRIVATE
	  iconv
	  crypto
	  ws2_32
	  Rpcrt4
	)
else()
	if(WITH_GFC_JVM)	
		target_link_libraries(gfc PRIVATE
		  "/Library/Java/JavaVirtualMachines/jdk1.8.0_212.jdk/Contents/Home/jre/lib/jli/libjli.dylib"
		  "/Library/Java/JavaVirtualMachines/jdk1.8.0_212.jdk/Contents/Home/jre/lib/server/libjvm.dylib"
		)	
		target_link_libraries(gfc_shared PRIVATE
		  "/Library/Java/JavaVirtualMachines/jdk1.8.0_212.jdk/Contents/Home/jre/lib/jli/libjli.dylib"
		  "/Library/Java/JavaVirtualMachines/jdk1.8.0_212.jdk/Contents/Home/jre/lib/server/libjvm.dylib"
		)	
	endif(WITH_GFC_JVM)
	if(WITH_GFC_CRYPTO)	
		target_link_libraries(gfc PRIVATE 
			crypto
		)
		target_link_libraries(gfc_shared PRIVATE 
			crypto
		)
	endif(WITH_GFC_CRYPTO)
endif(WIN32)

add_executable(test_gfc_string
	src/gfc_string.c
	src/gfc_gc.c
	test/test_gfc_string.c
)

target_link_libraries(test_gfc_string PRIVATE
	gfc
)	

if(WIN32)
	target_link_libraries(test_gfc_string PRIVATE
	  libiconv.dll.a
	)
endif(WIN32)

add_executable(test_gfc_map
	src/gfc_map.c
	src/gfc_string.c
	src/gfc_gc.c
	test/test_gfc_map.c
)

target_link_libraries(test_gfc_map PRIVATE
	gfc
)	
	
if (WIN32)
	target_link_libraries(test_gfc_map PRIVATE
	  libiconv.dll.a
	)
endif (WIN32)

add_executable(test_gfc_date
  src/gfc_date.c
  test/test_gfc_date.c
)

add_executable(test_gfc_log
  src/gfc_log.c
  test/test_gfc_log.c
)

add_executable(test_gfc_gc
  src/gfc_gc.c
  test/test_gfc_gc.c
)

add_executable(test_gfc_mt
  src/gfc_mt.c
  src/gfc_gc.c
  test/test_gfc_mt.c
)

target_link_libraries(test_gfc_mt PRIVATE
  pthread
)

add_executable(test_gfc_lru
  src/gfc_lru.c
  src/gfc_gc.c
  test/test_gfc_lru.c
)

add_executable(test_gfc_list
  src/gfc_list.c
  src/gfc_gc.c
  test/test_gfc_list.c
)

add_executable(test_gfc_btree
  src/gfc_btree.c
  src/gfc_gc.c
  test/test_gfc_btree.c
)

add_executable(test_gfc_ws
  src/gfc_ws.c
  src/gfc_gc.c
  test/test_gfc_ws.c
)

if (WIN32)
	target_link_libraries(test_gfc_ws PRIVATE
	  pthread
	  wsock32
	)
else()
	target_link_libraries(test_gfc_ws PRIVATE
	  pthread
	)
endif (WIN32)

add_executable(test_gfc_fs
	src/gfc_fs.c
  test/test_gfc_fs.c
)

target_link_libraries(test_gfc_fs PRIVATE
	gfc
)	

if(WITH_GFC_CRYPTO)
add_executable(test_gfc_crypto
  src/gfc_crypto.c
  src/gfc_hex.c
  test/test_gfc_crypto.c
)

if (WIN32)
	target_link_libraries(test_gfc_crypto PRIVATE
	  libcrypto.dll.a
	)
else (UNIX)
	
	target_link_libraries(test_gfc_crypto PRIVATE
	  crypto
	)
endif (WIN32)
endif(WITH_GFC_CRYPTO)

if(WITH_GFC_JVM)	
	if(APPLE)
add_executable(test_gfc_jvm
  test/test_gfc_jvm.c
)
target_link_libraries(test_gfc_jvm PRIVATE
	gfc
)
	endif (APPLE)
endif(WITH_GFC_JVM)

add_executable(test_gfc_cli
  test/test_gfc_cli.c
)

target_link_libraries(test_gfc_cli PRIVATE
	gfc
)	

add_executable(test_gfc_id
  test/test_gfc_id.c
)

target_link_libraries(test_gfc_id PRIVATE
	gfc
)	
