/**********************************************************************************
* 
*  Copyright (C) 2008 Kashi Venkatesh Vishwanath <swingxficgenerator@gmail.com>
* 
*  Permission is hereby granted, free of charge, to any person obtaining a copy of
*  this software and associated documentation files (the "Software"), to  deal  in
*  the Software without restriction, including without limitation  the  rights  to
*  use, copy, modify, merge, publish, distribute, sublicense, and/or  sell  copies
*  of the Software, and to permit persons to whom the Software is furnished to  do
*  so, subject to the following conditions:
* 
*  The above copyright notice and this permission notice shall be included in  all
*  copies of the Software and its documentation and acknowledgment shall be  given
*  in the documentation  and  software  packages  that  this  Software  was  used.
*  
*  You should have received a copy of the GNU Library General Public License along
*  with this library; if not, write to the  Free  Software  Foundation,  Inc.,  59
*  Temple Place, Suite 330, Boston, MA 02111-1307, USA.
* 
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY  OF  ANY  KIND,  EXPRESS  OR
*  IMPLIED, INCLUDING BUT  NOT  LIMITED  TO  THE  WARRANTIES  OF  MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO  EVENT  SHALL  THE
*  AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR  OTHER  LIABILITY,  WHETHER  IN  AN
*  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF  OR  IN  CONNECTION
*  WITH  THE  SOFTWARE  OR  THE  USE  OR   OTHER   DEALINGS  IN   THE   SOFTWARE.
*
*
*
*  Last Modified : Thu Sep 18 22:51:58 PDT 2008
*
**********************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include "genfromfile.h"

void permute_it(int * initial_array, int ** p_permuted_array , int numitems)
{
	int *permute_items;
	int *items;
	int s,i;
	int doneitems;
	int * done_items;
	int * permuted_array = *p_permuted_array;

	items=initial_array;
	permute_items=permuted_array;

	done_items=(int *) malloc(sizeof(int) * numitems);
    doneitems=0;
	for(i=0;i<numitems;i++) done_items[i]=0;
	for(i=0;i<numitems;i++)
	{
	//	printf("inittial %i %d\n", i, initial_array[i]);
retry:
		s=drand48()*numitems/1;
		if (done_items[s]==0){
			permute_items[doneitems++]=items[s];
			done_items[s]=1;
		}
		else
		{
			goto retry;
		}

	//	printf("permuted %d %d \n", i, permute_items[i]);
		
	}
}

int readfromfile(char * FILENAME, int ** p_permute_items)
{
	FILE *f1 = fopen(FILENAME,"r");
	int numitems;
	char this_line[80];
	int this_item;
	int *items;
	int doneitems;


	fgets(this_line,80,f1);
	sscanf(this_line,"%d",&numitems);
	items=(int * )malloc(sizeof(int)*numitems);
	while(fgets(this_line, 80,f1)){
		sscanf(this_line,"%d", & items[this_item++]);
	}
	*p_permute_items=items;
	close(f1);
	return numitems;

}

int  genfromfile(char * FILENAME, int SEED, int ** p_permute_items, int * medianval)
{
	FILE *f1 = fopen(FILENAME,"r");
	int numitems;
	char this_line[80];
	int this_item,i;
	int *items;
	int *done_items;
    int s;
	int doneitems;
	int medianloc;
	int * permute_items = * p_permute_items;


	if(!f1)
	{
		fprintf(stderr, " FILE %s not found \n", FILENAME);
		exit(-1);
	}
	srand48(SEED);
	fgets(this_line,80,f1);
	sscanf(this_line,"%d",&numitems);
	medianloc=numitems/2;
	items=(int * )malloc(sizeof(int)*numitems);
	permute_items=(int *) malloc(sizeof(int)*numitems);
	done_items=(int *) malloc(sizeof(int)*numitems);
	this_item=0;
	while(fgets(this_line, 80,f1)){
		sscanf(this_line,"%d", & items[this_item++]);
		if (this_item==medianloc) *medianval=items[this_item-1];
		permute_items[this_item-1]=0;
		done_items[this_item-1]=0;
		if(this_item==numitems) break;
	}
	permute_it(items, &permute_items, numitems);
	for(i=0;i<numitems;i++){
	//	printf("permute %d = %d\n",i, permute_items[i]);
	}
	*p_permute_items = permute_items;
	return numitems;
}

/*main(int argc, char * argv[])
{
	
	//FILE * f1=fopen(argv[1],"r");
	FILE * f2=fopen(argv[2],"w");
	int SEED ;
	int numitems;
	char this_line[80];
	int this_item,i;
	int *items;
	int *permute_items;
	int *done_items;
    int s;
	int doneitems;

	
	numitems=genfromfile((argv[1]), atoi(argv[3]), &permute_items);
	
	for(i=0;i<numitems;i++){
		printf("permute %d = %d\n",i, permute_items[i]);
		fprintf(f2,"%d\n", permute_items[i]);
	}
	
}*/


void permute_it_udp(udp_req * initial_array, udp_req ** p_permuted_array , int numitems)
{
	udp_req *permute_items;
	udp_req *items;
	int s,i;
	int doneitems;
	int * done_items;
	udp_req * permuted_array = *p_permuted_array;

	items=initial_array;
	permute_items=permuted_array;

	done_items=(int *) malloc(sizeof(int) * numitems);
    doneitems=0;
	for(i=0;i<numitems;i++)
	{
	//	printf("inittial %i %d\n", i, initial_array[i]);
retry:
		s=drand48()*numitems/1;
		if (done_items[s]==0){
			permute_items[doneitems++]=items[s];
			done_items[s]=1;
		}
		else
		{
			goto retry;
		}

	//	printf("permuted %d %d \n", i, permute_items[i]);
		
	}
}

int readfromfile_udp(char * FILENAME, udp_req ** p_permute_items)
{
	FILE *f1 = fopen(FILENAME,"r");
	int numitems;
	char this_line[80];
	int this_item;
	udp_req *items;
	int doneitems;


	fgets(this_line,80,f1);
	sscanf(this_line,"%d",&numitems);
	items=(udp_req * )malloc(sizeof(udp_req)*numitems);
	while(fgets(this_line, 80,f1)){
		sscanf(this_line,"%d", & items[this_item++]);
	}
	*p_permute_items=items;
	close(f1);
	return numitems;

}


int  genfromfile_udp(char * FILENAME, int SEED, void ** p_permute_items)
{
	FILE *f1 = fopen(FILENAME,"r");
	int numitems;
	char this_line[80];
	int this_item,i;
	udp_req *items;
	int *done_items;
    int s;
	int doneitems;
	udp_req * permute_items =(udp_req *)  * p_permute_items;


	if(!f1)
	{
		fprintf(stderr, " FILE %s not found \n", FILENAME);
		exit(-1);
	}
	srand48(SEED);
	fgets(this_line,80,f1);
	sscanf(this_line,"%d",&numitems);
	items=(udp_req * )malloc(sizeof(udp_req)*numitems);
	permute_items=(udp_req *) malloc(sizeof(udp_req)*numitems);
	done_items=(int *) malloc(sizeof(int)*numitems);
	this_item=0;
	while(fgets(this_line, 80,f1)){
		sscanf(this_line,"%d %d %d", & items[this_item].bytes, &items[this_item].pkts, &items[this_item].duration);
		//printf("%d %d %d\n",  items[this_item].bytes, items[this_item].pkts, items[this_item].duration);
		this_item++;
		permute_items[this_item-1].bytes=0;
		permute_items[this_item-1].pkts=0;
		permute_items[this_item-1].duration=0;
		//done_items[this_item-1]=0;
		if(this_item==numitems) break;
	}
	permute_it_udp(items, &permute_items, numitems);
	for(i=0;i<numitems;i++){
	//	printf("ier loop permute %d = %d  %d %d\n",i, permute_items[i].bytes, permute_items[i].pkts, permute_items[i].duration);
	}
	*p_permute_items = permute_items;
	return numitems;
}


