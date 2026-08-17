#include "../headers/utils.h"

#include <stdio.h>
#include <stdlib.h>

char* readFile(const char* path) {
	char* text = NULL;

	FILE* file;
	errno_t error;

	// Read binary mode
	if((error = fopen_s(&file, path, "rb")) != 0) {
		fprintf(stderr, "Error when reading the file %s, error code : %d", path, error);
	}else {
		fseek(file, 0, SEEK_END);
		long length = ftell(file);
		rewind(file);
		//fseek(file, 0, SEEK_SET);
		text = (char*) malloc((length + 1) * sizeof(char));

		if(text == NULL) {
			fclose(file);
			fprintf(stderr, "Error when allocating memory space for the file %s", path);
			return NULL;
		}

		fread(text, 1, length, file);
		text[length] = '\0';
		fclose(file);
	}

	return text;
}
