SRCBASE = ../..
MODSRCDIR = .
MODULE = test

include $(SRCBASE)/Makefile.common

CXXFLAGS += -I$(SRCBASE)/include -I. -I./include
LDFLAGS += -L$(LIBDIR) -lprotocol -lpthread -lpcap

SRCFILES = $(wildcard *.cpp)
#SRCOBJ = $(SRCFILES:.cpp=.o)
SRCBIN = $(patsubst %.cpp, $(BINDIR)/%, $(notdir $(SRCFILES)))

all: $(OBJDIR) $(BINDIR)/$(SRCBIN)

$(OBJDIR):
	mkdir -p $@

$(SRCBIN): $(BINDIR)/%: $(OBJDIR)/%.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(SRCOBJ): $(OBJDIR)/%.o: $(MODSRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	$(RM) -f $(SRCOBJ) $(BINDIR)/$(SRCBIN)

.PHONY: all clean


