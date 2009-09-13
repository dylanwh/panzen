#include "panzen.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct Panzen {/*{{{*/
	GIOChannel *chan;
	guint tag;

	GtkWidget *label;

	GString    *buffer;
};/*}}}*/

static gboolean on_input(GIOChannel *source, GIOCondition cond, gpointer data)/*{{{*/
{
	Panzen *panzen = data;
	gsize buf_pos = 0;
	GError *error = NULL;
	GIOStatus status = g_io_channel_read_line_string(source, panzen->buffer, &buf_pos, &error);

	switch (status) {
		case G_IO_STATUS_NORMAL:
			{
				GString *buffer = panzen->buffer;
				g_string_truncate(buffer, buffer->len - 1);
				gtk_label_set_markup(GTK_LABEL(panzen->label), buffer->str);
				break;
			}
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
}/*}}}*/

/* must free result */
static gchar *compute_fifo_filename(void)
{
	GString *buffer = g_string_new("");
	const char *home = g_getenv ("HOME");
	gchar *result;

  	if (!home)
		home = g_get_home_dir ();

	g_string_printf(buffer, "%s/%s", home, ".panzen");
	result = buffer->str;
	g_string_free(buffer, FALSE);

	return result;
}

Panzen *panzen_new(void)/*{{{*/
{
	gchar *filename = compute_fifo_filename();

	int unlink_rv = remove(filename);
	if (unlink_rv == -1 && errno != ENOENT)
		g_error("panzen failed to unlink(%s): %s", filename, g_strerror(errno));

	int mkfifo_rv = mkfifo(filename, 0600);
	if (mkfifo_rv == -1)
		g_error("panzen failed to mkfifo(%s): %s", filename, g_strerror(errno));

	GIOChannel *chan = g_io_channel_new_file(filename, "r+", NULL);
	if (chan == NULL)
		g_error("Unable to open fifo!");

	GIOStatus status = g_io_channel_set_flags(chan, G_IO_FLAG_NONBLOCK, NULL);
	if (status != G_IO_STATUS_NORMAL)
		g_error("Unable to set non-blocking on fifo!");
		
	Panzen *panzen = g_new0(Panzen, 1);
	panzen->buffer = g_string_new("");
	panzen->label  = gtk_label_new("Panzen");
	panzen->chan   = chan;
	panzen->tag    = g_io_add_watch(chan, G_IO_IN, on_input, (gpointer) panzen);

	gtk_label_set_single_line_mode(GTK_LABEL(panzen->label), TRUE);

	g_free(filename);
	return panzen;
}/*}}}*/

GtkWidget *panzen_get_widget(Panzen *panzen)/*{{{*/
{
	return panzen->label;
}/*}}}*/
