SRCDIR = src

BASEBUILDDIR = ./build

ifeq ($(DEBUG), TRUE)
	BUILDDIR = $(BASEBUILDDIR)/debug
else
	BUILDDIR = $(BASEBUILDDIR)/release
endif

export BUILDDIR

SUBBUILDDIR = bin lib obj
BUILDDIRS = $(patsubst %,$(BUILDDIR)/%,$(SUBBUILDDIR))

BUILDPHASES = libprotocol test app

all: builddir
	for d in $(BUILDPHASES) ; do \
		echo "Making $$d" ; \
		make -C $(SRCDIR)/$$d ; \
	done \

test: libprotocol
app: libprotocol
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

.PHONY: all builddir clean distclean ycmtag
.PHONY: $(BUILDPHASES)
