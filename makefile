EX=final
all: $(EX)

# Main target
all: $(EXE)

#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW -std=c99 -pedantic -Wextra -lm
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
CLEAN=del *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
RES=$(shell uname -r|sed -E 's/(.).*/\1/'|tr 12 21)
CFLG=-O3 -Wall -Wno-deprecated-declarations -DRES=$(RES)
LIBS=-framework GLUT -framework OpenGL
CLEAN=rm -f $(EX) *.o *.a
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
CLEAN=rm -f $(EX) *.o *.a
endif
endif

#  Generic compile and link
%:%.c
	gcc $(CFLG) -o $@ $^ $(LIBS)

#  Clean
clean:
	$(CLEAN)
