
CC = g++
SRC = main.cpp
PROGRAM = main

ifeq ($(OS),Windows_NT)
    # Windows-specific libraries
    LIBS = -lraylib -lwinmm -lgdi32 -lopengl32 -L/usr/local/include
    EXE_SUFFIX = .exe
    RM = del /Q
else
    # Linux-specific libraries
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -I/usr/local/include -L/usr/local/lib 
    EXE_SUFFIX =
    RM = rm -f
endif

all: $(PROGRAM)$(EXE_SUFFIX)

$(PROGRAM)$(EXE_SUFFIX): $(SRC)
	$(CC) $(SRC) $(LIBS) -o $(PROGRAM)$(EXE_SUFFIX)
	@echo "Build successful!"

clean:
	@$(RM) $(PROGRAM)$(EXE_SUFFIX)
	@echo "Cleaned up!"

# this will build AND run the program...
run: $(PROGRAM)$(EXE_SUFFIX)
	@echo "Running $(PROGRAM)$(EXE_SUFFIX)..."
	./$(PROGRAM)$(EXE_SUFFIX)
