#include "gui_utils.h"

void show_notification(GtkWindow *parent, const char *title, const char *message, gboolean is_error) {
    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 340, -1);

    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(content, "card");
    gtk_window_set_child(GTK_WINDOW(dialog), content);

    GtkWidget *header = gtk_label_new(title);
    gtk_widget_add_css_class(header, "header");
    gtk_box_append(GTK_BOX(content), header);

    gtk_box_append(GTK_BOX(content), gtk_label_new(message));

    GtkWidget *ok_button = gtk_button_new_with_label("OK");
    gtk_widget_add_css_class(ok_button, is_error ? "danger" : "primary");
    g_signal_connect_swapped(ok_button, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
    gtk_box_append(GTK_BOX(content), ok_button);

    gtk_window_present(GTK_WINDOW(dialog));
}