CC := gcc
CFLAGS := -Wall -g

SRC_DIR := src
OBJ_DIR := build
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

MANPAGE := docs/fv.1
MAN_INSTALL_DIR := /usr/local/man/man1/

DIRS := build

BIN_INSTALL_DIR := /usr/local/bin/fv

$(shell mkdir -p $(DIRS))

# rules for the fv binary
fv: $(OBJ_FILES)
	${CC} ${CFLAGS} ${OBJ_FILES} -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	${CC} $(CFLAGS) $(CXXFLAGS) -c $< -o $@

# install rules
install: fv ${MANPAGE}
	sudo cp fv ${BIN_INSTALL_DIR}
	sudo cp ${MANPAGE} ${MAN_INSTALL_DIR}

remove:
	sudo rm ${BIN_INSTALL_DIR}

clean:
	rm -rf ${OBJ_DIR} fv
