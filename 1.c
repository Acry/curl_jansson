#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
// using curl to dump json to disk via POST-Request
// here: shadertoy-shader

// see: https://jansson.readthedocs.io/en/2.11/tutorial.html
// and: https://curl.haxx.se/libcurl/c/http-post.html

#define BUFFER_SIZE  (256 * 1024)  /* 256 KB */

struct write_result
{
	char *data;
	int   pos;
};

size_t   write_response(void *, size_t , size_t , void *);
char 	*request_shader_without_key(const char *);

int main(int argc, char *argv[])
{
	// check arg
	if(argc != 2){
		fprintf(stderr, "usage: %s Shadertoy shader-ID\n", argv[0]);
		return 2;
	}

	// get shader info in json format
	char *text;
	text = request_shader_without_key(argv[1]);
	if(!text)
		return 1;

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

	free(text);
	return EXIT_SUCCESS;

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

char *request_shader_without_key(const char *shader)
{
	char buffer[128];
	CURL *curl 			= NULL;
	CURLcode status;
	struct curl_slist *headers 	= NULL;
	char *data 			= NULL;
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
	
	curl_easy_setopt(curl, CURLOPT_URL, "https://www.shadertoy.com/shadertoy");
	
	headers = curl_slist_append(headers, "Referer: https://www.shadertoy.com/");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	// set postfields
	snprintf(buffer, sizeof(buffer), "s={\"shaders\":+[\"%s\"]}", shader);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS	, buffer);
	
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION	, write_response);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA	, &write_result);
	
	status = curl_easy_perform(curl);
	if(status != 0){
		fprintf(stderr, "error: unable to request data:\n");
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
