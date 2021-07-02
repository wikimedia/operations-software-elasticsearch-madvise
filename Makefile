DESTDIR ?=

all:
	gcc -Lptrace_do/ -o elasticsearch-madvise elasticsearch-madvise-random.c proc_maps_parser/pmparser.c -l:libptrace_do.a

install:
	install -d $(DESTDIR)/usr/bin/
	install -m 755 elasticsearch-madvise $(DESTDIR)/usr/bin/

clean:
	rm -f madvise
