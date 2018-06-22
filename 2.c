#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#define CONFFILE	"api_key"
#define BUFFER_SIZE  (256 * 1024)  /* 256 KB */

#define URL_FORMAT   "https://www.shadertoy.com/api/v1/shaders/%s?key=%s"
#define URL_SIZE     256

struct write_result
{
	char *data;
	int   pos;
};

// read API key from conffile
char 	*read_file(char *);
size_t   write_response(void *, size_t , size_t , void *);
char 	*request(const char *);

int main(int argc, char *argv[])
{
	// check arg
	if(argc != 2){
		fprintf(stderr, "usage: %s shader-ID\n", argv[0]);
		return 1;
	}
	
	// read API-Key
	char *key;
	key=read_file(""CONFFILE);
	if(!key)
		return 2;
	
	// set URL
	char url[URL_SIZE];
	snprintf(url, URL_SIZE, URL_FORMAT, argv[1], key);

	// get shader info in json format
	char *text;
	text = request(url);
	if(!text)
		return 3;
	
	// save shader json
	FILE *fp;
	char buffer[64];
	snprintf(buffer, sizeof(buffer), "%s.json",argv[1]);
	fp = fopen( buffer , "w" );
	fputs(text, fp);
	fclose(fp);
	
	// set file non executable, one probably won't need this
	snprintf(buffer, sizeof(buffer), "chmod -x %s.json", argv[1]);
	system(buffer);
	
	free(key);
	free(text);
	return EXIT_SUCCESS;

}


char * read_file(char *filename)
{
	long length 	= 0;
	char *result 	= NULL;
	FILE *file 	= fopen(filename, "r");
	
	if(file) {
		int status = fseek(file, 0, SEEK_END);
		if(status != 0) {
			fclose(file);
			return NULL;
		}
		length = ftell(file);
		status = fseek(file, 0, SEEK_SET);
		if(status != 0){
			fclose(file);
			return NULL;
		}
		result = malloc((length) * sizeof(char));
		//substitute newline against string termination
		if(result) {
			size_t actual_length = fread(result, sizeof(char), length , file);
			result[actual_length-1] = '\0';
		} 
		fclose(file);
		return result;
	}
	fprintf(stderr,"Couldn't read %s\n", filename);
	return NULL;
}

size_t write_response(void *ptr, size_t size, size_t nmemb, void *stream)
{
	struct write_result *result = (struct write_result *)stream;
	
	if(result->pos + size * nmemb >= BUFFER_SIZE - 1){
		fprintf(stderr, "error: too small buffer\n");
		return 0;
	}
	
	memcpy(result->data + result->pos, ptr, size * nmemb);
	result->pos += size * nmemb;
	
	return size * nmemb;
}

char *request(const char *url)
{
	CURL *curl 		= NULL;
	CURLcode status;
	struct curl_slist *headers = NULL;
	char *data = NULL;
	long code;
	
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(!curl)
		goto error;
	
	data = malloc(BUFFER_SIZE);
	if(!data)
		goto error;
	
	struct write_result write_result = {
		.data = data,
		.pos = 0
	};
	
	curl_easy_setopt(curl, CURLOPT_URL, url);
	headers = curl_slist_append(headers, "User-Agent: Acry Shadertoy API crawler");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_result);
	
	status = curl_easy_perform(curl);
	if(status != 0){
		fprintf(stderr, "error: unable to request data from %s:\n", url);
		fprintf(stderr, "%s\n", curl_easy_strerror(status));
		goto error;
	}
	
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
	if(code != 200){
		fprintf(stderr, "error: server responded with code %ld\n", code);
		goto error;
	}
	
	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);
	curl_global_cleanup();
	
	/* zero-terminate the result */
	data[write_result.pos] = '\0';
	
	return data;
	
	error:
	if(data)
		free(data);
	if(curl)
		curl_easy_cleanup(curl);
	if(headers)
		curl_slist_free_all(headers);
	curl_global_cleanup();
	return NULL;
}
