#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

// Function declarations
void list_all_processes(GtkWidget *widget, gpointer data);
void list_processes_by_user(GtkWidget *widget, gpointer data);
void run_process(GtkWidget *widget, gpointer data);
void stop_process(GtkWidget *widget, gpointer data);
void send_signal(GtkWidget *widget, gpointer data);
void update_process_list(GtkWidget *widget, gpointer data);

// Global variables
GtkWidget *process_list;
GtkWidget *status_label;
GtkWidget *user_entry;
GtkWidget *process_entry;
GtkWidget *signal_combo;

static void activate(GtkApplication *app, gpointer user_data) {
    // Create main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Process Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Create main vertical box
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    // Create button box
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append(GTK_BOX(main_box), button_box);

    // Create buttons
    GtkWidget *list_all_btn = gtk_button_new_with_label("List All Processes");
    GtkWidget *list_user_btn = gtk_button_new_with_label("List by User");
    GtkWidget *run_btn = gtk_button_new_with_label("Run Process");
    GtkWidget *stop_btn = gtk_button_new_with_label("Stop Process");
    GtkWidget *signal_btn = gtk_button_new_with_label("Send Signal");

    // Add buttons to button box
    gtk_box_append(GTK_BOX(button_box), list_all_btn);
    gtk_box_append(GTK_BOX(button_box), list_user_btn);
    gtk_box_append(GTK_BOX(button_box), run_btn);
    gtk_box_append(GTK_BOX(button_box), stop_btn);
    gtk_box_append(GTK_BOX(button_box), signal_btn);

    // Create scrolled window for process list
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_box_append(GTK_BOX(main_box), scrolled_window);

    // Create text view for process list
    process_list = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(process_list), FALSE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), process_list);

    // Create input area
    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append(GTK_BOX(main_box), input_box);

    // Create entry fields
    user_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(user_entry), "Enter username");
    process_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(process_entry), "Enter process command or ID");

    // Create signal combo box
    signal_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(signal_combo), "SIGQUIT");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(signal_combo), "SIGUSR1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(signal_combo), "SIGCONT");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(signal_combo), "SIGTERM");
    gtk_combo_box_set_active(GTK_COMBO_BOX(signal_combo), 0);

    // Add input widgets to input box
    gtk_box_append(GTK_BOX(input_box), user_entry);
    gtk_box_append(GTK_BOX(input_box), process_entry);
    gtk_box_append(GTK_BOX(input_box), signal_combo);

    // Create status label
    status_label = gtk_label_new("");
    gtk_box_append(GTK_BOX(main_box), status_label);

    // Connect signals
    g_signal_connect(list_all_btn, "clicked", G_CALLBACK(list_all_processes), NULL);
    g_signal_connect(list_user_btn, "clicked", G_CALLBACK(list_processes_by_user), NULL);
    g_signal_connect(run_btn, "clicked", G_CALLBACK(run_process), NULL);
    g_signal_connect(stop_btn, "clicked", G_CALLBACK(stop_process), NULL);
    g_signal_connect(signal_btn, "clicked", G_CALLBACK(send_signal), NULL);

    gtk_widget_show(window);
}

void update_process_list(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(process_list));
    gtk_text_buffer_set_text(buffer, (char *)data, -1);
}

void list_all_processes(GtkWidget *widget, gpointer data) {
    FILE *fp;
    char path[1035];
    GString *output = g_string_new("");

    fp = popen("ps -A", "r");
    if (fp == NULL) {
        gtk_label_set_text(GTK_LABEL(status_label), "Failed to execute command");
        return;
    }

    while (fgets(path, sizeof(path), fp) != NULL) {
        g_string_append(output, path);
    }

    pclose(fp);
    update_process_list(NULL, output->str);
    g_string_free(output, TRUE);
    gtk_label_set_text(GTK_LABEL(status_label), "Process list updated");
}

void list_processes_by_user(GtkWidget *widget, gpointer data) {
    const char *username = gtk_editable_get_text(GTK_EDITABLE(user_entry));
    if (strlen(username) == 0) {
        gtk_label_set_text(GTK_LABEL(status_label), "Please enter a username");
        return;
    }

    char command[100];
    snprintf(command, sizeof(command), "ps -u %s", username);
    
    FILE *fp;
    char path[1035];
    GString *output = g_string_new("");

    fp = popen(command, "r");
    if (fp == NULL) {
        gtk_label_set_text(GTK_LABEL(status_label), "Failed to execute command");
        return;
    }

    while (fgets(path, sizeof(path), fp) != NULL) {
        g_string_append(output, path);
    }

    pclose(fp);
    update_process_list(NULL, output->str);
    g_string_free(output, TRUE);
    gtk_label_set_text(GTK_LABEL(status_label), "User process list updated");
}

void run_process(GtkWidget *widget, gpointer data) {
    const char *command = gtk_editable_get_text(GTK_EDITABLE(process_entry));
    if (strlen(command) == 0) {
        gtk_label_set_text(GTK_LABEL(status_label), "Please enter a command");
        return;
    }

    char full_command[150];
    snprintf(full_command, sizeof(full_command), "%s &", command);
    
    if (system(full_command) == 0) {
        gtk_label_set_text(GTK_LABEL(status_label), "Process started successfully");
        list_all_processes(NULL, NULL);
    } else {
        gtk_label_set_text(GTK_LABEL(status_label), "Failed to start process");
    }
}

void stop_process(GtkWidget *widget, gpointer data) {
    const char *pid_str = gtk_editable_get_text(GTK_EDITABLE(process_entry));
    if (strlen(pid_str) == 0) {
        gtk_label_set_text(GTK_LABEL(status_label), "Please enter a process ID");
        return;
    }

    int pid = atoi(pid_str);
    if (kill(pid, SIGSTOP) == 0) {
        gtk_label_set_text(GTK_LABEL(status_label), "Process stopped successfully");
        list_all_processes(NULL, NULL);
    } else {
        gtk_label_set_text(GTK_LABEL(status_label), "Failed to stop process");
    }
}

void send_signal(GtkWidget *widget, gpointer data) {
    const char *pid_str = gtk_editable_get_text(GTK_EDITABLE(process_entry));
    if (strlen(pid_str) == 0) {
        gtk_label_set_text(GTK_LABEL(status_label), "Please enter a process ID");
        return;
    }

    int pid = atoi(pid_str);
    const char *signal_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(signal_combo));
    int signal;

    if (strcmp(signal_name, "SIGQUIT") == 0) signal = SIGQUIT;
    else if (strcmp(signal_name, "SIGUSR1") == 0) signal = SIGUSR1;
    else if (strcmp(signal_name, "SIGCONT") == 0) signal = SIGCONT;
    else if (strcmp(signal_name, "SIGTERM") == 0) signal = SIGTERM;
    else {
        gtk_label_set_text(GTK_LABEL(status_label), "Invalid signal selected");
        return;
    }

    if (kill(pid, signal) == 0) {
        gtk_label_set_text(GTK_LABEL(status_label), "Signal sent successfully");
        list_all_processes(NULL, NULL);
    } else {
        gtk_label_set_text(GTK_LABEL(status_label), "Failed to send signal");
    }
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.process.manager", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
} 