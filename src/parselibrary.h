#ifndef PARSELIBRARY_H
#define PARSELIBRARY_H

#include <stdio.h>

/*============================================================================
=== splits the strings into individual ids and stores them into given file ===
============================================================================*/
void save_apps( char* app_line, FILE* file ){

    char* curr_id; char* next_id;
    const char* sep = ",";

    //get a token and the next one
    curr_id = strtok( app_line, sep);
    next_id = strtok( NULL, sep );

    //while I have two valid tokens, which means: skipping the last token (because the next one is NULL)
    while(curr_id && next_id){
        fprintf( file, "%s\n", curr_id+1 ); //all ids have a space in front
        curr_id = next_id; //moving to the next one
        next_id = strtok( NULL, sep ); //getting the next token
    }
    return;

}

/*================================================================
=== parses the licence file and extracts the lines with appids ===
================================================================*/
int parse_licences( const char* in, const char* out ){

    FILE* in_file;
    FILE* out_file;
    size_t line_size = 4095;
    char* tmp = calloc( line_size, sizeof(char) );
    const char* key = " - Apps    :"; //lines of interest start with this

    //open streams
    in_file = fopen( in, "r" );
    if( !in_file ){
        perror( "Failed to open licences file to read" );
        return errno;
    }
    out_file = fopen( out, "w" );
    if( !out_file ){
        perror( "Failed to open library file to write" );
        return errno;
   }

    while( fgets( tmp, line_size, in_file ) ){

        //if line with apps is encountered
        if( !strncmp(key, tmp, strlen(key)) ){
            char* app_line;
            //it is unlikely, but at least I want to know that it happened. For comparison: The longest line had 200+ signs
            if( tmp[strlen(tmp)-1] != '\n' ) printf( "WARNING: A line was longer then %lu! The signs after got ignored\n", line_size );
            app_line = tmp+strlen(key); // 3170, 25900, 63910, 63920, 203350, 203353, 203358,  (7 in total)
            save_apps( app_line, out_file );
        }
    }

    //closing files and freeing space
    free(tmp);
    int close_result;
    close_result = fclose(in_file);
    if( close_result ){
        perror("Failed to close licences file after reading");
        return errno;
    }
    close_result = fclose(out_file);
    if( close_result ){
        perror("Failed to close library file after writing");
        return errno;
    }

}

#endif //PARSELIBRARY_H
