#ifndef CHARTS_H
#define CHARTS_H

#include <gtk/gtk.h>
#include <sqlite3.h>

void draw_pie_chart(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer db);

#endif