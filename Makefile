SRCDIR = src

CXXFLAGS += -Wall -Werror -std=c++11

BASEBUILDDIR = ./build

ifeq ($(DEBUG), TRUE)
	CXXFLAGS += -ggdb3 -O0
	BUILDDIR = $(BASEBUILDDIR)/debug
else
	CXXFLAGS += -O2
	BUILDDIR = $(BASEBUILDDIR)/release
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
	rm -rf $(BASEBUILDDIR)

ycmtag: YCM-Generator
	./YCM-Generatori/config_gen.py .

.PHONY: all clean test protocol ycmtag
