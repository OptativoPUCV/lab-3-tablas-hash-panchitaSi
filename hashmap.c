#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}

/*Esta función inserta un nuevo dato (key,value) en el mapa y actualiza el índice current a esa posición.
 Recuerde que para insertar un par (clave,valor) debe:*/
void insertMap(HashMap * map, char * key, void * value) {
    long posicion = hash(key,map->capacity);
    long i = posicion;

    while(map->buckets[posicion] != NULL){ //recorrer hasta encontrar una casilla nula
        //situacion en la que se inserte la misma clave de nuevo
        if (map->buckets[posicion]->key != NULL && strcmp(map->buckets[posicion]->key,key) == 0){
            return; 
        }

        //siguiente posicion
        posicion = (posicion + 1) % map->capacity; //si la posicion se pasa de la capacidad va a volver al principio como arreglo circular
        if (posicion == i) return; //quiere decir que ya dio toda la vuelta, es decir que está lleno el mapa
    } //cuando finaliza bien, tiene la posicion deseada para insertar.
    //insertamos
    map->buckets[posicion] = createPair(key,value);
    map->size++;
    map->current = posicion;
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
    //guardar los datos del mapa viejo
    Pair** old_buckets = map->buckets; //el auxiliar que guarda los buckets
    long capacidadAnterior = map->capacity; //la anterior capacidad
    //actualizar la nueva capacidad
    map->capacity*=2;
    //reservar memoria segun la nueva capacidad
    //"Asigne a map->buckets un nuevo arreglo con la nueva capacidad."
    map->buckets = (Pair**)calloc(map->capacity,sizeof(Pair*));
    map->size = 0;

    for (size_t k = 0; k < map->capacity ; k++){
        if (old_buckets[k] != NULL && old_buckets[k]->key != NULL){
            insertMap(map,old_buckets[k]->key,old_buckets[k]->value);
        }
    }
    free(old_buckets);
}


HashMap * createMap(long capacity) {
    HashMap *map = (HashMap*) malloc(sizeof(HashMap));
    if (map == NULL) return NULL;

    map->buckets = (Pair**) calloc(capacity, sizeof(Pair*));
    if (map->buckets == NULL) {
        free(map);
        return NULL;
    }

    map->size = 0;
    map->current = -1;
    map->capacity = capacity;
    return map;
}

void eraseMap(HashMap * map,  char * key) {
    long posicion = hash(key,map->capacity);
    long i = posicion;

    while(map->buckets[posicion] != NULL){
        if (map->buckets[posicion]->key != NULL && strcmp(map->buckets[posicion]->key, key) == 0){
            map->buckets[posicion]->key = NULL;
            map->size--;
            return;
        }
        posicion = (posicion +1) % map->capacity;
        if (posicion == i) return;
    }
}

Pair * searchMap(HashMap * map,  char * key) {   
    long posicion = hash(key,map->capacity);
    long i = posicion;

    while (map->buckets[posicion] != NULL) {
        if (map->buckets[posicion]->key != NULL && strcmp(map->buckets[posicion]->key, key) == 0) {
            map->current = posicion;
            return map->buckets[posicion];
        }

        posicion = (posicion + 1) % map->capacity;
        if (posicion == i) break;
    }

    return NULL;
}

Pair * firstMap(HashMap * map) {
    for (long posi = 0; posi < map->capacity ; posi++){
        if (map->buckets[posi] != NULL && map->buckets[posi]->key != NULL){
            map->current = posi;
            return map->buckets[posi];
        }
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {
    

    for (long posicion = (map->current + 1);posicion < map->capacity;posicion++){
        if (map->buckets[posicion] != NULL){
            map->current = posicion;
            return map->buckets[posicion];
        }
    }
    return NULL;
}