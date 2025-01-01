CC = gcc
CFLAGS = -Wall -Wextra -ggdb -std=c11 -O0 -DDEBUG

SRC = main.c map.c draw_ext.c entity.c assets.c

OBJ = $(patsubst %.c, %.o, $(SRC))

ifeq ($(OS), Windows_NT)
INC = -I libs/raylib-5.5_win64_mingw-w64/include
LIB = -L libs/raylib-5.5_win64_mingw-w64/lib -l:libraylib.a -lgdi32 -lopengl32 -lwinmm
RM = del
else
INC = -I libs/raylib-5.5_linux_amd64/include 
LIB = -L libs/raylib-5.5_linux_amd64/lib -l:libraylib.a -lm
RM = rm -rf
endif

define depend
$(shell $(CC) -MM $(T))
	$(CC) $(CFLAGS) $(INC) -c -o $$@ $$<

endef

all: game.exe

game.exe: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

$(foreach T, $(SRC), $(eval $(call depend $(T))))

ifeq ($(OS), Windows_NT)
getdeps:
	mkdir libs && cd libs &&\
	       	curl -L -O https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_win64_mingw-w64.zip &&\
	       	tar xf raylib-5.5_win64_mingw-w64.zip &&\
	       	del /S /Q raylib-5.5_win64_mingw-w64.zip
else
getdeps:
	mkdir -p libs;\
		cd libs;\
		curl -L -O https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_linux_amd64.tar.gz;\
		tar xvf raylib-5.5_linux_amd64.tar.gz;\
		rm -rf raylib-5.5_linux_amd64.tar.gz
endif

map-builder:
	make -C map_builder

clean:
	$(RM) *.o *.exe
