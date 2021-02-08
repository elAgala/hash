#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "hash_iterador.h"
#include "tester.h"

#define COLOR_MAGENTA "\x1b[35m\x1b[1m"
#define COLOR_CYAN "\x1b[36m\x1b[1m"
#define COLOR_AMARILLO "\x1b[33m\x1b[1m"
#define COLOR_ROJO "\x1b[31m\x1b[1m"
#define COLOR_VERDE "\x1b[32m\x1b[1m"
#define COLOR_NORMAL "\x1b[0m"

const char alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789";

typedef struct tarea {
	int dia;
	int mes;
	char* desc;
} tarea_t; 

typedef struct elemento {
   const char* clave;
   tarea_t* tarea;
} elemento_t;

tarea_t* crear_tarea(int dia, int mes, char* descripcion){
	tarea_t* nueva_tarea = calloc(1, sizeof(tarea_t));
	if(!nueva_tarea)
		return NULL;
	nueva_tarea->dia = dia;
	nueva_tarea->mes = mes;
	nueva_tarea->desc = descripcion;
	return nueva_tarea;
}

int comparar_tareas(void* a, void* b){
	if(!a && !b)
		return 0;
	if(!a)
		return 1;
	if(!b)
		return -1;
	tarea_t* tarea_1 = (tarea_t*)a;
	tarea_t* tarea_2 = (tarea_t*)b;
	if(tarea_1->mes > tarea_2->mes)
		return 1;
	else if(tarea_1->mes < tarea_2->mes)
		return -1;
	else{
		if(tarea_1->dia > tarea_2->dia)
			return 1;
		else if(tarea_1->dia < tarea_2->dia)
			return -1;
		else
			return 0;
	}
}

bool compareTasks(void* a, void* b){
	tarea_t* tarea_1 = (tarea_t*)a;
	tarea_t* tarea_2 = (tarea_t*)b;
	
	if(tarea_1->mes != tarea_2->mes)
		return false;
	if(tarea_1->dia != tarea_2->dia)
		return false;
	return true;
}

void destruir_tarea(tarea_t* tarea){
	free(tarea);
}

void destruir_tareas(void* elemento){
	if(!elemento)
		return;
	destruir_tarea((tarea_t*)elemento);
}

void probar_insercion(hash_t* hash, elemento_t** elementos, tester_t* tester){
   hash_t* hash_clave_repetida = hash_crear(NULL, 3);

   createNewCategory(tester, "PRUEBAS DE INSERCION");

   int exito = 0;
   int result;
   char str[50];
   int cantidad_esperada = 10;

   for(int i= 0; i < 10; i++){
      result = hash_insertar(hash, elementos[i]->clave, elementos[i]->tarea);
      sprintf(str, "Inserto elemento %i", i + 1);
      test(tester, str, &result, &exito, compareInt);
   }
   result = (int)hash_cantidad(hash);
   test(tester, "Ahora el hash tiene 10 elementos", &result, &cantidad_esperada, compareInt);

   int a = 10;
   int b = 25;

   result = hash_insertar(hash_clave_repetida, "1234", &a);
   test(tester, "Inserto un elemento", &result, &exito, compareInt);

   result = hash_insertar(hash_clave_repetida, "1234", &b);
   int comparar = -1;
   if(result == 0)
      comparar = *(int*)hash_obtener(hash_clave_repetida, "1234");
   test(tester, "Inserto elemento con misma clave y chequeo valor", &comparar, &b, compareInt);

   hash_destruir(hash_clave_repetida);
}

void probar_busqueda(hash_t* hash, elemento_t** elementos, tester_t* tester){
   createNewCategory(tester, "PRUEBAS DE BUSQUEDA");
   
   int result;
   char str[50];
   int contiene = true;

   for(int i= 0; i < 10; i++){
      result = hash_contiene(hash, elementos[i]->clave);
      sprintf(str, "Busco elemento %i", i + 1);
      test(tester, str, &result, &contiene, compareBool);
   }
}

void probar_borrado(hash_t* hash, elemento_t** elementos, tester_t* tester){
   createNewCategory(tester, "PRUEBAS DE BORRADO");
   
   int exito = 0;
   int result;
   char str[50];
   int cantidad_esperada = 5;
   int contiene = true;
   
   for(int i = 0; i < 5; i++){
      result = hash_quitar(hash, elementos[i]->clave);
      sprintf(str, "Elimino elemento %i", i + 1);
      test(tester, str, &result, &exito, compareBool);
   }
   result = (int)hash_cantidad(hash);
   test(tester, "Ahora el hash tiene 5 elementos", &result, &cantidad_esperada, compareInt);
   result = hash_obtener(hash, elementos[3]->clave)==NULL?true:false;
   test(tester, "Busco elemento ya borrado", &result, &contiene, compareBool);
}

void mostrar_clave(const char* clave){
   printf("\nCLAVE: %s\n", clave);
}

bool funcion_iterador(hash_t* hash, const char* clave, void* aux){
   if(!clave)
      return true;
   aux=aux;
   hash=hash;
   if(aux)
      *(int*)aux+=1;
   return false;
}

void probar_iteradores(hash_t* hash, tester_t* tester){
   createNewCategory(tester, "PRUEBAS CON ITERADORES");

   int result;
   int cantidad_esperada = 5;

   int contador = 0;
   result = (int)hash_con_cada_clave(hash, funcion_iterador, &contador);
   test(tester, "Recorro hash con cada clave", &result, &contador, compareInt);
   
   hash_iterador_t* iter = hash_iterador_crear(hash);
   contador = 0;

   while(hash_iterador_tiene_siguiente(iter)){
      const char* clave = hash_iterador_siguiente(iter);
      if(clave)
         contador++;
   }
   test(tester, "Recorro el hash con el iterador externo", &contador, &cantidad_esperada, compareInt);

   hash_iterador_destruir(iter);
}

void probar_null(tester_t* tester){
   hash_t* hash_null = hash_crear(NULL, 20);

   createNewCategory(tester, "PRUEBAS CON NULL");
   
   int exito = 0;
   int error = -1;
   int result;
   bool exito_bool = true;
   bool error_bool = false;
   bool result_bool;
   
   result = hash_insertar(hash_null, "123123-NULL", NULL);
   test(tester, "Inserto un elemento NULL", &result, &exito, compareInt);

   result = hash_insertar(hash_null, NULL, NULL);
   test(tester, "Inserto un elemento con clave NULL", &result, &error, compareInt);
   
   result_bool = hash_contiene(hash_null, "123123-NULL");
   test(tester, "Busco elemento NULL", &result_bool, &exito_bool, compareBool);

   result_bool = hash_contiene(NULL, "a5sd4as");
   test(tester, "Busco elemento en hash NULL", &result_bool, &error_bool, compareBool);

   result = (int)hash_cantidad(NULL);
   test(tester, "Hash NULL tiene 0 elememtos", &result, &exito, compareInt);

   result = hash_quitar(NULL, "asdasd");
   test(tester, "Quito elemento de hash NULL", &result, &error, compareInt);
   

   hash_destruir(hash_null);
}

void liberar_elementos(elemento_t** elementos, int cantidad_elementos){
   for(int i = 0; i < cantidad_elementos; i++)
      free(elementos[i]);
}

elemento_t* generar_elemento(const char* clave, int dia, int mes, char* desc){
   elemento_t* elemento = calloc(1, sizeof(elemento_t));
   elemento->clave = clave;
   elemento->tarea = crear_tarea(dia, mes, desc);
   return elemento;
}

elemento_t** generar_elementos(){ 
   elemento_t** elementos = calloc(10, sizeof(elemento_t));

   elementos[0] = generar_elemento("ABC-TAREA-123", 12, 4, "Pasear al perro");
   elementos[1] = generar_elemento("CBA-TAREA-265", 28, 2, "Arreglar la heladera");
	elementos[2] = generar_elemento("ASD-TAREA-987", 1, 1, "Ir al super"); 
	elementos[3] = generar_elemento("HGJ-TAREA-411", 30, 7, "Retirar plata"); 
	elementos[4] = generar_elemento("QWE-TAREA-786", 3, 2 ,"Comprar verdura");
	elementos[5] = generar_elemento("KKA-TAREA-562", 17, 12, "Hacer una pizza"); 
	elementos[6] = generar_elemento("QWY-TAREA-845", 28, 8, "Examen de Algo2"); 
	elementos[7] = generar_elemento("YRW-TAREA-012", 14, 10, "Tomar unos mates"); 
	elementos[8] = generar_elemento("FGH-TAREA-820", 25, 8, "Hacer el asado"); 
	elementos[9] = generar_elemento("VNM-TAREA-613", 13, 3, "Pasar la aspiradora");

   return elementos;
}

int main(){
   tester_t* tester = createNewTester();
   
   elemento_t** elementos = generar_elementos();

   hash_t* hash = hash_crear(destruir_tareas, 2);
   
   probar_insercion(hash, elementos, tester);
   probar_busqueda(hash, elementos, tester);
   probar_borrado(hash, elementos, tester);
   probar_iteradores(hash, tester);
   probar_null(tester);

   showResults(tester);

   liberar_elementos(elementos, 10);
   free(elementos);
   freeTester(tester);
   hash_destruir(hash);
   return 0;
}
