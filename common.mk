# Scans for source files (without header files) in the given directory.
# Parameters: $1 - directory to scan
export scan_src = $$(shell find $$1 -name "*.c" -o -name "*.S")

# Scans for header files in the given directory.
# Parameters: $1 - directory to scan
export scan_hdr = $$(shell find $$1 -name "*.h")

# Generates a list of object files.
# Parameters:
#   $1 - name of the generated list
#   $2 - list of source files
define make_obj
	$$(eval __TEMP := $$(patsubst $$(TOP_DIR)/%.c, $$(OBJ_DIR)/%.c.o, $$2));
	$$(eval __TEMP := $$(patsubst $$(TOP_DIR)/%.S, $$(OBJ_DIR)/%.S.o, $$(__TEMP)));
	$$(eval $$1 := $$(__TEMP));
endef
export make_obj

# Includes common rules and all dependencies.
# Parameters: $1 - list of object files
define inc_rules_deps
	$$(eval include $$(TOP_DIR)/rules.mk);
	$$(eval -include $$(patsubst %.o, %.d, $$1));
endef
export inc_rules_deps
