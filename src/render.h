// To be only included in attoui.c

// May the compiler bless us with -O7
// > Inline the shit out of this.

// Byteswapping action
#ifdef __linux__
#include <endian.h>
#else
#include <sys/endian.h>
#endif

#ifndef __OpenBSD__
#define BUF_GET(x, y) (le32toh(*(uint32_t *) &atto->bufs[0][offset + stride * (y) + (x) * PIXEL_SIZE]) & UINT32_C(0x00ffffff))
#define BUF_SET(x, y, v) (*(uint32_t *) &atto->bufs[0][offset + stride * (y) + (x) * PIXEL_SIZE] = htole32((v) | UINT32_C(0xff000000)))
#else
#define BUF_GET(x, y) (letoh32(*(uint32_t *) &atto->bufs[0][offset + stride * (y) + (x) * PIXEL_SIZE]) & UINT32_C(0x00ffffff))
#define BUF_SET(x, y, v) (*(uint32_t *) &atto->bufs[0][offset + stride * (y) + (x) * PIXEL_SIZE] = htole32((v) | UINT32_C(0xff000000)))
#endif

static inline void render_box(struct attoui *atto, void *wgt, uint32_t offset,
                              uint32_t width, uint32_t height, uint32_t stride);
static inline void render_grid(struct attoui *atto, void *wgt, uint32_t offset,
                               uint32_t width, uint32_t height, uint32_t stride);
static inline void render_progbar(struct attoui *atto, void *wgt, uint32_t offset,
                                  uint32_t width, uint32_t height, uint32_t stride);
static inline void render_label(struct attoui *atto, void *wgt, uint32_t offset,
                                uint32_t width, uint32_t height, uint32_t stride);
static inline void draw_glyph(struct attoui *atto, FT_Bitmap *bitmap, uint32_t fg,
                              uint16_t x, uint16_t y, uint32_t offset, uint32_t width,
                              uint32_t height, uint32_t stride);

void
render_box(struct attoui *atto, void *wgt, uint32_t offset, uint32_t width,
           uint32_t height, uint32_t stride)
{
	struct atto_box *box = wgt;
	for (uint16_t y = box->o.pad_top; y < height - box->o.pad_bottom; ++y) {
		for (uint16_t x = box->o.pad_left; x < width - box->o.pad_right; ++x) {
			BUF_SET(x, y, box->o.fg);
		}
	}
	
	for (uint16_t y = box->o.border_width; y < box->o.pad_top; ++y) {
		for (uint16_t x = 0; x < width; ++x) {
			BUF_SET(x, y, box->o.bg);
		}
	}
	for (uint16_t y = height - box->o.pad_bottom; y < height - box->o.border_width; ++y) {
		for (uint16_t x = 0; x < width; ++x) {
			BUF_SET(x, y, box->o.bg);
		}
	}
	for (uint16_t y = box->o.pad_top; y < height - box->o.pad_bottom; ++y) {
		for (uint16_t x = 0; x < box->o.pad_left; ++x) {
			BUF_SET(x, y, box->o.bg);
		}
		for (uint16_t x = width - box->o.pad_right; x < width; ++x) {
			BUF_SET(x, y, box->o.bg);
		}
	}
	
	for (uint16_t y = 0; y < box->o.border_width; ++y) {
		for (uint16_t x = 0; x < width; ++x) {
			BUF_SET(x, y, box->o.border_color);
		}
	}
	for (uint16_t y = height - box->o.border_width; y < height; ++y) {
		for (uint16_t x = 0; x < width; ++x) {
			BUF_SET(x, y, box->o.border_color);
		}
	}
	for (uint16_t y = box->o.pad_top; y < height - box->o.pad_bottom; ++y) {
		for (uint16_t x = 0; x < box->o.border_width; ++x) {
			BUF_SET(x, y, box->o.border_color);
		}
		for (uint16_t x = width - box->o.border_width; x < width; ++x) {
			BUF_SET(x, y, box->o.border_color);
		}
	}
	
	if (box->o.widget) {
		render_widget(atto, box->o.widget,
		              offset + box->o.pad_top * stride + box->o.pad_left * PIXEL_SIZE,
		              width - box->o.pad_left - box->o.pad_right,
		              height - box->o.pad_top - box->o.pad_bottom,
		              stride);
	}
}

void
render_grid(struct attoui *atto, void *wgt, uint32_t offset, uint32_t width,
            uint32_t height, uint32_t stride)
{
	struct atto_grid *grid = wgt;
	uint32_t cell_h = height / grid->h;
	uint32_t cell_w = width / grid->w;
	for (uint32_t y = 0; y < grid->h; ++y) {
		for (uint32_t x = 0; x < grid->w; ++x) {
			uint32_t i = grid->w * y + x;
			if (grid->slots[i]) {
				render_widget(atto, grid->slots[i],
				              offset + y * cell_h * stride + x * cell_w * PIXEL_SIZE,
				              cell_w, cell_h, stride);
			}
		}
	}
}

void
render_progbar(struct attoui *atto, void *wgt, uint32_t offset, uint32_t width,
               uint32_t height, uint32_t stride)
{
	struct atto_progbar *pb = wgt;
	if (pb->o.vertical) {
		uint16_t front_y = pb->o.progress * height;
		for (uint16_t y = 0; y < front_y; ++y) {
			for (uint16_t x = 0; x < width; ++x) {
				BUF_SET(x, y, pb->o.fg);
			}
		}
		for (uint16_t y = front_y; y < height; ++y) {
			for (uint16_t x = 0; x < width; ++x) {
				BUF_SET(x, y, pb->o.bg);
			}
		}
	} else {
		uint16_t front_x = pb->o.progress * width;
		for (uint16_t y = 0; y < height; ++y) {
			for (uint16_t x = 0; x < front_x; ++x) {
				BUF_SET(x, y, pb->o.fg);
			}
			for (uint16_t x = front_x; x < width; ++x) {
				BUF_SET(x, y, pb->o.bg);
			}
		}
	}
}

void render_label(struct attoui *atto, void *wgt, uint32_t offset, uint32_t width,
                  uint32_t height, uint32_t stride)
{
	struct atto_label *lbl = wgt;
	if (!lbl->text || !lbl->font) return;
	
	if (lbl->changed & LABEL_CHANGED_FONT) {
		FcPattern *pat = FcNameParse((void *) lbl->font);
		FcConfigSubstitute(NULL, pat, FcMatchPattern);
		FcDefaultSubstitute(pat);
		
		FcResult res;
		FcPattern *pat2 = FcFontMatch(NULL, pat, &res);
		FcPatternDestroy(pat);
		if (pat2) {
			FcChar8 *path; // this need not be freed, the fontconfig docs say
			if (FcPatternGetString(pat2, FC_FILE, 0, &path) == FcResultMatch) {
				free(lbl->fontpath);
				lbl->fontpath = strdup((void *) path);
				if (!lbl->fontpath) {
					FcPatternDestroy(pat2);
					return;
				}
			}
			FcPatternDestroy(pat2);
		}
	}
	
	if (lbl->changed & (LABEL_CHANGED_FONT | LABEL_CHANGED_FONT_SIZE)) {
		FT_Face face;
		FT_New_Face(atto->ft, lbl->fontpath, 0, &face);
		FT_Set_Char_Size(face, 0, lbl->font_size * 64, 0, 0); // TODO: plug in DPI into here
		
		hb_font_t *font = hb_ft_font_create(face, NULL);
		if (lbl->hb_font) hb_font_destroy(lbl->hb_font);
		if (lbl->ft_face) FT_Done_Face(lbl->ft_face);
		lbl->hb_font = font;
		lbl->ft_face = face;
	}
	
	if (lbl->changed & LABEL_CHANGED_TEXT) {
		hb_buffer_clear_contents(lbl->hb_buf);
		hb_buffer_add_utf8(lbl->hb_buf, lbl->text, -1, 0, -1);
		hb_buffer_set_direction(lbl->hb_buf, HB_DIRECTION_LTR);
		hb_buffer_set_script(lbl->hb_buf, HB_SCRIPT_LATIN); // TODO: make script,lang,direction configurable
		hb_buffer_set_language(lbl->hb_buf, hb_language_from_string("en", -1));
	}
	
	if (lbl->changed & (LABEL_CHANGED_FONT | LABEL_CHANGED_FONT_SIZE | LABEL_CHANGED_TEXT)) {
		hb_shape(lbl->hb_font, lbl->hb_buf, NULL, 0);
	}
	
	unsigned int nglyphs = 0;
	hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(lbl->hb_buf, &nglyphs);
	hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(lbl->hb_buf, &nglyphs);
	
	int text_width = 0;
	for (unsigned int i = 0; i < nglyphs; ++i) {
		text_width += glyph_pos[i].x_advance / 64;
	}
	
	hb_font_extents_t exts;
	hb_font_get_extents_for_direction(lbl->hb_font, HB_DIRECTION_LTR, &exts);
	int asc = exts.ascender / 64, desc = -exts.descender / 64;
	
	int cur_x = width / 2 - text_width / 2;
	int cur_y = height / 2 - desc + (asc + desc) / 2;
	for (unsigned int i = 0; i < nglyphs; ++i) {
		int x_offset = glyph_pos[i].x_offset / 64;
		int y_offset = glyph_pos[i].y_offset / 64;
		FT_Load_Glyph(lbl->ft_face, glyph_info[i].codepoint, FT_LOAD_DEFAULT);
		FT_Render_Glyph(lbl->ft_face->glyph, FT_RENDER_MODE_NORMAL);
		draw_glyph(atto, &lbl->ft_face->glyph->bitmap, lbl->fg,
		           cur_x + x_offset + lbl->ft_face->glyph->bitmap_left,
		           cur_y + y_offset - lbl->ft_face->glyph->bitmap_top,
		           offset, width, height, stride);
		cur_x += glyph_pos[i].x_advance / 64;
		cur_y += glyph_pos[i].y_advance / 64;
	}
	
	lbl->changed = 0;
}

void
draw_glyph(struct attoui *atto, FT_Bitmap *bitmap, uint32_t fg, uint16_t x, uint16_t y,
           uint32_t offset, uint32_t width, uint32_t height, uint32_t stride)
{
	for (int row = 0; row < bitmap->rows; ++row) {
		if (row + y < height) {
			for (int col = 0; col < bitmap->width; ++col) {
				if (col + x < width) {
					// Blend colors (i don't know what i'm doing here, tell me if it's wrong pls)
					int gray = bitmap->buffer[row * bitmap->pitch + col];
					uint32_t c1 = BUF_GET(col + x, row + y);
					int r1 = (c1 & 0xff0000) >> 16;
					int g1 = (c1 & 0x00ff00) >> 8;
					int b1 = (c1 & 0x0000ff);
					int r2 = (fg & 0xff0000) >> 16;
					int g2 = (fg & 0x00ff00) >> 8;
					int b2 = (fg & 0x0000ff);
					int r = (r2 * gray + r1 * (255 - gray)) / 256;
					int g = (g2 * gray + g1 * (255 - gray)) / 256;
					int b = (b2 * gray + b1 * (255 - gray)) / 256;
					BUF_SET(col + x, row + y, ATTOUI_RGB(r, g, b));
				}
			}
		}
	}
}

#undef BUF_GET
#undef BUF_SET
