#ifndef _ATTOUI_attoui_h
#define _ATTOUI_attoui_h

#include <stddef.h>
#include <stdint.h>

#define ATTOUI_RGB(r, g, b) ((uint32_t) r << 16 | (uint32_t) g << 8 | b)
#define ATTOUI_WHITE ATTOUI_RGB(255, 255, 255)
#define ATTOUI_BLACK ATTOUI_RGB(0, 0, 0)

#define ATTOUI_WIDGET_LAYOUT 1
#define ATTOUI_WIDGET_PROGBAR 2
#define ATTOUI_WIDGET_LABEL 3

#define ATTOUI_LAYOUT_BOX 1
#define ATTOUI_LAYOUT_GRID 2

struct attoui;
struct atto_widget;
struct atto_layout;
struct atto_box;
struct atto_grid;
struct atto_progbar;
struct atto_label;

struct attoui_options {
	uint32_t width;
	uint32_t height;
};

struct attoui *attoui_init(const struct attoui_options *opts);
void attoui_render(struct attoui *atto);
void attoui_dispatch_events(struct attoui *atto);
void attoui_dispatch_events_blocking(struct attoui *atto);
void atto_widget_free(struct atto_widget *wgt); // _NOT_ recursive!
struct atto_widget *attoui_get_root(struct attoui *atto);
struct atto_widget *attoui_set_root(struct attoui *atto, struct atto_widget *wgt);

struct atto_box_options {
	uint16_t pad_left;
	uint16_t pad_right;
	uint16_t pad_top;
	uint16_t pad_bottom;
	uint16_t border_width;
	uint32_t fg;
	uint32_t bg;
	uint32_t border_color;
};

struct atto_box *atto_box_new(const struct atto_box_options *opts);
struct atto_widget *atto_box_get_widget(struct atto_box *bx);
struct atto_widget *atto_box_set_widget(struct atto_box *bx, struct atto_widget *wgt);
void atto_box_get_pad(struct atto_box *bx, uint16_t *left, uint16_t *right, uint16_t *top, uint16_t *bottom);
void atto_box_set_pad(struct atto_box *bx, int16_t left, int16_t right, int16_t top, int16_t bottom);
uint16_t atto_box_get_border_width(struct atto_box *bx);
uint16_t atto_box_set_border_width(struct atto_box *bx, uint16_t bw);
uint32_t atto_box_get_fg(struct atto_box *bx);
uint32_t atto_box_set_fg(struct atto_box *bx, uint32_t fg);
uint32_t atto_box_get_bg(struct atto_box *bx);
uint32_t atto_box_set_bg(struct atto_box *bx, uint32_t bg);
uint32_t atto_box_get_border_color(struct atto_box *bx);
uint32_t atto_box_set_border_color(struct atto_box *bx, uint32_t bc);

struct atto_grid_options {
	uint16_t w;
	uint16_t h;
};

struct atto_grid *atto_grid_new(const struct atto_grid_options *opts);
struct atto_widget *atto_grid_get(struct atto_grid *grid, uint16_t x, uint16_t y);
struct atto_widget *atto_grid_set(struct atto_grid *grid, uint16_t x, uint16_t y, struct atto_widget *wgt);

struct atto_progbar_options {
	int vertical;
	uint32_t fg;
	uint32_t bg;
};

struct atto_progbar *atto_progbar_new(const struct atto_progbar_options *opts);
double atto_progbar_get_progress(struct atto_progbar *pb);
double atto_progbar_set_progress(struct atto_progbar *pb, double progress); // clamps to [0, 1]
int atto_progbar_get_vertical(struct atto_progbar *pb);
int atto_progbar_set_vertical(struct atto_progbar *pb, int vertical);
uint32_t atto_progbar_get_fg(struct atto_progbar *pb);
uint32_t atto_progbar_set_fg(struct atto_progbar *pb, uint32_t fg);
uint32_t atto_progbar_get_bg(struct atto_progbar *pb);
uint32_t atto_progbar_set_bg(struct atto_progbar *pb, uint32_t bg);

struct atto_label_options {
	const char *text;
	uint32_t fg;
};

struct atto_label *atto_label_new(const struct atto_label_options *opts);
const char *atto_label_get_text(struct atto_label *lbl);
int atto_label_set_text(struct atto_label *lbl, const char *text);
uint32_t atto_label_get_fg(struct atto_label *lbl);
uint32_t atto_label_set_fg(struct atto_label *lbl, uint32_t fg);

#endif
