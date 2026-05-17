#include <stdio.h>
#include <string.h>
#include "view.h"  // Header file containing function declarations

// Function to read and display selected ID3 tag frames from MP3 file
void view(FILE *fp)
{
    fseek(fp, 10, SEEK_SET);  // Skip the 10-byte ID3 header
    int count = 0;  // Counter to stop after reading required 6 tags
    //We stop after 6 important frames (Title, Artist, Album, Year, Genre, Comment).

    while(count < 6) // Loop until all required tags are read
    {
        char tag[5];   
        unsigned char size_bytes[4];
        unsigned char flag_[2];
        unsigned char encoding;
        unsigned int size = 0;
        
        // Stop loop if no more frames are available
        if(fread(tag,4, 1, fp) != 1)
           break;
        tag[4] = '\0'; // Null-terminate to safely compare frame ID as string

        fread(size_bytes, 4, 1, fp);  //Read 4-byte frame size stored in big-endian format

      
        // Convert 4-byte big-endian frame size into integer using bit shifting
        //ID3 stores frame size in big-endian format, so we shift left 8 bits and combine.
        for(int i = 0; i < 4; i++)    // Convert big-endian size to integer
        {
            size = (size << 8) | (unsigned int) size_bytes[i];
        }            

        if(size == 0)   // If frame size is zero, stop reading frames
            break;

        fseek(fp, 2, SEEK_CUR);  // Skip 2-byte frame flags
        
       // First byte of frame data indicates text encoding type
       //Encoding byte tells whether text is ISO-8859-1 or Unicode.
        fread(&encoding, 1, 1, fp);   // READ encoding byte
        
        char info[size];  // Allocate buffer to store frame content. We subtract 1 because first byte is encoding, remaining bytes are text.
        fread(info, size - 1, 1, fp);
        info[size - 1 ] = '\0';
        
        if(!strcmp(tag,"TIT2") )    // Check for title frame
        {
            printf("Title   - %s\n", info );   // Print title
            count++;                          // Increment count
        }
        else if(!strcmp(tag,"TPE1"))  // TPE1 frame ID represents Artist
        {
            printf("Artist  - %s\n", info );
            count++;
        }
        else if(!strcmp(tag,"TALB"))  // TALB frame ID represents Album
        {
            printf("Album   - %s\n", info );
            count++;
        }
        else if(!strcmp(tag,"TYER"))  // TYER frame ID represents Year
        {           
            printf("Year    - %s\n", info );
            count++;
        }
        else if(!strcmp(tag,"TCON") )  // TCON frame ID represents Genre
        {
            printf("Genre   - %s\n", info );
            count++;
        }
        else if(!strcmp(tag,"COMM") )
        {
            // Skip 3-byte language code and 1 null byte in COMM frame
            printf("Comment - %s\n", info + 4);  // COMM frame ID represents Comment
            count++;
        }
        //in ID3, COMM frame format is:Encoding byte 3 bytes language 1 byte separator Actual comment text So we skip first 4 bytes to print only comment.
    }
        
}

