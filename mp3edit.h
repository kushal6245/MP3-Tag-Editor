#ifndef MP3EDIT_H
#define MP3EDIT_H

#define RED         "\033[31m"
#define GREEN       "\033[4;32m"
#define YELLOW      "\033[3;33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[1;35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define RED_BOLD    "\033[1;31m"
#define GREEN_BOLD  "\033[1;32m"
#define RESET       "\033[0m"

unsigned int get_tag_size(const unsigned char header[]);
void put_frame_size(unsigned char buffer[], unsigned int size);
void get_input(const char message[], char buffer[],unsigned int size);
void copy_string(char destination[], const char source[], unsigned int size);
unsigned int create_text_frame(unsigned char buffer[], const char frame_id[], const char text[]);
int extract_text_frame(const unsigned char tag[], unsigned int tag_size, const char wanted_id[], char output[], unsigned int output_size);

int copy_file_data(FILE *source, FILE *destination);

#endif