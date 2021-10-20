#include "internal.h"

struct atto_grid *
atto_grid_new(const struct atto_grid_options *opts)
{
	struct atto_grid *grid = malloc(sizeof(struct atto_grid));
	if (!grid) return NULL;
	grid->lyt.wgt.wgt_type = ATTOUI_WIDGET_LAYOUT;
	grid->lyt.lyt_type = ATTOUI_LAYOUT_GRID;
	if (opts) {
		grid->w = opts->w;
		grid->h = opts->h;
	} else {
		grid->w = grid->h = 1;
	}
	grid->slots = calloc(grid->w * grid->h, sizeof(struct atto_widget *));
	if (!grid->slots) {
		free(grid);
		return NULL;
	}
	return grid;
}

struct atto_widget *
atto_grid_get(struct atto_grid *grid, uint16_t x, uint16_t y)
{
	return grid->slots[(uint32_t) grid->w * y + x];
}

struct atto_widget *
atto_grid_set(struct atto_grid *grid, uint16_t x, uint16_t y, struct atto_widget *wgt)
{
	struct atto_widget *old = grid->slots[(uint32_t) grid->w * y + x];
	grid->slots[(uint32_t) grid->w * y + x] = wgt;
	return old;
}
