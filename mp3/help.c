#include <stdio.h>



void help()
{
    printf("\nMP3 TAG EDIT HELP\n");
    printf("------------------\n");
    printf("-t  : Edit Title\n");
    printf("-a  : Edit Artist\n");
    printf("-A  : Edit Album\n");
    printf("-y  : Edit Year\n");
    printf("-g  : Edit Genre\n");
    printf("-c  : Edit Comment\n");
    printf("-h  : Help\n");

    printf("\nUsage:\n");
    printf("./a.out -v filename.mp3\n");
    printf("./a.out -t \"New Title\" filename.mp3\n");
    
}