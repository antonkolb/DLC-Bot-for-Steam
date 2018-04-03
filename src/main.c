#include <stdio.h>
#include <errno.h>
#include <uthash.h>

#include "parsestore.h"
#include "parselibrary.h"

/* struct for uthash */
struct app_id{

    const char* id_as_str;   /* key only */
    UT_hash_handle hh;

};

struct app_id* my_library = NULL;                           //Hash with ids

int library_contains_app( const char* appid_str );
int create_hash( const char* licence_file );
int add2hash( char*const* appid );

int main(void){

    const char* steam_store = "./data/steam_store";         //appids from the steam store, line by line
    const char* steam_licences = "./data/steam_licences";   //output from Steam Console / SteamCMD
    const char* game_library = "./data/steam_library";      //appids from my library, line by line


    /*update lists*/
    //update_store_database( steam_store );
    //parse_licences( steam_licences, game_library );

    /* create database */
    create_hash( game_library );

    char* game = "607730\n";

    struct app_id* it;
    //for( it=my_library; it!=NULL; it=it->hh.next ) printf( "Item: |%s|\n", it->id_as_str );
    //printf( "There are %u items", HASH_COUNT( my_library ) );

    if( library_contains_app( game ) ) printf( "I do own %s!\n", game );
    else printf("I do NOT own %s!\n", game);



}

/*=============================================================================
=== Looks if the ID is in the map; return 1 if he finds something, if not 0 ===
=============================================================================*/
int library_contains_app( const char* appid_str ){

    struct app_id* app;
    HASH_FIND_STR( my_library, appid_str, app );

    if( app == NULL ) return 0;
    return 1;

}

/*====================================================================
=== gets the ids from the given file and puts them in a GLOBAL map ===
====================================================================*/
int create_hash( const char* licence_file ){

    FILE* in_file;
    in_file = fopen( licence_file, "r" );
    if( !in_file ){
        perror( "Could not open library file to read" );
        return errno;
    }

    size_t line_size = 10; //currently max. six digit numbers + \n + \0
    char* line = calloc( line_size, sizeof(char) );

    while( fgets( line, line_size, in_file ) ){
        line = calloc( line_size, sizeof(char) ); //for each item once
        if( line[ strlen(line)-1 ] == '\n' ) line[ strlen(line)-1 ]= '\0'; //removing new line sign, if necessary (last line has no new line)
        //printf("%s|", line);
        add2hash( &line );

    }

    /* cleanup and return */
    //free( line ); //don't free. The items in the has are needed!
    return 0;
}

/*====================================================================
=== wraps the steps to add a struct into the uthash ===
====================================================================*/
int add2hash( char*const* appid ){


    struct app_id* id;
    //HASH_FIND_STR( my_library, &line, id ); check if element with id exists; super unlikely, since Steam wants to be them unique out of personal reasons
    id = (struct app_id*) malloc( sizeof(struct app_id) );
    id->id_as_str = *appid;
    HASH_ADD_KEYPTR( hh, my_library, id->id_as_str, strlen(id->id_as_str), id ); //macro magic



    return 0;

}
