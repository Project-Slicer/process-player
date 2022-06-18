# external parameters
DEBUG ?= 0
OPT_LEVEL ?= 3

# check if is debug mode
ifeq ($(DEBUG), 0)
	C_DEBUG_ARG = -DNDEBUG
	C_OPT_ARG = -O$(OPT_LEVEL)
else
	C_DEBUG_ARG = -g
	C_OPT_ARG = -O0
endif

# RISC-V cross compile toolchain prefix
CROSS_PREFIX := riscv64-unknown-linux-gnu-

# cross C compiler
CFLAGS := -Wall -Werror -std=c11 -c -MMD -MP $(C_DEBUG_ARG) $(C_OPT_ARG)
export CC := $(CROSS_PREFIX)gcc $(CFLAGS)
CFLAGS :=

# cross linker
LDFLAGS :=
export LD := $(CROSS_PREFIX)gcc $(LDFLAGS)
LDFLAGS :=

# objcopy
OBJCFLAGS := -O binary
export OBJC := $(CROSS_PREFIX)objcopy $(OBJCFLAGS)
OBJCFLAGS :=

# objdump
OBJDFLAGS := -D
export OBJD := $(CROSS_PREFIX)objdump $(OBJDFLAGS)
OBJDFLAGS :=

# strip
STRIPFLAGS := -s
export STRIP := $(CROSS_PREFIX)strip $(STRIPFLAGS)
STRIPFLAGS :=
