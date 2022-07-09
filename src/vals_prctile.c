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
*  Last Modified : Thu Sep 18 22:52:00 PDT 2008
*
**********************************************************************************/
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

#define LINESIZE 1024

#define PRINTEXITERROR(a)  { fprintf (stderr, "Error: ( %s: %d: %s() ) ", __FILE__, __LINE__, __FUNCTION__); perror(a); exit(1);}
void Usage(char * argv[]) {
	fprintf(stderr, " Usage %s <infile> <prefix, ie. emg/psize/rtt/plr etc> <prctiles>\n",argv[0]);
	PRINTEXITERROR(" ");
}

char ip[LINESIZE];
int count;
float min;
float vals;
int i,j;
float * values;
int *prctiles;
FILE **prctilfiles;
int prctilecount;
char prctilefilebase[LINESIZE];
char thisfilename[LINESIZE];
float thisvalue;

float getpercentile(int prctile, int count) {
	//printf(" return %%ile %d total %d\n", prctile, count);
	return values[(int)prctile*(count-1)/100];
	
}
int sortvalues( int count ) {
	int i,j;
	float tmp;
	for(i=0; i< count-1; i++) {
		for(j=i+1;j<count ; j++) {
			if(values[i]>values[j]) {
				tmp=values[i];
				values[i]=values[j];
				values[j]=tmp;
			}
		}
	}
	return 0;
		
}

int main(int argc, char * argv[] ) {
	FILE * infile;
	if(argc<2) Usage (argv);
	//printf("argc %d\n", argc);

	infile=fopen(argv[1],"r");
	if(infile==NULL) PRINTEXITERROR("cant open infile");
	strcpy(prctilefilebase,argv[2]);
	prctilecount=argc-3;
	prctiles=(int *) malloc(sizeof(int)*prctilecount);
	if(prctiles==NULL) {
		PRINTEXITERROR("not enough memory for prctiles");
	}
	prctilfiles=(FILE **) malloc(sizeof(FILE *) * prctilecount);
	if(prctilfiles==NULL) {
		PRINTEXITERROR("not enough memory for prctilefilenames");
	}
	printf(" wanted %%iles ");
	for(i=3;i<=argc-1;i++) {
		prctiles[i-3]=atoi(argv[i]);
		printf(" %d ", prctiles[i-3]);
		sprintf(thisfilename,"%s_%d", prctilefilebase,prctiles[i-3]);
		prctilfiles[i-3]=fopen(thisfilename,"w");
		if(prctilfiles[i-3]==NULL) {
			PRINTEXITERROR("could not open file for writing this prctile");
		}
		
	}
	printf("\r");

	while(fscanf(infile, "%s",ip)==1) {
	//	printf(" read ip %s ", ip);
		if(fscanf(infile,"%d",&count)!=1) {
			PRINTEXITERROR(" no count field in infile for this particular line");
		}
	//	printf (" count %d values ", count);
		if(fscanf(infile,"%f",&vals)!=1) {
			PRINTEXITERROR(" cant read min val for this line ");
		}
		values=(float *) malloc(sizeof(float)*count);
		if(values==NULL) {
			fprintf(stderr," for IP %s ", ip);
			PRINTEXITERROR("cannot allocate enough memory to store all values on this line");
		}
	//	printf("read ");
		for(i=1;i<=count;i++) {
			if(fscanf(infile,"%f",&values[i-1])!=1) {
				PRINTEXITERROR(" not enough fields for this line as for count");
			}
	//		printf("%f ", values[i-1]);
		}
	//	printf("\n");
	//	printf(" sorted ");
		if(sortvalues(count)!=0) {
			PRINTEXITERROR(" could not sort this line fields");
		}
	//	printf("\n");
	//	for(j=0;j<count;j++) 
	//		printf("%f ", values[j]);
	//	printf("\n");
		for(j=0;j<prctilecount;j++) {
			thisvalue=getpercentile(prctiles[j], count);
			//printf(" %d->%f ", prctiles[j], thisvalue);
			fprintf(prctilfiles[j],"%s %.5f\n", ip, thisvalue);
		}
		//printf("\n");
	}

	for(i=3;i<=argc-1;i++) {
		fclose(prctilfiles[i-3]);
	}
	fclose(infile);
	return 0;
	
}
