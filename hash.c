#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hash.h"
#include "lista.h"
#include "hash_iterador.h"

#define EXITO 0
#define ERROR -1
#define MIN_CAPACIDAD 3
#define FACTOR_OCUPACION_MAX 75

typedef struct elemento_hash { char* clave;
   void* elemento;
} elemento_hash_t;

typedef struct posicion {
   int ocupacion;
   lista_t* lista;
} posicion_t;

struct hash {
   posicion_t** posiciones;
   int ocupacion;
   size_t capacidad;
   hash_destruir_dato_t destructor;
};

struct hash_iter {
   hash_t* hash;
   int posicion_actual;
   lista_iterador_t* iterador_lista;
};

 /* -------------------------- HASH.H ---------------------------- */

int funcion_hash(const char* clave){
   int numero = 0;
   int i = 0;
   while(clave[i]){
      if(clave[i] >= '0' && clave[i] <= '9')
         numero += *(int*)clave;
      i++;
   }
   if(numero == 0)
      numero += (int)strlen(clave) * i;
   return numero>=0?numero:numero*(-1);
}

posicion_t* posicion_crear(){
   posicion_t* posicion = calloc(1, sizeof(posicion_t));
   if(!posicion) return NULL;
   posicion->lista = lista_crear();
   if(!posicion->lista){
      free(posicion);
      return NULL;
   }
   return posicion;
}

posicion_t** crear_vector(size_t capacidad){
   posicion_t** posiciones = calloc(1, capacidad * sizeof(posicion_t));
   if(!posiciones) return NULL;
   for(int i = 0; i < capacidad; i++)
      posiciones[i] = posicion_crear();
   return posiciones;
}

hash_t* hash_crear(hash_destruir_dato_t destruir_elemento, size_t capacidad){
   hash_t* hash = calloc(1, sizeof(hash_t)); 
   if(!hash) return NULL;
   hash->capacidad = capacidad<MIN_CAPACIDAD?MIN_CAPACIDAD:capacidad;
   hash->posiciones = crear_vector(hash->capacidad);
   if(!hash->posiciones){
      free(hash);
      return NULL;
   } 
   hash->destructor = destruir_elemento;
   return hash;
}

int calcular_porcentaje(size_t a, int b){
   return (b*100)/(int)a;
}

int encontrar_siguiente_lista(hash_t* hash, int posicion_actual){
   bool pos_vacia = true;
   int i = posicion_actual + 1;
   while(pos_vacia && i < hash->capacidad){
      if(hash->posiciones[i]->ocupacion > 0)
         pos_vacia = false;
      if(pos_vacia)
         i++;
   }
   return pos_vacia?ERROR:i;
}

lista_iterador_t* cambiar_iterador_lista(hash_iterador_t* iterador_hash, int* posicion){
   lista_iterador_destruir(iterador_hash->iterador_lista);
   *posicion = encontrar_siguiente_lista(iterador_hash->hash, iterador_hash->posicion_actual);
   if(*posicion == ERROR) return NULL;
   return lista_iterador_crear(iterador_hash->hash->posiciones[*posicion]->lista);
}

elemento_hash_t* hash_iterador_siguiente_elemento(hash_iterador_t* iterador){   
   if(!iterador) return NULL;
   elemento_hash_t* elemento_hash = NULL;
   if(lista_iterador_tiene_siguiente(iterador->iterador_lista))
      elemento_hash = (elemento_hash_t*)lista_iterador_siguiente(iterador->iterador_lista);
   else{
      if(iterador->posicion_actual + 1 > iterador->hash->capacidad - 1)
         return NULL;
      int posicion;
      iterador->iterador_lista = cambiar_iterador_lista(iterador, &posicion);
      if(!iterador->iterador_lista) return NULL;
      iterador->posicion_actual = posicion;
      elemento_hash = (elemento_hash_t*)lista_iterador_siguiente(iterador->iterador_lista);
   }
   return elemento_hash;
}

size_t calcular_nueva_capacidad(size_t capacidad_vieja){
   return 2 * capacidad_vieja;
}

elemento_hash_t* crear_elemento(const char* clave, void* elemento){
   elemento_hash_t* elemento_hash = calloc(1, sizeof(elemento_hash_t));
   if(!elemento_hash) return NULL;
   elemento_hash->clave = malloc(strlen(clave) + 1);
   if(!elemento_hash->clave){
      free(elemento_hash);
      return NULL;
   }
   strcpy(elemento_hash->clave, clave);
   elemento_hash->elemento = elemento;
   return elemento_hash;
}

void insertar_en_posicion(posicion_t* posicion, void* elemento, const char* clave, elemento_hash_t* elemento_hash){ 
   if(elemento_hash)
      lista_insertar(posicion->lista, elemento_hash);
   else
      lista_insertar(posicion->lista, crear_elemento(clave, elemento));
   posicion->ocupacion++;
}

void liberar_vector(posicion_t** posiciones, size_t cantidad){
   for(int i = 0; i < cantidad; i++){
      lista_destruir(posiciones[i]->lista);
      free(posiciones[i]);
   }
   free(posiciones);
}

posicion_t** rehash(hash_t* hash){
   size_t nueva_capacidad = calcular_nueva_capacidad(hash->capacidad);
   posicion_t** posiciones_aux = crear_vector(nueva_capacidad);
   hash_iterador_t* iterador = hash_iterador_crear(hash);
   while(hash_iterador_tiene_siguiente(iterador)){
      elemento_hash_t* elemento_hash = hash_iterador_siguiente_elemento(iterador);
      int posicion = funcion_hash(elemento_hash->clave) % (int)nueva_capacidad;
      insertar_en_posicion(posiciones_aux[posicion], NULL, NULL, elemento_hash);
   }
   liberar_vector(hash->posiciones, hash->capacidad);
   hash_iterador_destruir(iterador);
   hash->capacidad = nueva_capacidad;
   return posiciones_aux;
}

elemento_hash_t* encontrar_elemento(lista_t* lista, const char* clave){
   lista_iterador_t* iter = lista_iterador_crear(lista);
   bool encontrado = false;
   elemento_hash_t* elemento = NULL;
   while(lista_iterador_tiene_siguiente(iter) && !encontrado){
      elemento = (elemento_hash_t*)lista_iterador_siguiente(iter);
      encontrado = strcmp(elemento->clave, clave) == 0?true:false;
   }
   lista_iterador_destruir(iter);
   return encontrado?elemento:NULL;
}

int actualizar_valor(elemento_hash_t* elemento_hash, void* nuevo_elemento, hash_destruir_dato_t destructor){
   if(destructor)
      destructor(elemento_hash->elemento);
   elemento_hash->elemento = nuevo_elemento;
   return EXITO;
}

int hash_insertar(hash_t* hash, const char* clave, void* elemento){
   if(!hash || !clave) return ERROR;
   int posicion = funcion_hash(clave) % (int)hash->capacidad;
   elemento_hash_t* elemento_repetido = encontrar_elemento(hash->posiciones[posicion]->lista, clave);
   if(elemento_repetido) return actualizar_valor(elemento_repetido, elemento, hash->destructor);
   insertar_en_posicion(hash->posiciones[posicion], elemento, clave, NULL);
   hash->ocupacion++;
   if(calcular_porcentaje(hash->capacidad, hash->ocupacion) > FACTOR_OCUPACION_MAX)
      hash->posiciones = rehash(hash);
   return EXITO;
}

int encontrar_posicion_elemento(lista_t* lista, const char* clave){
   lista_iterador_t* iter = lista_iterador_crear(lista);
   bool encontrado = false;
   elemento_hash_t* elemento = NULL;
   int i = 0;
   while(lista_iterador_tiene_siguiente(iter) && !encontrado){
      elemento = (elemento_hash_t*)lista_iterador_siguiente(iter);
      encontrado = strcmp(elemento->clave, clave) == 0?true:false;
      if(!encontrado)
         i++;
   }
   lista_iterador_destruir(iter);
   return encontrado?i:ERROR;
}

int hash_quitar(hash_t* hash, const char* clave){
   if(!hash || !clave) return ERROR;
   int posicion = funcion_hash(clave) % (int)hash->capacidad;
   int posicion_en_lista = encontrar_posicion_elemento(hash->posiciones[posicion]->lista, clave);
   if(posicion_en_lista == ERROR) return ERROR;
   elemento_hash_t* elemento = (elemento_hash_t*)lista_elemento_en_posicion(hash->posiciones[posicion]->lista, (size_t)posicion_en_lista);
   if(hash->destructor)
      hash->destructor(elemento->elemento);
   free(elemento->clave);
   free(elemento);
   lista_borrar_de_posicion(hash->posiciones[posicion]->lista, (size_t)posicion_en_lista);
   hash->ocupacion--;
   hash->posiciones[posicion]->ocupacion--;
   return EXITO;
}

elemento_hash_t* hash_buscar(hash_t* hash, const char* clave, int posicion){
   return encontrar_elemento(hash->posiciones[posicion]->lista, clave);
}

void* hash_obtener(hash_t* hash, const char* clave){
   if(!hash || !clave) return NULL;
   int posicion = funcion_hash(clave) % (int)hash->capacidad;
   elemento_hash_t* elemento = hash_buscar(hash, clave, posicion);
   return elemento?elemento->elemento:NULL; 
}

bool hash_contiene(hash_t* hash, const char* clave){
   if(!hash || !clave) return false;
   int posicion = funcion_hash(clave) % (int)hash->capacidad;
   return hash_buscar(hash, clave, posicion)?true:false;
}

size_t hash_cantidad(hash_t* hash){
   return hash?(size_t)hash->ocupacion:0;
}

void liberar_lista(lista_t* lista, hash_destruir_dato_t destructor){
   lista_iterador_t* iterador = lista_iterador_crear(lista);
   while(lista_iterador_tiene_siguiente(iterador)){
      elemento_hash_t* elemento_hash = (elemento_hash_t*)lista_iterador_siguiente(iterador);
      if(destructor)
         destructor(elemento_hash->elemento);
      free(elemento_hash->clave);
      free(elemento_hash);
   }
   lista_iterador_destruir(iterador);
   lista_destruir(lista);
}

void liberar_posiciones(size_t capacidad, posicion_t** posiciones, hash_destruir_dato_t destructor){
   for(int i = 0; i < capacidad; i++){
      liberar_lista(posiciones[i]->lista, destructor);
      free(posiciones[i]);
   }
   free(posiciones);
}

void hash_destruir(hash_t* hash){
   if(!hash) return;
   if(hash->posiciones)
      liberar_posiciones(hash->capacidad, hash->posiciones, hash->destructor);
   free(hash);
}

bool iterar_lista(hash_t* hash, posicion_t* posicion, bool(*funcion)(hash_t* hash, const char* clave, void* aux), void* aux, int* cantidad){
   size_t cantidad_elementos = (size_t)posicion->ocupacion;
   bool continuar = false;
   size_t i = 0;
   while(i < cantidad_elementos && !continuar){
      elemento_hash_t* elemento = (elemento_hash_t*)lista_elemento_en_posicion(posicion->lista, i);
      continuar = funcion(hash, elemento->clave, aux);
      *cantidad += 1;
      i++;
	}
   return continuar;
}

size_t hash_con_cada_clave(hash_t *hash, bool (*funcion)(hash_t *, const char *clave, void *aux), void *aux){
   if(!hash || !funcion) return 0;
   size_t cantidad_posiciones = hash->capacidad;
   bool continuar = false;
   int i = 0;
   int cantidad_elementos = 0;
   while(i < cantidad_posiciones && !continuar){
	   continuar = iterar_lista(hash, hash->posiciones[i], funcion, aux, &cantidad_elementos);
      i++;
   }
   return (size_t)cantidad_elementos;
}

 /* -------------------------- HASH_ITERADOR.H ---------------------------- */

hash_iterador_t* hash_iterador_crear(hash_t* hash){
   if(!hash) return NULL;
   hash_iterador_t* iterador = calloc(1, sizeof(hash_iterador_t));
   if(!iterador) return NULL;
   iterador->hash = hash;
   iterador->iterador_lista = lista_iterador_crear(hash->posiciones[0]->lista);
   return iterador;
}

const char* hash_iterador_siguiente(hash_iterador_t* iterador){
   if(!iterador) return NULL;
   elemento_hash_t* elemento = hash_iterador_siguiente_elemento(iterador);
   return elemento?elemento->clave:NULL; 
}

bool hash_iterador_tiene_siguiente(hash_iterador_t* iterador){
   if(!iterador) return false;
   if(lista_iterador_tiene_siguiente(iterador->iterador_lista))
      return true;
   if(iterador->posicion_actual + 1 > iterador->hash->capacidad - 1) return false;
   if(encontrar_siguiente_lista(iterador->hash, iterador->posicion_actual) == ERROR) return false;
   return true;
}

void hash_iterador_destruir(hash_iterador_t* iterador){
   if(!iterador) return;
   if(iterador->iterador_lista) lista_iterador_destruir(iterador->iterador_lista);
   free(iterador);
}
