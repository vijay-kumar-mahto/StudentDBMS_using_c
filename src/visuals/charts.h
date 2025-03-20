#ifndef CHARTS_H
#define CHARTS_H

#include <gtk/gtk.h>
#include <sqlite3.h>

// Structure to store department data and pie slice angles
typedef struct {
    char *dept_name;      // Department name
    int student_count;    // Number of students in this department
    double start_angle;   // Start angle of the pie slice (in radians)
    double end_angle;     // End angle of the pie slice (in radians)
} DeptData;

// Structure to hold all department data for the pie chart
typedef struct {
    DeptData *depts;      // Array of department data
    int dept_count;       // Number of departments
} PieChartData;

void draw_pie_chart(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data);
PieChartData *get_pie_chart_data(void); // Function to access the pie chart data

#endif