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
*  Last Modified : Thu Sep 18 22:51:59 PDT 2008
*
**********************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#define PRINTEXITERROR(a)  { fprintf (stderr, "Error: ( %s: %d: %s() ) ", __FILE__, __LINE__, __FUNCTION__); perror(a); exit(-1);}

#define LINESIZE (1024*10)


char thisline[LINESIZE];

void Usage(char * argv[]) {
	fprintf(stderr, "Usage %s <tcpdumpfile> <duration> [<startofsset>\n", argv[0]);
	exit(0);
}
	

main (int argc, char *argv[]) {
	FILE * infile;
	float first;
	float  tm;
	float  duration;
	float  startoffset;
	if(argc<3) Usage(argv);

	first=0.0;
	if(strcmp(argv[1],"-")!=0) {
		infile=fopen(argv[1],"r");
		if(infile==NULL) PRINTEXITERROR("opening infile");
	}
	else {
		infile=fopen("/dev/stdin","r");
		if(infile==NULL) PRINTEXITERROR("opening stdin");
	}

	duration=atof(argv[2]);
	if(argc>3) startoffset=atof(argv[3]);
	else startoffset=0.0;
	
	while(fgets(thisline,LINESIZE,infile)) {
		
		//fprintf(stderr, "%s", thisline);
		sscanf(thisline,"%f", &tm);
		if(first==0.0) first=tm;
		if(tm>(startoffset+first+duration) ) {
			exit(0);
		}
		if(tm>=(first+startoffset)) {
			printf("%s", thisline);
		}
	//fprintf(stderr , " tm %f first %f start %f duration %f \n", tm , first,  startoffset, duration); 
	}
		exit(0);
}
