/*
Do not modify this file.
Make all of your changes to main.c instead.
*/

#include "program.h"

#include <stdio.h>
#include <stdlib.h>

static int compare_bytes( const void *pa, const void *pb )
{
	int a = *(char*)pa;
	int b = *(char*)pb;

	if(a<b) {
		return -1;
	} else if(a==b) {
		return 0;
	} else {
		return 1;
	}

}

void access_pattern1( char *data, int length )
{
	int a = 1;
	for (int i = length-1; i >= 0; i--) {
	data[i] = 0;
	}
	for (int i = 0; i < length; i++) {
	a = data[i];
	}
	// patron lineal, desde el fin a inicio pasa por todos dandole valores
	// luego de inicio a fin lee los valores de todos
	//TODO: Implementar
}

void access_pattern2( char *data, int length )
{
	int a;
	for(int i = 0 ; i<5000 ; i++){
		a = lrand48()%length;
		if(i%2==0){
			data[a] = i; 
		}
		else{
			if(a==0){
				data[a+1] = data[a];	
			}
			else{
				data[a-1] = data[a];
			}
		}
	}
	// patron aleatorio, 5000 veces entra a una posocion aleatoria
	// si es par la iteracion escribe
	// si es impar lee el valor del elegido y lo escribe en el antecesor.
	// TODO: Implementar
}

void access_pattern3( char *data, int length )
{
	for(int i = 0 ; i<100 ; i++){
		int a = lrand48()%length;
		for(int j = 0 ; j <20 ; j++){
			int b = lrand48()%length;
			if (a!=b){
				data[a] = data[b]; 
			}
		}
	}
	// a diferencia de los otros dos patrones aca hay mucha mas repeticion en la frecuencia de uso de las paginas,
	// en el patron 1 se usa una pagina y no se vuelve a usar, aca se repite muchas veces seguidas el uso de una pagina.
	// TODO: Implementar
}
