# Variables
BIN := bin

UTILS := utils
UTILS_C := $(wildcard $(UTILS)/*.c)
UTILS_OBJ := $(patsubst $(UTILS)/%.c, $(BIN)/$(UTILS)/%.o, $(UTILS_C))

SERVER := server
SERVER_C := $(wildcard $(SERVER)/*.c)
SERVER_SRC := $(SERVER_C) $(UTILS_C)
SERVER_OBJ := $(patsubst $(SERVER)/%.c, $(BIN)/$(SERVER)/%.o, $(SERVER_C)) $(UTILS_OBJ)
SERVER_EXE := $(BIN)/$(SERVER).exe

CLIENT := client
CLIENT_C := $(wildcard $(CLIENT)/*.c)
CLIENT_SRC := $(CLIENT_C) $(UTILS_C)
CLIENT_OBJ := $(patsubst $(CLIENT)/%.c, $(BIN)/$(CLIENT)/%.o, $(CLIENT_C)) $(UTILS_OBJ)
CLIENT_EXE := $(BIN)/$(CLIENT).exe

CC := gcc
CFLAGS := -Wall -Wextra -Werror

# Phony Targets
.PHONY: all clean $(SERVER) $(CLIENT)

# Default target: Build both server and client
all: $(SERVER) $(CLIENT)
	@echo "all built successfully!"

# Build server
$(SERVER): $(SERVER_OBJ) $(UTILS_OBJ)
	@mkdir -p $(dir $(BIN)/$(SERVER))
	$(CC) $(CFLAGS) -o $(SERVER_EXE) $^
	@chmod +x $(SERVER_EXE)
	@echo "$(SERVER) built successfully!"

# Build client
$(CLIENT): $(CLIENT_OBJ)  $(UTILS_OBJ)
	@mkdir -p $(dir $(BIN)/$(CLIENT))
	$(CC) $(CFLAGS) -o $(CLIENT_EXE) $^
	@chmod +x $(CLIENT_EXE)
	@echo "$(CLIENT) built successfully!"

# Generic rule to compile .c files to .o files
$(BIN)/$(UTILS)/%.o: $(UTILS)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/$(SERVER)/%.o: $(SERVER)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/$(CLIENT)/%.o: $(CLIENT)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -r $(BIN)
	@echo "Cleaned up all files."
