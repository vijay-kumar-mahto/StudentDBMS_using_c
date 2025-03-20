#include "logs.h"
#include <gtk/gtk.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../gui/main_window.h"

extern AppData *app_data;

static FILE *log_file = NULL;

void init_logging(void) {
    struct stat st = {0};
    if (stat("data", &st) == -1) {
        mkdir("data", 0700);
    }

    log_file = fopen("data/activity.log", "a");
    if (!log_file) {
        g_printerr("Failed to open activity.log for writing\n");
        return;
    }

    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = '\0';
    fprintf(log_file, "[%s] Logging system initialized\n", timestamp);
    fflush(log_file);
}

void log_action(const char *message) {
    if (!app_data || !app_data->log_buffer || !app_data->log_view) return;

    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = '\0';

    char log_message[1024];
    snprintf(log_message, sizeof(log_message), "[%s] %s\n", timestamp, message);

    if (log_file) {
        fprintf(log_file, "%s", log_message);
        fflush(log_file);
    }

    GtkTextIter end;
    gtk_text_buffer_get_end_iter(app_data->log_buffer, &end);
    gtk_text_buffer_insert(app_data->log_buffer, &end, log_message, -1);

    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(app_data->log_view), &end, 0.0, FALSE, 0, 0);
}