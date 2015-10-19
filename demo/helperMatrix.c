#include "ift.h"

#define MAX_STRING 100

int main(){
	int ncols, nrows;
	float value;
	char filename[MAX_STRING];

	printf("Insira Colunas e linhas e apertar ENTER\n");
	scanf("%d %d", &ncols, &nrows);

	iftMatrix* matrix = iftCreateMatrix(ncols, nrows);

	printf("Matriz com %d valores\n", matrix->n);
	for (int i = 0; i < matrix->n; i++){
		printf("insira o calor [%d, %d]: ", iftGetMatrixRow(matrix, i), iftGetMatrixCol (matrix, i));
		scanf("%f", &value);
		matrix->val[i] = value;
	}

	iftPrintMatrix(matrix);

	printf("Insira o nome do arquivo a ser salvo : ");
	scanf("%s",filename);
	iftWriteMatrix(matrix, filename);
	printf("Salvo em %s\n", filename);

	return 0;
}