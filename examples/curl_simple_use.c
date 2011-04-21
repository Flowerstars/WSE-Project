#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>


FILE * g_fp;



size_t write_page_source_to_file(void *ptr, size_t size, size_t nmemb, void *stream)

{
    int written = fwrite(ptr, size, nmemb, (FILE *)g_fp);

    return written;
}



int main(int argc, char *argv[])

{
    CURL *curl;


    curl_global_init(CURL_GLOBAL_ALL);

    curl=curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, argv[2]); // argv[2] is the web url

    if (NULL == (g_fp=fopen(argv[1], "w"))) // argv[1] is the file name
    {
        curl_easy_cleanup(curl);

        return 0;
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_page_source_to_file);

    curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    return 1;
}

// to compile using: gcc -o curl_example curl_simple_use.c -lcurl
