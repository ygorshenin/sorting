SRC_DIR = src
SRC_SUBDIRS = . base generators sorters
SRC_DIRS = $(addprefix $(SRC_DIR)/, $(SRC_SUBDIRS))
SRCS = $(wildcard $(addsuffix /*.cc, $(SRC_DIRS)))

HDRS = $(wildcard $(addsuffix /*.h, $(SRC_DIRS)))

OBJ_DIR = obj
OBJ_DIRS = $(addprefix $(OBJ_DIR)/, $(SRC_SUBDIRS))
OBJS := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(SRCS))
OBJS := $(patsubst %.cc, %.o, $(OBJS))

BIN_DIR = bin

PROGRAM = $(BIN_DIR)/tester

CPP = g++
CPPFLAGS = -O2 -Wall -I$(SRC_DIR)
LDFLAGS = -lgflags

all: bin_dir obj_dir $(PROGRAM)

bin_dir:
	mkdir -p $(BIN_DIR)

obj_dir:
	mkdir -p $(OBJ_DIRS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc $(HDRS)
	$(CPP) $(CPPFLAGS) -c -o $@ $<

$(PROGRAM): $(OBJS)
	$(CPP) $(CPPFLAGS) $(LDFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
