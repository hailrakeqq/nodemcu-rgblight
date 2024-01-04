#include <gtk-3.0/gtk/gtk.h>

void on_color_changed(GtkColorSelection *color_selection, gpointer user_data) {
    GdkRGBA color;
   
    gtk_color_selection_get_current_rgba(color_selection, &color);

    int red = CLAMP((int)(color.red * 255.0 + 0.5), 0, 255);
    int green = CLAMP((int)(color.green * 255.0 + 0.5), 0, 255);
    int blue = CLAMP((int)(color.blue * 255.0 + 0.5), 0, 255);

    g_print("Selected Color: %d, %d, %d\n", red, green, blue);
}

void set_random_color(GtkButton *button, GtkColorSelection *color_selection, gpointer user_data){
    GdkRGBA color;
    color.red = g_random_double_range(0.0, 1.0);
    color.green = g_random_double_range(0.0, 1.0);
    color.blue = g_random_double_range(0.0, 1.0);

    gtk_color_selection_set_current_rgba(color_selection, &color);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();
    GError *error = NULL;
    gtk_builder_add_from_file(builder, "../ui/main.glade", NULL);

    if (error != NULL) {
        g_printerr("Error loading UI: %s\n", error->message);
        g_clear_error(&error);
        return 1; 
    }

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    GtkWidget *random_btn = GTK_WIDGET(gtk_builder_get_object(builder, "random_btn"));
    GtkWidget *exit_btn = GTK_WIDGET(gtk_builder_get_object(builder, "exit_btn"));

    GtkColorSelection *color_selection = GTK_COLOR_SELECTION(gtk_builder_get_object(builder, "color_picker"));

    gtk_builder_connect_signals(builder, NULL);


    g_signal_connect(color_selection, "color-changed", G_CALLBACK(on_color_changed), NULL);
    g_signal_connect(random_btn, "clicked", G_CALLBACK(set_random_color), color_selection);
    g_signal_connect(exit_btn, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}