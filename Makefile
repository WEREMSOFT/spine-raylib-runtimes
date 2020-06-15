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

SPINE_VERSION_3_8 := _3_8
SPINE_SRC_D_3_8 := $(LIBS_D)spine/spine$(SPINE_VERSION_3_8)/src/spine/
SPINE_SOURCES_3_8 := $(wildcard $(SPINE_SRC_D_3_8)*.c)
SPINE_INCLUDE_3_8 := -I$(LIBS_D)spine/spine$(SPINE_VERSION_3_8)/include

SPINE_VERSION_3_7 := _3_7
SPINE_SRC_D_3_7 := $(LIBS_D)spine/spine$(SPINE_VERSION_3_7)/src/spine/
SPINE_SOURCES_3_7 := $(wildcard $(SPINE_SRC_D_3_7)*.c)
SPINE_INCLUDE_3_7 := -I$(LIBS_D)spine/spine$(SPINE_VERSION_3_7)/include

INCLUDE_D := -I$(LIBS_D)include/
STATIC_LIBS_D := -L$(LIBS_D)static/
CFLAGS := -O0 -Wpedantic -g -Wall -std=c99 -g3 -DOS_$(DETTECTED_OS) 
DEBUGGER := kdbg # Other options: cgdb gdb
MK_DIR:= mkdir -p
BIN_EXTENSION = bin


# Vars for emscripten build
EMSC_CFLAGS := -O2 -s -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0 -g4 -s USE_GLFW=3 -s -s TOTAL_MEMORY=33554432 -v -D PLATFORM_WEB
EMSC_CC := emcc
EMSC_STATIC_LIBS_D := $(LIBS_D)static/libraylib.bc

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

.PHONY: test run_% print_information create_folders

all: print_information create_folders bin/assets bin/example_2d.bin bin/example_3d.bin bin/example_2d_owl.bin bin/example_2d_cat.bin bin/example_2d_cat_witch.bin html/example_2d.html html/example_3d.html html/example_2d_owl.html html/example_2d_cat.html html/example_2d_cat_witch.html

bin/assets: assets
	cp -r assets bin

$(BLD_D)/example_2d_cat.$(BIN_EXTENSION): $(SRC_D)/example_2d_cat.c
	@echo "### Building $(@) START ###"
	$(CC_COMMAND) $(SPINE_INCLUDE_3_7) -o $@ $^ $(SPINE_SOURCES_3_7) $(LINK_LIBS)
	@echo "### End ###"
	@echo ""

$(BLD_D)/example_2d_cat_witch.$(BIN_EXTENSION): $(SRC_D)/example_2d_cat_witch.c
	@echo "### Building $(@) START ###"
	$(CC_COMMAND) $(SPINE_INCLUDE_3_7) -o $@ $^ $(SPINE_SOURCES_3_7) $(LINK_LIBS)
	@echo "### End ###"
	@echo ""

$(TEST_BLD_D)%.spec.$(BIN_EXTENSION): $(TEST_SRC_D)%.spec.c
	@echo "### Building tests for $(@) START ###"
	$(CC_COMMAND) -o $@ $^ $(TEST_LINK_LIBS) $(LINK_LIBS)
	@echo "### End ###"
	@echo ""

$(BLD_D)%.$(BIN_EXTENSION): $(SRC_D)%.c
	@echo "### Building $(@) START ###"
	$(CC_COMMAND) $(SPINE_INCLUDE_3_8) -o $@ $^ $(SPINE_SOURCES_3_8) $(LINK_LIBS)
	@echo "### End ###"
	@echo ""

$(HTML_D)example_2d_cat.html: $(SRC_D)example_2d_cat.c
	$(EMSC_CC_COMMAND) $(SPINE_INCLUDE_3_7) -o $@ $^ $(SPINE_SOURCES_3_7) $(EMSC_STATIC_LIBS_D) --preload-file ./assets/$(subst .html, ,$(@F))

$(HTML_D)example_2d_cat_witch.html: $(SRC_D)example_2d_cat_witch.c
	$(EMSC_CC_COMMAND) $(SPINE_INCLUDE_3_7) -o $@ $^ $(SPINE_SOURCES_3_7) $(EMSC_STATIC_LIBS_D) --preload-file ./assets/$(subst .html, ,$(@F))


$(HTML_D)%.html: $(SRC_D)%.c
	$(EMSC_CC_COMMAND) $(SPINE_INCLUDE_3_8) -o $@ $^ $(SPINE_SOURCES_3_8) $(EMSC_STATIC_LIBS_D) --preload-file ./assets/$(subst .html, ,$(@F))

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

clean:
	rm -rf $(BLD_D)*
	rm -rf $(HTML_D)*
	rm -rf $(OBJ_D)*
	rm -rf $(TEST_BLD_D)*
	rm -rf $(ASM_D)*

run_%: $(BLD_D)%.$(BIN_EXTENSION)
	$^

test_%: $(TEST_BLD_D)%.spec.$(BIN_EXTENSION)
	$^

$(ASM_D)%.S: $(SRC_D)%.c
	$(CC_COMMAND) -o $@ $(CFLAGS) -S $^ $(LINK_LIBS)  