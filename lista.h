#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct lista lista_t;

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear(void);

// Devuelve verdadero o falso, según si la lista tiene o no elementos.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un elemento al principio de la lista. En caso de error devuelve false.
// Pre: la lista fue creada.
// Post: Se agrego un nuevo elemento a la lista, dato se encuentra al principio.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un elemento al final de la lista. En caso de errorr devuelve false.
// Pre: la lista fue creada.
// Post: Se agrego un nuevo elemento a la lista, dato se encuentra al final.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Borra el primero elemento de la lista.
// Pre: la lista fue creada.
// Post: Se elimino el primer elemento de la lista.
void *lista_borrar_primero(lista_t *lista);

// Devuelve el primer elemento de la lista. En caso de estar vacia, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvio el primer elemento, cuando no esta vacia.
void *lista_ver_primero(const lista_t *lista);

// Devuelve el ultimo elemento de la lista. En caso de estar vacia, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvio el ultimo elemento, cuando no esta vacia.
void *lista_ver_ultimo(const lista_t* lista);

// Devuelve el largo de la lista.
// Pre: la lista fue creada.
// Post: se devolvio el largo de la lista.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la funcion destruir_dato por parametro,
// para cada uno de los elemento de la lista llama a destruir dato.
// Pre: la lista fue creada. Destruir dato es una funcion capaz de destruir los
// datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *)); //NO TMPO. CTE.

//	ITERADOR INTERNO

//  Se itera la lista y se le aplica a cada dato la funcion visitar.
//	Mediante un booleano que devuelve la función visitar(), 
//	la primitiva continúa o termina el recorrido sobre el TAD.
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra); //NO TMPO. CTE.

// ITERADOR EXTERNO

typedef struct lista_iter lista_iter_t;

// Crea un iterador de la lista.
// Pre: la lista fue creada.
// Post: Se devolvio el iterador de la lista. En caso de no poder crear el iterador devuelve NULL.
lista_iter_t *lista_iter_crear(lista_t *lista);

// Avanza el iterador.
// Pre: el iterador fue creado.
// Post: El iterador avanza y se devuelve true, caso contrario se devuelve false.
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el elemento que esta en la posicion del iterador.
// Pre: el iterador fue creado.
// Post: se devolvio el elemento que se encuentra en la posicion del iterador. En caso de estar al final se devuelve NULL.
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Devuelve un valor booleano indicando si el iterador esta al final.
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador.
void lista_iter_destruir(lista_iter_t *iter);

// Inserta un elemento en la posicion actual del iterador. En caso de tener algo en dicha posicion,
// el/los elemento/s permanecen a continuacion del elemento insertado.
// Pre: el iterador fue creado.
// Post: se inserta un elemento en la posicion del iterador, se devuelve true. En caso de no poder insertar, se devuelve false.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Borra el elemento que se encuentra en la posicion del iterador y devuelve dicho elemento.
// Pre: el iterador fue creado.
// Post: se borra el elemento que se encuentra en la posicion del iterador y devuelve el mismo. En caso de no poder borrar el elemento,
// se devuelve NULL.
void *lista_iter_borrar(lista_iter_t *iter);

//PRUEBAS

void pruebas_lista_alumno(void);

#endif // LISTA_H