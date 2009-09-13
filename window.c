#include <gtk/gtk.h>
#include "panzen.h"

static void destroy (void)
{
	gtk_main_quit ();
}

int main (int argc, char *argv[])
{
	Panzen *panzen;
	GtkWidget *label;
	GtkWidget *window;

	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect (GTK_OBJECT (window),
			"destroy",
			GTK_SIGNAL_FUNC (destroy), 
			NULL);

	gtk_container_border_width (GTK_CONTAINER (window), 10);
	
	panzen = panzen_new();
	g_assert(panzen);

	label = panzen_get_widget(panzen);
	g_assert(label);

	gtk_container_add (GTK_CONTAINER (window), label);
	gtk_widget_show(label);
	gtk_widget_show(window);
	gtk_main ();

	return 0;
}
