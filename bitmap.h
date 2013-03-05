#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  int width, height;
  int wmax, hmax;
  uint8_t *pixels;
} bitmap_t;

bool bitmap_init(bitmap_t *bm, int w, int h);

uint8_t bitmap_get(bitmap_t *bm, int x, int y);

void bitmap_set(bitmap_t *bm, int x, int y, uint8_t value);

bool bitmap_setx(bitmap_t *bm, int x, int y, uint8_t value);

bool bitmap_extend(bitmap_t *bm, int w);

void bitmap_clear(bitmap_t *bm);

void bitmap_free(bitmap_t *bm);

void bitmap_dump_ascii(bitmap_t *bm, char c, uint8_t threshold);

#endif /* BITMAP_H */
