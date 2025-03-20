#ifndef FORM_VIEW_H
#define FORM_VIEW_H

#include <gtk/gtk.h>
#include <sqlite3.h>
#include "main_window.h"

extern sqlite3 *db;
extern AppData *app_data;

void show_form_view(GtkWidget *window);

#endif