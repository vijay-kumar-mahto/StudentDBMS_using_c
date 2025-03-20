#include "charts.h"
#include "../db/db.h"
#include <gtk/gtk.h>
#include <math.h>

void draw_pie_chart(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer db) {
    GHashTable *stats = db_get_dept_stats(db);
    if (!stats) return;

    double total = 0;
    GHashTableIter iter;
    gpointer value;
    g_hash_table_iter_init(&iter, stats);
    while (g_hash_table_iter_next(&iter, NULL, &value)) {
        total += *(int *)value;
    }

    double start_angle = 0;
    int center_x = width / 2;
    int center_y = height / 2;
    int radius = MIN(width, height) / 3;

    g_hash_table_iter_init(&iter, stats);
    int i = 0;
    while (g_hash_table_iter_next(&iter, NULL, &value)) {
        double fraction = (*(int *)value) / total;
        double end_angle = start_angle + fraction * 2 * G_PI;

        cairo_set_source_rgb(cr, (i % 3 == 0) ? 1 : 0, (i % 3 == 1) ? 1 : 0, (i % 3 == 2) ? 1 : 0);
        cairo_move_to(cr, center_x, center_y);
        cairo_arc(cr, center_x, center_y, radius, start_angle, end_angle);
        cairo_close_path(cr);
        cairo_fill(cr);

        start_angle = end_angle;
        i++;
    }

    g_hash_table_destroy(stats);
}