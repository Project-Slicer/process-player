# Directories
CUR_DIR := $(shell pwd)

# Flags
CFLAGS := -I$(CUR_DIR) -I$(SRC_DIR)/shared -I$(BUILD_DIR)
ASFLAGS := $(CFLAGS)

# Files
SRCS := $(call scan_src, $(CUR_DIR)) $(call scan_src, $(SRC_DIR)/shared)
$(call make_obj, OBJS, $(SRCS))
OBJS := $(patsubst $(OBJ_DIR)/src/shared/%, $(OBJ_DIR)/src/pre/%, $(OBJS))


.SILENT:

$(PP): $(OBJS)
	$(info LD   $@)
	$(LD) -o $@ $(OBJS)

$(call inc_rules_deps, $(OBJS))
