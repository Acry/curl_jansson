#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

// Extract Shader Name from shadertoy-json
char 	*read_file(char *);

int main(int argc, char *argv[])
{
	// check arg
	if(argc != 2){
		fprintf(stderr, "usage: %s shader.json\n", argv[0]);
		return 1;
	}

	// load shader in json format
	// using my own function, not json_loadf
	// see: https://jansson.readthedocs.io/en/2.11/apiref.html#decoding
	
	char *json_file;
	json_file = read_file(argv[1]);
	if(!json_file)
		return 2;
	

	// decode json from char array
	json_error_t error;
	json_t *root;
	root = json_loads(json_file, 0, &error);
	free(json_file);
	if(!root){
		fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
		return 1;
	}
	// The root element of this JSON string is an object, not an array.
	// Arrays in JSON are denoted with square brackets [ and ], and objects
	// are denoted with curly brackets { and }.
	// You don't need to iterate over the “object” root element if you already know
	// what keys you want to access, just use:
	// json_object_get(root, "keyname");
	// until you have accessed all the values you need.

	// request info
	// I want: Shader->info->name
	const char 	*requested_info;
	json_t 		*Shader, *info, *name;
	// travel down the "objects" until name is a valid key
	Shader 	= json_object_get(root	, "Shader");
	info 	= json_object_get(Shader, "info");
	name 	= json_object_get(info	, "name");

	requested_info = json_string_value(name);
	printf("Shader-Name: %s\n",requested_info);
	
	json_decref(root);
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
		result = malloc((length+1) * sizeof(char));
		if(result) {
			size_t actual_length = fread(result, sizeof(char), length , file);
			result[actual_length++] = '\0';
		} 
		fclose(file);
		return result;
	}
	fprintf(stderr,"Couldn't read %s\n", filename);
	return NULL;
}

