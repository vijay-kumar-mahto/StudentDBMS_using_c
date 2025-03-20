#include "charts.h"
#include "../db/db.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variable to store pie chart data
static PieChartData pie_chart_data = {NULL, 0};

PieChartData *get_pie_chart_data(void) {
    return &pie_chart_data;
}

void draw_pie_chart(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    sqlite3 *db = (sqlite3 *)user_data;
    sqlite3_stmt *stmt;
    const char *sql = "SELECT dept, COUNT(*) as count FROM students GROUP BY dept";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        g_print("SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Free previous pie chart data if it exists
    if (pie_chart_data.depts) {
        for (int i = 0; i < pie_chart_data.dept_count; i++) {
            free(pie_chart_data.depts[i].dept_name);
        }
        free(pie_chart_data.depts);
        pie_chart_data.depts = NULL;
        pie_chart_data.dept_count = 0;
    }

    // Count the number of departments
    int dept_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        dept_count++;
    }
    sqlite3_reset(stmt);

    if (dept_count == 0) {
        // If no data, draw a gray circle
        cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
        cairo_arc(cr, width / 2, height / 2, MIN(width, height) / 2 - 10, 0, 2 * M_PI);
        cairo_fill(cr);
        sqlite3_finalize(stmt);
        return;
    }

    // Allocate memory for department data
    pie_chart_data.depts = malloc(dept_count * sizeof(DeptData));
    pie_chart_data.dept_count = dept_count;

    // Populate department data
    int total_students = 0;
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *dept = (const char *)sqlite3_column_text(stmt, 0);
        int count = sqlite3_column_int(stmt, 1);
        pie_chart_data.depts[i].dept_name = strdup(dept ? dept : "Unknown");
        pie_chart_data.depts[i].student_count = count;
        total_students += count;
        i++;
    }
    sqlite3_finalize(stmt);

    // Calculate angles and draw the pie chart
    double start_angle = 0;
    for (i = 0; i < dept_count; i++) {
        double angle = (2 * M_PI * pie_chart_data.depts[i].student_count) / total_students;
        pie_chart_data.depts[i].start_angle = start_angle;
        pie_chart_data.depts[i].end_angle = start_angle + angle;

        // Set color based on department index (cycling through colors)
        double r = (i % 3 == 0) ? 1.0 : 0.0;
        double g = (i % 3 == 1) ? 1.0 : 0.0;
        double b = (i % 3 == 2) ? 1.0 : 0.0;
        cairo_set_source_rgb(cr, r, g, b);

        cairo_move_to(cr, width / 2, height / 2);
        cairo_arc(cr, width / 2, height / 2, MIN(width, height) / 2 - 10, start_angle, start_angle + angle);
        cairo_close_path(cr);
        cairo_fill(cr);

        start_angle += angle;
    }
}