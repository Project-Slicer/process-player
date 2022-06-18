$(OBJ_DIR)/%.c.o: $(TOP_DIR)/%.c
	$(info CC   $@)
	-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.S.o: $(TOP_DIR)/%.S
	$(info AS   $@)
	-mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -o $@ $<
