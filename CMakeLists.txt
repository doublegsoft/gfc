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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_BSD_SOURCE")

include_directories(include)

add_library(gfc STATIC
  src/gfc_base64.c
  src/gfc_crypto.c
  src/gfc_hex.c
  src/gfc_list.c
  src/gfc_log.c
  src/gfc_lru.c
  src/gfc_map.c
  src/gfc_ring.c
  src/gfc_sha1.c
  src/gfc_string.c
  src/gfc_tcp.c
  src/gfc_utf8.c
  src/gfc_uuid.c
  src/gfc_date.c
  src/gfc_btree.c
  src/gfc_mt.c
  src/gfc_ws.c
  src/gfc_gc.c
  src/gfc_fs.c
)

if (WIN32)
	link_directories(
		"/opt/local/x86_64-w64-mingw32/lib"
	)
endif (WIN32)

add_executable(test_gfc_string
	src/gfc_string.c
	src/gfc_gc.c
	test/test_gfc_string.c
)

if (WIN32)
	target_link_libraries(test_gfc_string PRIVATE
	  libiconv.dll.a
	)
endif (WIN32)

add_executable(test_gfc_map
	src/gfc_map.c
	src/gfc_string.c
	src/gfc_gc.c
	test/test_gfc_map.c
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