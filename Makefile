SRCDIR = src

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

all: $(BUILDDIR)/bin $(BUILDDIR)/lib protocol test

$(BUILDDIR)/bin:
	mkdir -p $@

$(BUILDDIR)/lib:
	mkdir -p $@

protocol:
	make -C $(SRCDIR)/$@

test: protocol
	make -C $(SRCDIR)/$@

clean:
	@for subdir in $(MODULES); \
	do $(MAKE) -C $(SRCDIR)/$$subdir $@; \
	done


.PHONY: all clean test protocol
