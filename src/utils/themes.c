#include "themes.h"
#include <gtk/gtk.h>

void apply_theme(gboolean dark) {
    GtkSettings *settings = gtk_settings_get_default();
    g_object_set(settings, "gtk-application-prefer-dark-theme", dark, NULL);
}