#ifndef PANZEN_H
#define PANZEN_H

#include <glib.h>
#include <gtk/gtklabel.h>


typedef struct Panzen Panzen;

Panzen *panzen_new(void);
GtkWidget *panzen_get_widget(Panzen *panzen);


#endif
