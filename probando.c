#include <stdio.h>
#include "lista.h"

size_t funcion_hash(const char *key, size_t tam){
	size_t hash = 0;
	for(int i = 0;key[i];++i){
		hash = 31 * hash + key[i];
	}
	return hash % tam;
}

int main(void){
	lista_t** tabla = malloc(10*sizeof(lista_t*));
	if(!tabla) return 0;
	tabla[1] = NULL;
	lista_t* lista1 = lista_crear();
	tabla[1] = NULL;
	printf("%p",tabla[1]);
	return 0;
}

//linea 100,294