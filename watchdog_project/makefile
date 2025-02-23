# FILES
SRC = src/
TST = test/
UTIL = ../../
API = include/

# DEPENDENCIES
INCLUDE = -I $(API) -I $(UTIL)
LIB_ARCH = libwatchdog.a
LIB_SHARED = -lwatchdog_common -L.

CC = gcc
CPPFLAGS = $(INCLUDE)
CFLAGS = -ansi -pedantic-errors -Wall -Wextra

RUN_OBJS = $(SRC)watchdog.o $(TST)run_watchdog.o
KEEPWATCHING_OBJS = $(TST)keep_watching_main.o

RUN = run_watchdog
KEEPWATCHING = keep_watching_main


.PHONY : all clean export_lib
all : export_lib $(RUN) $(KEEPWATCHING)

export_lib :
	export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH

$(RUN) : $(RUN_OBJS)
	$(CC) -pthread $(LIB_SHARED) $(LIB_ARCH) $^ -o $@

$(KEEPWATCHING) : $(KEEPWATCHING_OBJS)
	$(CC) -pthread $(LIB_SHARED) $(LIB_ARCH) $^ -o $@

$(SRC) :
	$(CC) $(CFLAGS) -c $<

$(TST) :
	$(CC) $(CFLAGS) -c $<


# UTILITY RULES
clean :
	rm -f $(RUN) $(KEEPWATCHING)