#include "form_view.h"
#include "../db/db.h"
#include "../utils/voice.h"
#include "../utils/logs.h"
#include "../utils/gui_utils.h"
#include "../visuals/charts.h"
#include "list_view.h"
#include <gtk/gtk.h>

extern sqlite3 *db;
extern AppData *app_data;

static void check_backup(GtkWindow *parent) {
    app_data->change_count++;
    if (app_data->change_count % 5 == 0) {
        GtkWidget *dialog = gtk_window_new();
        gtk_window_set_title(GTK_WINDOW(dialog), "Backup Reminder");
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
        gtk_window_set_default_size(GTK_WINDOW(dialog), 380, -1);

        GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
        gtk_widget_add_css_class(content, "card");
        gtk_window_set_child(GTK_WINDOW(dialog), content);

        GtkWidget *header = gtk_label_new("Database Backup");
        gtk_widget_add_css_class(header, "header");
        gtk_box_append(GTK_BOX(content), header);

        gtk_box_append(GTK_BOX(content), gtk_label_new("You've made 5 changes. Backup now?"));

        GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
        gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);

        GtkWidget *yes_btn = gtk_button_new_with_label("Yes");
        gtk_widget_add_css_class(yes_btn, "primary");
        g_signal_connect(yes_btn, "clicked", G_CALLBACK(export_to_csv), db);
        g_signal_connect_swapped(yes_btn, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
        g_signal_connect(yes_btn, "clicked", G_CALLBACK(log_action), "Database backed up to CSV");
        gtk_box_append(GTK_BOX(button_box), yes_btn);

        GtkWidget *no_btn = gtk_button_new_with_label("No");
        g_signal_connect_swapped(no_btn, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
        gtk_box_append(GTK_BOX(button_box), no_btn);

        gtk_box_append(GTK_BOX(content), button_box);
        gtk_window_present(GTK_WINDOW(dialog));
    }
}

static void on_add_student(GtkWidget *button, gpointer window) {
    const char *reg_no = gtk_editable_get_text(GTK_EDITABLE(app_data->reg_entry));
    const char *name = gtk_editable_get_text(GTK_EDITABLE(app_data->name_entry));
    const char *dept = gtk_editable_get_text(GTK_EDITABLE(app_data->dept_entry));
    const char *email = gtk_editable_get_text(GTK_EDITABLE(app_data->email_entry));
    const char *mobile = gtk_editable_get_text(GTK_EDITABLE(app_data->mobile_entry));

    if (strlen(reg_no) == 0 || strlen(name) == 0 || strlen(dept) == 0) {
        show_notification(GTK_WINDOW(window), "Input Error", "Registration No., Name, and Department are required", TRUE);
        voice_speak("Please enter required fields");
        return;
    }

    Student *student = student_new(reg_no, name, dept, email, mobile);
    if (db_add_student(db, student)) {
        log_action(g_strdup_printf("Added student: %s", reg_no));
        voice_speak("Student added successfully");
        gtk_label_set_text(GTK_LABEL(app_data->status_bar), "Student added successfully");
        gtk_widget_queue_draw(app_data->chart_area);
        check_backup(GTK_WINDOW(window));
        gtk_editable_set_text(GTK_EDITABLE(app_data->reg_entry), "");
        gtk_editable_set_text(GTK_EDITABLE(app_data->name_entry), "");
        gtk_editable_set_text(GTK_EDITABLE(app_data->dept_entry), "");
        gtk_editable_set_text(GTK_EDITABLE(app_data->email_entry), "");
        gtk_editable_set_text(GTK_EDITABLE(app_data->mobile_entry), "");
        show_notification(GTK_WINDOW(window), "Success", "Student added successfully", FALSE);
    } else {
        voice_speak("Failed to add student");
        log_action("Failed to add student");
        gtk_label_set_text(GTK_LABEL(app_data->status_bar), "Error: Failed to add student");
        show_notification(GTK_WINDOW(window), "Error", "Failed to add student", TRUE);
    }
    student_free(student);
}

static void on_update_student(GtkWidget *button, gpointer window) {
    const char *reg_no = gtk_editable_get_text(GTK_EDITABLE(app_data->reg_entry));
    const char *name = gtk_editable_get_text(GTK_EDITABLE(app_data->name_entry));
    const char *dept = gtk_editable_get_text(GTK_EDITABLE(app_data->dept_entry));
    const char *email = gtk_editable_get_text(GTK_EDITABLE(app_data->email_entry));
    const char *mobile = gtk_editable_get_text(GTK_EDITABLE(app_data->mobile_entry));

    if (strlen(reg_no) == 0) {
        show_notification(GTK_WINDOW(window), "Input Error", "Registration No. required", TRUE);
        voice_speak("Please enter registration number");
        return;
    }

    Student *student = student_new(reg_no, name, dept, email, mobile);
    if (db_update_student(db, student)) {
        log_action(g_strdup_printf("Updated student: %s", reg_no));
        voice_speak("Student updated successfully");
        gtk_label_set_text(GTK_LABEL(app_data->status_bar), "Student updated successfully");
        gtk_widget_queue_draw(app_data->chart_area);
        check_backup(GTK_WINDOW(window));
        show_notification(GTK_WINDOW(window), "Success", "Student updated successfully", FALSE);
    } else {
        voice_speak("Failed to update student");
        log_action("Failed to update student");
        gtk_label_set_text(GTK_LABEL(app_data->status_bar), "Error: Failed to update student");
        show_notification(GTK_WINDOW(window), "Error", "Failed to update student", TRUE);
    }
    student_free(student);
}

static void on_delete_student(GtkWidget *button, gpointer window) {
    const char *reg_no = gtk_editable_get_text(GTK_EDITABLE(app_data->reg_entry));

    if (strlen(reg_no) == 0) {
        show_notification(GTK_WINDOW(window), "Input Error", "Registration No. required", TRUE);
        voice_speak("Please enter registration number");
        return;
    }

    if (db_delete_student(db, reg_no)) {
        log_action(g_strdup_printf("Deleted student: %s", reg_no));
        voice_speak("Student deleted successfully");
        gtk_label_set_text(GTK_LABEL(app_data->status_bar), "Student deleted successfully");
        gtk_widget_queue_draw(app_data->chart_area);
        check_backup(GTK_WINDOW(window));
        show_notification(GTK_WINDOW(window), "Success", "Student deleted successfully", FALSE);
    } else {
        voice_speak("Failed to delete student");
        log_action("Failed to delete student");
        gtk_label_set_text(GTK_LABEL(app_data->status_bar), "Error: Failed to delete student");
        show_notification(GTK_WINDOW(window), "Error", "Failed to delete student", TRUE);
    }
}

static void on_key_pressed(GtkEventController *controller, guint keyval, guint keycode, GdkModifierType state, gpointer window) {
    if (state & GDK_CONTROL_MASK) {
        switch (keyval) {
            case GDK_KEY_n:
            case GDK_KEY_N:
                gtk_editable_set_text(GTK_EDITABLE(app_data->reg_entry), "");
                gtk_editable_set_text(GTK_EDITABLE(app_data->name_entry), "");
                gtk_editable_set_text(GTK_EDITABLE(app_data->dept_entry), "");
                gtk_editable_set_text(GTK_EDITABLE(app_data->email_entry), "");
                gtk_editable_set_text(GTK_EDITABLE(app_data->mobile_entry), "");
                gtk_widget_grab_focus(app_data->reg_entry);
                break;
            case GDK_KEY_s:
            case GDK_KEY_S:
                on_add_student(NULL, window);
                break;
            case GDK_KEY_u:
            case GDK_KEY_U:
                on_update_student(NULL, window);
                break;
            case GDK_KEY_d:
            case GDK_KEY_D:
                on_delete_student(NULL, window);
                break;
            case GDK_KEY_l:
            case GDK_KEY_L:
                show_list_view(window);
                break;
        }
    }
}

void show_form_view(GtkWidget *window) {
    if (app_data->form_view) {
        gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), "form");
        gtk_label_set_text(GTK_LABEL(app_data->status_bar), "Ready");
        return;
    }

    app_data->form_view = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(app_data->form_view, "card");
    gtk_widget_set_hexpand(app_data->form_view, TRUE); // Added to make form view expand horizontally

    GtkWidget *title = gtk_label_new("Student Information");
    gtk_widget_add_css_class(title, "header");
    gtk_box_append(GTK_BOX(app_data->form_view), title);

    app_data->reg_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->reg_entry), "Registration No.");
    gtk_widget_set_hexpand(app_data->reg_entry, TRUE); // Added to make entry expand horizontally
    gtk_box_append(GTK_BOX(app_data->form_view), app_data->reg_entry);

    app_data->name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->name_entry), "Name");
    gtk_widget_set_hexpand(app_data->name_entry, TRUE); // Added to make entry expand horizontally
    gtk_box_append(GTK_BOX(app_data->form_view), app_data->name_entry);

    app_data->dept_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->dept_entry), "Department");
    gtk_widget_set_hexpand(app_data->dept_entry, TRUE); // Added to make entry expand horizontally
    gtk_box_append(GTK_BOX(app_data->form_view), app_data->dept_entry);

    app_data->email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->email_entry), "Email (optional)");
    gtk_widget_set_hexpand(app_data->email_entry, TRUE); // Added to make entry expand horizontally
    gtk_box_append(GTK_BOX(app_data->form_view), app_data->email_entry);

    app_data->mobile_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->mobile_entry), "Mobile (optional)");
    gtk_widget_set_hexpand(app_data->mobile_entry, TRUE); // Added to make entry expand horizontally
    gtk_box_append(GTK_BOX(app_data->form_view), app_data->mobile_entry);

    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(button_box, TRUE); // Added to allow button box to expand horizontally

    GtkWidget *add_btn = gtk_button_new_with_label("Add");
    gtk_widget_add_css_class(add_btn, "primary");
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_student), window);
    gtk_box_append(GTK_BOX(button_box), add_btn);

    GtkWidget *update_btn = gtk_button_new_with_label("Update");
    gtk_widget_add_css_class(update_btn, "primary");
    g_signal_connect(update_btn, "clicked", G_CALLBACK(on_update_student), window);
    gtk_box_append(GTK_BOX(button_box), update_btn);

    GtkWidget *delete_btn = gtk_button_new_with_label("Delete");
    gtk_widget_add_css_class(delete_btn, "danger");
    g_signal_connect(delete_btn, "clicked", G_CALLBACK(on_delete_student), window);
    gtk_box_append(GTK_BOX(button_box), delete_btn);

    gtk_box_append(GTK_BOX(app_data->form_view), button_box);

    app_data->chart_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(app_data->chart_area, -1, 300);
    gtk_widget_set_hexpand(app_data->chart_area, TRUE); // Added to make chart expand horizontally
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(app_data->chart_area), draw_pie_chart, db, NULL);
    gtk_box_append(GTK_BOX(app_data->form_view), app_data->chart_area);

    app_data->log_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app_data->log_view), FALSE);
    app_data->log_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app_data->log_view));
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), app_data->log_view);
    gtk_widget_set_size_request(scroll, -1, 150);
    gtk_widget_set_hexpand(scroll, TRUE); // Added to make log view expand horizontally
    gtk_widget_set_vexpand(scroll, TRUE); // Added to make log view expand vertically
    gtk_box_append(GTK_BOX(app_data->form_view), scroll);

    gtk_stack_add_named(GTK_STACK(app_data->stack), app_data->form_view, "form");
    gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), "form");

    GtkEventController *key_controller = gtk_event_controller_key_new();
    g_signal_connect(key_controller, "key-pressed", G_CALLBACK(on_key_pressed), window);
    gtk_widget_add_controller(app_data->form_view, key_controller);
}