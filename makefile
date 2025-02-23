# FILES
SRC = src/
TST = test/

# DEPENDENCIES
UTIL = ../
API = include/
DSAPI = ../ds/include/
THREADS = -pthread
INCLUDE = -I $(API) -I $(UTIL) -I $(DSAPI)

OBJS = $(SRC)$(EXE).o $(TST)$(EXE)_test.o

include ../makecommon.mk

