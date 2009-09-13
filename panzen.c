#include "panzen.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static gboolean on_input(GIOChannel *source, GIOCondition cond, gpointer data)
{
	Panzen *panzen = data;
	gsize buf_pos = 0;
	GError *error = NULL;
	GIOStatus status = g_io_channel_read_line_string(source, panzen->buffer, &buf_pos, &error);

	switch (status) {
		case G_IO_STATUS_NORMAL:
			gtk_label_set_markup(GTK_LABEL(panzen->label), panzen->buffer->str);
			break;
		case G_IO_STATUS_ERROR:
			g_warning("Got warning in on_input handler");
			break;
		case G_IO_STATUS_EOF:
			g_warning("Got EOF");
			break;
		case G_IO_STATUS_AGAIN:
			break;
		default:
			g_assert_not_reached();
	}

	return TRUE;
}

Panzen *panzen_new(const char *status_path)
{
	GIOChannel *chan = NULL;
	Panzen *panzen;

	unlink(status_path);
	g_return_val_if_fail(mkfifo(status_path, 0600) == 0, NULL);

	chan = g_io_channel_new_file(status_path, "r+", NULL);
	g_return_val_if_fail(chan != NULL, NULL);

	switch (g_io_channel_set_flags(chan, G_IO_FLAG_NONBLOCK, NULL)) {
		case G_IO_STATUS_ERROR:
			g_error("Cannot set nonblock");
			break;
		case G_IO_STATUS_NORMAL:
		case G_IO_STATUS_EOF:
		case G_IO_STATUS_AGAIN:
			break;
		default:
			break;
	}
	
	panzen         = g_new0(Panzen, 1);
	panzen->buffer = g_string_new("");
	panzen->path   = g_string_new(status_path);
	panzen->label  = gtk_label_new("Panzen");
	panzen->chan   = chan;
	panzen->tag    = g_io_add_watch(chan, G_IO_IN, on_input, (gpointer) panzen);

	gtk_label_set_single_line_mode(GTK_LABEL(panzen->label), TRUE);

	return panzen;
}

GtkWidget *panzen_get_widget(Panzen *panzen)
{
	return panzen->label;
}
