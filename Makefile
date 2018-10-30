SRCDIR = src

CXXFLAGS += -Wall -Werror -std=c++11

BASEBUILDDIR = ./build

ifeq ($(DEBUG), TRUE)
	BUILDDIR = $(BASEBUILDDIR)/debug
else
	BUILDDIR = $(BASEBUILDDIR)/release
endif

export BUILDDIR

SUBBUILDDIR = bin lib obj
BUILDDIRS = $(patsubst %,$(BUILDDIR)/%,$(SUBBUILDDIR))

BUILDPHASES = libprotocol libutils test

all: builddir
	for d in $(BUILDPHASES) ; do \
		echo "Making $$d" ; \
		make -C $(SRCDIR)/$$d ; \
	done \

$(BUILDPHASES): builddir
	make -C $(SRCDIR)/$@

builddir: $(BUILDDIRS)

$(BUILDDIRS):
	mkdir -p $@

clean:
	rm -rf $(BASEBUILDDIR)

distclean:
	rm -rf $(BASEBUILDDIR)
	rm .ycm_extra_conf.py

ycmtag: YCM-Generator
	rm .ycm_extra_conf.py
	./YCM-Generator/config_gen.py .

.PHONY: all clean distclean test protocol ycmtag
