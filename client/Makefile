mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(patsubst %/,%,$(dir $(mkfile_path)))

BUILD_FOLDER:=$(current_dir)/build
TARGET:=amperocontrol
C_FILES:=$(wildcard $(current_dir)/src/*.c)
H_FILES:=$(wildcard $(current_dir)/src/*.h)
CC:=gcc

PORTMIDI_FOLDER:=$(current_dir)/portmidi
PORTMIDI_LIB:=portmidi

CCLARGS_FOLDER:=$(current_dir)/CCLArgs

LIBS:=-I$(PORTMIDI_FOLDER) -I$(CCLARGS_FOLDER) -L$(PORTMIDI_FOLDER) -l$(PORTMIDI_LIB)
FLAGS:=-Wl,-R$(PORTMIDI_FOLDER)

.PHONY: debug
debug: FLAGS+= -DDEBUG
debug: $(BUILD_FOLDER)/$(TARGET)

$(BUILD_FOLDER)/$(TARGET): $(C_FILES) $(H_FILES) | $(BUILD_FOLDER)
	$(CC) $(FLAGS) -o $@ $(C_FILES) $(LIBS)

$(BUILD_FOLDER):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(BUILD_FOLDER)
