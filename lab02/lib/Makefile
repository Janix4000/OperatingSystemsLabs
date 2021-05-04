CC = gcc
CFLAGS = -Wall -O2 -g

ARFLAGS = rcs

all: clean libunifile.a libunifile.so

static: clean libunifile.a
shared: clean libunifile.so

# libunifile.so:
# 	$(CC) $(CFLAGS) lib_uni_file.c -fPIC -shared -o libunifile.so

libunifile.a: libreader.o libunifile.o 
	$(AR) $(ARFLAGS) libunifile.a lib_reader_static.o lib_uni_file_static.o 

libunifile.o:
	$(CC) $(CFLAGS) -c lib_uni_file.c -o lib_uni_file_static.o 

libreader.o:
	$(CC) $(CFLAGS) -c lib_reader.c -o lib_reader_static.o 

# lib_vector_static.o:
# 	$(CC) $(CFLAGS) -c lib_vector.c -o lib_vector_static.o 

clean:
	$(RM) -f *.o *.a *.so