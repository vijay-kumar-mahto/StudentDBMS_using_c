#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include <gtk/gtk.h>

void show_notification(GtkWindow *parent, const char *title, const char *message, gboolean is_error);

#endif