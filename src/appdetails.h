#ifndef APPDETAILS_H
#define APPDETAILS_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "cjson/cJSON.h"
#include "curlwrap.h"

int get_app_details( const char* appid_str );
int read_file();

const char* tmp_file = "./temp/appdetails";
struct cJSON* content;

//keys
const char* data_key = "data";
const char* type_key = "type";
const char* name_key = "name";
const char* fullgame_key = "fullgame";
const char* fullgame_id_key = "appid";
const char* fullgame_name_key = "name";

//data
char app_type[100];
char app_name[500];
char parent_id[10];
char parent_name[500];

char* get_app_type(){
    return app_type;
}

char* get_app_name(){
    return app_name;
}

char* get_parent_id(){
    return parent_id;
}

char* get_parent_name(){
    return parent_name;
}

/*=============================================================================
=== main function: gets api, fills cJSON struct and extracts relevant information ===
=============================================================================*/
int get_app_details( const char* appid_str ){

    //values from previous entry not needed, so clear them
    strcpy( app_type, "" );
    strcpy( app_name, "" );
    strcpy( parent_id, "" );
    strcpy( parent_name, "" );

    int status = 0;
    char* prefix = "store.steampowered.com/api/appdetails?appids=";

    /*building api address*/
    char* url = (char*) calloc( strlen(prefix)+strlen(appid_str)+1, sizeof(char) );
    strcpy( url, prefix );
    strcat( url, appid_str );

    /*get content from API and fill JSON struct*/
    if( get_page(url, tmp_file) == EACCES ) return errno;

    if( read_file(tmp_file) ){
        status = -1; //when not valid JSON
        goto end;
    }


    /* Walking the JSON tree

    appid
        data
            type
            name
            fullgame
                appid
                name
    */

    //printf( "\n%s\n", cJSON_Print(content) );
    cJSON* c_appid = cJSON_GetObjectItemCaseSensitive( content, appid_str );
    cJSON* c_data = cJSON_GetObjectItemCaseSensitive( c_appid, data_key );

    cJSON* c_type = cJSON_GetObjectItemCaseSensitive( c_data, type_key );
    cJSON* c_name = cJSON_GetObjectItemCaseSensitive( c_data, name_key );

    cJSON* c_fullgame = cJSON_GetObjectItemCaseSensitive( c_data, fullgame_key );
    cJSON* c_fullgame_id = cJSON_GetObjectItemCaseSensitive( c_fullgame, fullgame_id_key );
    cJSON* c_fullgame_name = cJSON_GetObjectItemCaseSensitive( c_fullgame, fullgame_name_key );

    //extracting needed values, if available
    if( cJSON_IsString(c_type) && (c_type->valuestring != NULL) ) strcpy( app_type, c_type->valuestring );
    if( cJSON_IsString(c_name) && (c_name->valuestring != NULL) ) strcpy( app_name, c_name->valuestring );
    if( cJSON_IsString(c_fullgame_id) && (c_fullgame_id->valuestring != NULL) ) strcpy( parent_id,  c_fullgame_id->valuestring );
    if( cJSON_IsString(c_fullgame_name) && (c_fullgame_name->valuestring != NULL) ) strcpy( parent_name, c_fullgame_name->valuestring );
    //printf( "\n%s\n", cJSON_Print(c_type) );


    //cleanup & return
end:
    free(url);
    cJSON_Delete( content );
/*    if( remove(tmp_file) ){
        perror("appdetails:cleanup: Could not delete tmp json file file");
        status = 1;
    }*/
    return status;
}

/*===================================================
=== puts the content of a file into a JSON struct ===
===================================================*/
int read_file( char* file2read ){

    int status =  0;
    FILE* file;
    //size_t total_size = 100;
    char* file_content = (char*) calloc( 1, sizeof(char) );
    size_t chunk_size = 100;
    char* chunk = (char*) calloc( chunk_size, sizeof(char) );

    /*opening*/
    file = fopen( file2read, "r" );
    if( !file ){
        perror( "appdetails:read_file: Could not open appdetails file to read" );
        return errno;
    }

    while( fgets( chunk, chunk_size, file ) ){

        file_content = (char*) realloc( file_content, strlen(file_content)+strlen(chunk)+1 );
        if( file_content == NULL ){
            perror("appdetails:read_file: Couldn't realloc memory for appdteails JSON content");
            break;
        }
        file_content = strcat( file_content, chunk );
    }

    content = cJSON_Parse( file_content );
    if( content == NULL ){
        //perror( "appdetails:read_file: Not valid JSON!" );
        status = -1;
    }

    /*closing and cleanup*/
    free( chunk );
    free( file_content );
    int close_result = fclose(file);
    if( close_result ){
        perror("appdetails:read_file: Failed to close appdetails file after reading");
        return errno;
    }

    return status;

}

#endif // APPDETAILS_H

/* JSON content tree

appid
    data
        type
        name
        fullgame
            appid
            name
{
   "471450":{
      "success":true,
      "data":{
         "type":"dlc",
         "name":"I and Me Original Soundtrack",
         "steam_appid":471450,
         "required_age":0,
         "is_free":true,
         "detailed_description":"I and Me Original Soundtrack contains 13 tracks:<br \/>\r\n01. Main Theme<br \/>\r\n02. Bizet's Flute<br \/>\r\n03. Piano and Cello<br \/>\r\n04. Warm Strings<br \/>\r\n05. Celesta<br \/>\r\n06. Mozart's Clarinet<br \/>\r\n07. Harp in Rain<br \/>\r\n08. For Cello<br \/>\r\n09. Sound of Snow<br \/>\r\n10. Dandelion<br \/>\r\n11. Poetry of Rain<br \/>\r\n12. Wiegenlied<br \/>\r\n13. I and Me<br \/>\r\n<br \/>\r\nSoundtracks can be found in the Steam game directory:<br \/>\r\n...\\Steam\\steamapps\\common\\I and Me\\I and Me Original Soundtrack",
         "about_the_game":"I and Me Original Soundtrack contains 13 tracks:<br \/>\r\n01. Main Theme<br \/>\r\n02. Bizet's Flute<br \/>\r\n03. Piano and Cello<br \/>\r\n04. Warm Strings<br \/>\r\n05. Celesta<br \/>\r\n06. Mozart's Clarinet<br \/>\r\n07. Harp in Rain<br \/>\r\n08. For Cello<br \/>\r\n09. Sound of Snow<br \/>\r\n10. Dandelion<br \/>\r\n11. Poetry of Rain<br \/>\r\n12. Wiegenlied<br \/>\r\n13. I and Me<br \/>\r\n<br \/>\r\nSoundtracks can be found in the Steam game directory:<br \/>\r\n...\\Steam\\steamapps\\common\\I and Me\\I and Me Original Soundtrack",
         "short_description":"I and Me Original Soundtrack",
         "fullgame":{
            "appid":"399600",
            "name":"I and Me"
         },

*/
