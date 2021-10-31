#include "internal.h"

struct atto_progbar *
atto_progbar_new(const struct atto_progbar_options *opts)
{
	struct atto_progbar *pb = malloc(sizeof(struct atto_progbar));
	if (!pb) return NULL;
	pb->wgt.wgt_type = ATTOUI_WIDGET_PROGBAR;
	if (opts) {
		pb->o = *opts;
		if (pb->o.progress < 0) pb->o.progress = 0;
		if (pb->o.progress > 1) pb->o.progress = 1;
	} else {
		pb->o = (struct atto_progbar_options) {
			.fg = ATTOUI_BLACK,
			.bg = ATTOUI_WHITE
		};
	}
	return pb;
}

double
atto_progbar_get_progress(struct atto_progbar *pb)
{
	return pb->o.progress;
}

double
atto_progbar_set_progress(struct atto_progbar *pb, double progress)
{
	double old = pb->o.progress;
	pb->o.progress = progress;
	if (pb->o.progress < 0) pb->o.progress = 0;
	if (pb->o.progress > 1) pb->o.progress = 1;
	return old;
}

int
atto_progbar_get_vertical(struct atto_progbar *pb)
{
	return pb->o.vertical;
}

int
atto_progbar_set_vertical(struct atto_progbar *pb, int vertical)
{
	int old = pb->o.vertical;
	pb->o.vertical = vertical;
	return old;
}

uint32_t
atto_progbar_get_fg(struct atto_progbar *pb)
{
	return pb->o.fg;
}

uint32_t
atto_progbar_set_fg(struct atto_progbar *pb, uint32_t fg)
{
	uint32_t old = pb->o.fg;
	pb->o.fg = fg;
	return old;
}

uint32_t
atto_progbar_get_bg(struct atto_progbar *pb)
{
	return pb->o.bg;
}

uint32_t
atto_progbar_set_bg(struct atto_progbar *pb, uint32_t bg)
{
	uint32_t old = pb->o.bg;
	pb->o.bg = bg;
	return old;
}
