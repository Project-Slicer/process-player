# Directories
export TOP_DIR := $(shell pwd)
export BUILD_DIR := $(TOP_DIR)/build
export OBJ_DIR := $(BUILD_DIR)/obj
export SRC_DIR := $(TOP_DIR)/src

include $(TOP_DIR)/toolchain.mk
include $(TOP_DIR)/common.mk

# Files
scan_files = $(shell find $1 -name "*.c" -o -name "*.S" -o -name "*.h" -o -name "*.ld")
PRE_FILES := $(call scan_files, $(SRC_DIR)/pre)
POST_FILES := $(call scan_files, $(SRC_DIR)/post)
SHARED_FILES := $(call scan_files, $(SRC_DIR)/shared)

# Targets
export PP := $(BUILD_DIR)/pp
export POST_PP := $(BUILD_DIR)/post_pp.bin


.SILENT:
.PHONY: clean

$(PP): $(POST_PP) $(PRE_FILES) $(SHARED_FILES)
	$(MAKE) -C $(SRC_DIR)/pre

$(POST_PP): $(POST_FILES) $(SHARED_FILES)
	$(MAKE) -C $(SRC_DIR)/post

clean:
	-rm -rf $(BUILD_DIR)
