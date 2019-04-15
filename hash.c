#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CAPINICIAL 10

struct hash{
	lista_t** tabla;
	size_t tam; //m		a = n / m
	size_t cant; //n
	hash_destruir_dato_t destruir_dato;
};

typedef struct hash_elem{
	void* dato;
	char* clave;
}hash_elem_t;

struct hash_iter {
	const hash_t* hash;
	lista_iter_t* act;
	size_t indice_act;
};

char *strdup (const char *s) { // Funcion que permite copiar la clave.
    char *d = malloc (strlen(s) + 1);
    if (d == NULL) return NULL;          
    strcpy (d,s);                        
    return d;                            
}

size_t funcion_hash(const char *key, size_t tam){
	size_t hash = 0;
	for(int i = 0;key[i];++i){
		hash = 31 * hash + key[i];
	}
	return hash % tam;
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if(!hash) return NULL;

	lista_t** tabla = malloc(CAPINICIAL*sizeof(lista_t*));
	if(!tabla) {
		free(hash);
		return NULL;
	} 
	hash->tam = CAPINICIAL;
	hash->tabla = tabla;
	hash->cant = 0;
	hash->destruir_dato = destruir_dato;

	for(int i = 0;i < CAPINICIAL;i++){ // Inicializo con NULL tabla de hash.
		tabla[i] = NULL;
	}
	return hash;
}

bool re_hashing(hash_t *hash){
	
	hash_t* hash_aux = hash_crear(NULL);
	if(!hash_aux) return false;

	size_t tam_nuevo = hash->tam*2;

	lista_t** tabla_nueva = malloc(tam_nuevo*sizeof(lista_t*));
	if(!tabla_nueva){
		hash_destruir(hash_aux);
		return false;
	}
	free(hash_aux->tabla);
	
	for(int i = 0;i < tam_nuevo;i++){ // Inicializo con NULL tabla nueva de hash.
		tabla_nueva[i] = NULL;
	}

	hash_aux->tabla = tabla_nueva;
	hash_aux->tam = tam_nuevo;

	for(int i = 0;i<hash->tam;i++){ // recorro tabla de hash
		if(hash->tabla[i]){ // Si existe una lista creamos un iterador.
			lista_iter_t* iter = lista_iter_crear(hash->tabla[i]);
			if(!iter){
				hash_destruir(hash_aux);
				return false;
			}
			while(!lista_iter_al_final(iter)){
				hash_elem_t* elem = lista_iter_ver_actual(iter);
				if(!hash_guardar(hash_aux,elem->clave,elem->dato)){
					hash_destruir(hash_aux);
					return false;
				}
				lista_iter_avanzar(iter);
			}
			lista_iter_destruir(iter);
		}
	}
	lista_t** aux_swap = hash->tabla; // hago un swap
	hash->tabla = hash_aux->tabla;
	hash_aux->tabla = aux_swap;
	
	hash_aux->tam = hash->tam;
	hash->tam = tam_nuevo;
	hash_destruir(hash_aux); // Se destruye hash aux junto con la tabla de hash vieja.
	return true;
}

hash_elem_t* crear_elem(const char *clave,void *dato){
	hash_elem_t* elem = malloc(sizeof(hash_elem_t));
	if(!elem) return NULL;

	char* copia_clave = strdup(clave);
	elem->clave = copia_clave;
	elem->dato = dato;
	return elem;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	
	hash_elem_t* elem = crear_elem(clave,dato);
	if(!elem) return NULL;

	//factor de carga
	size_t factor_carga = hash->cant / hash->tam; 
	if(factor_carga > 3){
		if(!re_hashing(hash)){
			free(elem->clave);
			free(elem);
			return false;
		}
	}
	size_t indice = funcion_hash(clave,hash->tam);

	if(!hash->tabla[indice]){ // No hay una lista enlazada -> la creo
		lista_t* lista = lista_crear();
		if(!lista) {
			free(elem);
			return false;
		}
		if(!lista_insertar_primero(lista,elem)){ // Inserto en la lista.
			free(elem);
			lista_destruir(lista,NULL);
			return false;
		}
		hash->cant++;
		hash->tabla[indice] = lista;
	}
	else {	// En caso de tener una lista.. 
		lista_iter_t* iter = lista_iter_crear(hash->tabla[indice]);
		if(!iter){
			free(elem);
			return false;
		}
		while(!lista_iter_al_final(iter)) { // Vemos si ya tenemos la clave.
			
			hash_elem_t* elem_act = lista_iter_ver_actual(iter);
			
			if(strcmp(elem_act->clave, clave) == 0) { // Clave repetida.
				elem_act = lista_iter_borrar(iter);
				if(hash->destruir_dato) {
					hash->destruir_dato(elem_act->dato);
				}
				free(elem_act->clave);
				free(elem_act);
				lista_iter_insertar(iter,elem);
				break;
			}
			lista_iter_avanzar(iter);
		}
		if(lista_iter_al_final(iter)){ // Estoy al final (no hay clave repetida), inserto.
			lista_iter_insertar(iter,elem);	
			hash->cant++;
		}
		lista_iter_destruir(iter);
	}
	return true;
}

size_t hash_cantidad(const hash_t *hash) {
	return hash->cant;
}

void *hash_borrar(hash_t *hash, const char *clave) {
	if(hash_cantidad(hash) == 0) return NULL;

	size_t indice = funcion_hash(clave,hash->tam);
	if(!hash->tabla[indice]) return NULL;

	lista_t* lista = hash->tabla[indice];

	lista_iter_t* iter = lista_iter_crear(lista);
	if(!iter) return NULL;

	void* dato = NULL;
	
	while(!lista_iter_al_final(iter)){

		hash_elem_t* elem_act = lista_iter_ver_actual(iter);
		if(strcmp(clave, elem_act->clave) == 0){ // Se encuntra la clave y se borra el dato.
			elem_act = lista_iter_borrar(iter);
			dato = elem_act->dato;
			free(elem_act->clave); // Se libera la copia de la clave.
			free(elem_act);
			hash->cant--;
			break;	
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);

	if(lista_esta_vacia(lista)){		// En caso de que la lista quede vacia, la borro.
		lista_destruir(lista,NULL);
		hash->tabla[indice] = NULL;
	}
	return dato; // En caso de no encontrar el dato devuelvo NULL.
}



void *hash_obtener(const hash_t *hash, const char *clave) {
	if(hash_cantidad(hash) == 0) return NULL;

	size_t indice = funcion_hash(clave,hash->tam);
	if(!hash->tabla[indice]) return NULL;

	lista_iter_t* iter = lista_iter_crear(hash->tabla[indice]);
	if(!iter) return NULL;

	void* dato = NULL;

	while(!lista_iter_al_final(iter)){

		hash_elem_t* elem_act = lista_iter_ver_actual(iter);
		if(strcmp(clave, elem_act->clave) == 0) {	
			dato = elem_act->dato;
			break;
		}
		lista_iter_avanzar(iter);
	}
	if(lista_iter_al_final(iter)){
		dato = NULL;
	}
	lista_iter_destruir(iter);
	return dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave) {
	if(hash_cantidad(hash) == 0) return false;

	size_t indice = funcion_hash(clave,hash->tam);
	if(!hash->tabla[indice]) return false;

	lista_iter_t* iter = lista_iter_crear(hash->tabla[indice]);
	if(!iter) return false;

	bool clave_encontrada = false;
	
	while(!lista_iter_al_final(iter)){

		hash_elem_t* elem_act = lista_iter_ver_actual(iter);	
		if(strcmp(clave, elem_act->clave) == 0) {
			clave_encontrada = true;
			break;
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);
	return clave_encontrada;
}


void hash_destruir(hash_t *hash){

	for (int i = 0; i < hash->tam; i++) {
		lista_t* lista_act = hash->tabla[i];

		if (lista_act){
			while (!lista_esta_vacia(lista_act)){
				hash_elem_t* elem_act = lista_borrar_primero(lista_act);	
				if (hash->destruir_dato){
					hash->destruir_dato(elem_act->dato);
				}
				free(elem_act->clave); 
				free(elem_act);
			}
			lista_destruir(lista_act, NULL);
		}
	}
	free(hash->tabla);
	free(hash);
}


/* Funciones Iterador del hash */

// Avanza a la siguiente lista, si es que es posible

lista_iter_t* iter_siguiente_lista(hash_iter_t* h_iter,size_t* indice){
	lista_iter_t* iter_act = NULL;
	size_t i = *indice;

	while(i < h_iter->hash->tam){
		if(h_iter->hash->tabla[i]){ // Lista en pos. i.
			iter_act = lista_iter_crear(h_iter->hash->tabla[i]);
			if(!iter_act) return NULL;
			break;
		}
		i++;
	}
	*indice = i;
	return iter_act;
}

// Crea iterador
hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* h_iter = malloc(sizeof(hash_iter_t));
	if(!h_iter) return NULL;

	h_iter->hash = hash;

	size_t indice = 0;
	lista_iter_t* iter_act = iter_siguiente_lista(h_iter, &indice);

	if(!iter_act && indice != hash->tam){ // Algo salio mal en iter_siguiente_lista.
		free(h_iter);					  // iter_siguiente_lista tambien devuelve NULL
		return NULL;					  // cuando se llega al final. Por eso se comprueba
	} 									  // si el indice es igual al tamaño del hash.
	h_iter->act = iter_act; 
	h_iter->indice_act = indice;
	
	return h_iter;
}

// Avanza iterado.

bool hash_iter_avanzar(hash_iter_t *iter) {
	if (hash_iter_al_final(iter)) return false;

	if(lista_iter_avanzar(iter->act)) {
		if(lista_iter_al_final(iter->act)) { // Si avanzamos y estamos al final, buscamos proxima lista.
			size_t indice = iter->indice_act + 1;
			lista_iter_t* iter_act = iter_siguiente_lista(iter, &indice);

			if (iter_act){ // Devuelve el prox. iterador.
				lista_iter_destruir(iter->act);
				iter->act = iter_act;
				iter->indice_act = indice;
				return true;
			} 
			if(indice == iter->hash->tam){ // No hay prox. iterador (estamos al final)
				lista_iter_destruir(iter->act);
				iter->act = NULL;
				iter->indice_act = indice;
				return true; 
			}
			return false; // Algo salio mal.
		} 
		return true;		
	} 
	return false;	
}

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if (hash_iter_al_final(iter)) {
		return NULL;
	} 
	hash_elem_t* elem_act = lista_iter_ver_actual(iter->act);
	return elem_act->clave;
}

// Comprueba si terminó la iteración
bool hash_iter_al_final(const hash_iter_t *iter){
	return iter->hash->tam == iter->indice_act;
}

// Destruye iterador
void hash_iter_destruir(hash_iter_t* iter){
	lista_iter_destruir(iter->act);
	free(iter);
}


