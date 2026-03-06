#include "cut.h"

/* 
	TODO: 
		- Parse -f and -d
		- Process the flags and output the result
		- Expand ranges and add them to the array. For example -f1-5 -> -f1,2,3,4,5 => They both are treated the same.
*/


int cut_main(int argc, char** argv) {
	int i;
	int column_no;
	int line_no;
	char ch;
	int ch_count = 0;
	char delim = '\t';
	int is_delim_set = 0;
	int is_fields_set = 0;
	char ch_temp;
	int is_number_value_set = 0;
	int number_value;
	int *fields_specified_values;
	int fields_specified_valuesp = 0;
	int fields_specified_values_size = 10;
	FILE* file = NULL;
	FILE** files = NULL;
	int arrayp;
	int files_size = 10;
	int should_be_printed;
	int is_column_exists;
	int print_count;

	fields_specified_values = fields_array_create(fields_specified_values_size);
	if (fields_specified_values == NULL) {
		fprintf(stderr, "an error occurred while allocating memory for fields values\n");
		return -1;
	}

	files = files_array_create(files_size);
	if (files == NULL) {
		return -1;
	}
	arrayp = 0;

	for (i = 1; i < argc; i++) {

		if (argv[i][0] == '-' && strlen(argv[i]) > 1) { /* Probably a flag */

			switch (argv[i][1]) {
			case 'd': /* Parse -d#, -d #, -d'd', -d 'd' */

				if (strlen(argv[i]) == 2) { /* It is in the next argument */


					/* Make sure the next argument exists */
					if (argc > i) {

					/* Single character handle */
						if (strlen(argv[i + 1]) == 1) { /* -d # */
							is_delim_set = 1;
							delim = argv[i + 1][0]; 
						} /* Quotation enclosed delimiter */
						else if (strlen(argv[i + 1]) == 3 && ((argv[i + 1][0] == '\'' && argv[i + 1][2] == '\'') || (argv[i + 1][0] == '"' && argv[i + 1][2] == '"'))) { /* -d '#', -d "#" */
							is_delim_set = 1;
							delim = argv[i + 1][1];
						}
						else {
							fprintf(stderr, __FUNCTION__": incomplete value for argument -d\n");
							return -1;
						}



					}
					else {
						fprintf(stderr, __FUNCTION__": incomplete argument -d\n");
						return -1;
					}




				}
				else if (strlen(argv[i]) == 3) { /* -d# */

					is_delim_set = 1;
					delim = argv[i][2];

				}
				else if (strlen(argv[i]) == 5) { /* It probably is enclosed in quotes either '' or "" */

					if ((argv[i][2] == '\'' && argv[i][4] == '\'') || argv[i][2] == '"' && argv[i][4] == '"') { /* -d'#', -d"#" */
						is_delim_set = 1;
						delim = argv[i][3];
					}

				}


				break;
			case 'f': /* Parse -f# or -f#,# or -f"# #", -f #, -f #,#,#, -f "# # #" */
				
				/* Maybe you could detect the range and give it to a function and it will take care of it. Identify the number range. */

				if (strlen(argv[i]) == 2 && argc > i) { /* It is on the next argument */
					cut_parse_fields(argv[i + 1], &fields_specified_values, &fields_specified_values_size, &fields_specified_valuesp);
				}
				else if (strlen(argv[i]) > 2) { /* It is probably on the same arugment */
					cut_parse_fields(argv[i] + 2, &fields_specified_values, &fields_specified_values_size, &fields_specified_valuesp);
				}
				else {
					fprintf(stderr, __FUNCTION__ ": invalid argument %s\n", argv[i]);
				}

				is_fields_set = 1;

				break;
			case 'n': /* Parse -n# */

				if (strlen(argv[i]) == 2) { /* -n # */

					if (argc > i) {

						if ((sscanf(argv[i + 1], "%d", &number_value)) != 1) {
							fprintf(stderr, __FUNCTION__": invalid value for argument -n\n");
							return -1;
						}

						is_number_value_set = 1;

					}
					else {
						fprintf(stderr, __FUNCTION__": incomplete argument -n\n");
						goto end;
					}



				}
				else { /* -n# */


					if ((sscanf(argv[i] + 2, "%d", &number_value)) != 1) {
						fprintf(stderr, __FUNCTION__": invalid value for argument -n\n");
						return -1;
					}

					is_number_value_set = 1;

				}


				break;
			default:
				fprintf(stderr, __FUNCTION__": invalid flag %c\n", argv[i][1]);
				return -1;
				break;
			}

		}
		else if (argv[i][0] == '-' && strlen(argv[i]) == 1) { /* - */
			file = stdin;
		}
		else if (i > 1) { /* program_name -f# <file_name> */
			
			if (!is_fields_set) {
				fprintf(stderr, __FUNCTION__": fields flag is not set!\n");
				return -1;
			}

			file = fopen(argv[i], "r");
			if (file == NULL) {
				fprintf(stderr, __FUNCTION__": error opening file %s\n", argv[i]);
				goto end;
			}

			if (files_array_add(files, &files_size, &arrayp, file) != 0) {
				return -1;
			}

		}
		else { /* anything else */
			fprintf(stderr, __FUNCTION__": invalid flag %s\n", argv[i]);
			goto end;
		}


	}

	/* Processing the files */
	if (arrayp == 0) {
		files[0] = stdin;
		arrayp = 1;
	}

	printf("delim is %c\n", delim);

	for (i = 0; i < arrayp; i++) {

		line_no = 0;
		column_no = 1;
		print_count = 0;
		should_be_printed = fields_array_field_exist(fields_specified_values, fields_specified_valuesp, 1);
		while ((ch = fgetc(files[i])) != EOF) {

			if (ch == '\n') {
				line_no++;
				column_no = 1;
				print_count = 0;
				should_be_printed = fields_array_field_exist(fields_specified_values, fields_specified_valuesp, 1);
				putc('\n', stdout);
				continue;
			}

			if (ch == delim) {
				column_no++;

				if ((fields_array_field_exist(fields_specified_values, fields_specified_valuesp, (column_no))) == 1) {
					should_be_printed = 1;
					if (print_count > 0 || column_no == 2) { /* Sometimes, it doesn't print the delimiter. Now, it should! */
						putc(delim, stdout);
					}

					print_count++;
					continue;
				}
				else {
					should_be_printed = 0;
				}

				continue;
			}
			else {
				
			}

			if (should_be_printed) {
				
				putc(ch, stdout);
				
				ch_count++;
			}

		}



	}




end:
	/* You could deallocate stuff right here. */

	fields_array_free(&fields_specified_values);
	fields_specified_values = 0;

	files_array_free(files, &arrayp);

	
}


int cut_parse_fields(char* argument, int** array, int *size, int* arrayp) {
	int i;
	int number;
	int is_number_set = 0;
	int is_range = 0;
	int range_max;
	char ch;
	int input_count = 0;
	int offset = 0;
	int chars_consumed = 0;
	int j;
	int return_value;

	if (argument == NULL) {
		fprintf(stderr, __FUNCTION__": argument is NULL\n");
		return -1;
	}

	i = 0;
	if ((argument[0] == '\'' || argument[0] == '"') && (argument[strlen(argument) - 1] == '\'' | argument[strlen(argument) - 1]) == '"') { /* enclosed in quotations */
		i = 1;
	}


	while ((ch = argument[i]) != '\0') {
		switch (ch) {
		case '0':
			fprintf(stderr, __FUNCTION__": invalid number 0\n");
			return -1;
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':

			if (is_range && is_number_set) { /* Then we are halfway there. #- */
				j = number;
			}

			if ((input_count = sscanf(argument + i, "%d%n", &number, &chars_consumed)) != 1) {
				fprintf(stderr, __FUNCTION__": could not get any number");
				return -1;
			}

			printf("number I got is %d\n", number);

			if (is_range && is_number_set) { /* Then we are halfway there. #- */
				for (j; j <= number; j++) {
					if (fields_array_add(*array, size, arrayp, j) != 0) {
						return -1;
					}
				}
				is_number_set = 0;
				is_range = 0;

			} 
			else if (is_range && !is_number_set) {
				for (j = 1; j <= number; j++) {
					if (fields_array_add(*array, size, arrayp, j) != 0) {
						return -1;
					}
				}

				is_number_set = 0;
				is_range = 0;
			}
			else {

				if ((return_value = fields_array_add(*array, size, arrayp, number)) == 0) {
					is_number_set = 1;
				}
				else {
					return -1;
				}

			}

			break;
		case '-': /* #-# */

			if (is_range) { /* Probably a duplicate - */
				fprintf(stderr, __FUNCTION__": invalid double -!");
				return -1;
			}
			chars_consumed = 1;

			if ((strlen(argument)) == (i + 1) && is_number_set) { /* #- */
				if ((fields_array_add(*array, size, arrayp, -1)) != 0) { /* -1 mean the end of the columns. Like up to end */
					return -1;
				}

				is_number_set = 0;
				is_range = 0;

				break;
			}
			else if ((strlen(argument)) > (i + 1) && is_number_set && argument[i+1] == ',') {
				if ((fields_array_add(*array, size, arrayp, -1)) != 0) { /* -1 mean the end of the columns. Like up to end */
					return -1;
				}

				is_number_set = 0;
				is_range = 0;

				break;
			}


			if (is_number_set) { /* The range has a minimum #-# */
				is_range = 1;
			}
			else { /* the minimum is 1-# */
				is_range = 1;
			}


			break;
		case ',': /* #, # */
		case ' ': /* # # */

			chars_consumed = 1;

			break;
		case '\'':
		case '"':
		case '\0':

			chars_consumed = 1;

			goto loop_out;
			break;
		default:
			fprintf(stderr, __FUNCTION__": invalid token %c for fields\n", ch);
			return -1;
		}

		i = i + chars_consumed; /* To avoid repetitive read only from the beginning section */
	
	}
loop_out:

	fields_array_print(*array, *arrayp);

	return 0;
}

int* fields_array_create(int size) {
	int* array;

	array = malloc(sizeof(int) * size);
	if (array == NULL) {
		fprintf(stderr, __FUNCTION__": allocating memory for array failed\n");
		return NULL;
	}

	memset(array, 0, sizeof(int) * size);

	return array;
}

void fields_array_print(int* array, int arrayp) {
	int i;

	if (array == NULL) {
		fprintf(stderr, __FUNCTION__": array is NULL\n");
		return;
	}

	printf("[");
	for (i = 0; i < arrayp; i++) {
		printf("%d ", array[i]);
	}
	printf("]\n");

}

/* TODO: take into consideration the limitless range -1 */
int fields_array_element_exists(int* array, int arrayp, int value) { /* This is to avoid duplicate values */
	int i;

	if (array == NULL) {
		fprintf(stderr, __FUNCTION__": array is NULL\n");
		return -1;
	}

	for (i = 0; i < arrayp; i++) {
		if (array[i] == -1) {

			if (i == 0) {
				fprintf(stderr, __FUNCTION__": invalid position for -1\n");
				return 0;
			}
			else {
				if (array[i] <= value) {
					return 1;
				}
			}


			break;
		}
		if (array[i] == value) {
			return 1;
		}
	}

	return 0;
}

int fields_array_add(int* array, int *size, int* arrayp, int value) {
	if (array == NULL) {
		fprintf(stderr, __FUNCTION__": array is NULL\n");
		return -1;
	}

	if (size == *arrayp) {
		if (fields_array_extend(&array, size) != 0) {
			fprintf(stderr, __FUNCTION__": an error occurred while extending array\n");
			return -1;
		}
	}

	if (fields_array_element_exists(array, *arrayp, value)) {
		return 0; /* The element exists! */
	}

	array[*arrayp] = value;
	printf("element %d added %d\n", value, array[*arrayp]);
	(*arrayp)++;

	return 0;
}

int fields_array_extend(int** array, int *size) {
	int* new_array = NULL;

	if (array == NULL || (*array) == NULL) {
		fprintf(stderr, __FUNCTION__": array is NULL\n");
		return -1;
	}

	new_array = malloc(sizeof(int) * (*size * 2));
	if (new_array == NULL) {
		fprintf(stderr, __FUNCTION__": allocating memory for new array failed\n");
		return -1;
	}

	*size = *size * 2;

	free((*array));
	*array = new_array;

	return 0;
}


int fields_array_free(int** array) {
	if ((*array) == NULL) {
		fprintf(stderr, __FUNCTION__": array is NULL\n");
		return -1;
	}

	free((*array));
	return 0;
}

int fields_array_field_exist(int* array, int arrayp, int field) {
	int i;

	if (array == NULL) {
		fprintf(stderr, __FUNCTION__": array is NULL\n");
		return -1;
	}

	for (i = 0; i < arrayp; i++) {
		/* printf("%d exist in array\n", array[i]); */
		if (array[i] == field) {
			return 1;
		}
	}

	return 0;
}

FILE** files_array_create(int size) {
	FILE** files;

	files = malloc(sizeof(FILE*) * size);
	if (files == NULL) {
		fprintf(stderr, __FUNCTION__": allocating memory for files failed\n");
		return NULL;
	}

	return files;
}

int files_array_add(FILE** files, int* size, int* arrayp, FILE* file) {

	if (files == NULL) {
		fprintf(stderr, __FUNCTION__": files is null\n");
		return -1;
	}

	if (*size == *arrayp) {
		if (files_array_extend(files, size, arrayp) != 0) {
			return -1;
		}
	}

	files[*arrayp] = file;
	(*arrayp)++;

	return 0;
}

int files_array_pop(FILE** files, int* arrayp) {
	FILE* file;
	
	if (files == NULL) {
		fprintf(stderr, __FUNCTION__": files is null\n");
		return -1;
	}

	file = files[*arrayp - 1];
	(*arrayp)--;

	return file;
}

int files_array_extend(FILE** files, int* size, int* arrayp) {
	FILE** new_files;
	int i;

	if (files == NULL) {
		fprintf(stderr, __FUNCTION__": files is null\n");
		return -1;
	}

	new_files = malloc(sizeof(FILE) * (*size * 2));
	if (new_files == NULL) {
		fprintf(stderr, __FUNCTION__": allcating memory for files failed!\n");
		return -1;
	}

	for (i = 0; i < (*arrayp); i++) {
		new_files[i] = files[i];
	}

	if (files_array_free(files, arrayp) != 0) {
		return -1;
	}

	return 0;
}

int files_array_print(FILE** files, int arrayp) {

	if (files == NULL) {
		fprintf(stderr, __FUNCTION__": files is null\n");
		return -1;
	}

	printf("There are %d elements in the files array\n", arrayp);

	return 0;
}

int files_array_free(FILE** files, int* arrayp) {
	int i; 


	if (files == NULL) {
		fprintf(stderr, __FUNCTION__": files is null\n");
		return -1;
	}

	for (i = 0; i < *arrayp; i++) {
		free(files[i]);
	}
	free(files);
	*arrayp = 0;

	return 0;
}
