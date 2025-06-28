# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Output executable
OUTPUT = Steganography_CLI_Tool

# Source file
SRC = Steganography_CLI_Tool.c

# Rule to build the program
all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUTPUT)

# Rule to clean the compiled files
clean:
	del /F /Q $(OUTPUT).exe 2>nul

# Rule to run the program after compilation
run: $(OUTPUT)
	./$(OUTPUT).exe

# Phony OUTPUTs
.PHONY: all clean run
