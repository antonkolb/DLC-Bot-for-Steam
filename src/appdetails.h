#ifndef APPDETAILS_H
#define APPDETAILS_H

#include <stdio.h>
#include <string.h>
#include "curlwrap.h"

const char* tmp_file = "./temp/appdetails";
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

    //cleanup & return
    free(url);
    return res;
}

#endif // APPDETAILS_H
