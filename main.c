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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int *ft = NULL; //frame table
struct disk *disk;
char *physmem;


void page_fault_handler( struct page_table *pt, int page )
{
	int nframes = *(ft);

	int hay_espacio = 0;
	int donde;
	for(int i = 1 ; i<nframes-1 ; ++i){
		if(*(ft + i)==-1){
			hay_espacio = 1;
			donde = i;
		}
	} // se revisa si hay espacio disponible y donde
	if(hay_espacio){
		printf("\nwat da fuk %d",page);
		page_table_set_entry(pt,page,donde-1,PROT_READ|PROT_WRITE); //lol
		disk_read(disk,page,&physmem[donde-1]);
		*(ft+donde) = page;
	}

	else{
		printf("\nmuerte y dolor");
		printf("page fault on page #%d\n",page);
		exit(1);
	}

	
	//page_table_set_entry(pt,page,page,PROT_READ|PROT_WRITE);
	//printf("page fault on page #%d\n",page);
	//exit(1);
}

int main( int argc, char *argv[] )
{
	if(argc!=5) {
		printf("use: virtmem <npages> <nframes> <lru|fifo> <access pattern>\n");
		return 1;
	}

	int npages = atoi(argv[1]);
	int nframes = atoi(argv[2]);
	const char *program = argv[4];


	// frame table
	ft = realloc( ft, (nframes+1)*sizeof(int) );
	*(ft) = nframes; // en la primera pocision se guarda la cantidad de marcos
	for(int i = 1 ; i<nframes+1 ; ++i)
	{
		*(ft + i) = -1;
	}


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

	page_table_delete(pt);
	disk_close(disk);

	return 0;
}
