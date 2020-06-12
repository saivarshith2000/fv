CC := gcc
CFLAGS := -Wall -g

SRC_DIR := src
OBJ_DIR := build
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

DIRS := build

$(shell mkdir -p $(DIRS))

fv: $(OBJ_FILES)
	${CC} ${CFLAGS} ${OBJ_FILES} -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	${CC} $(CFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf ${OBJ_DIR} fv
