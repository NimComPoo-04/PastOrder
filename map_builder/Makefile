CC = gcc
CFLAGS = -Wall -Wextra -ggdb -std=c11 -O0 -DDEBUG

SRC = main.c

OBJ = $(patsubst %.c, %.o, $(SRC))

ifeq ($(OS), Windows_NT)
INC = -I ../libs/raylib-5.5_win64_mingw-w64/include
LIB = -L ../libs/raylib-5.5_win64_mingw-w64/lib -l:libraylib.a -lgdi32 -lopengl32 -lwinmm
RM = del
else
INC = -I ../libs/raylib-5.5_linux_amd64/include 
LIB = -L ../libs/raylib-5.5_linux_amd64/lib -l:libraylib.a -lm
RM = rm -rf
endif

define depend
$(shell $(CC) -MM $(T))
	$(CC) $(CFLAGS) $(INC) -c -o $$@ $$<

endef

all: builder.exe

builder.exe: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

$(foreach T, $(SRC), $(eval $(call depend $(T))))

clean:
	$(RM) *.o *.exe
