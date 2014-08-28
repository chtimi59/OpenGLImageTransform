CC=gcc
CPP=g++
FLAGS=-Wall -ansi -pedantic -DLOGENABLE -Wno-unused-function $(INC)
CFLAGS=-std=c99 
CPPFLAGS=-std=c++0x -Wno-write-strings $(INC)

OPENGL_INCLUDE=-Iglew/include -Iglm

LDFLAGS=-g -O2 -Wall -W $(LD_LIB)
OPENGLLIB=-Lglew/lib/Release/Win32 -lglew32s -lopengl32 -lgdi32
STDCPPLIB=-lstdc++


.PHONY: clean all

all: main test

test:
	./main.exe
	
main: log.o gldebug.o w32.o glgrid.o xbuf.o main.o
	$(CC) -o $@ $^  $(LIBS) $(LDFLAGS) $(OPENGLLIB) $(STDCPPLIB) 

# basics rules	
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(OPENGL_INCLUDE)
	
%.o: %.cpp
	$(CPP) -o $@ -c $< $(CPPFLAGS) $(OPENGL_INCLUDE)
	
# Clean UP 
clean:
	@rm -f *.o 
	@rm -f *.exe
	



