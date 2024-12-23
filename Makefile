CC = gcc
CFLAGS = -Wall -Wextra -ggdb -std=c11 -O0

SRC = main.c
OBJ = $(patsubst %.c, %.o, $(SRC))

ifeq ($(OS), Window_NT)
INC = -I libs/raylib-5.5_win64_mingw-w64/include
LIB = -L libs/raylib-5.5_win64_mingw-w64/lib -l:libraylib.a
RM = del
else
INC = -I libs/raylib-5.5_linux_amd64/include 
LIB = -L libs/raylib-5.5_linux_amd64/lib -l:libraylib.a -lm
RM = rm -rf
endif

define depend
$(shell echo `$(CC) -MM $(T)`)
	$(CC) $(CFLAGS) $(INC) -c -o $$@ $$<

endef

all: game.exe

game.exe: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

$(foreach T,$(SRC), $(eval $(call depend $(T))))

clean:
	$(RM) *.o *.exe
