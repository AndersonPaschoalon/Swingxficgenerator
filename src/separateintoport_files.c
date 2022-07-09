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
#include<stdio.h>
#include<string.h>
#include<limits.h>
#include<stdlib.h>

#define LINESIZE 1024
enum protocol {UDP=-1, TCP=1};

char this_line[LINESIZE];

void Usage(char * argv[]){
	fprintf(stderr, "Usage %s <logfile> <portsfile> <dport|sport|port> \n", argv[0]);
	fprintf(stderr, "takes input file and produces outputfiles for various port numbers\n");
	exit(1);
}
int main(int argc, char *argv[]) {

	FILE *flogfile, * fportfile , **portfiles;
	char ** portnames;
	int thisport;
	int portwatch;
	int numports;
	int match;
	int printthis=0;
	int * portnums;
	char dummychars[20], dummy1[100], dummy2[100], dummy3[100], dummy4[100], dummy5[100], dummy6[100];
	char sport[20], dport[20];
	int thistcp, thisudp;
	char portfilename[LINESIZE];
	int portval;

	if(argc< 4) Usage(argv);
	if(!strcmp(argv[3], "sport")) 
		portwatch=1;
	else if(!strcmp(argv[3], "dport"))
		portwatch=2;
	else if(!strcmp(argv[3],"port"))
		portwatch=3;
	else	Usage(argv);

	fportfile=fopen(argv[2],"r");
	if(!fportfile) {
		fprintf(stderr, "cannot open file %s ",argv[2]);
		perror(" ");
		exit(-1);
	}
	flogfile=fopen(argv[1],"r");
	if(!flogfile) {
		fprintf(stderr, "cannot open file %s ",argv[1]);
		perror(" ");
		exit(-1);
	}

	if(fgets(this_line, LINESIZE, fportfile)==NULL) {
		fprintf(stderr, " cannot read from file %s", argv[2]);
		perror(" ");
		exit(-1);
	}
	if(sscanf(this_line, "%s %d", dummychars, &numports)!=2) {
		fprintf(stderr, " error in reading in proper format the first line from %s", argv[2]);
		perror(" ");
		exit(-1);
	}
	portnums=(int *) malloc(sizeof(int)*numports);
	portfiles=(FILE **) malloc(sizeof(FILE *) * (numports+2));
	portnames=(char **) malloc(sizeof(char *) * (numports+2));

	/* read in all the ports and put times -1 for udp */ 

	for(thisport=0;thisport<numports; thisport++) {
		if(fgets(this_line, LINESIZE, fportfile)==NULL) {
			fprintf(stderr, " cannot read from file %s", argv[2]);
			perror(" ");
			fflush(stderr);
			exit(-1);
		}
		portnames[thisport]=(char *) malloc(sizeof(char)*20);
		if(sscanf(this_line, "%d %s %s", &portnums[thisport], dummychars, portnames[thisport])!=3) {
			sprintf(portnames[thisport],"name%d",thisport);
			if(sscanf(this_line, "%d %s", &portnums[thisport], dummychars)!=2) {
				fprintf(stderr, " error in reading in proper format line from %s", argv[2]);
				perror(" ");
				fflush(stderr);
				exit(-1);
			}
		}
		else {
			sscanf(dummychars,"tcp ",portnames[thisport]);
		}
	//	printf("portname %s\n",portnames[thisport]);
		if(!strncasecmp("udp", dummychars, 3)) {
			sprintf(portfilename, "%s.udp.%d", argv[1], portnums[thisport]);
			portnums[thisport]=portnums[thisport]*-1;
		}
		else {
	//		printf("dummychars=%s\n",dummychars);
			sprintf(portfilename, "%s.tcp.%d", argv[1], portnums[thisport]);
		}
		portfiles[thisport]=fopen(portfilename,"w");
		if(!portfiles[thisport]) {
			fprintf(stderr, "could not open port file %s \n", portfilename);
			perror(" ");
			fflush(stderr);
			exit(-1);
		}
		
	}
	sprintf(portfilename,"%s.tcp.other", argv[1]);
	if((portfiles[numports]=fopen(portfilename, "w"))==NULL) {
		fprintf(stderr, "problem opening file %s", portfilename);
		perror(" ");
		fflush(stderr);
		exit(-1);
	}
	
	sprintf(portfilename,"%s.udp.other", argv[1]);
	if((portfiles[numports+1]=fopen(portfilename, "w"))==NULL) {
		fprintf(stderr, "problem opening file %s", portfilename);
		perror(" ");
		fflush(stderr);
		exit(-1);
	}

	fclose(fportfile);
	//for(thisport=0;thisport<numports;thisport++) printf(" port %d %d\n", thisport, portnums[thisport]);
	//exit(1);
	while(fgets(this_line, LINESIZE, flogfile)) {
		if(sscanf(this_line, "%s %s %s %s %s", dummy1, dummy2, dummy3, dummy4, dummy5)!=5) {
			goto nextline;
		}
		printthis=0;
		thisudp=0;
		thistcp=0;
		if(!strncasecmp(dummy5, "udp",3))  {
			thisudp=1;
			goto handleudp;
		}
		if(strstr(this_line, " win ")!=NULL || strstr(this_line, " ack ")!=NULL) {
			thistcp=1;
		} else goto nextline;

		match=0;
		if(strrchr(dummy2, '.')==NULL) goto nextline;
		else if(strrchr(dummy4,'.')==NULL) goto nextline;
	//	if((portwatch&0x1)==1) {
			/* get sport */
			sprintf(sport,"%s", strrchr(dummy2, '.')+(sizeof(char)));
			portval=atoi(sport);
			if(!(portval>=INT_MIN && portval<=INT_MAX)) { 
				//fprintf(stderr, "weird  1 ");
				goto nextline             ;}
			for(thisport=0;thisport<numports;thisport++)
				if((portnums[thisport]==portval) || ( portnums[thisport]==(portval*-1))){
					printthis=1;
					match=1;
					fprintf(portfiles[thisport], "%s", this_line);
				}
	//	}
	//	match=0;
	//	if((portwatch&0x2)==2) {
			/* get dport */
		if(match==0){
			sprintf(dport,"%s", strrchr(dummy4, '.')+(sizeof(char)));
			if(strrchr(dport, ':')==NULL) continue;
			*(strrchr(dport, ':'))='\0';
			portval=atoi(dport);
			if(!(portval>=INT_MIN && portval<=INT_MAX)) {
				//fprintf(stderr, "weird 2 ");
				goto nextline             ;}
			for(thisport=0;thisport<numports;thisport++)
				if((portnums[thisport]==portval) || ( portnums[thisport]==(portval*-1))){
					printthis=1;
					match=1;
					fprintf(portfiles[thisport], "%s", this_line);
				}
		}
	//					fprintf(stderr, "1 \n");
	//	}
handleudp:
		if(printthis==0) { 
			if(thisudp==1) {
				printthis=1;
				fprintf(portfiles[numports+1],"%s", this_line);
			}
			else if(thistcp==1) {
				printthis=1;
				fprintf(portfiles[numports],"%s", this_line);
			}
		}



		/* test */
nextline:
		if(printthis==0) {
	//		fprintf(stderr, "%s", this_line);
		}
	//	fprintf(stderr, "\n--------\n");
	}
	fclose(flogfile);
	for(thisport=0;thisport< numports; thisport++) {
		fclose(portfiles[thisport]);
	}
		
	return 0;
}
