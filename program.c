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

	//TODO: Implementar
}

void access_pattern2( char *data, int length )
{
	int a;
	for(int i = 0 ; i<1000 ; i++){
		a = lrand48()%length;
		if(i%2==0){
			data[a] = i; 
		}
		else{
			data[a-1] = data[a];
		}
	}
	// TODO: Implementar
}

void access_pattern3( char *cdata, int length )
{
	// TODO: Implementar
}
