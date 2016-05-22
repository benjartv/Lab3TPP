#include "detfunct.h"

int main(int argc, char *argv[]){
	//Obtener argumentos de entrada
	Options variables = get_variables(argc, argv);

	//Almacenar matriz en un arreglo
	float *buffer = read_input(variables.input_file, variables.matrix_size);
	float determinant = 0;

  	//Variables para calcular el tiempo
  	double time_spent;
  	struct timeval start, end;
  	gettimeofday(&start, NULL);

  	//Bloque paralelo
	#pragma omp parallel num_threads(variables.n_threads)
	{
		determinant = 0;
		//Una sola hebra va a crear las tareas
		#pragma omp single
		{
			int i;
		    for (i = 0; i < variables.matrix_size; i++)
		    {
		    	//Crea una tarea para cada elemento de la fila 0
		    	#pragma omp task untied
		    	{	
		    		int j;
		    		int *col_vector = (int*)malloc(sizeof(int)*variables.matrix_size);
				    for (j = 0; j < variables.matrix_size; j++){
				        col_vector[j] = 1;
				    }
		    		col_vector[i] = 0;
		    		determinant += pow(-1.0, i)*buffer[i]*calculate_det(buffer, variables.matrix_size, variables.matrix_size-1, 1, col_vector, variables.lvl-1);
		    		free(col_vector);
		    	}
		    }
		    //Barrera para que las todas tareas sean ejecutadas
		    #pragma omp taskwait
		}
	}

  	gettimeofday(&end, NULL);
  	time_spent = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

  	printf("Time: %f sec\n", time_spent);
	printf("Determinant: %f\n", determinant);

	return 0;
}
