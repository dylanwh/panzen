#include "panzen.h"
#include <glib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

struct Panzen {/*{{{*/
	GString    *buffer;
	GtkWidget  *label;
	GIOChannel *chan;
	guint      tag;
};/*}}}*/


static GList *panzen_pipefiles = NULL;

static gboolean panzen_on_input(GIOChannel *source, GIOCondition cond, gpointer data)/*{{{*/
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

static void panzen_on_exit(void)/*{{{*/
{
	g_return_if_fail(panzen_pipefiles != NULL);

	for (GList *item = panzen_pipefiles; item != NULL; item = g_list_next(item)) {
		remove((gchar *)item->data);
		g_free((gchar *)item->data);
	}
}/*}}}*/

static gchar *compute_fifo_pipefile(void)/*{{{*/
{
	const char *home = g_getenv ("HOME");

  	if (!home)
		home = g_get_home_dir ();

	return g_strconcat(home, "/.panzen", NULL);
}/*}}}*/

inline static Panzen *panzen_new_real(gchar *pipefile);

Panzen *panzen_new(void)
{
	gchar *pipefile = compute_fifo_pipefile();
	Panzen *panzen = panzen_new_real(pipefile);
	if (panzen == NULL)
		g_free(pipefile);
	return panzen;
}

inline static Panzen *panzen_new_real(gchar *pipefile)/*{{{*/
{

	if (g_file_test(pipefile, G_FILE_TEST_IS_REGULAR))
		unlink(pipefile);

	int file_exists = g_file_test(pipefile, G_FILE_TEST_EXISTS);
	g_return_val_if_fail(file_exists == FALSE, NULL);

	int mkfifo_rv = mkfifo(pipefile, 0600);
	g_return_val_if_fail(mkfifo_rv == 0, NULL);

	GIOChannel *chan = g_io_channel_new_file(pipefile, "r+", NULL);
	g_return_val_if_fail(chan != NULL, NULL);

	GIOStatus status = g_io_channel_set_flags(chan, G_IO_FLAG_NONBLOCK, NULL);
	g_return_val_if_fail(status == G_IO_STATUS_NORMAL, NULL);

	Panzen *panzen = g_new0(Panzen, 1);
	panzen->buffer = g_string_new("");
	panzen->label  = gtk_label_new("Panzen");
	panzen->chan   = chan;
	panzen->tag    = g_io_add_watch(chan, G_IO_IN, panzen_on_input, (gpointer) panzen);

	gtk_label_set_single_line_mode(GTK_LABEL(panzen->label), TRUE);

	if (panzen_pipefiles == NULL)
		atexit(panzen_on_exit);

	panzen_pipefiles = g_list_prepend(panzen_pipefiles, (gpointer)pipefile);

	return panzen;
}/*}}}*/

GtkWidget *panzen_get_widget(Panzen *panzen)/*{{{*/
{
	return panzen->label;
}/*}}}*/
