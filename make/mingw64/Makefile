CC = x86_64-w64-mingw32-gcc

CFLAGS = -g -O2 																	\
				 -Wno-int-to-pointer-cast									\
				 -DGFC_USE_OPENSSL 												\
				 -DNO_OPEN_MEMSTREAM											\
				 -I/opt/local/x86_64-w64-mingw32/include	\
				 -Iinclude
 
LFLAGS = \
				 -L/opt/local/x86_64-w64-mingw32/lib
				 
				 
LIBS = -liconv -lrpcrt4 -lws2_32 -lcrypto -ljson-c
				 
SRC = ./src

.PHONY: all

all: libgfc

libgfc:
#	$(CC) $(CFLAGS) -c src/gfc_string.c -o obj/gfc_string.o
#	$(CC) $(CFLAGS) -c src/gfc_ring.c -o obj/gfc_ring.o
#	$(CC) $(CFLAGS) -c src/gfc_list.c -o obj/gfc_list.o
#	$(CC) $(CFLAGS) -c src/gfc_map.c -o obj/gfc_map.o
#	$(CC) $(CFLAGS) -c src/gfc_lru.c -o obj/gfc_lru.o
#	$(CC) $(CFLAGS) -c src/gfc_hex.c -o obj/gfc_hex.o
#	$(CC) $(CFLAGS) -c src/gfc_threads.c -o obj/gfc_threads.o
#	$(CC) $(CFLAGS) -c src/gfc_crypto.c -o obj/gfc_crypto.o
#	$(CC) $(CFLAGS) -c src/gfc_tcp.c -o obj/gfc_tcp.o
#	$(CC) $(CFLAGS) -c src/gfc_log.c -o obj/gfc_log.o
#	$(CC) $(CFLAGS) -c src/gfc_sql.c -o obj/gfc_sql.o
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
			$(SRC)/gfc_utf8.c																						\
			$(LIBS)																											\
	-Wl,--output-def,bin/mingw64/libgfc.def,--out-implib,bin/mingw64/libgfc.dll.a