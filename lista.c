#include "lista.h"
#include <stdlib.h>

typedef struct nodo{
	void* dato;
	struct nodo* prox;
}nodo_t;

struct lista{
	nodo_t* prim;
	nodo_t* ult;
	size_t largo;
};

struct lista_iter{
	nodo_t* act;
	nodo_t* ant;
	lista_t* lista;
};

// PRIMITIVAS LISTA ENLAZADA

lista_t *lista_crear(void){
	lista_t* lista = malloc(sizeof(lista_t));

	if(!lista){
		return NULL;
	}
	lista->prim = NULL;
	lista->ult = NULL;
	lista->largo = 0;
	return lista;
}

bool lista_esta_vacia(const lista_t *lista){
	return lista->prim == NULL;
}

nodo_t* crear_nodo(void *valor){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if(!nodo){
		return NULL;
	}
	nodo->dato = valor;
	nodo->prox = NULL;
	return nodo;
}

bool lista_insertar_primero(lista_t *lista, void *dato){
	nodo_t* nodo_nuevo = crear_nodo(dato);
	
	if(!nodo_nuevo){
		return false;
	}
	if(lista_esta_vacia(lista)){
		lista->prim = nodo_nuevo;
		lista->ult = nodo_nuevo;
		(lista->largo)++;
		return true;
	}
	nodo_nuevo->prox = lista->prim;
	lista->prim = nodo_nuevo;
	(lista->largo)++;
	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
	nodo_t* nodo_nuevo = crear_nodo(dato);
	
	if(!nodo_nuevo){
		return false;
	}
	if(lista_esta_vacia(lista)){
		lista->prim = nodo_nuevo;
		lista->ult = nodo_nuevo;
		(lista->largo)++;
		return true;
	}
	lista->ult->prox = nodo_nuevo;
	lista->ult = nodo_nuevo;
	(lista->largo)++;
	return true;
}

void *lista_borrar_primero(lista_t *lista){
	
	if(lista_esta_vacia(lista)){
		return NULL;
	}
	nodo_t* aux = lista->prim;
	void* x = aux->dato;

	if(lista->prim != lista->ult){
		lista->prim = aux->prox;
	}else{
		lista->prim = NULL;
		lista->ult = NULL;
	}
	free(aux);
	(lista->largo)--;
	return x;
}

void *lista_ver_primero(const lista_t *lista){
	if(lista_esta_vacia(lista)){
		return NULL;
	}
	return lista->prim->dato;	
}

void *lista_ver_ultimo(const lista_t* lista){
	if(lista_esta_vacia(lista)){
		return NULL;
	}
	return lista->ult->dato;
}

size_t lista_largo(const lista_t *lista){
	return lista->largo;
}

void lista_destruir(lista_t *lista, void destruir_dato(void *)){
	void* x;

	while(!lista_esta_vacia(lista)){
		x = lista_borrar_primero(lista);
		if(destruir_dato){
			destruir_dato(x);
		}
	}
	free(lista);
}

// PRIMITIVAS ITERADOR EXTERNO

lista_iter_t *lista_iter_crear(lista_t *lista){
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	
	if(!iter){
		return NULL;
	}

	iter->act = lista->prim;
	iter->ant = NULL; 
	iter->lista = lista;
	return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter){
	if(lista_iter_al_final(iter)){
		return false;
	}
	iter->ant = iter->act;
	iter->act = iter->act->prox;
	return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter){
	
	if(lista_iter_al_final(iter)){
		return NULL;
	}
	return iter->act->dato;
}

bool lista_iter_al_final(const lista_iter_t *iter){
	return !(iter->act);
}

bool lista_iter_al_principio(const lista_iter_t *iter){
	return !(iter->ant);
}

void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){

	// INSERTAR AL PRINCIPIO:

	if(lista_iter_al_principio(iter)){
		if(!lista_insertar_primero(iter->lista,dato)){
			return false;
		}	
		iter->act = iter->lista->prim;
		return true;
	}

	// INSERTAR AL FINAL

	if(lista_iter_al_final(iter)){
		if(!lista_insertar_ultimo(iter->lista,dato)){
			return false;
		}
		iter->act = iter->lista->ult;
		return true;
	}

	// INSERTAR EN EL MEDIO

	nodo_t* nodo_nuevo = malloc(sizeof(nodo_t));
	if(!nodo_nuevo){
		return false;
	}
	nodo_nuevo->dato = dato;
	iter->ant->prox = nodo_nuevo;
	nodo_nuevo->prox = iter->act;
	iter->act = nodo_nuevo;
	(iter->lista->largo)++;
	return true;
}


void *lista_iter_borrar(lista_iter_t *iter){
	if(lista_esta_vacia(iter->lista) || lista_iter_al_final(iter)){
		return NULL;
	}
	void* aux;
	if(lista_iter_al_principio(iter)){
		aux = lista_borrar_primero(iter->lista);
		iter->act = iter->lista->prim;
		return aux;
	}
	if(!iter->act->prox){ // BORRAR AL FINAL
		aux = iter->act->dato;
		free(iter->act);
		iter->act = NULL;
		iter->ant->prox = iter->act;
		iter->lista->ult = iter->ant;
		(iter->lista->largo)--;
		return aux;
	}
	//BORRAR EN EL MEDIO.
	iter->ant->prox = iter->act->prox;
	aux = iter->act->dato;
	free(iter->act);
	iter->act = iter->ant->prox;
	(iter->lista->largo)--;
	return aux;
}

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
	lista_iter_t* iter = lista_iter_crear(lista);

	while(!lista_iter_al_final(iter)){
		if(!visitar(lista_iter_ver_actual(iter),extra)){
			lista_iter_destruir(iter);
			return;
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);
}