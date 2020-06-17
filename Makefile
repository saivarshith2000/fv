CC := cc
CFLAGS := -std=c99 -Wall -Werror -pedantic -O2
# Uncomment the below line to enable debug flags
CFLAGS += -g3

SRC_DIR := src
OBJ_DIR := build
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

MANPAGE := docs/fv.1
MAN_INS_PATH := /usr/local/man/man1/fv.1

DIRS := build

FV_INS_PATH := /usr/local/bin/fv

$(shell mkdir -p $(DIRS))

# rules for the fv binary
fv: $(OBJ_FILES)
	${CC} ${CFLAGS} ${OBJ_FILES} -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	${CC} $(CFLAGS) $(CXXFLAGS) -c $< -o $@

# install rules
install: fv ${MANPAGE}
	sudo cp fv ${FV_INS_PATH}
	sudo cp ${MANPAGE} ${MAN_INS_PATH}

remove:
	sudo rm ${FV_INS_PATH} ${MAN_INS_PATH}

clean:
	rm -rf ${OBJ_DIR} fv
