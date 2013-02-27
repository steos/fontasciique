#include <bitmap.h>

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool bitmap_init(bitmap_t *bm, int w, int h) {
  assert(w > 0 && h > 0 && "width and height must be > 0");
  bm->width = 0;
  bm->height = 0;
  bm->wmax = w;
  bm->hmax = h;
  bm->pixels = malloc(w * h * sizeof(uint8_t));
  if (bm->pixels == NULL) {
    return false;
  }
  memset(bm->pixels, 0, w * h * sizeof(uint8_t));
  return true;
}

uint8_t bitmap_get(bitmap_t *bm, int x, int y) {
  assert(bm != NULL && "null pointer");
  assert(x >= 0 && x < bm->width && "x out of bounds");
  assert(y >= 0 && y < bm->height && "y out of bounds");
  return bm->pixels[x * bm->hmax + y];
}

void bitmap_set(bitmap_t *bm, int x, int y, uint8_t value) {
  assert(bm != NULL && "null pointer");
  assert(x >= 0 && x < bm->wmax && "x out of bounds");
  assert(y >= 0 && y < bm->hmax && "y out of bounds");
  bm->pixels[x * bm->hmax + y] = value;
  if (x >= bm->width) {
    bm->width = x + 1;
  }
  if (y >= bm->height) {
    bm->height = y + 1;
  }
}

bool bitmap_setx(bitmap_t *bm, int x, int y, uint8_t value) {
  assert(bm != NULL && "null pointer");
  if (x >= bm->wmax) {
    if (!bitmap_extend(bm, bm->wmax * 2)) {
      return false;
    }
  }
  bitmap_set(bm, x, y, value);
  return true;
}

bool bitmap_extend(bitmap_t *bm, int w) {
  assert(bm != NULL && "null pointer");
  assert(w > bm->wmax && "new width must be greater than current");
  int size = w * bm->hmax;
  int offset = bm->wmax * bm->hmax;
  bm->pixels = realloc(bm->pixels, size);
  if (bm->pixels == NULL) {
    return false;
  }
  memset(bm->pixels + offset, 0, size - offset);
  bm->wmax = w;
  return true;
}

void bitmap_free(bitmap_t *bm) {
  assert(bm != NULL && "null pointer");
  if (bm->pixels != NULL) {
    free(bm->pixels);
    bm->pixels = NULL;
  }
}

void bitmap_clear(bitmap_t *bm) {
  assert(bm != NULL && "null pointer");
  bm->width = 0;
  bm->height = 0;
  memset(bm->pixels, 0, bm->wmax * bm->hmax);
}

void bitmap_dump_ascii(bitmap_t *bm, char c) {
  assert(bm != NULL && "null pointer");
  char s[] = {c,c,'\0'};
  for (int y = bm->height - 1; y >= 0; --y) {
    for (int x = 0; x < bm->width; ++x) {
      uint8_t val = bitmap_get(bm, x, y);
      fputs(val > 0 ? s : "  ", stdout);
    }
    fputs("\n", stdout);
  }
}

