#ifndef _CUT_H_
#define _CUT_H_

/* 
	This project followed the Coding Challenges (https://codingchallenges.fyi/challenges/challenge-cut/) tutotial on the Unix core util Cut tool. 
	It only supports a small and simple version of the tools, mainly, -f and -d.
	It also does not support UTF-8 or multibyte characters but only supports ASCII character encoding.

	@Author: zoobaer
	@Date: 3/3/2026

*/


#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int cut_main(int argc, char** argv);

int cut_parse_fields(char* argument, int** array, int *size, int* arrayp);
int* fields_array_create(int size);
void fields_array_print(int* array, int arrayp);
int fields_array_add(int* array, int *size, int* arrayp, int value);
int fields_array_extend(int** array, int* size);
int fields_array_free(int** array);
int fields_array_field_exist(int* array, int arrayp, int field);

FILE** files_array_create(int size);
int files_array_add(FILE** files, int* size, int* arrayp, FILE* file);
int files_array_pop(FILE** files, int* arrayp);
int files_array_extend(FILE** files, int* size, int* arrayp);
int files_array_print(FILE** files, int arrayp);
int files_array_free(FILE** files, int* arrayp);

#endif