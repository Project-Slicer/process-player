# Directories
CUR_DIR := $(shell pwd)

# Flags
CFLAGS := -I$(SRC_DIR) -I$(BUILD_DIR)
ASFLAGS := $(CFLAGS)

# Files
SRCS := $(call scan_src, $(CUR_DIR)) $(call scan_src, $(SRC_DIR)/shared)
$(call make_obj, OBJS, $(SRCS))
OBJS := $(patsubst $(OBJ_DIR)/src/shared/%, $(OBJ_DIR)/src/pre/%, $(OBJS))


$(PP): $(OBJS)
	$(info LD   $@)
	$(LD) -o $@ $(OBJS)
	$(info OBJD $@)
	$(OBJD) $@ > $@.dump
	$(if $(filter 0, $(DEBUG)), $(STRIP) $@ -o $@)

$(OBJ_DIR)/src/pre/utils.c.o: $(SRC_DIR)/shared/utils.c
	$(info CC   $@)
	-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $<

$(call inc_rules_deps, $(OBJS))
