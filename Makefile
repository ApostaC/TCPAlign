include Makefile.in
SUBDIRS = test src 
SUBOBJS = $(addprefix bin/, $(addsuffix .a, $(SUBDIRS)))
CUDA_CPPFLAGS := -std=c++14 -g -O3 ${LDFLAGS} -Icuda/inc	# no -xHost


all: bin/test bin/main #$(SUBDIRS)
	make check

bin/main: main.cc ${SUBOBJS}
	${CXX} $^ -o $@ ${CPPFLAGS}

bin/test: test.cc ${SUBOBJS}
	${CXX} $^ -o $@ ${CPPFLAGS}

$(SUBDIRS): 
	make -C $@ -j2

$(SUBOBJS): $(SUBDIRS)

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
	rm -rf bin/*

check: bin/test
	- bin/test

lines: 
	- find . -name \*.hh -print -o -name \*.cc -print | xargs wc -l

.PHONY: clean $(SUBDIRS) lines

