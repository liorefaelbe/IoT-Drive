#targets:
TARGETNAME = $(shell basename -a $(wildcard ./src/*.cpp))
TESTNAME = $(basename $(TARGETNAME) .cpp)
#gcc:
CC = g++ 
ARM_CC = arm-linux-gnueabihf-g++
#gcc flags:
CFLAGS = -pedantic-errors -Wall -Wextra -fPIC
INCLUDE = -I ./inc -I /home/lior/git/cpp/inc -I /home/lior/git/projects/final_project/framework/inc -I /home/lior/git/system_programing/src/network/inc
ARM_CON_LIB = -lconcrete_arm -L . -Wl,-rpath=. -lm
ARM_FW_LIB = -lframework_arm -L . -Wl,-rpath=. -lm
ARM_SER_LIB = -lnetwork_arm -L . -Wl,-rpath=. -lm
CON_LIB = -lconcrete -L $(DBG_DIR)/. -Wl,-rpath=$(DBG_DIR)/. -lm
FW_LIB = -lframework -L $(FW_DIR)/. -Wl,-rpath=$(FW_DIR)/. -lm
SER_LIB = -lnetwork_cpp -L $(SER_DIR)/. -Wl,-rpath=$(SER_DIR)/. -lm
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
FW_DIR = /home/lior/git/projects/final_project/framework/bin/release
SER_DIR = /home/lior/git/system_programing/src/network
PI_OBJ_DIR = $(REL_DIR)/obj_pi
#files:
SOURCES =  $(wildcard $(SRC_DIR)/*.cpp)
DEBUG_OBJ = $(subst $(SRC_DIR),$(DBG_OBJ_DIR),$(SOURCES:.cpp=.o))
REL_OBJ = $(subst $(SRC_DIR),$(REL_OBJ_DIR),$(SOURCES:.cpp=.o))
#library
LIB = libconcrete.so
PI_LIB = libconcrete_arm.so

.PHONY: debug release pi master minion_1 minion_2 minion_3 clean all

all: debug release master minion_pi minion_1 minion_2 minion_3
	
#so:
debug: $(DEBUG_OBJ)
	$(GD) -shared -o $(DBG_DIR)/$(LIB) $?

release: $(REL_OBJ)
	$(GC) -shared -o $(REL_DIR)/$(LIB) $?
	
#exe:	
master:
	$(GD) ./test/master.cpp -o ./master.out $(CON_LIB) $(FW_LIB) $(SER_LIB)

define compile_minion
	@echo "Building minion_$(1) with PORT_REC=$(2)"
	@mkdir -p $(DBG_OBJ_DIR)_$(1)
	$(GD) -DPORT_REC=$(2) -DFILE_PATH='"./minions/minion_$(1).txt"' -c $(SRC_DIR)/master_proxy.cpp -o $(DBG_OBJ_DIR)_$(1)/master_proxy.o
	@for src in $(filter-out $(SRC_DIR)/master_proxy.cpp, $(SOURCES)); do \
		$(GD) -c $$src -o $(DBG_OBJ_DIR)_$(1)/$$(basename $$src .cpp).o; \
	done
	$(GD) -shared -o $(DBG_DIR)/libconcrete_$(1).so $(DBG_OBJ_DIR)_$(1)/*.o
	$(GD) ./test/minion.cpp -o ./minion_$(1).out -lconcrete_$(1) -L$(DBG_DIR)/. -Wl,-rpath=$(DBG_DIR)/. -lm $(FW_LIB) $(SER_LIB)
endef

minion_1:
	$(call compile_minion,1,8000)

minion_2:
	$(call compile_minion,2,8100)

minion_3:
	$(call compile_minion,3,8200)

minion_pi:
	@echo "Cross-compiling minion_1 for Raspberry Pi with static linking"
	@mkdir -p $(PI_OBJ_DIR)
	$(GR) -DPORT_REC=8200 -DFILE_PATH='"./minions/minion_3.txt"' -c $(SRC_DIR)/master_proxy.cpp -o $(PI_OBJ_DIR)/master_proxy.o
	@for src in $(filter-out $(SRC_DIR)/master_proxy.cpp, $(SOURCES)); do \
		echo "Compiling $$src for ARM..."; \
		$(GR) -c $$src -o $(PI_OBJ_DIR)/$$(basename $$src .cpp).o; \
	done
	@echo "Creating static library..."
	ar rcs libconcrete_arm.a $(PI_OBJ_DIR)/*.o
	@echo "Linking final executable..."
	$(GR) -static ./test/pi.cpp -o ./minion_pi.out -L. -lconcrete_arm -L. -lframework_arm -L. -lnetwork_arm

#obj:
HEADERS = $(wildcard inc/*.h) $(wildcard inc/*.hpp)

$(DBG_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	mkdir -p $(DBG_OBJ_DIR)
	$(GD) -c -o $@ $<

$(REL_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	mkdir -p $(REL_OBJ_DIR)
	$(GC) -c -o $@ $<

$(PI_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(PI_OBJ_DIR)
	$(GR) -c -o $@ $<

clean:  
	rm -rf *.out ./$(DBG_OBJ_DIR) ./$(REL_OBJ_DIR) ./$(PI_OBJ_DIR) libconcrete_arm.a