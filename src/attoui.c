#include <sys/mman.h>
#include "internal.h"

#define PIXEL_SIZE 4

static void registry_listener_global_fn(void *data, struct wl_registry *reg, uint32_t name,
                                        const char *interface, uint32_t version);
static inline void render_widget(struct attoui *atto, struct atto_widget *wgt, uint32_t offset,
                                 uint32_t width, uint32_t height, uint32_t stride);

#include "render.h"

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
	
	atto->wl.disp = wl_display_connect(NULL);
	if (!atto->wl.disp) {
		free(atto);
		return NULL;
	}
	
	struct wl_registry *reg = wl_display_get_registry(atto->wl.disp);
	wl_registry_add_listener(reg, &(struct wl_registry_listener) {
		.global = registry_listener_global_fn,
		.global_remove = NULL // I hope this is allowed?
	}, atto);
	wl_display_roundtrip(atto->wl.disp);
	
	atto->wl.surf = wl_compositor_create_surface(atto->wl.comp);
	atto->wl.shell_surf = wl_shell_get_shell_surface(atto->wl.shell, atto->wl.surf);
	wl_shell_surface_set_toplevel(atto->wl.shell_surf);
	
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
		free(lbl->text);
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
registry_listener_global_fn(void *data, struct wl_registry *reg, uint32_t name,
                            const char *interface, uint32_t version)
{
	struct attoui *atto = data;
	if (strcmp(interface, "wl_compositor") == 0) {
		atto->wl.comp = wl_registry_bind(reg, name, &wl_compositor_interface, 3);
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
