#include <stdio.h>
#include <errno.h>
#include <uthash.h>

#include "parsestore.h"
#include "parselibrary.h"
#include "appdetails.h"

/* struct for uthash */
struct app_id{

    const char* id_as_str;   /* key only */
    UT_hash_handle hh;

};

struct app_id* my_library = NULL;                           //Hash with ids

int search_new_dlcs( const char* file_with_games );
int library_contains_app( const char* appid_str );
int create_hash( const char* licence_file );
int add2hash( char*const* appid );

int main(void){

    const char* steam_store = "./data/steam_store";         //appids from the steam store, line by line
    const char* steam_licences = "./data/steam_licences";   //output from Steam Console / SteamCMD
    const char* game_library = "./data/steam_library";      //appids from my library, line by line


    /*update lists*/
    update_store_database( steam_store );
    //parse_licences( steam_licences, game_library );

    /* create database and list unpurchased DLCs */
    //create_hash( game_library );
    //search_new_dlcs( steam_store );

    return 0;

}

int search_new_dlcs( const char* file_with_games ){

    FILE* file;
    file = fopen( file_with_games, "r" );
    if( !file ){
        perror( "main:search_new_dlcs: Could not open library file to read" );
        return errno;
    }

    size_t size = 10;
    char app_from_list[size];

                                                                                                FILE* debug = fopen( "./data/debug", "w" );
                                                                                                if( !debug ){
                                                                                                    perror( "main:search_new_dlcs: Could not open debug file to write" );
                                                                                                    return errno;
                                                                                                }

    printf( "Parsing Steam Store...\n" );
    while( fgets(app_from_list, size, file) ){
        app_from_list[strlen(app_from_list)-1] = '\0'; //remove new line character
        //printf( "%s\n", app_from_list );

        if( get_app_details(app_from_list) ) continue; //skip null pages
                                                                                                if ( 0 > fprintf(debug, "%s ( %s )", get_app_name(), app_from_list) ){
                                                                                                    perror( "main:search_new_dlcs: A problem occurred while writing the appids into the debug file" );
                                                                                                    return errno;
                                                                                                }
        //Is it a DLC?
        if( !strcmp(get_app_type(), "dlc") ){
                                                                                                if ( 0 > fprintf(debug, " is a dlc.") ){
                                                                                                    perror( "main:search_new_dlcs: A problem occurred while writing the type into the debug file" );
                                                                                                    return errno;
                                                                                                }
            //Do I own the base game?
            if( library_contains_app(get_parent_id()) ){
                                                                                                if ( 0 > fprintf(debug, " Base game is %s ( %s ).", get_parent_name(), get_parent_id() ) ){
                                                                                                    perror( "main:search_new_dlcs: A problem occurred while writing the parent into the debug file" );
                                                                                                    return errno;
                                                                                                }
                //Do I own the DLC?
                if( !library_contains_app(app_from_list) ){
                                                                                                if ( 0 > fprintf(debug, "I do not own it." ) ){
                                                                                                    perror( "main:search_new_dlcs: A problem occurred while writing the ownership into the debug file" );
                                                                                                    return errno;
                                                                                                }
                    //printf( "%s (%s). Parent is %s (%s).\n", get_app_name(), app_from_list, get_parent_name(), get_parent_id() );
                }
            }
        }

                                                                                                if ( 0 > fprintf(debug, "\n---------------\n" ) ){
                                                                                                    perror( "parseastore:parse_store: A problem occurred while writing the endline into the debug file" );
                                                                                                    return errno;
                                                                                                }
}//while fgets
                                                                                                fclose(debug);
    int close_result = fclose(file);
    if( close_result ){
        perror("main:search_new_dlcs: Failed to close library file after reading");
        return errno;
    }
    return 0;

}

/*=============================================================================
=== Looks if the ID is in the map; return 1 if he finds something, if not 0 ===
=============================================================================*/
int library_contains_app( const char* appid_str ){

    struct app_id* app = NULL;
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
        if( line[ strlen(line)-1 ] == '\n' ) line[ strlen(line)-1 ]= '\0'; //removing new line sign, if necessary (last line has no new line)
        add2hash( &line );
        line = calloc( line_size, sizeof(char) ); //for each item once

    }

    /* cleanup and return */
    //free( line ); //don't free. The items in the hash are needed!
    return 0;
}

/*=====================================================
=== wraps the steps to add a struct into the uthash ===
=====================================================*/
int add2hash( char*const* appid ){

    struct app_id* id = NULL;
    HASH_FIND_STR( my_library, *appid, id ); //check if element with id exists; There are as of now 110 apps, that I purchased twice (via bundles f.ex.)
    if( id ){
        //fprintf(stderr, "There is already an element with the this id! %s\n", *appid);
        return 1;
    }
    id = (struct app_id*) malloc( sizeof(struct app_id) );

    id->id_as_str = *appid;
    HASH_ADD_KEYPTR( hh, my_library, id->id_as_str, strlen(id->id_as_str), id ); //macro magic

    return 0;

}
