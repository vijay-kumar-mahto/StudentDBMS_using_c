#include "main_window.h"
#include "../db/db.h"
#include "../utils/voice.h"
#include "../utils/themes.h"
#include "../utils/logs.h"
#include "../utils/gui_utils.h"
#include "../visuals/charts.h"
#include "form_view.h"
#include "list_view.h"
#include <gtk/gtk.h>

sqlite3 *db;
AppData *app_data;

static GtkWidget *last_clicked_button = NULL;

static const char *css_data =
    "window { background: @theme_bg_color; }"
    "button { border-radius: 6px; padding: 10px 16px; margin: 6px; transition: all 250ms cubic-bezier(0.4, 0, 0.2, 1); }"
    "button:hover { background-color: alpha(@theme_selected_bg_color, 0.8); transform: translateY(-2px); box-shadow: 0 4px 8px rgba(0,0,0,0.1); }"
    "button.primary { background: linear-gradient(135deg, @theme_selected_bg_color, shade(@theme_selected_bg_color, 1.2)); color: white; font-weight: bold; }"
    "button.danger { background: linear-gradient(135deg, #e74c3c, #c0392b); color: white; }"
    "button.active { background: linear-gradient(135deg, #ff6200, #ff8c00); color: white; font-weight: bold; }"
    "entry { border-radius: 6px; padding: 10px; margin: 6px; box-shadow: inset 0 1px 3px rgba(0,0,0,0.1); }"
    ".sidebar { background: linear-gradient(180deg, alpha(@theme_bg_color, 0.92), alpha(@theme_bg_color, 0.96)); border-right: 1px solid alpha(@theme_fg_color, 0.1); }"
    ".header { font-size: 22px; font-weight: bold; margin-bottom: 20px; color: #6D8196; }"
    ".card { background: alpha(@theme_bg_color, 0.9); border-radius: 12px; padding: 20px; margin: 12px; box-shadow: 0 6px 16px rgba(0,0,0,0.08); transition: all 300ms ease; }"
    ".card:hover { box-shadow: 0 8px 24px rgba(0,0,0,0.12); transform: translateY(-2px); }"
    ".status-bar { padding: 8px; background: alpha(@theme_bg_color, 0.8); box-shadow: 0 2px 4px rgba(0,0,0,0.05); }"
    ;

static void load_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css_data);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

static void on_button_clicked(GtkWidget *button, gpointer data) {
    if (last_clicked_button && last_clicked_button != button) {
        gtk_widget_remove_css_class(last_clicked_button, "active");
    }
    gtk_widget_add_css_class(button, "active");
    last_clicked_button = button;
}

static void on_easter_egg_pressed(GtkGesture *gesture, int n_press, double x, double y, gpointer window) {
    if (n_press == 2) {
        GtkWidget *dialog = gtk_window_new();
        gtk_window_set_title(GTK_WINDOW(dialog), "About");
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
        gtk_window_set_default_size(GTK_WINDOW(dialog), 380, -1);

        GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
        gtk_widget_add_css_class(content, "card");
        gtk_window_set_child(GTK_WINDOW(dialog), content);

        GtkWidget *header = gtk_label_new("Student DBMS");
        gtk_widget_add_css_class(header, "header");
        gtk_box_append(GTK_BOX(content), header);

        gtk_box_append(GTK_BOX(content), gtk_label_new("Version 1.0.0"));
        gtk_box_append(GTK_BOX(content), gtk_label_new("© 2025 xAI"));

        GtkWidget *close_button = gtk_button_new_with_label("Close");
        gtk_widget_add_css_class(close_button, "primary");
        g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
        gtk_box_append(GTK_BOX(content), close_button);

        gtk_window_present(GTK_WINDOW(dialog));
        voice_speak("About dialog opened");
        log_action("About dialog opened");
    }
}

static void on_theme_toggle(GtkWidget *button, gpointer data) {
    gboolean dark_theme;
    g_object_get(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", &dark_theme, NULL);
    apply_theme(!dark_theme);
    voice_speak("Theme switched");
    log_action("Theme switched");
    gtk_label_set_text(GTK_LABEL(app_data->status_bar), "Theme changed");
}

static void on_export_to_csv(GtkWidget *button, gpointer data) {
    export_to_csv(db);
    voice_speak("Students exported to CSV");
    log_action("Students exported to CSV");
    gtk_label_set_text(GTK_LABEL(app_data->status_bar), "File Exported");
}

static void on_maximize_notify(GObject *object, GParamSpec *pspec, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(object);
    g_print("Maximize state changed: maximized=%d\n", gtk_window_is_maximized(window));
}

static void on_key_pressed(GtkEventController *controller, guint keyval, guint keycode, GdkModifierType state, gpointer window) {
    if (state & GDK_CONTROL_MASK) {
        switch (keyval) {
            case GDK_KEY_m:
            case GDK_KEY_M:
                if (gtk_window_is_maximized(GTK_WINDOW(window))) {
                    gtk_window_unmaximize(GTK_WINDOW(window));
                    gtk_label_set_text(GTK_LABEL(app_data->status_bar), "Window unmaximized");
                    log_action("Window unmaximized");
                    voice_speak("Window unmaximized");
                } else {
                    gtk_window_maximize(GTK_WINDOW(window));
                    gtk_label_set_text(GTK_LABEL(app_data->status_bar), "Window maximized");
                    log_action("Window maximized");
                    voice_speak("Window maximized");
                }
                g_print("Maximize toggled: maximized=%d\n", gtk_window_is_maximized(GTK_WINDOW(window)));
                break;
        }
    }
}

// Modified: Added callback for deleting all students with confirmation
static void on_confirm_delete_all(GtkWidget *button, gpointer data) {
    GtkWidget *dialog = GTK_WIDGET(data);
    if (db_delete_all_students(db)) {
        log_action("All student records deleted");
        voice_speak("All students deleted successfully");
        gtk_label_set_text(GTK_LABEL(app_data->status_bar), "All students deleted");
        gtk_widget_queue_draw(app_data->chart_area); // Refresh chart
        show_notification(GTK_WINDOW(gtk_window_get_transient_for(GTK_WINDOW(dialog))),
                         "Success", "All student records deleted successfully", FALSE);

    } else {
        log_action("Failed to delete all student records");
        voice_speak("Failed to delete all students");
        gtk_label_set_text(GTK_LABEL(app_data->status_bar), "Error");
        show_notification(GTK_WINDOW(gtk_window_get_transient_for(GTK_WINDOW(dialog))),
                         "Error", "Failed to delete all student records", TRUE);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

// Modified: Added callback for deleting all students
// Modified: Updated on_delete_all_students to show a confirmation dialog
static void on_delete_all_students(GtkWidget *button, gpointer window) {
    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "Confirm Delete All");
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
    gtk_window_set_default_size(GTK_WINDOW(dialog), 380, -1);

    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(content, "card");
    gtk_window_set_child(GTK_WINDOW(dialog), content);

    GtkWidget *header = gtk_label_new("Delete All Records");
    gtk_widget_add_css_class(header, "header");
    gtk_box_append(GTK_BOX(content), header);

    gtk_box_append(GTK_BOX(content), gtk_label_new("Are you sure you want to delete all student records? This action cannot be undone."));

    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);

    GtkWidget *yes_btn = gtk_button_new_with_label("Yes");
    gtk_widget_add_css_class(yes_btn, "danger");
    g_signal_connect(yes_btn, "clicked", G_CALLBACK(on_confirm_delete_all), dialog);
    gtk_box_append(GTK_BOX(button_box), yes_btn);

    GtkWidget *no_btn = gtk_button_new_with_label("No");
    gtk_widget_add_css_class(no_btn, "primary");
    g_signal_connect_swapped(no_btn, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
    gtk_box_append(GTK_BOX(button_box), no_btn);

    gtk_box_append(GTK_BOX(content), button_box);
    gtk_window_present(GTK_WINDOW(dialog));
}

// New callback to refresh list view before showing it
static void on_list_students_clicked(GtkWidget *button, gpointer window) {
    refresh_list_view(); // Refresh data before showing
    show_list_view(window);
    on_button_clicked(button, NULL); // Maintain existing button highlight logic
}

void create_main_window(GtkApplication *app) {
    db = db_init();
    if (!db) {
        g_printerr("Failed to initialize database\n");
        return;
    }

    init_logging();

    app_data = g_new0(AppData, 1);
    app_data->change_count = 0;

    // Initialize list_store once
    app_data->list_store = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Student Database Management System");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
    gtk_window_set_decorated(GTK_WINDOW(window), TRUE);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    g_print("Window created: decorated=%d, resizable=%d, maximized=%d\n",
            gtk_window_get_decorated(GTK_WINDOW(window)),
            gtk_window_get_resizable(GTK_WINDOW(window)),
            gtk_window_is_maximized(GTK_WINDOW(window)));

    g_signal_connect(window, "notify::maximized", G_CALLBACK(on_maximize_notify), NULL);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_set_size_request(sidebar, 240, -1);
    gtk_widget_add_css_class(sidebar, "sidebar");
    gtk_box_append(GTK_BOX(main_box), sidebar);

    GtkWidget *logo_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_top(logo_box, 16);
    GtkWidget *logo = gtk_image_new_from_file("resources/logo.png");
    gtk_widget_set_size_request(logo, 64, 64);
    gtk_box_append(GTK_BOX(logo_box), logo);
    GtkWidget *app_name = gtk_label_new("Student DBMS");
    gtk_widget_add_css_class(app_name, "header");
    gtk_box_append(GTK_BOX(logo_box), app_name);
    GtkGesture *click_gesture = gtk_gesture_click_new();
    g_signal_connect(click_gesture, "pressed", G_CALLBACK(on_easter_egg_pressed), window);
    gtk_widget_add_controller(logo_box, GTK_EVENT_CONTROLLER(click_gesture));
    gtk_box_append(GTK_BOX(sidebar), logo_box);

    GtkWidget *add_btn = gtk_button_new_with_label("Add Student");
    gtk_widget_add_css_class(add_btn, "primary");
    g_signal_connect(add_btn, "clicked", G_CALLBACK(show_form_view), window);
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_button_clicked), NULL);
    gtk_box_append(GTK_BOX(sidebar), add_btn);

    GtkWidget *list_btn = gtk_button_new_with_label("List Students");
    gtk_widget_add_css_class(list_btn, "primary");
    g_signal_connect(list_btn, "clicked", G_CALLBACK(show_list_view), window);
    g_signal_connect(list_btn, "clicked", G_CALLBACK(on_button_clicked), NULL);
    gtk_box_append(GTK_BOX(sidebar), list_btn);

    GtkWidget *theme_btn = gtk_button_new_with_label("Toggle Theme");
    gtk_widget_add_css_class(theme_btn, "primary");
    g_signal_connect(theme_btn, "clicked", G_CALLBACK(on_theme_toggle), NULL);
    g_signal_connect(theme_btn, "clicked", G_CALLBACK(on_button_clicked), NULL);
    gtk_box_append(GTK_BOX(sidebar), theme_btn);

    GtkWidget *export_btn = gtk_button_new_with_label("Export to CSV");
    gtk_widget_add_css_class(export_btn, "primary");
    g_signal_connect(export_btn, "clicked", G_CALLBACK(on_export_to_csv), NULL);
    g_signal_connect(export_btn, "clicked", G_CALLBACK(on_button_clicked), NULL);
    gtk_box_append(GTK_BOX(sidebar), export_btn);

    // Modified: Added Delete All button
    GtkWidget *delete_all_btn = gtk_button_new_with_label("Delete All");
    gtk_widget_add_css_class(delete_all_btn, "danger"); // Styled as danger to indicate destructive action
    g_signal_connect(delete_all_btn, "clicked", G_CALLBACK(on_delete_all_students), window);
    g_signal_connect(delete_all_btn, "clicked", G_CALLBACK(on_button_clicked), NULL);
    gtk_box_append(GTK_BOX(sidebar), delete_all_btn);

    app_data->status_bar = gtk_label_new("Ready");
    gtk_widget_add_css_class(app_data->status_bar, "status-bar");
    gtk_box_append(GTK_BOX(sidebar), app_data->status_bar);

    app_data->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_widget_set_vexpand(app_data->stack, TRUE);
    gtk_widget_set_hexpand(app_data->stack, TRUE); // Added to make stack expand horizontally
    gtk_box_append(GTK_BOX(main_box), app_data->stack);

    app_data->form_view = NULL;
    show_form_view(window);

    GtkEventController *key_controller = gtk_event_controller_key_new();
    g_signal_connect(key_controller, "key-pressed", G_CALLBACK(on_key_pressed), window);
    gtk_widget_add_controller(window, key_controller);

    load_css();

    log_action("Application started");
    voice_speak("Welcome to Student Database Management System");
    gtk_window_present(GTK_WINDOW(window));
}