CC = gcc
CFLAGS = `pkg-config --cflags gtk4`
LIBS = `pkg-config --libs gtk4`

all: process_manager_gui

process_manager_gui: process_manager_gui.c
	$(CC) -o process_manager_gui process_manager_gui.c $(CFLAGS) $(LIBS)

clean:
	rm -f process_manager_gui 