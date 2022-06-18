# Directories
CUR_DIR := $(shell pwd)

# Flags
CFLAGS := -I$(CUR_DIR) -I$(SRC_DIR)/shared -DPP_POST -fPIC
ASFLAGS := $(CFLAGS)

# Files
SRCS := $(call scan_src, $(CUR_DIR)) $(call scan_src, $(SRC_DIR)/shared)
$(call make_obj, OBJS, $(SRCS))
OBJS := $(patsubst $(OBJ_DIR)/src/shared/%, $(OBJ_DIR)/src/post/%, $(OBJS))
LINKER_SCRIPT := $(CUR_DIR)/post.ld


.SILENT:

$(POST_PP): $(OBJS) $(LINKER_SCRIPT)
	$(info LD   $(basename $@))
	$(LD) -T$(LINKER_SCRIPT) -o $(basename $@) $(OBJS)
	$(info OBJC $@)
	$(OBJC) -j .text -j .rodata -j .data -j .bss_len $(basename $@) $@

$(call inc_rules_deps, $(OBJS))
