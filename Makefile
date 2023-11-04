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

SFS_TEST_SRCS = $(wildcard src/tests/*.c)
SFS_TEST_OBJS   = $(SFS_TEST_SRCS:.c=.o)
SFS_UNIT_TESTS	= $(patsubst src/tests/%,bin/%,$(patsubst %.c,%,$(wildcard src/tests/unit_*.c)))


# This means that all files ending in .o will be rebuilt when the .c file corresponding to it is rebuilt
%.o:		%.c $(SFS_LIB_HDRS)
	@echo "Compiling $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(SFS_LIBRARY):	$(SFS_LIB_OBJS)
	@echo "Linking   $@"
	@$(AR) $(ARFLAGS) $@ $^

# This means that all files that match bin/unit_ will be rebuilt with any change to src/tests/unit_%.o and $(SFS_LIBRARY)
bin/unit_%: src/tests/unit_%.o $(SFS_LIBRARY)
	@echo "Linking   $@"
	@$(LD) $(LDFLAGS) -o $@ $^

# THIS IS FOR WHEN THE UNIT TESTS ARE READY
# test-unit: $(SFS_UNIT_TESTS)
# 	@for test in bin/unit_*; do 		\
# 	    for i in $$(seq 0 $$($$test 2>&1 | tail -n 1 | awk '{print $$1}')); do \
# 		echo "Running   $$(basename $$test) $$i";		\
# 		valgrind --leak-check=full $$test $$i > test.log 2>&1;	\
# 		grep -q 'ERROR SUMMARY: 0' test.log || cat test.log;	\
# 		! grep -q 'Assertion' test.log || cat test.log; 	\
# 	    done				\
# 	done

test-unit: $(SFS_UNIT_TESTS)
	@for test in bin/unit_*; do 		\
		echo "Running   $$(basename $$test)";		\
		$$test> test.log 2>&1;	\
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