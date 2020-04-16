MS       = $(shell getconf LONG_BIT) # 32/64
CFLAGS   = -g -O0 -fPIC -m$(MS)
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
  OSFLAG = l
  CFLAGS += -shared
else ifeq ($(UNAME_S),Darwin)
  OSFLAG = m
  CFLAGS += -dynamiclib -undefined dynamic_lookup
endif

QARCH = $(OSFLAG)$(MS)
Q    = ${QHOME}/$(QARCH)

R_HOME     = $(shell R RHOME)
R_INCLUDES = $(shell R CMD config --cppflags)
LIBS       = -lpthread -L$(R_HOME)/lib -lR

SRC = src/embedr.c
TGT = embedr.so

all: src/k.h
	R CMD gcc -o $(TGT) $(CFLAGS) $(R_INCLUDES) $(SRC) $(LIBS) -Wall
install:
	install $(TGT) $(Q)
clean:
	rm -rf $(TGT)
fmt:
	clang-format -style=file embedr.c src/rserver.c src/qserver.c src/common.c -i
src/k.h:
	curl -s -O -L src/https://github.com/KxSystems/kdb/raw/master/c/c/k.h
