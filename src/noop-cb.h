// No-op callbacks for libwayland listeners

static inline void noopcb_registry_global_remove(void *data, struct wl_registry *wl_registry, uint32_t name) {}

static inline void noopcb_pointer_axis(void *data, struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {}
static inline void noopcb_pointer_frame(void *data, struct wl_pointer *wl_pointer) {}
static inline void noopcb_pointer_axis_source(void *data, struct wl_pointer *wl_pointer, uint32_t axis_source) {}
static inline void noopcb_pointer_axis_stop(void *data, struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis) {}
static inline void noopcb_pointer_axis_discrete(void *data, struct wl_pointer *wl_pointer, uint32_t axis, int32_t discrete) {}
