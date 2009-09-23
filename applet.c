#include <string.h>
#include <panel-applet.h>
#include "panzen.h"

static gboolean panzen_applet_fill (PanelApplet *applet, const gchar *iid, gpointer data)/*{{{*/
{
	static Panzen *panzen = NULL;
	
	if (panzen == NULL)
		panzen = panzen_new();

	g_return_val_if_fail(strcmp(iid, "OAFIID:Panzen") == 0, FALSE );
	g_return_val_if_fail(panzen != NULL, FALSE );

	gtk_container_add(GTK_CONTAINER (applet), panzen_get_widget(panzen));
	gtk_widget_show_all(GTK_WIDGET (applet));

	return TRUE;
}/*}}}*/

PANEL_APPLET_BONOBO_FACTORY ("OAFIID:Panzen_Factory",
                             PANEL_TYPE_APPLET,
                             "The Panzen Applet",
                             "0",
                             panzen_applet_fill,
                             NULL);
