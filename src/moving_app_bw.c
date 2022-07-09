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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int dummy1;
#define QUICKIE_STRCMP(a,b) (*(a)!=(*b) ? (int) ( (unsigned char ) *(a) - (unsigned char ) *(b) ) : strcmp((&a[1]),(&b[1])))
#define LINESIZE 1024
void Usage( char * argv[]){
	fprintf(stderr, "Usage %s <inputfile> <dur>\n" ,argv[0]);
	fprintf(stderr, "output: current - firsttm, pkts, mbps, pps, bytes \n");
	exit(-1);
}

FILE * infile;
char thisline[LINESIZE];
double timebin , tm;
double current=0,firsttm;
int linenum;
char *gt, *colon;
char d[20][LINESIZE];
long long int bytes;
int intbytes;
long long int pkts;
typedef struct {
	long long int seqstart;
	long long int seqend;
	int pkt_data;
} thisseqstruct;
thisseqstruct thisseqinfo;

void getseqnum(char * data){
	int i;
	int state;
	int len;
	thisseqinfo.seqstart=0;
	thisseqinfo.seqend=0;
	thisseqinfo.pkt_data=0;
	state=0;
	len=strlen(data);
	for(i=0;i<len;i++) {
		if(state==0) {
			if(data[i]==':') {
				state=1;
			}else thisseqinfo.seqstart=thisseqinfo.seqstart*10+data[i]-'0';
		} else if(state==1) {
			if(data[i]=='(') {
				state=2;
			}else thisseqinfo.seqend=thisseqinfo.seqend*10+data[i]-'0';
		} else if(state==2) {
			if(data[i]==')'){
				state=3;
			}else thisseqinfo.pkt_data=thisseqinfo.pkt_data*10+data[i]-'0';
		} else {
			fprintf(stderr, " something illegal %s\n", data);
			exit(0);
		}
	}
}


int main (int argc, char *argv[]){
	if(argc<3) Usage(argv);

	if(!strcmp(argv[1],"-")){
		if((infile=fopen("/dev/stdin","r"))==NULL) {
			perror("cant open stdin");
			exit(-1);
		}
		
	}
    else
	if((infile=fopen(argv[1],"r"))==NULL) {
		perror("cant open infile");
		exit(-1);
	}
	timebin=atof(argv[2]);
	current=0;
	linenum=0;
	//printf("start\n");
	while(fgets(thisline,LINESIZE,infile)!=NULL ){
		//if(sscanf(thisline, "%s %s %s %s %s %s %s %s %s %s %s", d[1],d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9], d[10], d[11])<8 && QUICKIE_STRCMP(d[5],"UDP,")!=0) {}
		if(sscanf(thisline, "%s %s %s %s %s %s %s %s ", d[1],d[2], d[3], d[4], d[5], d[6], d[7], d[8] )<8 && QUICKIE_STRCMP(d[5],"UDP,")!=0) {
			//perror("error reading from line");
			goto getnextline;
		}
		//printf("%s", thisline);

		tm=atof(d[1]);

		gt=d[3];
		//if(strcmp(gt,">")!=0) {}
		if(gt[0]!='>') {
		//	fprintf(stderr, " this symbol %s should have been >\n", d[4]);
			continue;
	//		exit(-1);
		}
		
		if(linenum==0) {
			firsttm=tm;
			current=tm;
			bytes=0;
			pkts=0;
		}
		if((tm - current)>=timebin) {
		//printf("%.3f %lld %.4f %.4f %lld\n", current - firsttm, pkts, bytes*8/timebin/1000000, pkts/timebin, bytes);
		printf("%.3f %lld %.4f %.4f %lld\n", current - firsttm, pkts, bytes*8/(tm - current)/1000000, pkts/timebin, bytes);
			current+=timebin;
			while((current+timebin)<tm) {
				printf("%.3f 0 0 0 0\n",current - firsttm);
				current+=timebin;
			}
	//		current=tm;
			pkts=0;
			bytes=0;
	// "time, pkts per given duration, mbps, pps, MB generated"
		}
	//	pkts++;

		if(QUICKIE_STRCMP(d[7],"ack")==0) {
	//		if(d[7][0]=='a'&& d[7][1]=='c' && d[7][2]=='k') {}
				getseqnum(d[6]);
			bytes+=thisseqinfo.pkt_data;
	//		if(thisseqinfo.pkt_data>0) pkts++;
			pkts++;
		} else if(QUICKIE_STRCMP(d[5],"UDP,")==0){
			intbytes=atoi(d[7]);
			bytes+=intbytes;
			if(intbytes>0) pkts++;
		//	pkts++;
		}else {
			bytes+=0;
	//		pkts++;
		}


		

	linenum=1;
getnextline:
	dummy1=1;
	}
	//printf("stop\n");
	if((tm - current) > 1) {
		printf("%.3f %lld %.4f %.4f %lld\n", current - firsttm, pkts, bytes*8/timebin/1000000, pkts/timebin, bytes);
	}
	return 0;
}

