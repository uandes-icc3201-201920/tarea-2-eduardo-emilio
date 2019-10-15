/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/

#include "page_table.h"
#include "disk.h"
#include "program.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int front = 0;
int rear = -1;
int itemCount = 0; // estas tres variables son para implementar un queue, por ahorrar tiempo se dejo de manera global en vez de empaquetarlo
// en un archivo separado

// las siguientes variables son del codigo original
// fueron cambiadas a alcanze global para poder usarlas dentro del 
// page fault handler sin tener que modificar los parametros.
int *ft = NULL; //frame table en la pocision i se guarda el numero de pagina del elemento que esta en el marco i.
int *queue = NULL; // el elemento i representa un frame 
struct disk *disk;
int nframes;
char *physmem;
char *algoritmo;// argv[3] en un futuro muy muy cercano


// las siguientes 6 funciones son para implementar un queue circular con un array
// fuente: geeksforgeeks
int peek(int *intArray) {
   return intArray[front];
}

bool isEmpty() {
   return itemCount == 0;
}

bool isFull() {
   return itemCount == nframes;
}

int size(){
   return itemCount;
}  

void insert(int *intArray,int data) 
{

   if(!isFull()) {
	
      if(rear == nframes-1) {
         rear = -1;            
      }       

      intArray[++rear] = data;
      itemCount++;
   }
}

int removeData(int *intArray) 
{
   int data = intArray[front++];
	
   if(front == nframes) {
      front = 0;
   }
	
   itemCount--;
   return data;  
}



void page_fault_handler( struct page_table *pt, int page )
{
	int nframes = *(ft);

	int hay_espacio = 0;
	int donde;
	for(int i = 1 ; i<nframes+1 ; ++i){
		if(*(ft + i)==-1){
			hay_espacio = 1;
			donde = i;
			break;
		}
	} // se revisa si hay espacio disponible y donde

	// si hay espacio disponible
	if(hay_espacio){
		page_table_set_entry(pt,page,donde-1,PROT_READ|PROT_WRITE); 
		disk_read(disk,page,&physmem[donde-1]);
		*(ft+donde) = page;
		insert(queue,donde-1);
	}
	// si hay que hacer espacio
	else{
		int pos;
		// pos representa la pocision en la memoria fisica (pos 1 es  el marco 1)

		if(!strcmp(algoritmo,"FIFO"))
		{
			pos = removeData(queue); // sacas el frame primero en ser llenado, primero en la cola
			insert(queue,pos); // lo vuelves a meter a la cola pero al final.
		}
		else if(!strcmp(algoritmo,"RAND"))
		{
			pos = lrand48() % (nframes-1) + 1; 
			// se elige una posicion aleatoriamente
			// esta tecnicamente mal pq va de 1 hasta nframe-1, (debiera ser desde el 0 hasta el nframe-1)

			// pero no logramos descubrir pq solo en el caso RAND, si pos==0
			// genera error y page_table_set_entry lo reconoce como fuera del dominio de los frames,
			// resultando en que el codigo base envie el mensaje de error correspondiente y se caiga el programa,
			// es por esto que fue dejado de esta forma.

			// sin embargo esto no pasa si pos==0 viene de FIFO.
			// esto provoca que funcione con un marco menos el algoritmo RAND, lo cual fue tomado en cuenta en la comparacion experimental y compensado al darle un frame mas.
		}
		else
		{
			printf("algoritmo de paginacion no valido. RAND/FIFO\n");
			printf("use: virtmem <npages> <nframes> <RAND|FIFO> <access pattern>\n");
			exit(1);
		}

		disk_write(disk,ft[pos],&physmem[pos]); // se escribe en disco el valor de la pagina en caso de que haya sido modificada.
		page_table_set_entry(pt,ft[pos],pos,0); // se actualiza la entrada para indicar que ya no esta cargada en memoria

		disk_read(disk,page,&physmem[pos]); // se lee del disco y se deja en memoria la pagina que causo la falta
		page_table_set_entry(pt,page,pos,PROT_READ|PROT_WRITE); // se actualiza la entrada de la pagina en la tabla.

		ft[pos] = page; // se actualiza la estructura en que se registran los estados de los frames.


		// printf("\nmuerte y dolor");
		// printf("page fault on page #%d\n",page);
		// exit(1);
	}

	

}

int main( int argc, char *argv[] )
{
	if(argc!=5) {
		printf("use: virtmem <npages> <nframes> <RAND|FIFO> <access pattern>\n");
		return 1;
	}

	int npages = atoi(argv[1]);
	nframes = atoi(argv[2]);
	algoritmo =  argv[3];
	const char *program = argv[4];

	// frame table
	// consiste en un array en donde la pocision indica el frame
	// y el contenido del array indica la page que esta en esa posicion
	// adicionalmente tenemos un queue que solo se utiliza en el caso FIFO
	// sirve para indicar al frame/page victima.
    queue = malloc( nframes*sizeof(int) );
	ft = realloc( ft, (nframes+1)*sizeof(int) );

	*(ft) = nframes; // en la primera pocision se guarda la cantidad de marcos
	for(int i = 1 ; i<nframes+1 ; ++i)
	{
		*(ft + i) = -1;
	}
	// se llena con -1 cada posicion pq -1 es el flag que indica que un frame esta vacio.
	// fin de lass modificaciones al codigo base.


	disk = disk_open("myvirtualdisk",npages);
	if(!disk) {
		fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
		return 1;
	}


	struct page_table *pt = page_table_create( npages, nframes, page_fault_handler );
	if(!pt) {
		fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
		return 1;
	}

	char *virtmem = page_table_get_virtmem(pt);

	
	physmem = page_table_get_physmem(pt);

	if(!strcmp(program,"pattern1")) {
		access_pattern1(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"pattern2")) {
		access_pattern2(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"pattern3")) {
		access_pattern3(virtmem,npages*PAGE_SIZE);

	} else {
		fprintf(stderr,"unknown program: %s\n",argv[3]);

	}

	free(ft);
	free(queue);
	page_table_delete(pt);
	disk_close(disk);

	return 0;
}
