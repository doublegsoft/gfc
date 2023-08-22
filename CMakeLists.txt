cmake_minimum_required(VERSION 3.6)

project(gfc VERSION 1.0.0 DESCRIPTION "g foundation class")

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
  src/gfc_threads.c
  src/gfc_utf8.c
  src/gfc_uuid.c
  src/gfc_date.c
  src/gfc_btree.c
  src/gfc_gc.c
)

add_executable(test_gfc_string
  src/gfc_string.c
  test/test_gfc_string.c
)

add_executable(test_gfc_log
  src/gfc_log.c
  test/test_gfc_log.c
)

add_executable(test_gfc_gc
  src/gfc_gc.c
  test/test_gfc_gc.c
)