# This file is part of fontasciique.
override CFLAGS += -Wall -Wextra -pedantic -std=c99 -I.

FT_LDFLAGS = $(shell freetype-config --libs)
FT_CFLAGS = $(shell freetype-config --cflags)

.PHONY: all
all: fontasciique

.PHONY: clean
clean:
	rm -f *.o fontasciique

bitmap.o: bitmap.h

fontasciique: fontasciique.o bitmap.o -lm
	$(LINK.c) $(OUTPUT_OPTION) $^ \
		-Wl,-Bstatic -lmagot -Wl,-Bdynamic $(FT_LDFLAGS)

fontasciique.o: fontasciique.c
	$(COMPILE.c) $(FT_CFLAGS) $(OUTPUT_OPTION) $<

