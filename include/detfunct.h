#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <ctype.h>
#include <string.h>
#include <omp.h>
#include <math.h>
#include <time.h>

/*	struct getopt_opc{} Options
	nombre archivo matriz de entrada
	tamaño de la matriz
	numero de hebras
*/
typedef struct getopt_opc{
	char* input_file;
	int matrix_size;
	int n_threads;
	int lvl;
}Options;

void help();
float* read_input(char*, int);
Options get_variables(int, char **);
float calculate_det(float*, int, int, int, int*, int);
float parallel_det(int, float*, int);
