# Directories
CUR_DIR := $(shell pwd)

# Flags
CFLAGS := -I$(SRC_DIR) -I$(BUILD_DIR)
ASFLAGS := $(CFLAGS)

# Files
SRCS := $(call scan_src, $(CUR_DIR))
$(call make_obj, OBJS, $(SRCS))


$(PP): $(OBJS)
	$(info LD   $@)
	$(LD) -o $@ $(OBJS)
	$(info OBJD $@)
	$(OBJD) $@ > $@.dump
	$(if $(filter 0, $(DEBUG)), $(STRIP) $@ -o $@)

$(OBJ_DIR)/src/pre/post.S.o: $(CUR_DIR)/post.S $(POST_PP)

$(call inc_rules_deps, $(OBJS))
