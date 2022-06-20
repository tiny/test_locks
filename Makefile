IFLAGS     = -I. -std=c++14
OS_DEFINES = -DUNIX -DLINUX 
OS_FLAGS   = -g -Wall
LIBDIRS    = 
LIBS       = -lpthread

.SUFFIXES: .cpp .c 

.c.o:
	@echo "Compiling " $<
	@$(cc) -c $(CFLAGS) $<

.cpp.o:
	@echo "Compiling " $<
	@$(CC) -c $(CFLAGS) $<

cc         = gcc
CC         = g++

CFLAGS     = $(IFLAGS) $(OS_FLAGS) $(OS_DEFINES) $(DEBUG)
LDfLAGS    = $(LIBDIRS) $(LIBS) 
AR         = ar 

MAIN_SRC   = test_locks.cpp
CPP_SRC    = $(MAIN_SRC)
C_SRC      = 
MAIN_OBJS  = $(CPP_SRC:.cpp=.o) $(C_SRC:.c=.o) 

TARGETS    = test_locks

all: $(TARGETS)

test_locks:  $(MAIN_OBJS)
	g++ -o $@ $(MAIN_OBJS) $(LIBDIRS) $(LIBS)

clean:
	rm -f *.o *~ $(TARGETS) core.*
