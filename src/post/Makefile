# Directories
CUR_DIR := $(shell pwd)

# Flags
CFLAGS := -I$(SRC_DIR) -DPP_POST -fPIC
ASFLAGS := $(CFLAGS)
LDFLAGS := -nostdlib -nostdinc -nostartfiles -static -fPIC -flto

# Files
SRCS := $(call scan_src, $(CUR_DIR))
$(call make_obj, OBJS, $(SRCS))
LINKER_SCRIPT := $(CUR_DIR)/post.ld


$(POST_PP): $(OBJS) $(LINKER_SCRIPT)
	$(info LD   $(basename $@))
	$(LD) $(LDFLAGS) -T$(LINKER_SCRIPT) -o $(basename $@) $(OBJS)
	$(info OBJD $(basename $@))
	$(OBJD) $(basename $@) > $(basename $@).dump
	$(info OBJC $@)
	$(OBJC) -j .text -j .rodata -j .data -j .bss_len $(basename $@) $@

$(call inc_rules_deps, $(OBJS))
