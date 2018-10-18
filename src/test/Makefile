SRCBASE = ../..
MODSRCDIR = .
MODULE = test

include $(SRCBASE)/Makefile.common

CXXFLAGS += -I$(SRCBASE)/include -I. -I./include
LDFLAGS += -lpcap -L$(LIBDIR) -lprotocol

SRCFILES = $(wildcard *.cpp)
#SRCOBJ = $(SRCFILES:.cpp=.o)
SRCBIN = basicTests

all: $(OBJDIR) $(BINDIR)/$(SRCBIN)

$(OBJDIR):
	mkdir -p $@

$(BINDIR)/$(SRCBIN): $(SRCOBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(SRCOBJ): $(OBJDIR)/%.o: $(MODSRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	$(RM) -f $(SRCOBJ) $(BINDIR)/$(SRCBIN)

.PHONY: all clean


