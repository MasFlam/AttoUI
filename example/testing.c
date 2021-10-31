#include <inttypes.h>
#include <stdio.h>
#include <attoui.h>

void
clickme_cb(struct attoui *atto, struct atto_box *bx, void *userptr, uint32_t button, int pressed)
{
	int *rerender = userptr;
	printf("button = %" PRIu32 ", pressed = %d\n", button, pressed);
	if (pressed) {
		atto_box_set_border_color(bx, ATTOUI_RGB(255, 0, 0));
	} else {
		atto_box_set_border_color(bx, ATTOUI_BLACK);
	}
	*rerender = 1;
}

int
main()
{
	struct attoui *atto = attoui_init(NULL);
	
	struct atto_box *box = atto_box_new(&(struct atto_box_options) {
		.pad_left = 30,
		.pad_right = 30,
		.pad_top = 10,
		.pad_bottom = 10,
		.fg = 0xbbbbbb,
		.bg = 0x7f7f7f
	});
	struct atto_grid *grid = atto_grid_new(&(struct atto_grid_options) {
		.w = 2,
		.h = 2
	});
	atto_box_set_widget(box, (void *) grid);
	attoui_set_root(atto, (void *) box);
	
	atto_grid_set(grid, 1, 1, (void *) atto_progbar_new(&(struct atto_progbar_options) {
		.progress = 0.7,
		.fg = 0xff0000,
		.bg = 0x0000ff
	}));
	atto_grid_set(grid, 0, 0, (void *) atto_progbar_new(&(struct atto_progbar_options) {
		.progress = 0.5,
		.vertical = 1,
		.fg = ATTOUI_BLACK,
		.bg = ATTOUI_WHITE
	}));
	
	int rerender = 1;
	
	atto_grid_set(grid, 1, 0, (void *) atto_box_new(&(struct atto_box_options) {
		.widget = (void *) atto_label_new(&(struct atto_label_options) {
			.text = "Click me!",
			.font = "Fira Code:bold,monospace",
			.font_size = 42,
			.fg = ATTOUI_RGB(127, 0, 127)
		}),
		.button_cb = clickme_cb,
		.userptr = &rerender,
		.pad_left = 3,
		.pad_right = 3,
		.pad_top = 3,
		.pad_bottom = 3,
		.border_width = 3,
		.fg = 0xbbbbbb,
		.bg = 0x7f7f7f,
		.border_color = ATTOUI_BLACK,
	}));
	
	while (1) {
		// something weird is going on, the border stays red forever :/
		if (rerender) {
			puts("rerendering");
			printf("border_color = %#" PRIu32 "\n", atto_box_get_border_color((void *) atto_grid_get(grid, 1, 0)));
			attoui_render(atto);
			rerender = 0;
		}
		// We don't need to do anything in between events, so don't spin the thread for no reason
		attoui_dispatch_events_blocking(atto);
	}
	return 0;
}
