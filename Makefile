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
TEST_LDFLAGS= -shared -undefined dynamic_lookup $(shell $(PKG_CONFIG) --libs lua)


PLUGIN_SRC=src/Plugin.cpp src/Function.cpp src/Module.cpp src/Userdata.cpp
PLUGIN_OBJ=$(addprefix build/obj/,$(PLUGIN_SRC:.cpp=.cpp.o))

libclanglua.so: $(PLUGIN_OBJ)
	@/usr/bin/printf "[\033[1;35mPlugin\033[0m] \033[32mLinking \033[33m$<\n\033[0m"
	$(LD) $^ -o $@ -shared $(LDFLAGS)

build/obj/src/%.cpp.o: src/%.cpp
	@/usr/bin/printf "[\033[1;35mPlugin\033[0m] \033[32mCompiling \033[33m$<\n\033[0m"
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test.so: Test.o libclanglua.so
	@/usr/bin/printf "[\033[1;35mTest\033[0m] \033[32mLinking \033[33m$<\n\033[0m"
	$(CC) Test.o -o $@ $(TEST_LDFLAGS)

Test.o: Test.c libclanglua.so
	@/usr/bin/printf "[\033[1;35mTest\033[0m] \033[32mCompiling \033[33m$<\n\033[0m"
	$(CC) -v -c -fplugin=./libclanglua.so -Xclang -add-plugin -Xclang lua $(TEST_CFLAGS) $< -o $@

test: test.so
	lua test-generated.lua

clean:
	rm -rf build
	rm -rf libclanglua.so
	rm -rf Test.o
	rm -rf test.so
