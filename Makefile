# configs

CC = gcc
LD = gcc
AR = AR
CFLAGS		= -g -std=gnu99 -Wall -Iinclude -fPIC
LDFLAGS		= -Llib
LIBS		= -lm
ARFLAGS		= rcs

# Variables

SFS_LIB_HDRS = $(wildcard src/include/*.h)
SFS_LIB_SRCS = $(wildcard src/library/*.c)
SFS_LIB_OBJS = $(SFS_LIB_SRCS:.c=.o)
SFS_LIBRARY = lib/libsfs.a

SFS_CLI_SRCS = $(wildcard src/cli/*.c)
SFS_CLI_OBJS = $(SFS_CLI_SRCS:.c=.o)
SFS_CLI = bin/cli

SFS_TEST_SRCS = $(wildcard src/tests/*.c)
SFS_TEST_OBJS   = $(SFS_TEST_SRCS:.c=.o)
# path patsubst follows the following form (patsubst pattern,replacement,text)
SFS_UNIT_TESTS	= $(patsubst src/tests/%,bin/%,$(patsubst %.c,%,$(wildcard src/tests/unit_*.c)))

all: $(SFS_LIBRARY) $(SFS_UNIT_TESTS) $(SFS_CLI)
# This means that all files ending in .o will be recompiled when the .c file corresponding or library headers have changed
%.o:		%.c $(SFS_LIB_HDRS)
	@echo "Compiling $@ with $^"
	@$(CC) $(CFLAGS) -c -o $@ $<

# This links the sfs library form the lib object files when sfs_lib_objects are recompiled
$(SFS_LIBRARY):	$(SFS_LIB_OBJS)
	@echo "Linking   $@ with $^"
	@$(AR) $(ARFLAGS) $@ $^

$(SFS_CLI): $(SFS_CLI_OBJS) $(SFS_LIBRARY)
	@echo "Linking $@ with $^"
	@$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

# This means that all files that match bin/unit_ will be rebuilt with any change to src/tests/unit_%.o and $(SFS_LIBRARY)
bin/unit_%: src/tests/unit_%.o $(SFS_LIBRARY)
	@echo "Linking   $@"
	@$(LD) $(LDFLAGS) -o $@ $^

test-unit: $(SFS_UNIT_TESTS)
	@for test in bin/unit_*; do 		\
		for i in $$(seq 0 $$($$test 2>&1 | tail -n 1 | awk '{print $$1}')); do \
			echo "Running   $$(basename $$test) $$i";		\
	    done				\
	done


# Cleans everything
clean:
	@echo "Removing  objects"
	@rm -f $(SFS_LIB_OBJS) $(SFS_SHL_OBJS) $(SFS_TEST_OBJS)

	@echo "Removing  libraries"
	@rm -f $(SFS_LIBRARY)

	@echo "Removing  tests"
	@rm -f $(SFS_UNIT_TESTS) test.log

.PRECIOUS: %.o