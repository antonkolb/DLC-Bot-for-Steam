#include <stdio.h>

#include "parsestore.h"
#include "parselibrary.h"

int main(void){

    const char* steam_store = "./data/steam_store";
    const char* steam_licences = "./data/steam_licences";
    const char* game_library = "./data/steam_library";

    //update_store_database( steam_store );
    parse_licences( steam_licences, game_library );

}
