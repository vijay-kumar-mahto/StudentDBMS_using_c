#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>
#include <sqlite3.h>

typedef struct {
    GtkWidget *reg_entry;
    GtkWidget *name_entry;
    GtkWidget *dept_entry;
    GtkWidget *email_entry;
    GtkWidget *mobile_entry;
    GtkTextBuffer *log_buffer;
    GtkWidget *log_view;
    GtkWidget *chart_area;
    GtkWidget *search_entry;
    GtkWidget *status_bar;
    GtkWidget *stack;
    GtkWidget *form_view;
    int change_count;
} AppData;

extern sqlite3 *db;
extern AppData *app_data;

void create_main_window(GtkApplication *app);

#endif