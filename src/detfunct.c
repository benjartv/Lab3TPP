#include "detfunct.h"


/*	void usage()
	Entrada: none
	Salida: none
	Func: imprime por stdout las diferentes opciones de uso del programa det
*/
void usage(){
	printf("usage: det \t-i input_matrix\n");
	printf("\t\t-N size of the matrix\n");
	printf("\t\t-H number of threads\n");
	printf("\t\t-L deep level of task (must be greater than 0)\n");
	printf("\t\t-h (Display this message)\n");
}


/*	int* read_input(char* input_file, matrix_size)
	Entrada: nombre archivo de matriz de entrada, tamaño de la matriz
	Salida: buffer con el contenido de la matriz de entrada
	Func: lee el contenido del archivo de entrada y almacena los valores de los elementos
		de la matriz en un buffer de flotantes
*/
float* read_input(char* input_file, int matrix_size){
	int t_elem_matrix = matrix_size*matrix_size;
	float* buffer = (float*)malloc(sizeof(float)*t_elem_matrix);
	int filedesc = open(input_file, O_RDONLY);
	read(filedesc, buffer, matrix_size*sizeof(float)*t_elem_matrix);
	close(filedesc);
	return buffer;
}



/*	float calculate_det(float *matrix, int mt_size, int mn_size, int n_row, int*col, int lvl)
	Entrada: buffer con los elementos de la matriz, tamaño de la matriz, tamaño del minor
		fila actual, arreglo que representa que columnas no se deben considerar para construir
		el minor, 1 si la columna debe considerarse, 0 en caso contrario, nivel de profundidad
	Saluda: determinante de la matriz
	Func: Si el minor es igual a 2, la función retorna el determinante como la multiplicación
	cruzada de los elementos del minor.
	Sino para cada índice en la fila actual, si debe considerar la columna, llama recursivamente
	a la función calculate_det, indicando un tamaño menor del minor, la fila siguiente y el 
	arreglo de columnas(con la columna actual con valor 0, para que no sea considerada por la
	siguiente iteración). Cuando recupera el valor del determinante del minor, desmarca la columna
	para que la siguiente iteración pueda considerarla al momento de calcular el determinante del nuevo
	minor. En caso de que el nivel de profundidad sea igual o mayor a 0, se creará una tarea por cada 
	elemento de esa fila, con su respectivo arreglo de columnas y luego se llamara a la función
	de manera recursiva. Al finalizar todas las tareas se sumaran los valores parciales obtenidos por
	cada una.
*/

float calculate_det(float *matrix, int mt_size, int mn_size, int n_row, int*col, int lvl){
	int i;
	float determinant;
	float* det_array;
	if (mn_size == 2){
		int c1 = -1, c2;
		for (i = 0; i < mt_size; i++){
			if (col[i] == 1){
				if (c1 == -1)
					c1 = i;
				else
					c2 = i;
			}
		}
		determinant = (matrix[mt_size*n_row + c1] * matrix[mt_size*(n_row+1) + c2]) - (matrix[mt_size*(n_row+1) + c1] * matrix[mt_size*n_row + c2]);
	}
	else{
		determinant = 0;
		int sub = 0;
		if (lvl >= 0)
		{
			//Arreglo para almacenar resultados de las tareas
			det_array = (float*)malloc(sizeof(float)*mn_size);
			for (i = 0; i < mt_size; i++)
			{
				if (col[i] == 1){
					#pragma omp task untied
					{
						int *col_vector = (int*)malloc(sizeof(int)*mt_size);
						int j;
						for (j = 0; j < mt_size; j++){
							col_vector[j] = col[j];
						}
						col_vector[i] = 0;
						det_array[sub]= pow(-1.0, sub)*matrix[mt_size*n_row + i]*calculate_det(matrix, mt_size, mn_size-1, n_row+1, col_vector, lvl-1);
						free(col_vector);
					}
					sub++;
				}
			}
			#pragma omp taskwait
			for (i = 0; i < mn_size; i++)
			{
				determinant+=det_array[i];
			}
			free(det_array);
		}
		else
		{
			for (i = 0; i < mt_size; i++)
			{
				if (col[i] == 1){
					col[i] = 0;
					determinant += pow(-1.0, sub)*matrix[mt_size*n_row + i]*calculate_det(matrix, mt_size, mn_size-1, n_row+1, col, lvl);
					//Desmarcar columna para siguiente iteración
					col[i] = 1;
					sub++;
				}
			}
		}
	}
	return(determinant);
}

/*	Options get_variables(int argc, char *argv[])
	Entrada: cantidad de argumentos de ejecucióm_size del programa, argumentos de ejecucióm_size
	Salida: estructura con la informacióm_size de los argumentos de ejecucióm_size
	Func: utilizando getopt() almecena los diferentes argumentos de ejecucióm_size en una
		estructura de datos
*/
Options get_variables(int argc, char *argv[]){
	if (argc < 9){
		usage();
		exit(1);
	}
	Options variables;
	int gopt;
	while((gopt = getopt(argc, argv, "hi:N:H:L:") ) != -1)
		switch(gopt){
			case 'h':
				usage();
				exit(1);
			case 'i':
				variables.input_file = (char*)malloc(sizeof(char)*strlen(optarg));
				strcpy(variables.input_file, optarg);
				break;
			case 'N':
				variables.matrix_size = atoi(optarg); 
				break;
			case 'H':
				variables.n_threads = atoi(optarg);
				break;
			case 'L':
				variables.lvl = atoi(optarg);
				if (variables.lvl < 0)
				{
					usage();
					exit(1);
				}
				break;
			default:
				abort();
		}
	return variables;
}