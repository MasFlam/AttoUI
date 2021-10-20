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
	
	if (box->widget) {
		render_widget(atto, box->widget,
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
	if (pb->vertical) {
		uint16_t front_y = pb->progress * height;
		for (uint16_t y = 0; y < front_y; ++y) {
			for (uint16_t x = 0; x < width; ++x) {
				BUF_SET(x, y, pb->fg);
			}
		}
		for (uint16_t y = front_y; y < height; ++y) {
			for (uint16_t x = 0; x < width; ++x) {
				BUF_SET(x, y, pb->bg);
			}
		}
	} else {
		uint16_t front_x = pb->progress * width;
		for (uint16_t y = 0; y < height; ++y) {
			for (uint16_t x = 0; x < front_x; ++x) {
				BUF_SET(x, y, pb->fg);
			}
			for (uint16_t x = front_x; x < width; ++x) {
				BUF_SET(x, y, pb->bg);
			}
		}
	}
}

void render_label(struct attoui *atto, void *wgt, uint32_t offset, uint32_t width,
                  uint32_t height, uint32_t stride)
{
	struct atto_label *lbl = wgt;
	if (!lbl->text) return;
	// TODO: use harfbuzz + freetype (+ maybe cairo)
}

#undef BUF_GET
#undef BUF_SET
