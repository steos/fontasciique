#include <magot.h>
#include <bitmap.h>

#include <stdio.h>
#include <assert.h>
#include <locale.h>
#include <wchar.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

typedef struct {
  char *text, *font;
  int size;
  bool anti_alias;
} args_t;

bool process_args(args_t *args, int argc, char **argv) {
  magot_t text, font, size, aa;
  magot_t *opts[] = {
    magot_opt(&font, "font", "f", true, "the font file"),
    magot_opt(&text, "text", "t", true, "the string to render"),
    magot_flag(&aa, "anti-alias", "a", "enable anti-aliasing"),
    magot_opt(&size, "size", "s", true, "the font size")
  };
  font.arg_name = "file";
  text.arg_name = "text";
  size.arg_name = "size";
  int optc = sizeof(opts) / sizeof(opts[0]);
  magot_parser_t parser;
  magot_parser(&parser, argc, argv);
  if (argc == 1) {
    puts("Usage: fontasciique [OPTIONS]");
    puts("SYNOPSIS");
    puts("  fontasciique -f Arial.ttf -s 24 -t quux");
    puts("OPTIONS");
    magot_print_help(stdout, optc, opts, parser.style);
    return true;
  }
  magot_err_t err;
  bool success = magot_parse(optc, opts, &parser, &err);
  if (success) {
    args->size = atoi(size.value);
    args->font = font.value;
    args->text = text.value;
    args->anti_alias = magot_isset(&aa);
    return true;
  }
  printf("Usage error, %s: %s\n", magot_errstr(&err), err.arg);
  return false;
}

bool init_ft(FT_Library *ft, FT_Face *face, char *font, int size) {
  FT_Error err;
  if (FT_Init_FreeType(ft)) {
    puts("freetype init failed");
    return false;
  }
  err = FT_New_Face(*ft, font, 0, face);
  if (err == FT_Err_Unknown_File_Format) {
    puts("unknown font file format");
    return false;
  } else if (err) {
    puts("failed to load font file");
    return false;
  }
  if (FT_Set_Pixel_Sizes(*face, 0, size)) {
    puts("failed to set font size");
    return false;
  }
  return true;
}

typedef struct {
  int x, y;
  bitmap_t *bitmap;
} render_t;

void renderfn(int y, int count, const FT_Span *spans, void *user) {
  render_t *render = (render_t*)user;
  int row = y + render->y;
  assert(row >= 0 && "horiBearingY fail");
  for (int i = 0; i < count; ++i) {
    const FT_Span span = spans[i];
    for (int j = 0; j < span.len; ++j) {
      int x = render->x + span.x + j;
      assert(x >= 0 && "horiBearingX fail");
      bitmap_setx(render->bitmap, x, row, span.coverage);
    }
  }
}

int main(int argc, char **argv) {
  args_t args;
  bool args_ok = process_args(&args, argc, argv);
  if (argc == 1 && args_ok) {
    return 0;
  } else if (!args_ok) {
    return 1;
  }

  FT_Library ft;
  FT_Face face;

  if (!init_ft(&ft, &face, args.font, args.size)) {
    return 2;
  }

  FT_Raster_Params ft_raster;
  render_t render;
  bitmap_t bm;
  memset(&ft_raster, 0, sizeof(ft_raster));

  ft_raster.flags = FT_RASTER_FLAG_DIRECT | FT_RASTER_FLAG_AA;
  ft_raster.gray_spans = renderfn;
  ft_raster.user = &render;

  bitmap_init(&bm, face->height / 64, 100);
  render.bitmap = &bm;
  render.x = 0;
  render.y = -1 * (face->descender / 64);

  char pixel = 'O';
  setlocale(LC_ALL, "");
  int max = strlen(args.text);
  wchar_t *text = malloc(max * sizeof(wchar_t));
  mbstate_t ps;
  const char *ptext = args.text;
  memset(&ps, 0, sizeof(ps));
  mbsrtowcs(text, &ptext, max, &ps);

  for (wchar_t *c = text; *c != '\0'; ++c) {
    if (*c == '\n') {
      bitmap_dump_ascii(&bm, pixel);
      puts("\n");
      bitmap_clear(&bm);
      render.x = 0;
      render.y = -1 * (face->descender / 64);
      continue;
    }
    FT_UInt index = FT_Get_Char_Index(face, *c);
    if (index == 0) {
      fprintf(stderr, "no glyph found for '%lc'\n", *c);
    }
    if (FT_Load_Glyph(face, index, FT_LOAD_DEFAULT)) {
      fprintf(stderr, "no glyph for '%lc' at index %d\n", *c, index);
      continue;
    }
    render.x -= (face->glyph->metrics.horiBearingX) / 64;
    FT_Outline_Render(ft, &face->glyph->outline, &ft_raster);
    render.x += face->glyph->metrics.horiAdvance / 64;
  }
  bitmap_dump_ascii(&bm, pixel);
  bitmap_free(&bm);

  return 0;
}
