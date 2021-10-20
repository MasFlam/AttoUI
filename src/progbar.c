#include "internal.h"

struct atto_progbar *
atto_progbar_new(const struct atto_progbar_options *opts)
{
	struct atto_progbar *pb = malloc(sizeof(struct atto_progbar));
	if (!pb) return NULL;
	pb->wgt.wgt_type = ATTOUI_WIDGET_PROGBAR;
	if (opts) {
		pb->vertical = opts->vertical;
		pb->fg = opts->fg;
		pb->bg = opts->bg;
	} else {
		pb->vertical = 0;
		pb->fg = ATTOUI_BLACK;
		pb->bg = ATTOUI_WHITE;
	}
	pb->progress = 0;
	return pb;
}

double
atto_progbar_get_progress(struct atto_progbar *pb)
{
	return pb->progress;
}

double
atto_progbar_set_progress(struct atto_progbar *pb, double progress)
{
	double old = pb->progress;
	pb->progress = progress;
	if (pb->progress < 0) pb->progress = 0;
	if (pb->progress > 1) pb->progress = 1;
	return old;
}

int
atto_progbar_get_vertical(struct atto_progbar *pb)
{
	return pb->vertical;
}

int
atto_progbar_set_vertical(struct atto_progbar *pb, int vertical)
{
	int old = pb->vertical;
	pb->vertical = vertical;
	return old;
}

uint32_t
atto_progbar_get_fg(struct atto_progbar *pb)
{
	return pb->fg;
}

uint32_t
atto_progbar_set_fg(struct atto_progbar *pb, uint32_t fg)
{
	uint32_t old = pb->fg;
	pb->fg = fg;
	return old;
}

uint32_t
atto_progbar_get_bg(struct atto_progbar *pb)
{
	return pb->bg;
}

uint32_t
atto_progbar_set_bg(struct atto_progbar *pb, uint32_t bg)
{
	uint32_t old = pb->bg;
	pb->bg = bg;
	return old;
}
