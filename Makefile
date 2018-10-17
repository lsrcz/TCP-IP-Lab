SRCDIR = src
MODULES = $(shell ls $(SRCDIR))

CXXFLAGS += -Wall -Werror -std=c++11

ifeq ($(DEBUG), TRUE)
	CXXFLAGS += -ggdb3 -O0
	BUILDDIR = ./build/debug
else
	CXXFLAGS += -O2
	BUILDDIR = ./build/release
endif

export CXXFLAGS
export DEBUG

all: $(BUILDDIR) $(MODULES)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/bin $(BUILDDIR)/lib

$(MODULES):
	make -C $(SRCDIR)/$@

clean:
	@for subdir in $(MODULES); \
	do $(MAKE) -C $(SRCDIR)/$$subdir $@; \
	done

.PHONY: all clean
