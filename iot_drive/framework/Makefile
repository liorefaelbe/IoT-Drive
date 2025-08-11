#targets:
TARGETNAME = $(shell basename -a $(wildcard ./src/*.cpp))
TESTNAME = $(basename $(TARGETNAME) .cpp)
#gcc:
CC = g++ 
ARM_CC = arm-linux-gnueabihf-g++
#gcc flags:
CFLAGS = -pedantic-errors -Wall -Wextra -fPIC
INCLUDE = -I ./inc -I /home/lior/git/cpp/inc
LDFLAGS = -lframework -L $(DBG_DIR)/. -Wl,-rpath=$(DBG_DIR)/. -lm
FW_LIB = -lframework -L $(REL_DIR)/. -Wl,-rpath=$(REL_DIR)/. -lm
DEBUG = -g
REL = -DNDEBUG -O3
#compiler:
GD := $(CC) $(CFLAGS) $(DEBUG) $(INCLUDE)
GC := $(CC) $(CFLAGS) $(REL) $(INCLUDE)
GR := $(ARM_CC) $(CFLAGS) $(REL) $(INCLUDE)
#dirs:
SRC_DIR = src
BIN_DIR = bin
DBG_DIR = bin/debug
DBG_OBJ_DIR = bin/debug/obj
REL_DIR = bin/release
REL_OBJ_DIR = bin/release/obj
TEST_DIR = bin/test
ARM_OBJ_DIR = bin/release/obj_arm
#files:
SOURCES =  $(wildcard $(SRC_DIR)/*.cpp)
DEBUG_OBJ = $(subst $(SRC_DIR),$(DBG_OBJ_DIR),$(SOURCES:.cpp=.o))
REL_OBJ = $(subst $(SRC_DIR),$(REL_OBJ_DIR),$(SOURCES:.cpp=.o))
ARM_OBJ = $(subst $(SRC_DIR),$(ARM_OBJ_DIR),$(SOURCES:.cpp=.o))
#library
LIB = libframework.so
LIB_ARM = 

.PHONY: debug release test framework framework_arm_static clean all

all: debug release framework framework_arm_static

#so:
debug: $(DEBUG_OBJ)
	$(GD) -shared -o $(DBG_DIR)/$(LIB) $?

release: $(REL_OBJ)
	$(GC) -shared -o $(REL_DIR)/$(LIB) $?

framework_arm_static: $(ARM_OBJ)
	ar rcs -o /home/lior/git/projects/final_project/concrete/libframework_arm.a $?
	
#exe:
framework:
	$(GD) ./test/framework_test.cpp -o ./framework.out $(LDFLAGS)

test :
	$(foreach d, $(TESTNAME), $(GD) ./test/$d_test.cpp -o ./$d.out \
							    $(LDFLAGS);)

#obj:
$(REL_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(REL_OBJ_DIR)
	$(GC) -c -o $@ $<

$(DBG_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(DBG_OBJ_DIR)
	$(GD) -c -o $@ $<

$(ARM_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(ARM_OBJ_DIR)
	$(GR) -c -o $@ $<

clean: 
	rm -rf *.out ./$(DBG_OBJ_DIR) ./$(REL_OBJ_DIR) ./$(ARM_OBJ_DIR)

