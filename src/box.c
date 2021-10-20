#include "internal.h"

struct atto_box *
atto_box_new(const struct atto_box_options *opts)
{
	struct atto_box *box = malloc(sizeof(struct atto_box));
	if (!box) return NULL;
	box->lyt.wgt.wgt_type = ATTOUI_WIDGET_LAYOUT;
	box->lyt.lyt_type = ATTOUI_LAYOUT_BOX;
	if (opts) {
		box->o = *opts;
	} else {
		box->o = (struct atto_box_options) {
			.fg = ATTOUI_WHITE
		};
	}
	return box;
}

struct atto_widget *
atto_box_get_widget(struct atto_box *bx)
{
	return bx->widget;
}

struct atto_widget *
atto_box_set_widget(struct atto_box *bx, struct atto_widget *wgt)
{
	struct atto_widget *old = bx->widget;
	bx->widget = wgt;
	return old;
}

void
atto_box_get_pad(struct atto_box *bx, uint16_t *left, uint16_t *right, uint16_t *top, uint16_t *bottom)
{
	if (left) *left = bx->o.pad_left;
	if (right) *right = bx->o.pad_right;
	if (top) *top = bx->o.pad_top;
	if (bottom) *bottom = bx->o.pad_bottom;
}

void
atto_box_set_pad(struct atto_box *bx, int16_t left, int16_t right, int16_t top, int16_t bottom)
{
	if (left >= 0) bx->o.pad_left = left;
	if (right >= 0) bx->o.pad_right = right;
	if (top >= 0) bx->o.pad_top = top;
	if (bottom >= 0) bx->o.pad_bottom = bottom;
}

uint16_t
atto_box_get_border_width(struct atto_box *bx)
{
	return bx->o.border_width;
}

uint16_t
atto_box_set_border_width(struct atto_box *bx, uint16_t bw)
{
	uint16_t old = bx->o.border_width;
	bx->o.border_width = bw;
	return old;
}

uint32_t
atto_box_get_fg(struct atto_box *bx)
{
	return bx->o.fg;
}

uint32_t
atto_box_set_fg(struct atto_box *bx, uint32_t fg)
{
	uint32_t old = bx->o.fg;
	bx->o.fg = fg;
	return old;
}

uint32_t
atto_box_get_bg(struct atto_box *bx)
{
	return bx->o.bg;
}

uint32_t
atto_box_set_bg(struct atto_box *bx, uint32_t bg)
{
	uint32_t old = bx->o.bg;
	bx->o.bg = bg;
	return old;
}

uint32_t
atto_box_get_border_color(struct atto_box *bx)
{
	return bx->o.border_color;
}

uint32_t
atto_box_set_border_color(struct atto_box *bx, uint32_t bc)
{
	uint32_t old = bx->o.border_color;
	bx->o.border_color = bc;
	return old;
}
