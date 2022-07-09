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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 
   Takes in the hosts file and cdfdir files to separate tlatest into tlatest.0 and tlatest.1
   For the cdfdir it assumes that first u describe lets sapp APP0 and then APP1
   for APP0 you have client followed by server and similarly for APP1
   thus,
   		APP0 - client = dir 0 = left
   		APP0 - server = dir 1 = right
   		APP1 - client = dir 1 = right
   		APP1 - server = dir 0 = left
*/
   

#define LINESIZE 1024
char thisline[LINESIZE];
char file0[LINESIZE],file1[LINESIZE];
FILE * fhostsfile, * thisfile, *ftcpdump, *ffile1, *ffile0;
int vn,ip1,ip2,ip3,ip4;
int thisbucket;
int vip[256*256];
int vn2ip[256*256];
int direction[256*256];
int args;
int s;
int clientnum, servernum, thisvn;




#define PRINTEXITERROR(a)  { fprintf (stderr, "Error: ( %s: %d: %s() ) ", __FILE__, __LINE__, __FUNCTION__); perror(a); exit(-1);}


int getclient(char * thisline) {
	int i1,i2,i3,i4;
	float tm;
	if(sscanf(thisline, "%f %d.%d.%d.%d", &tm, &i1, &i2,&i3,&i4)!=5) return -1;
	return i3*256+i4;
}

void Usage ( char *argv[] ) {
	fprintf(stderr, "Usage %s <tcpdumpfile> <hostsfile> <inputforgeneric1> [<inputforgeneric2>] ...\n", argv[0]);
	PRINTEXITERROR(" command line arguments ");
}



int main(int argc, char *argv[]) {
	if(argc<4) Usage(argv);

	/* first create 'hashtable' */

	if((fhostsfile=fopen(argv[2],"r"))==NULL) PRINTEXITERROR("while opening hostsfile");

	while(fgets(thisline, LINESIZE, fhostsfile)!=NULL) {
		if(sscanf(thisline,"client%d %d.%d.%d.%d ", &vn, &ip1, &ip2,&ip3,&ip4)!=5)
			PRINTEXITERROR("reading fields from hostsfile");
		thisbucket=ip3*256+ip4;
		/* ModelNet vips will be in the 10.0/16 subnet */
		vip[thisbucket]=vn;
		direction[thisbucket]=-1;
		vn2ip[vn]=thisbucket;
		
	}

	/* fill in the direction info */

	for (args=3;args<argc;args++) {
		
		fprintf(stderr, "\r arg %d %s ", args-2, argv[args]);
		
		/* for each file fill in the info */

		thisfile=fopen(argv[args],"r");
	
		/* client 1 */

		if(fgets(thisline,LINESIZE,thisfile)==NULL) PRINTEXITERROR("reading lines from inputforgeneric file");

		if(sscanf(thisline,"client %d", &clientnum)!=1) PRINTEXITERROR("reading in format client <clientnum>");

		for(s=0;s<clientnum;s++) {
			if(s==0) {
				if(fscanf(thisfile,"vn%d", &thisvn)!=1) PRINTEXITERROR("reading clients from inputforgeneric file");
			}
			else {
				if(fscanf(thisfile," vn%d", &thisvn)!=1) PRINTEXITERROR("reading clients from inputforgeneric file");
			}
			direction[vn2ip[thisvn]]=0;
		}
		/* now server */
		if(fgets(thisline,LINESIZE,thisfile)==NULL) PRINTEXITERROR("reading lines from inputforgeneric file");
		if(fgets(thisline,LINESIZE,thisfile)==NULL) PRINTEXITERROR("reading lines from inputforgeneric file");
		if(sscanf(thisline,"server %d", &clientnum)!=1) PRINTEXITERROR("reading in format client <servernum>");
		for(s=0;s<clientnum;s++) {
			if(s==0) {
				if(fscanf(thisfile,"vn%d", &thisvn)!=1) PRINTEXITERROR("reading servers from inputforgeneric file");
			}
			else {
				if(fscanf(thisfile," vn%d", &thisvn)!=1) PRINTEXITERROR("reading servers from inputforgeneric file");
			}
			direction[vn2ip[thisvn]]=1;
		}

		/* get the next client line */
		
tryagain:
		if(fgets(thisline,LINESIZE,thisfile)==NULL) PRINTEXITERROR("reading lines from inputforgeneric file");
		if(sscanf(thisline,"client %d", &clientnum)!=1) goto tryagain;
		for(s=0;s<clientnum;s++) {
			if(s==0) {
				if(fscanf(thisfile,"vn%d", &thisvn)!=1) PRINTEXITERROR("reading clients from inputforgeneric file");
			}
			else {
				if(fscanf(thisfile," vn%d", &thisvn)!=1) PRINTEXITERROR("reading clients from inputforgeneric file");
			}
			direction[vn2ip[thisvn]]=1;
		}
		/* next server */
		if(fgets(thisline,LINESIZE,thisfile)==NULL) PRINTEXITERROR("reading lines from inputforgeneric file");
		if(fgets(thisline,LINESIZE,thisfile)==NULL) PRINTEXITERROR("reading lines from inputforgeneric file");
		if(sscanf(thisline,"server %d", &clientnum)!=1) PRINTEXITERROR("reading in format client <servernum>");
		for(s=0;s<clientnum;s++) {
			if(s==0) {
				if(fscanf(thisfile,"vn%d", &thisvn)!=1) PRINTEXITERROR("reading servers from inputforgeneric file");
			}
			else {
				if(fscanf(thisfile," vn%d", &thisvn)!=1) PRINTEXITERROR("reading servers from inputforgeneric file");
			}
			direction[vn2ip[thisvn]]=0;
		}
		fclose(thisfile);
	}
	fclose(fhostsfile);
	/* open the tcpdump file */
	fprintf(stderr, "\r                                                                     \r");
	sprintf(file0,"%s.0", argv[1]);
	sprintf(file1,"%s.1", argv[1]);
	if((ffile0=fopen(file0,"w"))==NULL) PRINTEXITERROR("opening file0 for writing");
	if((ffile1=fopen(file1,"w"))==NULL) PRINTEXITERROR("opening file1 for writing");
	if((ftcpdump=fopen(argv[1],"r"))==NULL) PRINTEXITERROR("opening tcpdumpfile");
	while(fgets(thisline,LINESIZE,ftcpdump)!=NULL) {
		thisvn=getclient(thisline);
		/* what is 0 and 1 is reversed from the original trace adn hence the swap */
	//	fprintf(stderr, " thisvn %d direction %d\n", thisvn, direction[thisvn]);
		if(thisvn>=0){
			if(direction[thisvn]==1) fprintf(ffile1,"%s",thisline);
			else if(direction[thisvn]==0) fprintf(ffile0,"%s",thisline);
			else printf("cant resolve direction %s\n", thisline);
		}
		//else printf("direction not determined %d: \n", thisvn);
	}
	fclose(ffile0);
	fclose(ffile1);
	fclose(ftcpdump);

	
}
