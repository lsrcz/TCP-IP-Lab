SRCBASE = ../..
MODULE = test

LDFLAGS += -L$(LIBDIR) -lprotocol -lutils -lpthread -lpcap

BINS = $(patsubst %.cpp, $(BINDIR)/%, $(notdir $(CPPFILES)))

include $(SRCBASE)/Makefile.common
