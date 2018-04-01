#ifndef CURLWRAP_H
#define CURLWRAP_H

#include <stdio.h>
#include <curl/curl.h>
#include <errno.h>

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

#endif // CURLWRAP_H
