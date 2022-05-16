CC = gcc

CFLAGS = -I/opt/local/include 											\
				 -Iinclude																	\
				 -DGFC_USE_OPENSSL 													\
				 -Wno-pointer-sign													\
				 -Wno-implicit-function-declaration					\
				 -Wno-format
 
LFLAGS = -L/opt/local/lib			\
				 -Lbin/darwin					\
				 -lcrypto -lm

SRC = ./src
				 
TESTDIR = bin/darwin

.PHONY: all

all: libgfc tests

libgfc:
	$(CC) $(CFLAGS) $(LFLAGS) -shared -o bin/mingw64/gfc.dll 				\
			$(SRC)/gfc_string.c																					\
			$(SRC)/gfc_ring.c																						\
			$(SRC)/gfc_list.c																						\
			$(SRC)/gfc_map.c																						\
			$(SRC)/gfc_lru.c																						\
			$(SRC)/gfc_hex.c																						\
			$(SRC)/gfc_uuid.c																						\
			$(SRC)/gfc_threads.c																				\
			$(SRC)/gfc_crypto.c																					\
			$(SRC)/gfc_tcp.c																						\
			$(SRC)/gfc_log.c																						\
			$(SRC)/gfc_sql.c																						\
			$(SRC)/gfc_sha1.c																						\
			$(LIBS)																											\
	-dynamiclib -undefined suppress -flat_namespace -o bin/darwin/libgfc.dylib
				 
tests:
	# 