#include <gtk/gtk.h>
#include "src/gui/main_window.h"

static void activate(GtkApplication *app, gpointer user_data) {
    create_main_window(app);
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("org.student.dbms", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}