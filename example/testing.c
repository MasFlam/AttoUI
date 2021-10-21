#include <unistd.h>
#include <wayland-client.h>
#include <attoui.h>

int
main()
{
	struct attoui *atto = attoui_init(NULL);
	struct atto_box *box = atto_box_new(NULL);
	atto_box_set_pad(box, 30, 30, 10, 10);
	atto_box_set_fg(box, 0xbbbbbb);
	atto_box_set_bg(box, 0x7f7f7f);
	struct atto_grid *grid = atto_grid_new(&(struct atto_grid_options) {
		.w = 2,
		.h = 2
	});
	struct atto_progbar *pb1 = atto_progbar_new(NULL);
	atto_progbar_set_progress(pb1, 0.7);
	atto_progbar_set_fg(pb1, 0xff0000);
	atto_progbar_set_bg(pb1, 0x0000ff);
	atto_grid_set(grid, 0, 0, (void *) pb1);
	struct atto_progbar *pb2 = atto_progbar_new(NULL);
	atto_progbar_set_progress(pb2, 0.5);
	atto_progbar_set_vertical(pb2, 1);
	atto_grid_set(grid, 1, 1, (void *) pb2);
	struct atto_label *lbl = atto_label_new(&(struct atto_label_options) {
		.text = "Hijack",
		.fg = ATTOUI_RGB(127, 0, 127)
	});
	atto_grid_set(grid, 1, 0, (void *) lbl);
	atto_box_set_widget(box, (void *) grid);
	attoui_set_root(atto, (void *) box);
	while (1) {
		attoui_render(atto);
		// We don't need to do anything in between events, so don't spin the thread for no reason
		attoui_dispatch_events_blocking(atto);
	}
	return 0;
}
