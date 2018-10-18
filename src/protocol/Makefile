SRCBASE = ../..
MODSRCDIR = .
MODULE = protocol

include $(SRCBASE)/Makefile.common

CXXFLAGS += -I$(SRCBASE)/include

SRCFILES = $(wildcard *.cpp)
#SRCOBJ = $(SRCFILES:.cpp=.o)
SRCLIB = libprotocol.a

all: $(OBJDIR) $(LIBDIR)/$(SRCLIB)

$(OBJDIR):
	mkdir -p $@

$(LIBDIR)/$(SRCLIB): $(SRCOBJ)
	$(AR) rcs $@ $^

$(SRCOBJ): $(OBJDIR)/%.o: %.cpp
	@echo $(SRCOBJ)
	@echo $(OBJDIR)
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	$(RM) -f $(SRCOBJ) $(LIBDIR)/$(SRCLIB)

.PHONY: all clean

