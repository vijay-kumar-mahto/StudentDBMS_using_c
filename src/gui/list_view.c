
#include "list_view.h"
#include "../db/db.h"
#include "../utils/voice.h"
#include "../utils/logs.h"
#include "form_view.h"
#include <gtk/gtk.h>

/*
// Suppress GTK deprecation warnings for this file
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
*/

extern sqlite3 *db;
extern AppData *app_data;

static void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer window) {
    GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        char *reg_no, *name, *dept, *email, *mobile;
        gtk_tree_model_get(model, &iter,
                           0, &reg_no,
                           1, &name,
                           2, &dept,
                           3, &email,
                           4, &mobile,
                           -1);

        gtk_editable_set_text(GTK_EDITABLE(app_data->reg_entry), reg_no);
        gtk_editable_set_text(GTK_EDITABLE(app_data->name_entry), name);
        gtk_editable_set_text(GTK_EDITABLE(app_data->dept_entry), dept);
        gtk_editable_set_text(GTK_EDITABLE(app_data->email_entry), email ? email : "");
        gtk_editable_set_text(GTK_EDITABLE(app_data->mobile_entry), mobile ? mobile : "");

        g_free(reg_no);
        g_free(name);
        g_free(dept);
        g_free(email);
        g_free(mobile);

        show_form_view(window);
    }
}

void show_list_view(GtkWidget *window) {
    GtkWidget *list_view = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_widget_add_css_class(list_view, "card");

    GtkWidget *title = gtk_label_new("Student List");
    gtk_widget_add_css_class(title, "header");
    gtk_box_append(GTK_BOX(list_view), title);

    app_data->search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->search_entry), "Search by Registration No.");
    gtk_box_append(GTK_BOX(list_view), app_data->search_entry);

    GList *students = db_get_all_students(db);
    GtkListStore *store = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    for (GList *iter = students; iter != NULL; iter = iter->next) {
        Student *student = (Student *)iter->data;
        GtkTreeIter tree_iter;
        gtk_list_store_append(store, &tree_iter);
        gtk_list_store_set(store, &tree_iter,
                           0, student->reg_no,
                           1, student->name,
                           2, student->dept,
                           3, student->email,
                           4, student->mobile,
                           -1);
    }

    g_list_free_full(students, (GDestroyNotify)student_free);

    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Reg No.", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Name", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Department", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Email", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Mobile", renderer, "text", 4, NULL);

    g_signal_connect(tree_view, "row-activated", G_CALLBACK(on_row_activated), window);

    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), tree_view);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(list_view), scroll);

    gtk_stack_add_named(GTK_STACK(app_data->stack), list_view, "list");
    gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), "list");

    voice_speak("Student list displayed");
    log_action("Student list displayed");
    gtk_label_set_text(GTK_LABEL(app_data->status_bar), "Student list loaded");
}

// Re-enable warnings after this file
//#pragma GCC diagnostic pop