#////////////////////
# check which OS Am I
#////////////////////

ifeq ($(OS),Windows_NT)
	DETTECTED_OS := $(OS)
else
	UNAME_S := $(shell uname -s)
	DETTECTED_OS := $(UNAME_S)
endif

#//////////////////////////////
# Define some usefull variables
#//////////////////////////////

# Vars for binary build
CC := gcc
SRC_D := src/
BLD_D := bin/
OBJ_D := obj/
ASST_D := assets/
TEST_SRC_D := tests/
TEST_BLD_D := $(TEST_SRC_D)bin/
LIBS_D := lib/
HTML_D := html/
ASM_D := asm/
SPINE_VERSION ?= _3_7
SPINE_SRC_D := $(LIBS_D)spine/spine$(SPINE_VERSION)/src/spine/
SPINE_SOURCES := $(wildcard $(SPINE_SRC_D)*.c)

INCLUDE_D := -I$(LIBS_D)include/ -I$(LIBS_D)spine/spine$(SPINE_VERSION)/include
STATIC_LIBS_D := -L$(LIBS_D)static/
CFLAGS := -O0 -Wpedantic -g -Wall -std=c99 -g3 -DOS_$(DETTECTED_OS) 
DEBUGGER := kdbg # Other options: cgdb gdb
MK_DIR:= mkdir -p
BIN_EXTENSION = bin

# Vars for emscripten build
RAYLIB_PATH := /Users/pabloweremczuk/Documents/Proyectos/c/raylib
EMSC_CFLAGS := -O2 -s -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0 -g4 -s USE_GLFW=3 -s -s TOTAL_MEMORY=16777216 -v -D PLATFORM_WEB
EMSC_CC := emcc
EMSC_STATIC_LIBS_D := $(LIBS_D)static/libraylib.bc
# EMSC_STATIC_LIBS_D := $(LIBS_D)static/libraylib.bc


# Call to compilers / linkers
CC_COMMAND := $(CC) $(CFLAGS) $(INCLUDE_D) $(STATIC_LIBS_D)
EMSC_CC_COMMAND := $(EMSC_CC) $(EMSC_CFLAGS) $(INCLUDE_D) $(STATIC_LIBS_D)

#//////////////////////////////////////////////////
# Set static libraries depending on de dettected OS
#//////////////////////////////////////////////////

ifeq ($(DETTECTED_OS),Linux)
	LINK_LIBS := -l:libraylib_$(DETTECTED_OS).a -lm -ldl -lpthread -lX11 -lxcb -lGL -lGLX -lXext -lGLdispatch -lXau -lXdmcp
	TEST_LINK_LIBS := -lunity 
else ifeq ($(DETTECTED_OS),Darwin)
	LINK_LIBS := -O0 -framework IOKit -v -lraylib_$(DETTECTED_OS) -framework OpenGL -framework cocoa 
endif

#//////////////
# Build Targets
#//////////////

.PHONY: example_2d_cat.html test run_% debug_optimized debug_unoptimized print_information create_folder_structure run_html_u run_html_o run_performance_test init_project

all: print_information $(BLD_D)main.$(BIN_EXTENSION)

$(OBJ_D)%.o: $(SRC_D)%.c
	$(CC_COMMAND) -o $(OBJ_D)$@ $^ $(LINK_LIBS)

$(TEST_BLD_D)%.spec.$(BIN_EXTENSION): $(TEST_SRC_D)%.spec.c
	@echo "### Building tests for $(@) START ###"
	$(CC_COMMAND) -o $@ $^ $(TEST_LINK_LIBS) $(LINK_LIBS)
	@echo "### End ###"
	@echo ""

$(BLD_D)%.$(BIN_EXTENSION): $(SRC_D)%.c
	@echo "### Building tests for $(@) START ###"
	$(CC_COMMAND) -o $@ $^ $(SPINE_SOURCES) $(LINK_LIBS)
	@echo "### End ###"
	@echo ""

$(HTML_D)%.html: $(SRC_D)%.c
	$(EMSC_CC_COMMAND) -o $@ $^ $(SPINE_SOURCES) $(EMSC_STATIC_LIBS_D) --preload-file ./assets/$(subst .html, ,$(@F))

nothing:
	$(EMSC_CC_COMMAND) -o $@ $^ $(SPINE_SOURCES) $(EMSC_STATIC_LIBS_D) --preload-file ./assets/$(subst .html, ,$(@F))

print_information:
	@echo "Dettected OS: $(DETTECTED_OS)"
	@echo "Spine version: $(SPINE_VERSION)"

create_folders:
	$(MK_DIR) $(BLD_D)
	$(MK_DIR) $(SRC_D)
	$(MK_DIR) libs/include
	$(MK_DIR) libs/static
	$(MK_DIR) $(HTML_D)
	$(MK_DIR) $(TEST_BLD_D)
	$(MK_DIR) $(ASM_D)
	
init_project: create_folders
	touch ./src/main.c

clean:
	rm -rf $(BLD_D)*
	rm -rf $(HTML_D)*
	rm -rf $(OBJ_D)*
	rm -rf $(TEST_BLD_D)*
	rm -rf $(ASM_D)*

run_perf_%.$(BIN_EXTENSION): $(BLD_D)%.$(BIN_EXTENSION)
	perf stat -e task-clock,cycles,instructions,cache-references,cache-misses $^
	
debug_%: $(BLD_D)%.$(BIN_EXTENSION)
	$(DEBUGGER) $^

run_%: $(BLD_D)%.$(BIN_EXTENSION)
	$^

test_%: $(TEST_BLD_D)%.spec.$(BIN_EXTENSION)
	$^

$(ASM_D)%.S: $(SRC_D)%.c
	$(CC_COMMAND) -o $@ $(CFLAGS) -S $^ $(LINK_LIBS)  