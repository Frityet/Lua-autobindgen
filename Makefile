CC=clang
CXX=clang++
LLVM_CONFIG=llvm-config
PKG_CONFIG=pkg-config
CXXFLAGS=-fPIC -g -Og\
		-Wall -Werror -Wextra\
		-Wno-unused-private-field -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable\
		-Wnon-virtual-dtor\
    	-Woverloaded-virtual\
    	-Wformat=2\
		$(shell $(LLVM_CONFIG) --cxxflags) -std=c++20
LD=$(CXX)
LDFLAGS=$(shell $(LLVM_CONFIG) --ldflags) -lclang -lclang-cpp  -undefined dynamic_lookup  $(shell $(LLVM_CONFIG) --libs)

TEST_CFLAGS=-std=c2x -Wall -Wno-unknown-pragmas -Werror -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable $(shell $(PKG_CONFIG) --cflags lua)
TEST_LDFLAGS=-shared -undefined dynamic_lookup $(shell $(PKG_CONFIG) --libs lua)
PLUGIN_ARGS=-Xclang -add-plugin -Xclang luaclang -Xclang -plugin-arg-luaclang -Xclang

PLUGIN_SRC=	src/Plugin.cpp\
			src/Modules/Entries/FunctionEntry.cpp src/Modules/Entries/UserdataEntry.cpp src/Modules/Entries/VariableEntry.cpp\
			src/Modules/Module.cpp src/Modules/Lua54Module.cpp
PLUGIN_OBJ=$(addprefix build/obj/,$(PLUGIN_SRC:.cpp=.cpp.o))

TEST_SRC=test/src/Functions.c
TEST_OBJ=$(addprefix build/obj/,$(TEST_SRC:.c=.c.o))

PLUGIN_PATH=$(BUILD_DIR)/lib/libclanglua.so

BUILD_DIR=build

all: $(PLUGIN_PATH) $(BUILD_DIR)/lib/test.so
build: $(PLUGIN_PATH)

$(PLUGIN_PATH): $(PLUGIN_OBJ)
	@/usr/bin/printf "[\033[1;35mPlugin\033[0m] \033[32mLinking \033[33m$<\n\033[0m"
	@mkdir -p $(dir $@)
	$(LD) $^ -o $@ -shared $(LDFLAGS)

$(BUILD_DIR)/obj/src/%.cpp.o: src/%.cpp
	@/usr/bin/printf "[\033[1;35mPlugin\033[0m] \033[32mCompiling \033[33m$<\n\033[0m"
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -Isrc $< -o $@


$(BUILD_DIR)/obj/test/src/%.c.o: test/src/%.c $(PLUGIN_PATH)
	@/usr/bin/printf "[\033[1;35mTest\033[0m] \033[32mCompiling \033[33m$<\n\033[0m"
	@mkdir -p $(dir $@)
	$(CC) -v -c -fplugin=./$(PLUGIN_PATH) $(PLUGIN_ARGS) $(TEST_CFLAGS) $< -o $@

$(BUILD_DIR)/lib/test.so: $(TEST_OBJ)
	@/usr/bin/printf "[\033[1;35mTest\033[0m] \033[32mLinking \033[33m$<\n\033[0m"
	@mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(TEST_LDFLAGS)

test: $(BUILD_DIR)/lib/test.so
	@/usr/bin/printf "[\033[1;35mTest\033[0m] \033[32mRunning \033[33m$<\n\033[0m"
	lua test/suite.lua

clean:
	rm -rf $(BUILD_DIR)
	rm -rf libclanglua.so
	rm -rf Test.o
	rm -rf test.so