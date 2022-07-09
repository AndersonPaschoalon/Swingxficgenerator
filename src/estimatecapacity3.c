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

#define PRINTFS 1
//# define DEBUG 1
#define PRINTEXITERROR(a)  { fprintf (stderr, "Error: ( %s: %d: %s() ) ", __FILE__, __LINE__, __FUNCTION__); perror(a); exit(-1);}
#define LINESIZE (1024*10)
#define MAGIC_NUMBER 22222
int dummyoccupy=1;
/*

Want to estimate capacity using packet dispersion techniques
If back-to-back packets are sent from a source then the sample of capacity is  Pfi/delati
where Pfi is packet size of consecutive pacjets( they have to be the same)
and deltai is the difference in timestamps of the two packets
This gives a sample of the capacity.

We have to drop the lower 50% of the sample for a flow because of delayed-ack assumption possibly not holding for all
of the packet pairs but definitely 50% of the packets are sent back to back because of delayed ack.

0 57871.6810600 10.0.0.1.80 > 10.0.0.10.1220 : P 1:209(208) ack 1202 win 17520
0 57871.6824980 10.0.0.1.80 > 10.0.0.10.1220 : . 209:745(536) ack 1202 win 17520
0 57871.6828630 10.0.0.1.80 > 10.0.0.10.1220 : . 745:1073(328) ack 1202 win 17520
0 57871.9740690 10.0.0.1.80 > 10.0.0.10.1220 : . 1073:1609(536) ack 1202 win 17520
0 57871.9741820 10.0.0.1.80 > 10.0.0.10.1220 : . 1609:2145(536) ack 1202 win 17520

   */
double tfirst, tlast;
long long int bytes;
FILE * tcpdumpfile, * perhostfile;
char thisline[LINESIZE];
char perhostfilename[LINESIZE];
char d[20][1024];
char  *src;
char  *dst;
char *flowid, *prevflowid;
double tm, prevtime;
int prevbytes;
int rev;
typedef struct {
	long long int seqstart;
	long long int seqend;
	int pkt_data;
} thisseqinfostruct;

thisseqinfostruct thisseqinfo;

void getseqnum(char * data){
	//seqinfo toreturn;
	int i;
	int state;
	thisseqinfo.seqstart=0;
	thisseqinfo.seqend=0;
	thisseqinfo.pkt_data=0;
	state=0;
	for(i=0;i<strlen(data);i++) {
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
#ifdef PRINTFS
#endif
}

int lossnum=0;
long long int prevseqbegin=0;
long long int prevseqend=0;
int duplicate_test( long long int seqbegin, long long int seqend) {
	//if(seqend<=prevseqend && ( prevseqend - seqend)>MAGIC_NUMBER) {
	if(seqend==prevseqend && seqbegin==prevseqbegin) {
		prevseqend=seqend;
		prevseqbegin=seqbegin;
		lossnum++;
		return 1;
	} else {
		prevseqend=seqend;
		prevseqbegin=seqbegin;
		lossnum=0;
		return 0;
	}
}

void Usage (char *argv[]){
	fprintf(stderr,"Usage %s tcpdumpfile\n",argv[0]);
	PRINTEXITERROR("arguments");
}
int num;
int psize;
long ack, prevack;
FILE * fpktsizefile;
int pktsizepktnum, pktsizepktsize;
char pktsizesrc[50];
int main(int argc, char *argv[]) {
	
	if(argc<2) Usage (argv);


	flowid=(char *) malloc(sizeof(char)*1024);
	prevflowid=(char *) malloc(sizeof(char)*1024);
	prevflowid[0]='\0';
	
	if((tcpdumpfile=fopen(argv[1],"r"))==NULL) PRINTEXITERROR("opening tcpdumpfile");
	prevack=0;
	fpktsizefile=fopen("pktsizefile","w");
	if(fpktsizefile==NULL) PRINTEXITERROR("could not open file pktsizefile for writing");
	num=0;psize=0;
	while(fgets(thisline,LINESIZE,tcpdumpfile)!=NULL) {
#ifdef PRINTFS
		printf("%s ", thisline);
#endif
		if(sscanf(thisline, "%s %s %s %s %s %s %s %s %s %s %s", d[1],d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9], d[10], d[11])<7) {
			PRINTEXITERROR("not enough fields");
		}
		if(strcmp(d[9],"ack")!=0) {
			/* only want packet "pairs" */
			prevflowid[0]='1'; prevflowid[1]='\0';
			goto getnextline;
		}
		rev=atoi(d[1]);
		tm=atof(d[2]);
		if(rev==0) {
			src=d[3]; dst=d[5];

		} else {
			src=d[5]; dst=d[3];
		}
		sprintf(flowid,"%s#%s\0",src,dst);
		getseqnum(d[8]);
		ack=atol(d[10]);
		//			printf("%s",thisline);
		if(strcmp(flowid,prevflowid)==0 && ack==prevack){
#ifdef DEBUG
			fprintf(stderr, " possibly packet pair\n");
#endif
			/* packet pair only if same 5 tuple and consecutive
			   data packets with same ack num */
			num++;
#ifdef DEBUG
				fprintf(stderr, " num is %d\n" , num);
#endif
			if(num>1) {
				if(psize==thisseqinfo.pkt_data && (psize>0) )
				{
					if(duplicate_test(thisseqinfo.seqstart, thisseqinfo.seqend) == 0)
					{

						if(psize>=500 && (tm> tlast))	printf("%s %f %.3f kbps %s\n", src, tm-tlast, (psize+40)*8/(tm-tlast)/1000,dst);
						pktsizepktnum++;
						pktsizepktsize=psize;
						printf("duplicate no %s", thisline);
					}
					else {
						if(lossnum==1)
							printf("duplicate yes %s",thisline);
					}
					//			printf("psize %d\n", psize);
				}

			}


			if(psize!=thisseqinfo.pkt_data) {
				num=0;tfirst=tm;
				tlast=tm;
				bytes=thisseqinfo.pkt_data;
				psize=thisseqinfo.pkt_data;
			}
			//		bytes+=(thisseqinfo.pkt_data +40);
			tlast=tm;


			/* the same flow */
		} else { 
			//	if(num>5) {
			//printf("%s %.3f %lld  %.3f kbps\n",src, tlast-tfirst, bytes, bytes*8/(tlast-tfirst)/1000);
			//	}
			if(pktsizepktnum>1 && pktsizepktsize>=500) fprintf(fpktsizefile,"%s %d %ld\n", pktsizesrc, pktsizepktsize, pktsizepktnum);
			num=0;
			tfirst=tm;
			tlast=tm;
			bytes=thisseqinfo.pkt_data;
			psize=thisseqinfo.pkt_data;
			if(psize>0) {
				printf("duplicate no %s", thisline);
			}
			prevseqbegin=0;
			prevseqend=0;
			prevack=ack;
			pktsizepktnum=0;
			pktsizepktsize=psize;
			strcpy(pktsizesrc,src);
			num=1;
		}

		strcpy(prevflowid,flowid);
getnextline:
		dummyoccupy=1;
	}
}
	//fprintf(stderr, " read file 0 \n");
