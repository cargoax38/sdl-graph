#include "../headers/utils.h"

#include <stdio.h>
#include <stdlib.h>

char* readFile(const char* path) {
	char* text = NULL;

	FILE* file;
	errno_t error;

	if((error = fopen_s(&file, path, "r")) != 0) {
		fprintf(stderr, "Error when reading the file %s, error code : %d", path, error);
	}else {
		fseek(file, 0, SEEK_END);
		long length = ftell(file);
		fseek(file, 0, SEEK_SET);
		text = (char*) malloc(length);

		if(text == NULL) {
			fclose(file);
			fprintf(stderr, "Error when allocating memory space for the file %s", path);
			return NULL;
		}

		fread(text, 1, length, file);
		fclose(file);
	}

	return text;
}
