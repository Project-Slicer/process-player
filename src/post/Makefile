# Directories
CUR_DIR := $(shell pwd)

# Flags
CFLAGS := -I$(SRC_DIR) -DPP_POST -fPIC
ASFLAGS := $(CFLAGS)

# Files
SRCS := $(call scan_src, $(CUR_DIR)) $(call scan_src, $(SRC_DIR)/shared)
$(call make_obj, OBJS, $(SRCS))
OBJS := $(patsubst $(OBJ_DIR)/src/shared/%, $(OBJ_DIR)/src/post/%, $(OBJS))
LINKER_SCRIPT := $(CUR_DIR)/post.ld


$(POST_PP): $(OBJS) $(LINKER_SCRIPT)
	$(info LD   $(basename $@))
	$(LD) -nostartfiles -T$(LINKER_SCRIPT) -o $(basename $@) $(OBJS)
	$(info OBJD $(basename $@))
	$(OBJD) $(basename $@) > $(basename $@).dump
	$(info OBJC $@)
	$(OBJC) -j .text -j .rodata -j .data -j .bss_len $(basename $@) $@

$(OBJ_DIR)/src/post/utils.c.o: $(SRC_DIR)/shared/utils.c
	$(info CC   $@)
	-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $<

$(call inc_rules_deps, $(OBJS))
