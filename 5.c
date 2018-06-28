#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

// Code does: load a texture needed by shader from shadertoy
// see: https://www.shadertoy.com/api

// example:
// ./5 Ms2SWW

// focus on type and existence

/*
Access the assets
-----------------

When you retrieve a shader you will see a key called "inputs",
this can be a texture/video/keyboard/sound used by the shader.

The JSON returned when accessing a shader will look like this: 
[..]
{"inputs":[{"id":17,"src":"/media/a/(hash.extension)","ctype":"texture","channel":0}
[..] 
[array] | {"object"}

What kind of hash is it? It's not sha256sum!

Don't understand the logic/purpose behind the ID also right now either;
prob. DB connection in mindbut those should be auto.

To access this specific asset you can just cut and paste this path
https://www.shadertoy.com/media/a/(hash.extension)

*/

#define CONFFILE	"api_key"
#define BUFFER_SIZE  	(256 * 1024)  /* 256 KB */

// concat base and api
#define URL_BASE	"https://www.shadertoy.com"
#define URL_API		""URL_BASE"/api/v1/shaders/%s?key=%s"

#define URL_ASSET	"https://www.shadertoy.com%s"
#define URL_FORMAT   	"https://www.shadertoy.com/api/v1/shaders/%s?key=%s"
#define URL_SIZE     	512

struct write_result
{
	char *data;
	int   pos;
};

// read file into array
char 	*read_conf(char *);
// load json

char 	*request(const char *);
size_t   write_response(void *, size_t , size_t , void *);

// load and write image
int 	 save_image( char *);

int main(int argc, char *argv[])
{
	// check arg
	if(argc != 2){
		fprintf(stderr, "usage: %s shader-ID\n", argv[0]);
		return 1;
	}

	// read API-Key
	char *key;
	key=read_conf(""CONFFILE);
	if(!key)
		return 2;
	
	// set URL
	char url[URL_SIZE];
// 	snprintf(url, URL_SIZE, URL_FORMAT, argv[1], key);
	snprintf(url, URL_SIZE, URL_API, argv[1], key);
// 	printf(""URL_API"\n", argv[1], key);
	// get shader info in json format
	char *text;
	text = request(url);
	if(!text)
		return 3;
	
	char *json_file;
	json_file = text;
	if(!json_file)
		return 2;
	
	// decode json from char array
	json_error_t error;
	json_t *root;
	root = json_loads(json_file, 0, &error);
	if(!root){
		fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
		return 1;
	}
	
	// extract name
	const char 	*requested_info;
	json_t 		*Shader, *info, *name;

	Shader 	= json_object_get(root	, "Shader");
	info 	= json_object_get(Shader, "info");
	name 	= json_object_get(info	, "name");
	
	requested_info = json_string_value(name);
	printf("Shader-Name: %s\n",requested_info);
	json_t 		*renderpass, *inputs, *id;
	renderpass 	= json_object_get(Shader, "renderpass");
	for(size_t i = 0; i < json_array_size(renderpass); i++){
		json_t *data = json_array_get(renderpass, i);

		inputs = json_object_get(data, "inputs");
		if(json_is_array(inputs)){
			for(size_t j = 0; j < json_array_size(inputs); j++){
				json_t *data 	= json_array_get(inputs	, j);
				
				id 		= json_object_get(data, "id");
				// getting ID
				if(json_is_integer(id))
					printf("integer\n");
				int number = json_integer_value(id);
				printf("ID: %d\n",number);
				
				// getting type
				json_t *type;
				type = json_object_get(data, "ctype");
				
				// only texture support for now
				requested_info  = json_string_value(type);
				printf("type: %s\n",requested_info);
				// "channel":0
				json_t *channel;
				channel= json_object_get(data, "channel");
				int chan = json_integer_value(channel);
				printf("channel: %d\n",chan);
				// "sampler":{"filter":"mipmap","wrap":"repeat","vflip":"true","srgb":"false","internal":"byte"}
				// getting partial URL
				
				json_t *src;
				src = json_object_get(data, "src");
				requested_info  = json_string_value(src);
				printf("src: %s\n",requested_info);
				char *str = strdup(requested_info);
// 				printf("%s\n", requested_info);
				char *token = strtok(str, "/");
				char *last;
				while (token != NULL){
// 					printf("%s\n", token);
					last=token;
					token = strtok(NULL, "/");
				}
				
// 				printf(str);
				printf("Filename: %s\n", last);

				// add filename and check if file is there
				// if type is texture, check and set vars
				// merge into shade it with drop events
				// first text drop
				// then file drop
					// no sopport
				// .ST.glsl ST.json
				// then FFT
				
				// save image
				char url[URL_SIZE];
				snprintf(url, URL_SIZE, URL_ASSET, requested_info);
// 				printf("%s\n",url);
// 				fflush(stdout);
				save_image(url);
			}
		}
				

	}
	json_decref(root);
	
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

char * read_conf(char *filename)
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

int save_image( char *url)
{
	CURL *image; 
	image = curl_easy_init(); 
	if( image ){ 
		// Open file 
		FILE *fp;
		fp = fopen("tex.jpg", "wb");
		// set file non executable, one probably won't need this
// 		char buffer[64];
// 		snprintf(buffer, sizeof(buffer), "chmod -x %s.json", argv[1]);
		system("chmod -x tex.jpg");
		if( fp == NULL )
			printf("no image for you");

		CURLcode imgresult;
		curl_easy_setopt(image, CURLOPT_URL, url);
		curl_easy_setopt(image, CURLOPT_WRITEFUNCTION, NULL); 
		curl_easy_setopt(image, CURLOPT_WRITEDATA, fp); 

		// Grab image 
		imgresult = curl_easy_perform(image); 
		if( imgresult ){ 
			printf("still no image for you"); 
		}

		curl_easy_cleanup(image); 
		fclose(fp); 
		return 0;
	}
	return 1; 
}
