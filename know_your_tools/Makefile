OBJS_DIR=.objs

# define all of student executables
EXE1=secure_move
EXES_STUDENT=$(EXE1)

# tests are defined a little differently for this assignment, see below

# list object file dependencies for each
OBJS_SECURECP=$(EXE1).o read_wrap.o secure_move.o shred.o utils.o

# set up compiler
CC=clang
WARNINGS=-Wall -Wextra -Wmissing-variable-declarations -Wno-tautological-compare -Wno-sign-compare -Wmissing-variable-declarations -Wmissing-declarations -Wno-error=unused-const-variable -Wno-error=unused-parameter
CFLAGS_DEBUG=-O0 $(WARNINGS) -g -std=c99 -c -MMD -MP -D_GNU_SOURCE -DDEBUG
CFLAGS_RELEASE=-O2 $(WARNINGS) -g -std=c99 -c -MMD -MP -D_GNU_SOURCE

# set up linker
LD=clang
LDFLAGS=-lm

# the string in grep must appear in the hostname, otherwise the Makefile will
# not allow the assignment to compile
IS_VM=$(shell hostname | grep "cs241")
VM_OVERRIDE=$(shell echo $$HOSTNAME)
ifeq ($(IS_VM),)
ifneq ($(VM_OVERRIDE),cs241grader)
$(error This assignment must be compiled on the CS241 VMs)
endif
endif


.PHONY: all
all: release

# build types
.PHONY: release
.PHONY: debug

release: $(EXES_STUDENT)
debug:   clean $(EXES_STUDENT:%=%-debug)


# include dependencies
-include $(OBJS_DIR)/*.d

$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

# patterns to create objects
# keep the debug and release postfix for object files so that we can always
# separate them correctly
$(OBJS_DIR)/%-debug.o: %.c | $(OBJS_DIR)
	$(CC) $(CFLAGS_DEBUG) $< -o $@

$(OBJS_DIR)/%-release.o: %.c | $(OBJS_DIR)
	$(CC) $(CFLAGS_RELEASE) $< -o $@

# exes
# you will need a pair of exe and exe-debug targets for each exe
$(EXE1)-debug: $(OBJS_SECURECP:%.o=$(OBJS_DIR)/%-debug.o)
	$(LD) $^ $(LDFLAGS) -o $@

$(EXE1): $(OBJS_SECURECP:%.o=$(OBJS_DIR)/%-release.o)
	$(LD) $^ $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -rf .objs $(EXES_STUDENT) $(EXES_STUDENT:%=%-debug)

.PHONY: reset
reset:
	cp files/final.orig.txt files/final.txt
#	cp files/test_1.orig.txt files/test_1.txt
#	cp files/test_2.orig.txt files/test_2.txt
#	cp files/test_3.orig.txt files/test_3.txt
