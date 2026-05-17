#include <stdio.h>
#include <string.h>
#include "view.h"
#include "edit.h"

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Usage:\n");
        printf("./a.out -v filename.mp3\n");
        printf("./a.out -t|-a|-A|-y|-g|-c \"text\" filename.mp3\n");
        return 0;
    }

    /* HELP OPTION */
    if(strcmp(argv[1], "-h") == 0)
    {
        help();
        return 0;
    }

    if(strcmp(argv[1], "-v") == 0)
    {
        if(argc != 3)
        {
            printf("Usage: ./a.out -v filename.mp3\n");
            return 0;
        }


        FILE * fp;

        
        if((fp = fopen(argv[2], "rb+")) == NULL)
        {
            printf("Error: File not found\n");
            return 0;
        }
        

        char buffer[4];
        fread(buffer, 3, 1, fp);
        buffer[3] = '\0';

        if(strcmp(buffer, "ID3") != 0)
        {
            printf("Error: Not a valid MP3 file\n");
            fclose(fp);
            return 0;
        }
        rewind(fp);    // Move file pointer back to beginning
        view(fp);
        fclose(fp);
    }

    
    else
    {
        edit(argc, argv);  // edit() opens file itself
    }
    

    
    return 0;

}

