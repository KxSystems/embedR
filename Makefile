# rserver makefile

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OSFLAG = l
endif
ifeq ($(UNAME_S),Darwin)
	OSFLAG = m
endif
MS=$(shell getconf LONG_BIT)  # 32/64
QARCH=$(OSFLAG)$(MS)
Q=${QHOME}/$(QARCH)

CFLAGS=-g -O0 -fPIC -m$(MS)
ifeq ($(OSFLAG),m)
	CFLAGS+=-dynamiclib -undefined dynamic_lookup
endif
ifeq ($(OSFLAG),l)
	CFLAGS+=-shared
endif

R_HOME=$(shell R RHOME)
R_INCLUDES=$(shell R CMD config --cppflags)
LIBS=-lpthread -L$(R_HOME)/lib -lR
ifeq ($(OSFLAG),l)
LIBS+=-lrt
endif

SRC=embedr.c
TGT=$(addsuffix /embedr.so,$(QARCH))

all:
	mkdir -p $(QARCH)
	R CMD gcc -o $(TGT) $(CFLAGS) $(R_INCLUDES) $(SRC) $(LIBS) -Wall

install:
	install $(TGT) $(Q)

clean:
	rm -rf $(TGT)

fmt:
	clang-format -style=file embedr.c src/rserver.c src/qserver.c src/common.c -i
