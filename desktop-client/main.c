#include <gtk-3.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int clientSocket;  
struct sockaddr_in serverAddress;

void setup_connection_to_mcu(char *ip, int port){
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(ip);

    if(connect(clientSocket,(struct sockaddr_in *) &serverAddress, sizeof(serverAddress)) < 0){
        perror("Error connecting to the microcontroller");
        exit(EXIT_FAILURE);
    }
}

void send_color_to_mcu(uint8_t red, uint8_t green, uint8_t blue){
    char buffer[50];
    sprintf(buffer, "color: %d,%d,%d", red, green, blue);

    send(clientSocket, buffer, strlen(buffer), 0);
}

void send_mode_set_to_mcu(GtkWidget *widget,GtkBuilder* builder, gpointer user_data){
    GtkComboBox *combobox = GTK_COMBO_BOX(widget);
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_combo_box_get_active_iter(combobox, &iter)) {
        model = gtk_combo_box_get_model(combobox);
        gchar *value;
        gtk_tree_model_get(model, &iter, 0, &value, -1);

        char buffer[20];
        snprintf(buffer, "mode: %s", value);
        send(clientSocket, buffer, strlen(buffer), 0);

        if(strcmp(value, "Static") == 0){
            gtk_widget_set_sensitive(gtk_builder_get_object(builder, "color_picker"), TRUE);
            gtk_widget_set_sensitive(gtk_builder_get_object(builder, "random_btn"), TRUE);
        } else {
            gtk_widget_set_sensitive(gtk_builder_get_object(builder, "color_picker"), FALSE);
            gtk_widget_set_sensitive(gtk_builder_get_object(builder, "random_btn"), FALSE);
        }

        g_free(value);
    }
}

void on_color_changed(GtkColorSelection *color_selection, gpointer user_data) {
    GdkRGBA color;
   
    gtk_color_selection_get_current_rgba(color_selection, &color);

    int red = CLAMP((uint8_t)(color.red * 255.0 + 0.5), 0, 255);
    int green = CLAMP((uint8_t)(color.green * 255.0 + 0.5), 0, 255);
    int blue = CLAMP((uint8_t)(color.blue * 255.0 + 0.5), 0, 255);

    g_print("Selected Color: %d, %d, %d\n", red, green, blue);

    send_color_to_mcu(red, green, blue);
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

    setup_connection_to_mcu("192.168.0.12", 12345);
    
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
    GtkWidget *mode_combobox = GTK_WIDGET(gtk_builder_get_object(builder, "mode_combobox"));
    GtkColorSelection *color_selection = GTK_COLOR_SELECTION(gtk_builder_get_object(builder, "color_picker"));


    gtk_builder_connect_signals(builder, NULL);

    g_signal_connect(mode_combobox, "changed", G_CALLBACK(send_mode_set_to_mcu), builder);
    g_signal_connect(color_selection, "color-changed", G_CALLBACK(on_color_changed), NULL);
    g_signal_connect(random_btn, "clicked", G_CALLBACK(set_random_color), color_selection);
    g_signal_connect(exit_btn, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}