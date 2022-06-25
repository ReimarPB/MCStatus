#include <stdbool.h>

extern bool error_colors;

void show_color();

void hide_color();

void error_with_code();

void print_error(char *message);

void socket_error(char *message);

void system_error(char *message);

void error(char* message);

void assert_int(size_t actual, size_t expected, char* error);

