#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <errno.h>

static size_t filter_ids (void *ptr, size_t size, size_t nmemb, void *stream);
int get_page( const char* url, const char* file_name );
int parse_store( const char* out, const char* in );
int update_store_database( const char* store );

int main(void){

    const char* store = "./data/store";
    update_store_database( store );

}

/*===========================================================
=== gets all apps from Steam and stores the ids in a file ===
============================================================*/
int update_store_database( const char* store ){
    const char* steam_store_url = "http://api.steampowered.com/ISteamApps/GetAppList/v0001/";
    const char* store_tmp = "./temp/store";
    int res;
    res = get_page( steam_store_url, store_tmp );
    if( res != 0 ) return res; //abort because function wasn't successful
    res = parse_store( store_tmp, store );
    if( remove(store_tmp) ) perror("Could not delete store tmp file");
    return res;
}

/*===========================================================
=== parses the copied JSON file and extracts the ids ===
============================================================*/
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
    fclose(in_file);
    if( (int)in_file == EOF ){
        perror("Failed to close store file after reading");
        return errno;
    }
    fclose(out_file);
    if( (int)out_file == EOF ){
        perror("Failed to close store file after writing");
        return errno;
    }
    free( tmp );
    free( id );

    return 0;
}

/*===========================================================
=== makes a webcall to a given address and saves the content into a given file ===
============================================================*/
int get_page( const char* url, const char* file_name ){

    CURL *curl = curl_easy_init();
    CURLcode res = CURLE_OK;
    FILE* file;

    file = fopen(file_name, "w");
    if( !file ){ perror("Could not create file to save Steam store JSON"); return errno; }

    /* preparing curl */
    if(!curl){
        fprintf(stderr, "curl initialisation failure");
        return -1;
    }

    //setup URL
    res = curl_easy_setopt( curl, CURLOPT_URL, url );
    if( res != CURLE_OK ){
        fprintf( stderr, curl_easy_strerror(res) );
        return -1;
    }

    //setup Callback function
    /* the next option is making curl call a callback method for the downloaded content. The problem is, that it calls it several time depending on the size of the site (but still varying)
    In the case of the steam store API it was over 3000 times. This means most likely, that I receive the content in pieces with the danger, that the line with the ID or even the number itself
    might be split in between. So I probably have to save the content into a file and then parse the file myself. */
    //res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, filter_ids);

    //telling curl to write content into file
    res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    if( res != CURLE_OK ){
        fprintf( stderr, curl_easy_strerror(res) );
        return -1;
    }

    //execute
    res = curl_easy_perform(curl);
    if( res != CURLE_OK ){
        fprintf( stderr, curl_easy_strerror(res) );
        return -1;
    }

    //cleanup and closing file
    curl_easy_cleanup(curl);
    fclose( file );
    if( (int)file == EOF ){
            perror("Failed to close store file after download");
            return errno;
    }
    return 0;
}


/*========================
=== converts the JSON content of sold Steam Apps into a list with only the ids ===
=== ptr is the data from the page ===
=== nmemb is the amount of characters in the source page ===
=========================*/
//ptr is the data from the page
//nmemb is the amount of characters in the source page
static size_t filter_ids(void *ptr, size_t size, size_t nmemb, void *stream){

    /* parse content. Save IDs into a file*/

    const char* key = "  ";//                                      \"appid\":";
    // 49 printf( "key is %i long", strlen(key) );
    char* entry = strtok( (char*)ptr, "{}" ); //split the JSOn content
    while( entry ){

        if( !strncmp(key, entry, strlen(key)) ) printf( "App:\n%s\n-----------\n", entry );
        entry = strtok( NULL, "{}" );
    }

    //curl decides if the callback function is successful, by comparing the return value with the received data
    //https://stackoverflow.com/questions/46005906/libcurl-error-failed-writing-received-data-to-disk-application
    return nmemb;
}

/*========================
=== converts the JSON content of sold Steam Apps into a list with only the ids ===
=========================*//*
void convertList( const char* src, const char* dest ){

	std::ifstream inputstream( src );
	std::ofstream outputStream( dest );
	std::string str; 					//tmp string
	std::stringstream line; 			// tmp sstream
	char tmp [20]; 						//holds each ID
	int cnt = 0; 						//just to count amount of entries

	std::map<std::string, std::string> ids; //to store IDs

	while( std::getline( inputstream, str ) ){ //while not at eof
//DEBUG
//std::cout << str << std::endl;
		if( str.find( "appid" ) != std::string::npos ){ //line with appid found
			line.str(str); //turn string into stringstream
			line.get(tmp, 20, ':' ); //locate key (not needed)
			line.get(tmp, 20, ',' ); //locate value
			cnt++; //count
//DEBUG
//std::cout << tmp+2 << std::endl;
			std::string tmp_str ( tmp+2 ); //to convert into string, because with char* it get's fracked up
			ids.insert( std::pair<std::string, std::string>( tmp_str, tmp_str ) ); //tmp+2 because they are like this ": 12345"

		}
	}//while
std::cout << "Number of entries: " << cnt << std::endl;
//std::sort( ids.begin(), ids.end() ); //maybe not needed since I use a map now
//std::cout << ids.size();
	for (std::map<std::string, std::string>::const_iterator i = ids.begin(); i != ids.end(); ++i){
		//std::cout << *(ids.begin()+500) << std::endl;
		outputStream << i->first << '\n';
    }
}// convertlist
*/