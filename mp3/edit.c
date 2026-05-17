#include <stdio.h>
#include <string.h>
#include "edit.h"
void edit(int argc, char *argv[])
{
    // Ensure correct number of command-line arguments to avoid invalid access
    if(argc != 4)
    {
        // Show correct command format if arguments are incorrect
        printf("Usage: ./a.out -t|-a|-A|-y|-g|-c \"new text\" filename.mp3\n");

        return ;
    }

    /* OPTION → FRAME ID */
    char frame_id[5] = {0};
    // strcmp returns 0 if strings match → select Title frame
    if(!strcmp(argv[1],"-t"))
    // TIT2 is ID3 frame ID for Title
       strcpy(frame_id, "TIT2");
    else if(!strcmp(argv[1],"-a"))
       strcpy(frame_id, "TALB");
    else if(!strcmp(argv[1],"-A"))
       strcpy(frame_id, "TPE1");
    else if(!strcmp(argv[1],"-y"))
       strcpy(frame_id, "TYER");
    else if(!strcmp(argv[1],"-g"))
       strcpy(frame_id, "TCON");
    else if(!strcmp(argv[1],"-c"))
       strcpy(frame_id, "COMM");
    else
    {
        // Handle unsupported tag options
        printf("Invalid option\n");
        return;
    }
    

    FILE *fp = fopen(argv[3], "rb");
    // Open MP3 in binary mode to prevent corruption of audio data
    FILE *tempfp = fopen("temp.mp3", "wb");
    // Create temporary file to safely rewrite updated MP3


    if(fp == NULL)
    {
        printf("Error: File not found\n");
        return;
    }
    if(tempfp == NULL)
    {
        printf("Error creating temp file\n");
        fclose(fp);
        return;
    }

    /* MP3 Check */
    char buffer[4];
    fread(buffer, 3, 1, fp);
    // Read first 3 bytes to check ID3 signature
    buffer[3] = '\0';

    // ID3 tag must start with "ID3" → otherwise not valid tagged MP3
    if(strcmp(buffer, "ID3") != 0)
    {
        printf("Error: Not a valid MP3 file\n");
        fclose(fp);
        fclose(tempfp);
        return;
    }
    rewind(fp);
    // Reset file pointer after checking header
    
    /* Copy ID3 header */
    unsigned char header[10];
    // ID3 header size is fixed 10 bytes
    fread(header, 10, 1, fp);
    fwrite(header, 10, 1, tempfp);

    // Loop through all ID3 frames until end or size zero
    while(1) 
    {
        char tag[5] = {0};   
        unsigned char size_bytes[4];
        unsigned char flag_[2];
        unsigned char encoding;
        unsigned int size = 0;


        /* Read frame header */
        if(fread(tag,4, 1, fp) != 1)
           break;
        // Stop if no more frames to read
        tag[4] = '\0';
        // Null-terminate to safely compare as string

        fread(size_bytes, 4, 1, fp);  
        fread(flag_, 2, 1, fp);

        
        for(int i = 0; i < 4; i++) 
        {   
            size = (size << 8) | size_bytes[i];
        } 
        // Convert 4-byte big-endian frame size into integer using bit shifting
        //ID3 stores size in big-endian format, so we shift left 8 bits and combine.

        if(size == 0)   
            break; 
        // If frame size is zero, no more valid frames

         /* EDIT FRAME */
        if(strcmp(tag, frame_id) == 0)
        // If current frame matches requested frame ID
        {
            unsigned char old_size[size];
            // Read old frame content to skip it
            fread(old_size, size, 1, fp);
            
            int len = strlen(argv[2] )+ 1;
            // +1 for encoding byte stored before text

            fwrite(tag, 4, 1, tempfp);  

            unsigned char new_size[4];
            new_size[0] = (len >> 24) & 0xFF;// Convert integer size into 4-byte big-endian format
            new_size[1] = (len >> 16) & 0xFF;
            new_size[2] = (len >> 8)  & 0xFF;
            new_size[3] =  len        & 0xFF;
            fwrite(new_size, 4, 1, tempfp);
            // Write new text into frame
            fwrite(flag_, 2, 1, tempfp);

            encoding = 0x00; // ISO-8859-1 0x00 represents ISO-8859-1 text encoding in ID3

            fwrite(&encoding, 1, 1, tempfp);

            fwrite(argv[2], strlen(argv[2]), 1, tempfp);
            continue;

        }
        /* COPY FRAME */
        else
        {
            fwrite(tag, 4, 1, tempfp);  
            fwrite(size_bytes, 4, 1, tempfp);
            fwrite(flag_, 2, 1, tempfp);

            char info[size];
            // Temporary buffer to store frame data
            fread(info, size, 1, fp);
            // Read original frame content
            fwrite(info, size, 1, tempfp);
            // Write unchanged frame into temp file
        }
        
    } 

    /* Copy remaining audio data */
    int ch;
    while((ch = fgetc(fp)) != EOF)// Copy actual MP3 audio data after ID3 frames
        fputc(ch, tempfp);


    fclose(fp);
    fclose(tempfp);
//We use temp file to avoid corrupting original file if error occurs.
    remove(argv[3]);// Delete original file after successful temp creation
    rename("temp.mp3",argv[3]); // Rename temp file to original filename (safe editing technique)
    
    printf("Edit completed successfully\n");
}
