SRCBASE = ../..
MODULE = test

LDFLAGS += -L$(LIBDIR) -lprotocol -lpthread -lpcap

BINS = $(patsubst %.cpp, $(BINDIR)/%, $(notdir $(CPPFILES)))

include $(SRCBASE)/Makefile.common
