#include "internal.h"

struct atto_label *
atto_label_new(const struct atto_label_options *opts)
{
	struct atto_label *lbl = malloc(sizeof(struct atto_label));
	if (!lbl) return NULL;
	lbl->wgt.wgt_type = ATTOUI_WIDGET_LABEL;
	if (opts && opts->text) {
		lbl->text = strdup(opts->text);
		if (!lbl->text) {
			free(lbl);
			return NULL;
		}
	} else {
		lbl->text = NULL;
	}
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
	return 0;
}
