#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mp3edit.h"

int main()
{
    char filename[200];

    char title[200] = "";
    char artist[200] = "";
    char album[200] = "";
    char year[20] = "";
    char genre[100] = "";

    unsigned char header[10];

    //file name
    printf(CYAN "\n========================================\n"
                  "           MP3 TAG EDITOR\n"
                  "========================================\n"
           RESET);

    get_input("\nEnter MP3 filename : ", filename, sizeof(filename));

    //open original mp3 file
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf(RED_BOLD "\nError opening MP3 file.\n" RESET);

        return 1;
    }

    //Read the ID3 header (10 bytes)
    if (fread(header, 1, 10, fp) != 10)
    {
        printf(RED_BOLD "\nError reading ID3 header.\n" RESET);

        fclose(fp);
        return 1;
    }

    //Check if the file has an ID3v2 tag
    if (header[0] != 'I' ||
        header[1] != 'D' ||
        header[2] != '3')
    {
        printf(RED_BOLD
               "\nNo ID3v2 tag found.\n"
               RESET);

        fclose(fp);
        return 1;
    }

    //Check if the ID3 version is 2.3
    if (header[3] != 3)
    {
        printf(RED_BOLD
               "\nThis editor supports ID3v2.3 only.\n"
               RESET);

        fclose(fp);
        return 1;
    }


    printf(GREEN_BOLD
           "\nID3v2.3 tag found!\n"
           RESET);

    //get the size of the ID3 tag from the header
    unsigned int tag_size =
        get_tag_size(header);


    printf(YELLOW
           "Tag Size : %u bytes\n"
           RESET,
           tag_size);

    //Read the entire ID3 tag into memory
    unsigned char *tag =
        malloc(tag_size);

    if (tag == NULL)
    {
        printf(RED_BOLD
               "Memory allocation failed.\n"
               RESET);

        fclose(fp);
        return 1;
    }

    if (fread(tag, 1, tag_size, fp) != tag_size)
    {
        printf(RED_BOLD
               "Error reading ID3 tag.\n"
               RESET);

        free(tag);
        fclose(fp);

        return 1;
    }

    //Extract the existing metadata from the ID3 tag
    extract_text_frame(
        tag,
        tag_size,
        "TIT2",
        title,
        sizeof(title));

    extract_text_frame(
        tag,
        tag_size,
        "TPE1",
        artist,
        sizeof(artist));

    extract_text_frame(
        tag,
        tag_size,
        "TALB",
        album,
        sizeof(album));

    extract_text_frame(
        tag,
        tag_size,
        "TDRC",
        year,
        sizeof(year));

    extract_text_frame(
        tag,
        tag_size,
        "TCON",
        genre,
        sizeof(genre));

    //Display the current metadata to the user
    printf("\n"
           BLUE
           "========================================\n"
           "          CURRENT INFORMATION\n"
           "========================================\n"
           RESET);

    printf("Title  : %s\n", title);
    printf("Artist : %s\n", artist);
    printf("Album  : %s\n", album);
    printf("Year   : %s\n", year);
    printf("Genre  : %s\n", genre);

    //Edit the information
    printf("\n"
           MAGENTA
           "========================================\n"
           "             EDIT INFORMATION\n"
           "========================================\n"
           RESET);

    printf("\nPress ENTER to keep the current value.\n\n");


    char input[200];
    
    //Title
    get_input("Title  : ",
              input,
              sizeof(input));

    if (strlen(input) != 0)
    {
        copy_string(title,
                    input,
                    sizeof(title));
    }

    //Artist
    get_input("Artist : ",
              input,
              sizeof(input));

    if (strlen(input) != 0)
    {
        copy_string(artist,
                    input,
                    sizeof(artist));
    }

    //Album
    get_input("Album  : ",
              input,
              sizeof(input));

    if (strlen(input) != 0)
    {
        copy_string(album,
                    input,
                    sizeof(album));
    }

    //Year
    get_input("Year   : ",
              input,
              sizeof(input));

    if (strlen(input) != 0)
    {
        copy_string(year,
                    input,
                    sizeof(year));
    }

    //Genre
    get_input("Genre  : ",
              input,
              sizeof(input));

    if (strlen(input) != 0)
    {
        copy_string(genre,
                    input,
                    sizeof(genre));
    }

    //Create a new ID3 tag with the updated information
    unsigned char new_tag[8192];

    unsigned int new_tag_size = 0;


    new_tag_size +=
        create_text_frame(
            new_tag + new_tag_size,
            "TIT2",
            title);


    new_tag_size +=
        create_text_frame(
            new_tag + new_tag_size,
            "TPE1",
            artist);


    new_tag_size +=
        create_text_frame(
            new_tag + new_tag_size,
            "TALB",
            album);


    new_tag_size +=
        create_text_frame(
            new_tag + new_tag_size,
            "TDRC",
            year);


    new_tag_size +=
        create_text_frame(
            new_tag + new_tag_size,
            "TCON",
            genre);


    //Create a new ID3 header with the updated tag size
    unsigned char new_header[10];

    new_header[0] = 'I';
    new_header[1] = 'D';
    new_header[2] = '3';

    new_header[3] = 3;
    new_header[4] = 0;
    new_header[5] = 0;


    //Synchsafe integer for the new tag size
    new_header[6] =
        (new_tag_size >> 21) & 0x7F;

    new_header[7] =
        (new_tag_size >> 14) & 0x7F;

    new_header[8] =
        (new_tag_size >> 7) & 0x7F;

    new_header[9] =
        new_tag_size & 0x7F;


    //Create a new MP3 file to write the updated tag and original audio data
    FILE *out = fopen("edited.mp3", "wb");

    if (out == NULL)
    {
        printf(RED_BOLD
               "\nError creating edited.mp3\n"
               RESET);

        free(tag);
        fclose(fp);

        return 1;
    }


    //Write the new ID3 header to the output file
    if (fwrite(new_header,
               1,
               10,
               out) != 10)
    {
        printf(RED_BOLD
               "Error writing new header.\n"
               RESET);

        free(tag);
        fclose(fp);
        fclose(out);

        return 1;
    }

    //Write the new ID3 tag to the output file
    if (fwrite(new_tag,
               1,
               new_tag_size,
               out) != new_tag_size)
    {
        printf(RED_BOLD
               "Error writing new tag.\n"
               RESET);

        free(tag);
        fclose(fp);
        fclose(out);

        return 1;
    }

    //Copy the original audio data from the input file to the output file
    printf(YELLOW
           "\nCopying original audio data...\n"
           RESET);


    if (copy_file_data(fp, out) != 0)
    {
        printf(RED_BOLD
               "Error copying audio data.\n"
               RESET);

        free(tag);
        fclose(fp);
        fclose(out);

        return 1;
    }

    //Clean up and close files
    free(tag);

    fclose(fp);
    fclose(out);


    //Success message
    printf("\n"
           GREEN_BOLD
           "========================================\n"
           "       TAGS EDITED SUCCESSFULLY!\n"
           "========================================\n"
           RESET);

    printf("\nOutput file : edited.mp3\n");


    return 0;
}

//get the size of the ID3 tag from the header
unsigned int get_tag_size(
        const unsigned char header[])
{
    return ((header[6] & 0x7F) << 21)
         | ((header[7] & 0x7F) << 14)
         | ((header[8] & 0x7F) << 7)
         |  (header[9] & 0x7F);
}


//Put frame size
void put_frame_size(
        unsigned char buffer[],
        unsigned int size)
{
    buffer[0] = (size >> 24) & 0xFF;
    buffer[1] = (size >> 16) & 0xFF;
    buffer[2] = (size >> 8) & 0xFF;
    buffer[3] = size & 0xFF;
}


//Create text frame
unsigned int create_text_frame(
        unsigned char buffer[],
        const char frame_id[],
        const char text[])
{
    unsigned int text_length =
        strlen(text);

    unsigned int data_size =
        1 + text_length;

    unsigned int total_size =
        10 + data_size;

    unsigned int index = 0;


    /* Frame ID */

    buffer[index++] = frame_id[0];
    buffer[index++] = frame_id[1];
    buffer[index++] = frame_id[2];
    buffer[index++] = frame_id[3];


    /* Frame size */

    put_frame_size(
        buffer + index,
        data_size);

    index += 4;


    /* Flags */

    buffer[index++] = 0;
    buffer[index++] = 0;


    /* Encoding */

    buffer[index++] = 0;


    /* Text */

    memcpy(buffer + index,
           text,
           text_length);

    return total_size;
}


//get user input with a prompt message
void get_input(
        const char message[],
        char buffer[],
        unsigned int size)
{
    printf("%s", message);

    if (fgets(buffer, size, stdin) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}


//copy string with size limit
void copy_string(
        char destination[],
        const char source[],
        unsigned int size)
{
    strncpy(destination,
            source,
            size - 1);

    destination[size - 1] = '\0';
}


//extract text frame from the ID3 tag
int extract_text_frame(
        const unsigned char tag[],
        unsigned int tag_size,
        const char wanted_id[],
        char output[],
        unsigned int output_size)
{
    unsigned int position = 0;


    while (position + 10 <= tag_size)
    {
        char frame_id[5];

        frame_id[0] = tag[position];
        frame_id[1] = tag[position + 1];
        frame_id[2] = tag[position + 2];
        frame_id[3] = tag[position + 3];
        frame_id[4] = '\0';


        /* Padding */

        if (frame_id[0] == '\0')
        {
            break;
        }


        /* Frame size */

        unsigned int frame_size =
            ((tag[position + 4] << 24)
           | (tag[position + 5] << 16)
           | (tag[position + 6] << 8)
           |  tag[position + 7]);


        /* Prevent invalid frame */

        if (frame_size >
            tag_size - position - 10)
        {
            break;
        }


        /* Check requested frame */

        if (strcmp(frame_id, wanted_id) == 0)
        {
            if (frame_size <= 1)
            {
                output[0] = '\0';
                return 1;
            }


            /*
               First byte is encoding.
               Text starts at +1.
            */

            unsigned int text_length =
                frame_size - 1;


            if (text_length >= output_size)
            {
                text_length =
                    output_size - 1;
            }


            memcpy(output,
                   tag + position + 11,
                   text_length);

            output[text_length] = '\0';

            return 1;
        }


        position += 10 + frame_size;
    }


    output[0] = '\0';

    return 0;
}


//copy file data from source to destination
int copy_file_data(
        FILE *source,
        FILE *destination)
{
    unsigned char buffer[4096];

    size_t bytes;


    while ((bytes =
            fread(buffer,
                  1,
                  sizeof(buffer),
                  source)) > 0)
    {
        if (fwrite(buffer,
                   1,
                   bytes,
                   destination) != bytes)
        {
            return 1;
        }
    }


    if (ferror(source))
    {
        return 1;
    }


    return 0;
}