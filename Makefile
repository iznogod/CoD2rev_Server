# Compiler options.
CC = gcc

CFLAGS=-m32
LFLAGS=-m32 -no-pie -ldl

LLIBS=-lm -lpthread -lstdc++

# Setup binary names.
BIN_NAME=cod2rev_lnxded
LIB_NAME=libcod2rev
BIN_EXT=
LIB_EXT=.so

# Setup directory names.
BIN_DIR=bin
OBJ_DIR=obj
SRC_DIR=src

LINUX_DIR=$(SRC_DIR)/unix
ZLIB_DIR=$(SRC_DIR)/zlib

# Source dirs
BGAME_DIR=$(SRC_DIR)/bgame
CLIENTSCR_DIR=$(SRC_DIR)/clientscript
GAME_DIR=$(SRC_DIR)/game
QCOMMON_DIR=$(SRC_DIR)/qcommon
SERVER_DIR=$(SRC_DIR)/server
STRINGED_DIR=$(SRC_DIR)/stringed
UNIVERSAL_DIR=$(SRC_DIR)/universal
XANIM_DIR=$(SRC_DIR)/xanim

# Target files
TARGET=$(addprefix $(BIN_DIR)/,$(BIN_NAME)$(BIN_EXT))

# C files
BGAME_SOURCES=$(wildcard $(BGAME_DIR)/*.cpp)
CLIENTSCR_SOURCES=$(wildcard $(CLIENTSCR_DIR)/*.cpp)
GAME_SOURCES=$(wildcard $(GAME_DIR)/*.cpp)
QCOMMON_SOURCES=$(wildcard $(QCOMMON_DIR)/*.cpp)
SERVER_SOURCES=$(wildcard $(SERVER_DIR)/*.cpp)
STRINGED_SOURCES=$(wildcard $(STRINGED_DIR)/*.cpp)
UNIVERSAL_SOURCES=$(wildcard $(UNIVERSAL_DIR)/*.cpp)
XANIM_SOURCES=$(wildcard $(XANIM_DIR)/*.cpp)
LINUX_SOURCES=$(wildcard $(LINUX_DIR)/*.cpp)
ZLIB_SOURCES=$(wildcard $(ZLIB_DIR)/*.c)

# Object files.
BGAME_OBJ=$(patsubst $(BGAME_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(BGAME_SOURCES))
CLIENTSCR_OBJ=$(patsubst $(CLIENTSCR_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CLIENTSCR_SOURCES))
GAME_OBJ=$(patsubst $(GAME_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(GAME_SOURCES))
QCOMMON_OBJ=$(patsubst $(QCOMMON_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(QCOMMON_SOURCES))
SERVER_OBJ=$(patsubst $(SERVER_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SERVER_SOURCES))
STRINGED_OBJ=$(patsubst $(STRINGED_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(STRINGED_SOURCES))
UNIVERSAL_OBJ=$(patsubst $(UNIVERSAL_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(UNIVERSAL_SOURCES))
XANIM_OBJ=$(patsubst $(XANIM_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(XANIM_SOURCES))

# Platform specific lists
LINUX_OBJ=$(patsubst $(LINUX_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(LINUX_SOURCES))
ZLIB_OBJ=$(patsubst $(ZLIB_DIR)/%.c,$(OBJ_DIR)/%.o,$(ZLIB_SOURCES))


# Default rule.
cod2rev: mkdir $(TARGET)
    $(TARGET): \
	$(BGAME_OBJ) $(CLIENTSCR_OBJ) $(GAME_OBJ) $(QCOMMON_OBJ) $(SERVER_OBJ) $(STRINGED_OBJ) \
	$(UNIVERSAL_OBJ) $(XANIM_OBJ) $(LINUX_OBJ) $(ZLIB_OBJ)
	$(CC) $(LFLAGS) -o $@ $^ $(LLIBS)

mkdir:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)

# Build C sources

# A rule to build bgame source code.
$(OBJ_DIR)/%.o: $(BGAME_DIR)/%.cpp
	@echo $(CC)  $@
	@$(CC) -c $(CFLAGS) -o $@ $<

# A rule to build clientscript source code.
$(OBJ_DIR)/%.o: $(CLIENTSCR_DIR)/%.cpp
	@echo $(CC)  $@
	@$(CC) -c $(CFLAGS) -o $@ $<

# A rule to build game source code.
$(OBJ_DIR)/%.o: $(GAME_DIR)/%.cpp
	@echo $(CC)  $@
	@$(CC) -c $(CFLAGS) -o $@ $<

# A rule to build qcommon source code.
$(OBJ_DIR)/%.o: $(QCOMMON_DIR)/%.cpp
	@echo $(CC)  $@
	@$(CC) -c $(CFLAGS) -o $@ $<

# A rule to build server source code.
$(OBJ_DIR)/%.o: $(SERVER_DIR)/%.cpp
	@echo $(CC)  $@
	@$(CC) -c $(CFLAGS) -o $@ $<

# A rule to build stringed source code.
$(OBJ_DIR)/%.o: $(STRINGED_DIR)/%.cpp
	@echo $(CC)  $@
	@$(CC) -c $(CFLAGS) -o $@ $<

# A rule to build universal source code.
$(OBJ_DIR)/%.o: $(UNIVERSAL_DIR)/%.cpp
	@echo $(CC)  $@
	@$(CC) -c $(CFLAGS) -o $@ $<

# A rule to build xanim source code.
$(OBJ_DIR)/%.o: $(XANIM_DIR)/%.cpp
	@echo $(CC)  $@
	@$(CC) -c $(CFLAGS) -o $@ $<

# A rule to build linux source code.
$(OBJ_DIR)/%.o: $(LINUX_DIR)/%.cpp
	@echo $(CC)  $@
	@$(CC) -c $(CFLAGS) -o $@ $<

# A rule to build zlib source code.
$(OBJ_DIR)/%.o: $(ZLIB_DIR)/%.c
	@echo $(CC)  $@
	@$(CC) -c $(CFLAGS) -o $@ $<

# Cleanup


clean:
	rm -f $(BIN_DIR)/$(BIN_NAME)$(BIN_EXT)
	rm -f $(BIN_DIR)/$(LIB_NAME)$(LIB_EXT)
	rm -f $(OBJ_DIR)/*.o