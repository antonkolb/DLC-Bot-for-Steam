#ifndef PARSESTORE_H
#define PARSESTORE_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "curlwrap.h"

int update_store_database( const char* store );
int parse_store( const char* out, const char* in );

/*===========================================================
=== gets all apps from Steam and stores the ids in a file ===
============================================================*/
int update_store_database( const char* store ){
    const char* steam_store_url = "http://api.steampowered.com/ISteamApps/GetAppList/v0001/";
    const char* store_tmp = "./temp/store"; //if the folder doesn't exists -> error!
    int res;
    res = get_page( steam_store_url, store_tmp );
    if( res != 0 ) return res; //abort because function wasn't successful
    res = parse_store( store_tmp, store );
    if( remove(store_tmp) ) perror("Could not delete store tmp file");
    return res;
}

/*======================================================
=== parses the copied JSON file and extracts the ids ===
======================================================*/
int parse_store( const char* in, const char* out ){

    //opening
    FILE* in_file; FILE* out_file;
    const char* key = "					\"appid\": ";
    in_file = fopen( in, "r" );
    if( !in_file ){
        perror("Failed to open store file to read");
        return errno;
    }
    out_file = fopen( out, "w" );
    if( !out_file ){
            perror("Failed to open store file to write");
            return errno;
    }

    size_t tmp_size = 100; //23 would be enough (21+new line+another one for the next 10th digit)
    char* tmp = (char*) calloc( tmp_size, sizeof(char));
    char* id = (char*) calloc( 10, sizeof(char));

    while( !feof(in_file) ){
        tmp = fgets( tmp, tmp_size, in_file );
        //get lines, that have an appid
        if( !strncmp(key, tmp, strlen(key)) ){
                id = strncpy( id, tmp+strlen(key), strlen(tmp)-strlen(key)-2); //cut away the key in the beginning and the comma/ new line at the end
                if ( 0 > fprintf(out_file, "%s\n", id) ){
                    perror( "A problem occurred while writing the appids into the store file" );
                    return errno;
                }
        }
    }

    //closing and cleanup
    int close_result;
    close_result = fclose(in_file);
    if( close_result ){
        perror("Failed to close store file after reading");
        return errno;
    }
    close_result = fclose(out_file);
    if( close_result ){
        perror("Failed to close store file after writing");
        return errno;
    }
    free( tmp );
    free( id );

    return 0;
}

#endif // PARSESTORE_H
