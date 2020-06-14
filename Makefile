CC := gcc
CFLAGS := -Wall -g

SRC_DIR := src
OBJ_DIR := build
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

DIRS := build

INSTALL_LOC := /usr/local/bin/fv

$(shell mkdir -p $(DIRS))

# rules for the fv binary
fv: $(OBJ_FILES)
	${CC} ${CFLAGS} ${OBJ_FILES} -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	${CC} $(CFLAGS) $(CXXFLAGS) -c $< -o $@

# install rules
install: fv
	sudo cp fv ${INSTALL_LOC}

remove:
	sudo rm ${INSTALL_LOC}

clean:
	rm -rf ${OBJ_DIR} fv
