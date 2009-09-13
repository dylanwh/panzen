#ifndef PANZEN_H
#define PANZEN_H

#include <glib.h>
#include <gtk/gtklabel.h>


struct Panzen {
	GIOChannel *chan;
	guint tag;

	GtkWidget *label;

	GString    *buffer;
	GString    *path;
};

typedef struct Panzen Panzen;

Panzen *panzen_new(const char *path);
GtkWidget *panzen_get_widget(Panzen *panzen);


#endif
