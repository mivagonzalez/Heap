#include "heap.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#define TAM_INICIAL 4200
#define CTE_REDIMENSION 2
#define FACTOR_DECRECIMIENTO 4


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

struct heap {
	void** array;
	size_t cantidad;
	size_t tamanio;
	cmp_func_t cmp;
};

/* ******************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

bool heap_redimensionar(heap_t* heap, size_t tam_nuevo) {
	void *aux = realloc (heap->array, sizeof(void*) * tam_nuevo);
	if (!aux) return false;
    heap->array = aux;
    heap->tamanio = tam_nuevo;
    return true;
}

void swap(void** array, size_t pos1, size_t pos2) {
	void* aux = array[pos1];
 	array[pos1] = array[pos2];
 	array[pos2] = aux;
}

size_t posicion_padre(size_t pos_actual) {
	return (pos_actual-1)/2;
}

size_t posicion_hijo_izq(size_t pos_actual) {
	return pos_actual*2+1;
}

size_t posicion_hijo_der(size_t pos_actual) {
	return pos_actual*2+2;
}

size_t posicion_hijo_mayor(void** array, size_t cantidad_elementos, cmp_func_t cmp, size_t hijo_izq, size_t hijo_der) {
	if (hijo_der >= cantidad_elementos) return hijo_izq;
	if (hijo_izq >= cantidad_elementos) return hijo_der;
	return (cmp(array[hijo_izq], array[hijo_der]) < 0) ? hijo_der : hijo_izq;
}

void* buscar_padre(heap_t* heap, size_t pos_actual) {
	if (pos_actual == 0) return NULL;
	return heap->array[posicion_padre(pos_actual)]; 
}

bool tiene_hijos(void** array, size_t cantidad_elementos, size_t pos_actual) {
	size_t pos_hijo_der = posicion_hijo_der(pos_actual);
	size_t pos_hijo_izq = posicion_hijo_izq(pos_actual);

	return (pos_hijo_der < cantidad_elementos || pos_hijo_izq < cantidad_elementos);
}

void upheap(heap_t* heap, size_t pos) {
	if (pos == 0) return;
	size_t padre = posicion_padre(pos);
	if (heap->cmp(heap->array[pos], heap->array[padre]) <= 0) return;
	swap(heap->array,pos,padre);
	upheap(heap,padre);
}

void downheap(void** array, size_t cantidad_elementos, cmp_func_t cmp, size_t pos) {
	if (!tiene_hijos(array,cantidad_elementos,pos)) return;
	size_t hijo_mayor = posicion_hijo_mayor(array, cantidad_elementos, cmp, posicion_hijo_izq(pos),posicion_hijo_der(pos));
	if (cmp(array[pos], array[hijo_mayor]) >= 0) return;
	swap(array, pos, hijo_mayor);
	downheap(array, cantidad_elementos, cmp, hijo_mayor);
}

void heapify(void** array, cmp_func_t cmp, size_t cantidad_elementos) { 
	for(size_t i = cantidad_elementos; i > 0; i--) {
		downheap(array, cantidad_elementos, cmp, i-1);
	}
}


/* ******************************************************************
 *                    FUNCION EXTRA
 * *****************************************************************/

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp) {
	heapify(elementos, cmp, cant);
	for(size_t i = cant-1; i> 0; i--) {
		swap(elementos, 0, i);
		downheap(elementos, i, cmp, 0);
	}
}

/* ******************************************************************
 *                    PRIMITIVAS DEL HEAP
 * *****************************************************************/

heap_t* heap_crear(cmp_func_t cmp) {
	heap_t* heap = malloc(sizeof(heap_t));
	if (!heap) return NULL;
	void** array = malloc(TAM_INICIAL * sizeof(void*));
	if (!array) {
		free(heap);
		return NULL;
	}
	heap->array = array;
	heap->cantidad = 0;
	heap->tamanio = TAM_INICIAL;
	heap->cmp = cmp;
	return heap;
}

heap_t* heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp) {
	heap_t* heap = malloc(sizeof(heap_t));
	if (!heap) return NULL;
	heap->array = arreglo;
	heap->cmp = cmp;
	heap->cantidad = n;
	heap->tamanio = n;
 	heapify(heap->array, cmp, n);
 	return heap;
}

void heap_destruir(heap_t *heap, void destruir_elemento(void *e)) {
	if (destruir_elemento) { 
    	for(size_t i = 0; i < heap_cantidad(heap); i++) {
      		destruir_elemento(heap->array[i]);
    	}
    }
    free (heap->array);
  	free(heap);
}


size_t heap_cantidad(const heap_t *heap) {
	return heap->cantidad;
}

bool heap_esta_vacio(const heap_t *heap) {
	return (heap_cantidad(heap) == 0);
}

bool heap_encolar(heap_t *heap, void *elem) {
	if (heap->cantidad == heap->tamanio) {
		if (!heap_redimensionar(heap,heap->tamanio * CTE_REDIMENSION)) {
			return false;
		}
	}
	heap->array[heap->cantidad] = elem;
	upheap(heap, heap->cantidad);
	heap->cantidad++;
	return true;
}

void* heap_ver_max(const heap_t *heap) { 
	if (heap_esta_vacio(heap)) return NULL;
	return heap->array[0];
}

void* heap_desencolar(heap_t *heap) {
	if (heap->cantidad < (heap->tamanio/FACTOR_DECRECIMIENTO)) {
		if (!heap_redimensionar(heap, heap->tamanio / CTE_REDIMENSION)) {
			return NULL;
		}
	}
	if (heap->cantidad == 0) return NULL;
	void* maximo = heap_ver_max(heap);
	swap(heap->array, 0, heap->cantidad-1);
	heap->cantidad--;
	downheap(heap->array, heap->cantidad, heap->cmp, 0);
	return maximo;
}