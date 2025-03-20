#ifndef LIST_VIEW_H
#define LIST_VIEW_H

#include <gtk/gtk.h>
#include <sqlite3.h>
#include "main_window.h"  // Includes AppData definition

extern sqlite3 *db;
extern AppData *app_data;

void show_list_view(GtkWidget *window);
// Modified: Added declaration for refreshing the list view
void refresh_list_view(void);

#endif