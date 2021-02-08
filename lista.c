#include "lista.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define ERROR -1
#define EXITO 0

struct nodo{
	void* elemento;
	struct nodo* siguiente;
};

struct lista {
	nodo_t* nodo;
	size_t cantidad_elementos;
};

struct lista_iterador{ nodo_t* nodo;
	lista_t* lista;
};

nodo_t* crear_nodo(){
	nodo_t* nodo = calloc(1, sizeof(nodo_t));
	if(!nodo)
		return NULL;
	return nodo;
}

lista_t* lista_crear(){
	lista_t* lista = calloc(1, sizeof(lista_t));
	if(!lista)
		return NULL;
	return lista;
}

int insertar_primero(lista_t* lista, void* elemento, nodo_t* nodo_siguiente){
	lista->nodo = crear_nodo();
	if(!lista->nodo)
		return ERROR;
	lista->nodo->siguiente = nodo_siguiente;
	lista->nodo->elemento = elemento;
	lista->cantidad_elementos++;
	return EXITO;		
}

int lista_insertar(lista_t* lista, void* elemento){
	if(!lista)
		return ERROR;
	if(lista->cantidad_elementos == 0){
		insertar_primero(lista, elemento, NULL);
		return EXITO;
	}
	nodo_t* nodo = lista->nodo;
	while(nodo->siguiente)
		nodo = nodo->siguiente;
	nodo_t* nuevo_nodo = crear_nodo();
	if(!nuevo_nodo)
		return ERROR;
	nuevo_nodo->elemento = elemento;
	nodo->siguiente = nuevo_nodo;
	lista->cantidad_elementos++;
	return EXITO;
}

int lista_insertar_en_posicion(lista_t* lista, void* elemento, size_t posicion){
	if(!lista)
		return ERROR;
	if(lista->cantidad_elementos == 0){
		insertar_primero(lista, elemento, NULL);
		return EXITO;
	}
	nodo_t* nodo = lista->nodo;
	if(posicion == 0){
		nodo_t* nuevo_nodo = crear_nodo();
		nuevo_nodo->siguiente = lista->nodo;
		nuevo_nodo->elemento = elemento;
		lista->nodo = nuevo_nodo;
		lista->cantidad_elementos++;
		return EXITO;
	}
	int contador = 0;
	while(nodo->siguiente && contador < (int)(posicion - 1)){
		nodo = nodo->siguiente;
		contador++;
	}
	nodo_t* nuevo_nodo = crear_nodo();
	nuevo_nodo->siguiente = nodo->siguiente;
	nodo->siguiente = nuevo_nodo;
	nuevo_nodo->elemento = elemento;
	lista->cantidad_elementos++;
	return EXITO;
}

void borrar_primero(lista_t* lista){
	nodo_t* nodo_siguiente = lista->nodo->siguiente;
	free(lista->nodo);
	lista->nodo = nodo_siguiente;
	lista->cantidad_elementos--;
}

int lista_borrar(lista_t* lista){
	if(!lista || lista->cantidad_elementos == 0)
		return ERROR;
	if(lista->cantidad_elementos == 1){
		borrar_primero(lista);
		return EXITO;
	}
	nodo_t* nodo = lista->nodo;
	nodo_t* nodo_anterior;
	while(nodo->siguiente){
		nodo_anterior = nodo;
		nodo = nodo->siguiente;
	}
	free(nodo);
	nodo_anterior->siguiente = NULL;
	lista->cantidad_elementos--;
	return EXITO;
}

int lista_borrar_de_posicion(lista_t* lista, size_t posicion){
	if(!lista) 
		return ERROR;
	nodo_t* nodo = lista->nodo;
	if(lista->cantidad_elementos == 0)
		return ERROR;
	if(posicion == 0 || !lista->nodo->siguiente){
		borrar_primero(lista);
		return EXITO;
	}
	int contador = 0;
	nodo_t* nodo_anterior;	
	while(nodo->siguiente && contador < (int)(posicion)){
		nodo_anterior = nodo;
		nodo = nodo->siguiente;
		contador++;
	}
	if(!nodo->siguiente && posicion > contador){
		nodo->elemento = NULL;
		free(nodo);
		nodo_anterior->siguiente = NULL;
	}
	else{
		nodo_anterior->siguiente = nodo->siguiente;
		nodo->elemento = NULL;
		free(nodo);	
	}
	lista->cantidad_elementos--;
	return EXITO;
}

void* lista_elemento_en_posicion(lista_t* lista, size_t posicion){
	if(!lista || posicion > lista->cantidad_elementos - 1 || lista->cantidad_elementos == 0)
		return NULL;
	size_t contador = 0;
	nodo_t* nodo = lista->nodo;
	while(contador < posicion){
		nodo = nodo->siguiente;
		contador++;
	}
	return nodo->elemento; 
}

void* lista_ultimo(lista_t* lista){
	if(!lista)
		return NULL;
	if(lista->cantidad_elementos == 0)
		return NULL;
	nodo_t* nodo = lista->nodo;
	while(nodo->siguiente)
		nodo = nodo->siguiente;
	return nodo->elemento;
}

bool lista_vacia(lista_t* lista){
	if(!lista)
		return true;
	if(lista->cantidad_elementos > 0)
		return false; 
	return true; 
}

size_t lista_elementos(lista_t* lista){
	if(!lista)
		return 0;
	return lista->cantidad_elementos;
}

int lista_apilar(lista_t* lista, void* elemento){
	if(!lista)
		return ERROR;
	nodo_t* nodo = lista->nodo;
	if(lista->cantidad_elementos == 0){
		insertar_primero(lista, elemento, NULL);
		return EXITO;
	}
	while(nodo->siguiente)
		nodo = nodo->siguiente;
	nodo_t* nuevo_nodo = crear_nodo();
	if(!nuevo_nodo)
		return ERROR;
	nuevo_nodo->elemento = elemento;
	nodo->siguiente = nuevo_nodo;
	lista->cantidad_elementos++;
	return EXITO;
}

void liberar_nodo(nodo_t* nodo){
	nodo->elemento = NULL;
	free(nodo);
}

int lista_desapilar(lista_t* lista){
	if(!lista)
		return ERROR;
	return lista_borrar(lista);
}

void* lista_tope(lista_t* lista){
	if(!lista)
		return NULL;
	if(lista->cantidad_elementos == 0)
		return NULL;
	/* nodo_t* nodo = lista->nodo; */
	/* while(nodo->siguiente){ */
	/* 	nodo = nodo->siguiente; */
	/* } */
	return lista->nodo->elemento;
}

int lista_encolar(lista_t* lista, void* elemento){
	if(!lista)
		return ERROR;
	return lista_insertar(lista, elemento);
}

int lista_desencolar(lista_t* lista){
	if(!lista)
		return ERROR;
	return lista_borrar_de_posicion(lista, 0);
}

void* lista_primero(lista_t* lista){
	if(!lista)
		return NULL;
	nodo_t* nodo = lista->nodo;
	while(nodo->siguiente){
		nodo = nodo->siguiente;
	}
	return nodo->elemento;
}

void borrar_nodo_recursivo(nodo_t* nodo){
	if(nodo->siguiente)
		borrar_nodo_recursivo(nodo->siguiente);
	free(nodo);
}

void lista_destruir(lista_t* lista){
	if(lista){
		nodo_t* nodo = lista->nodo;
		if(lista->cantidad_elementos != 0)
			borrar_nodo_recursivo(nodo);
		free(lista);
	}
}

lista_iterador_t* lista_iterador_crear(lista_t* lista){
	if(!lista)
		return NULL;
	lista_iterador_t* iterador = malloc(sizeof(lista_iterador_t));
	if(!iterador)
		return NULL;
	iterador->nodo = lista->nodo;
	iterador->lista = lista;
	return iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t* iterador){
	if(!iterador)
		return false;
	if(iterador->nodo)
		return true;
	return false;
}

void* lista_iterador_siguiente(lista_iterador_t* iterador){
	if(!iterador)
		return NULL;
	void* elemento = iterador->nodo->elemento;
	iterador->nodo = iterador->nodo->siguiente;
	return elemento;
}

void lista_iterador_destruir(lista_iterador_t* iterador){
	free(iterador);
}

void lista_con_cada_elemento(lista_t* lista, void (*funcion)(void*, void*), void* contexto){
	size_t cantidad_elementos = lista_elementos(lista);
	for(size_t i=0;i<cantidad_elementos;i++){
		void* elemento = lista_elemento_en_posicion(lista, i);
		funcion(elemento, contexto);
	}
}
