mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(patsubst %/,%,$(dir $(mkfile_path)))

BUILD_FOLDER:=$(current_dir)/build
TARGET:=client
C_FILES:=$(wildcard $(current_dir)/src/*.c)
H_FILES:=$(wildcard $(current_dir)/src/*.h)
CC:=gcc

COMMON_FOLDER:=$(current_dir)/../common
PORTMIDI_FOLDER:=$(COMMON_FOLDER)/portmidi
PORTMIDI_LIB:=portmidi

CCLARGS_FOLDER:=$(COMMON_FOLDER)/CCLArgs

TOML_C_FOLDER:=$(COMMON_FOLDER)/toml-c

LIBS:=-I$(COMMON_FOLDER) -L$(PORTMIDI_FOLDER) -l$(PORTMIDI_LIB)
FLAGS:=-Wl,-R$(PORTMIDI_FOLDER)

BUILD_STATUS_FILE:=$(BUILD_FOLDER)/.status
BUILD_STATUS:=

.PHONY: debug
debug: FLAGS+= -DDEBUG -O0
debug: BUILD_STATUS=debug
debug: build_status $(BUILD_FOLDER)/$(TARGET)

.PHONY: release
release: FLAGS+= -O3
release: BUILD_STATUS=release
release: build_status $(BUILD_FOLDER)/$(TARGET)

.PHONY: build_status
build_status: | $(BUILD_FOLDER)
	@if ! grep -q "$(BUILD_STATUS)" $(BUILD_STATUS_FILE); then echo "$(BUILD_STATUS)" > $(BUILD_STATUS_FILE); fi

$(BUILD_FOLDER)/$(TARGET): $(BUILD_STATUS_FILE) $(C_FILES) $(H_FILES) | $(BUILD_FOLDER)
	@echo -n [CMD]\ 
	$(CC) $(FLAGS) -o $@ $(C_FILES) $(LIBS)

$(BUILD_FOLDER):
	@echo -n [CMD]\ 
	mkdir -p $@

.PHONY: clean
clean:
	@echo -n [CMD]\ 
	rm -rf $(BUILD_FOLDER)
