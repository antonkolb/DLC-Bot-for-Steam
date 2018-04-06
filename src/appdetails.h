#ifndef APPDETAILS_H
#define APPDETAILS_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "cjson/cJSON.h"
#include "curlwrap.h"

int get_appdetails( const char* appid_str );
int read_file();

const char* tmp_file = "./temp/appdetails";
const char* test_file = "./temp/dlc";
struct cJSON* content;
char* type;
char* fullgame;

int get_appdetails( const char* appid_str ){

    int res = 0;
    char* prefix = "store.steampowered.com/api/appdetails?appids=";

    /*building api address*/
    char* url = (char*) calloc( strlen(prefix)+strlen(appid_str)+1, sizeof(char) );
    strcpy( url, prefix );
    strcat( url, appid_str );


    get_page( url, tmp_file );
    read_file();

    //cleanup & return
    free(url);
    return res;
}

int read_file(){

    FILE* file;
    //size_t total_size = 100;
    char* file_content = (char*) calloc( 1, sizeof(char) );
    size_t chunk_size = 100;
    char* chunk = (char*) calloc( chunk_size, sizeof(char) );

    /*opening*/
    file = fopen( test_file, "r" );
    if( !file ){
        perror( "Could not open appdetails file to read" );
        return errno;
    }

    while( fgets( chunk, chunk_size, file ) ){

        file_content = (char*) realloc( file_content, strlen(file_content)+strlen(chunk)+1 );
        if( file_content == NULL ){
            perror("Couldn't realloc memory for appdteails JSON content");
            break;
        }
        file_content = strcat( file_content, chunk );
    }

    //content = cJSON_Parse( file_content );

    /*closing and cleanup*/
    cJSON_Delete( content );
    free( file_content );
    free( chunk );
    int close_result = fclose(file);
    if( close_result ){
        perror("Failed to close library file after writing");
        return errno;
    }

    return 0;

}

#endif // APPDETAILS_H
