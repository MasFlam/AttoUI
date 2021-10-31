#include <sys/mman.h>
#include "internal.h"

#define PIXEL_SIZE 4

static inline void handle_button_event(struct attoui *atto, struct atto_widget *wgt,
                                       uint32_t button, uint32_t state,
                                       uint32_t width, uint32_t height,
                                       int pointer_x, int pointer_y);
static void pointer_listener_button_fn(void *data, struct wl_pointer *pointer, uint32_t serial,
                                       uint32_t time, uint32_t button, uint32_t state);
static void pointer_listener_enter_fn(void *data, struct wl_pointer *pointer, uint32_t serial,
                                      struct wl_surface *surf, wl_fixed_t x, wl_fixed_t y);
static void pointer_listener_leave_fn(void *data, struct wl_pointer *pointer, uint32_t serial,
                                      struct wl_surface *surf);
static void pointer_listener_motion_fn(void *data, struct wl_pointer *pointer, uint32_t time,
                                      wl_fixed_t x, wl_fixed_t y);
static void registry_listener_global_fn(void *data, struct wl_registry *reg, uint32_t name,
                                        const char *interface, uint32_t version);
static inline void render_widget(struct attoui *atto, struct atto_widget *wgt, uint32_t offset,
                                 uint32_t width, uint32_t height, uint32_t stride);

#include "render.h"

static const struct wl_pointer_listener pointer_listener = {
	.enter = pointer_listener_enter_fn,
	.leave = pointer_listener_leave_fn,
	.motion = pointer_listener_motion_fn,
	.button = pointer_listener_button_fn,
	.axis = noopcb_pointer_axis,
	.frame = noopcb_pointer_frame,
	.axis_source = noopcb_pointer_axis_source,
	.axis_stop = noopcb_pointer_axis_stop,
	.axis_discrete = noopcb_pointer_axis_discrete
};
static const struct wl_registry_listener registry_listener = {
	.global = registry_listener_global_fn,
	.global_remove = noopcb_registry_global_remove
};

struct attoui *
attoui_init(const struct attoui_options *opts)
{
	struct attoui *atto = malloc(sizeof(struct attoui));
	if (!atto) return NULL;
	
	if (opts) {
		atto->width = opts->width;
		atto->height = opts->height;
	} else {
		atto->width = 640;
		atto->height = 360;
	}
	atto->stride = atto->width * PIXEL_SIZE;
	atto->root = NULL;
	
	if (!FcInit()) {
		free(atto);
		return NULL;
	}
	
	FT_Error fterr = FT_Init_FreeType(&atto->ft);
	if (fterr) {
		FcFini();
		free(atto);
		return NULL;
	}
	
	atto->wl.disp = wl_display_connect(NULL);
	if (!atto->wl.disp) {
		FT_Done_FreeType(atto->ft);
		FcFini();
		free(atto);
		return NULL;
	}
	
	struct wl_registry *reg = wl_display_get_registry(atto->wl.disp);
	wl_registry_add_listener(reg, &registry_listener, atto);
	wl_display_roundtrip(atto->wl.disp);
	
	atto->wl.surf = wl_compositor_create_surface(atto->wl.comp);
	atto->wl.shell_surf = wl_shell_get_shell_surface(atto->wl.shell, atto->wl.surf);
	wl_shell_surface_set_toplevel(atto->wl.shell_surf);
	
	atto->wl.pointer = wl_seat_get_pointer(atto->wl.seat);
	wl_pointer_add_listener(atto->wl.pointer, &pointer_listener, atto);
	wl_display_roundtrip(atto->wl.disp);
	
	size_t size = (size_t) atto->stride * atto->height;
	atto->buffd = memfd_create("buffers", 0);
	ftruncate(atto->buffd, size * 2);
	atto->bufptr = mmap(NULL, size * 2, PROT_READ | PROT_WRITE, MAP_SHARED, atto->buffd, 0);
	atto->bufs[0] = (uint8_t *) atto->bufptr;
	atto->bufs[1] = (uint8_t *) atto->bufptr + size;
	
	atto->wl.pool = wl_shm_create_pool(atto->wl.shm, atto->buffd, size * 2);
	atto->wl.buff[0] = wl_shm_pool_create_buffer(atto->wl.pool, 0, atto->width, atto->height,
	                                             atto->stride, WL_SHM_FORMAT_ARGB8888);
	atto->wl.buff[1] = wl_shm_pool_create_buffer(atto->wl.pool, size, atto->width, atto->height,
	                                             atto->stride, WL_SHM_FORMAT_ARGB8888);
	
	wl_surface_attach(atto->wl.surf, atto->wl.buff[0], 0, 0);
	wl_surface_commit(atto->wl.surf);
	
	return atto;
}

void
attoui_render(struct attoui *atto)
{
	if (atto->root) {
		// TODO: Implement the painter's algorithm and don't zero out the whole buffer
		memset(atto->bufs[0], 0, atto->height * atto->stride);
		render_widget(atto, atto->root, 0, atto->width, atto->height, atto->stride);
	}
	// Swap buffers
	wl_surface_attach(atto->wl.surf, atto->wl.buff[0], 0, 0);
	wl_surface_commit(atto->wl.surf);
	void *temp = atto->bufs[0];
	atto->bufs[0] = atto->bufs[1];
	atto->bufs[1] = temp;
	temp = atto->wl.buff[0];
	atto->wl.buff[0] = atto->wl.buff[1];
	atto->wl.buff[1] = temp;
}

void
attoui_dispatch_events(struct attoui *atto)
{
	wl_display_dispatch_pending(atto->wl.disp);
}

void
attoui_dispatch_events_blocking(struct attoui *atto)
{
	wl_display_dispatch(atto->wl.disp);
}

void
atto_widget_free(struct atto_widget *wgt)
{
	switch (wgt->wgt_type) {
	case ATTOUI_WIDGET_LAYOUT: {
		struct atto_layout *lyt = (void *) wgt;
		switch (lyt->lyt_type) {
		case ATTOUI_LAYOUT_BOX: free(wgt); break;
		case ATTOUI_LAYOUT_GRID: {
			struct atto_grid *grid = (void *) wgt;
			free(grid->slots);
			free(grid);
		} break;
		}
	} break;
	case ATTOUI_WIDGET_PROGBAR: free(wgt); break;
	case ATTOUI_WIDGET_LABEL: {
		struct atto_label *lbl = (void *) wgt;
		if (lbl->hb_buf) hb_buffer_destroy(lbl->hb_buf);
		if (lbl->hb_font) hb_font_destroy(lbl->hb_font);
		if (lbl->ft_face) FT_Done_Face(lbl->ft_face);
		free(lbl->text);
		free(lbl->font);
		free(lbl->fontpath);
		free(lbl);
	} break;
	}
}

struct atto_widget *
attoui_get_root(struct attoui *atto)
{
	return atto->root;
}

struct atto_widget *
attoui_set_root(struct attoui *atto, struct atto_widget *wgt)
{
	struct atto_widget *old = atto->root;
	atto->root = wgt;
	return old;
}

void
handle_button_event(struct attoui *atto, struct atto_widget *wgt, uint32_t button, uint32_t state,
                    uint32_t width, uint32_t height, int pointer_x, int pointer_y)
{
	printf("button event @ local x y %d %d\n", pointer_x, pointer_y);
	switch (wgt->wgt_type) {
	case ATTOUI_WIDGET_LAYOUT: {
		struct atto_layout *lyt = (void *) wgt;
		switch (lyt->lyt_type) {
		case ATTOUI_LAYOUT_BOX: {
			struct atto_box *bx = (void *) lyt;
			if (bx->widget) {
				int newx = pointer_x - bx->o.pad_left;
				int newy = pointer_y - bx->o.pad_top;
				uint32_t neww = width - bx->o.pad_left - bx->o.pad_right;
				uint32_t newh = height - bx->o.pad_top - bx->o.pad_bottom;
				if (newx > 0 && newy > 0 && newx < neww && newy < newh) {
					handle_button_event(atto, bx->widget, button, state,
					                    neww, newh, newx, newy);
				}
			}
		} break;
		case ATTOUI_LAYOUT_GRID: {
			struct atto_grid *grid = (void *) lyt;
			uint32_t cell_h = height / grid->h;
			uint32_t cell_w = width / grid->w;
			for (uint32_t y = 0; y < grid->h; ++y) {
				for (uint32_t x = 0; x < grid->w; ++x) {
					uint32_t i = grid->w * y + x;
					if (grid->slots[i]) {
						int newx = pointer_x - x * cell_w;
						int newy = pointer_y - y * cell_h;
						if (newx > 0 && newy > 0 && newx < cell_w && newy < cell_h) {
							handle_button_event(atto, grid->slots[i], button, state,
							                    cell_w, cell_h, newx, newy);
						}
					}
				}
			}
		} break;
		}
	} break;
	}
}

void
pointer_listener_button_fn(void *data, struct wl_pointer *pointer, uint32_t serial,
                           uint32_t time, uint32_t button, uint32_t state)
{
	//printf("button\n");
	struct attoui *atto = data;
	if (!atto->is_focused) return;
	if (atto->root) {
		handle_button_event(atto, atto->root, button, state,
		                    atto->width, atto->height,
		                    wl_fixed_to_int(atto->pointer_x),
		                    wl_fixed_to_int(atto->pointer_y));
	}
}

void
pointer_listener_enter_fn(void *data, struct wl_pointer *pointer, uint32_t serial,
                          struct wl_surface *surf, wl_fixed_t x, wl_fixed_t y)
{
	//printf("enter\n");
	struct attoui *atto = data;
	atto->is_focused = 1;
	atto->pointer_x = x;
	atto->pointer_y = y;
}

void
pointer_listener_leave_fn(void *data, struct wl_pointer *pointer, uint32_t serial,
                          struct wl_surface *surf)
{
	//printf("leave\n");
	struct attoui *atto = data;
	atto->is_focused = 0;
}

void
pointer_listener_motion_fn(void *data, struct wl_pointer *pointer, uint32_t time,
                           wl_fixed_t x, wl_fixed_t y)
{
	//printf("motion\n");
	struct attoui *atto = data;
	if (!atto->is_focused) return;
	atto->pointer_x = x;
	atto->pointer_y = y;
}

void
registry_listener_global_fn(void *data, struct wl_registry *reg, uint32_t name,
                            const char *interface, uint32_t version)
{
	struct attoui *atto = data;
	if (strcmp(interface, "wl_compositor") == 0) {
		atto->wl.comp = wl_registry_bind(reg, name, &wl_compositor_interface, 3);
	} else if (strcmp(interface, "wl_seat") == 0) {
		atto->wl.seat = wl_registry_bind(reg, name, &wl_seat_interface, 1);
	} else if (strcmp(interface, "wl_shm") == 0) {
		atto->wl.shm = wl_registry_bind(reg, name, &wl_shm_interface, 1);
	} else if (strcmp(interface, "wl_shell") == 0) {
		atto->wl.shell = wl_registry_bind(reg, name, &wl_shell_interface, 1);
	}
}

void
render_widget(struct attoui *atto, struct atto_widget *wgt, uint32_t offset, uint32_t width,
              uint32_t height, uint32_t stride)
{
	switch (wgt->wgt_type) {
	case ATTOUI_WIDGET_LAYOUT: {
		struct atto_layout *lyt = (void *) wgt;
		switch (lyt->lyt_type) {
		case ATTOUI_LAYOUT_BOX: render_box(atto, wgt, offset, width, height, stride); break;
		case ATTOUI_LAYOUT_GRID: render_grid(atto, wgt, offset, width, height, stride); break;
		}
	} break;
	case ATTOUI_WIDGET_PROGBAR: render_progbar(atto, wgt, offset, width, height, stride); break;
	case ATTOUI_WIDGET_LABEL: render_label(atto, wgt, offset, width, height, stride); break;
	}
}
