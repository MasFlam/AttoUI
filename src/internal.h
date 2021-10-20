#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wayland-client.h>
#include <attoui.h>

struct attoui {
	struct {
		struct wl_display *disp;
		struct wl_compositor *comp;
		struct wl_shm *shm;
		struct wl_shell *shell;
		struct wl_surface *surf;
		struct wl_shell_surface *shell_surf;
		struct wl_shm_pool *pool;
		struct wl_buffer *buff[2];
	} wl;
	int buffd;
	void *bufptr;
	uint8_t *bufs[2]; // ARGB little endian
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	struct atto_widget *root;
};

struct atto_widget {
	uint16_t wgt_type;
};

struct atto_layout {
	struct atto_widget wgt;
	uint16_t lyt_type;
};

struct atto_box {
	struct atto_layout lyt;
	struct atto_box_options o;
	struct atto_widget *widget;
};

struct atto_grid {
	struct atto_layout lyt;
	uint16_t w;
	uint16_t h;
	struct atto_widget **slots;
};

struct atto_progbar {
	struct atto_widget wgt;
	double progress; // normalized to [0, 1]
	int vertical;
	uint32_t fg;
	uint32_t bg;
};

struct atto_label {
	struct atto_widget wgt;
	char *text;
};
