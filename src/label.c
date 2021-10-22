#include "internal.h"

struct atto_label *
atto_label_new(const struct atto_label_options *opts)
{
	struct atto_label *lbl = malloc(sizeof(struct atto_label));
	if (!lbl) return NULL;
	lbl->wgt.wgt_type = ATTOUI_WIDGET_LABEL;
	if (opts) {
		if (opts->text) {
			lbl->text = strdup(opts->text);
			if (!lbl->text) {
				free(lbl);
				return NULL;
			}
		} else {
			lbl->text = NULL;
		}
		lbl->fg = opts->fg;
		lbl->font_size = opts->font_size > 0 ? opts->font_size : 12;
	} else {
		lbl->text = NULL;
		lbl->fg = ATTOUI_BLACK;
		lbl->font_size = 12;
	}
	lbl->hb_buf = hb_buffer_create();
	if (!lbl->hb_buf) {
		free(lbl->text);
		free(lbl);
		return NULL;
	}
	lbl->changed = LABEL_CHANGED_ALL;
	return lbl;
}

const char *
atto_label_get_text(struct atto_label *lbl)
{
	return lbl->text;
}

int
atto_label_set_text(struct atto_label *lbl, const char *text)
{
	free(lbl->text);
	if (!text) {
		lbl->text = NULL;
	} else {
		char *s = strdup(text);
		if (!s) return -1;
		lbl->text = s;
	}
	lbl->changed |= LABEL_CHANGED_TEXT;
	return 0;
}

unsigned int
atto_label_get_font_size(struct atto_label *lbl)
{
	return lbl->font_size;
}

unsigned int
atto_label_set_font_size(struct atto_label *lbl, unsigned int font_size)
{
	unsigned int old = lbl->font_size;
	lbl->font_size = font_size;
	return old;
}

uint32_t
atto_label_get_fg(struct atto_label *lbl)
{
	return lbl->fg;
}

uint32_t
atto_label_set_fg(struct atto_label *lbl, uint32_t fg)
{
	uint32_t old = lbl->fg;
	lbl->fg = fg;
	lbl->changed |= LABEL_CHANGED_FG;
	return old;
}
