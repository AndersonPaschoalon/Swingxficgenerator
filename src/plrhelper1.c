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
#include <string.h>
#include <stdlib.h>

#define PRINTFS 1
# define DEBUG 1
#define PRINTEXITERROR(a)  { fprintf (stderr, "Error: ( %s: %d: %s() ) ", __FILE__, __LINE__, __FUNCTION__); perror(a); exit(-1);}
#define LINESIZE (1024*10)
#define MAGIC_NUMBER 22222
enum flags {NUMPKTS, HOLE, OOO, EEE, NONE};
enum flags prevflag;
int entireflight=0;
int extraholes=0;
double tfirst, tlast;
long long int bytes;
FILE * tcpdumpfile, * perhostfile;
char thisline[LINESIZE];
char perhostfilename[LINESIZE];
char d[20][1024];
char  *src;
char  *dst;
char *flowid, *prevflowid;
double tm, prevtime, prevpkttime, difftime;
int prevbytes;
int rev;
long long int prevseqbegin;
long long int prevseqend;
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
	//fprintf(stderr, "start %lld  end %lld bytes %d\n", thisseqinfo.seqstart, thisseqinfo.seqend, thisseqinfo.pkt_data);
#endif
	//return thisseqinfo ;
}

int lossnum=0;
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
//FILE * fpktsizefile;
int pktsizepktnum, pktsizepktsize;
FILE * holesinfofile;
char pktsizesrc[50];
long long int numpkts;
int guesspsize=0;
long long int holes;

int main(int argc, char *argv[]) {
	
	if(argc<2) Usage (argv);


	flowid=(char *) malloc(sizeof(char)*1024);
	prevflowid=(char *) malloc(sizeof(char)*1024);
	prevflowid[0]='\0';
	
	if(!strcmp(argv[1],"-")) {

		if((tcpdumpfile=fopen("/dev/stdin","r"))==NULL) PRINTEXITERROR("opening tcpdumpfile");
	} else {
		if((tcpdumpfile=fopen(argv[1],"r"))==NULL) PRINTEXITERROR("opening tcpdumpfile");
	}
	if( (holesinfofile=fopen("holesinfofile","w") )==NULL) PRINTEXITERROR("opening holesinfofile");
	prevack=0;
	//fpktsizefile=fopen("pktsizefile","w");
	//if(fpktsizefile==NULL) PRINTEXITERROR("could not open file pktsizefile for writing");
	num=0;psize=0;numpkts=0;
	while(fgets(thisline,LINESIZE,tcpdumpfile)!=NULL) {
#ifdef PRINTFS
		printf("----------------------------------------------------------------\n");
		printf("%s ", thisline);
#endif
		if(sscanf(thisline, "%s %s %s %s %s %s %s %s %s %s %s", d[1],d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9], d[10], d[11])<7) {
			PRINTEXITERROR("not enough fields");
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
#ifdef PRINTFS
		printf(" thisseqinfo.seqstart=%lld prevseqend=%lld numpkts=%lld guesspsize=%d tm=%f prevtime=%f diff=%f\n", thisseqinfo.seqstart, prevseqend, numpkts, guesspsize, tm, prevpkttime, tm - prevpkttime);
		printf("now processing this new line \n");
#endif
		if((tm - prevpkttime)<3600) {
			difftime=tm- prevpkttime;
		} else {
			difftime=0;
		}
		if(!strcmp(flowid, prevflowid)) {
			if(thisseqinfo.pkt_data>guesspsize) guesspsize=thisseqinfo.pkt_data;
			if ( (thisseqinfo.seqstart==prevseqend) || (prevseqend > MAGIC_NUMBER && numpkts <=7) || ((thisseqinfo.seqstart - prevseqend) > MAGIC_NUMBER)) {
#ifdef PRINTFS
				printf("in order numpkts so far %lld\n", numpkts);
#endif
				if(difftime>=1 && ack==prevack) {
				//if(difftime>=1 ) {
#ifdef PRINTFS
				printf("maybe packet was lost and resent and we did not notice\n");
#endif
				fprintf(holesinfofile,"%s MAYBEHOLE %lld %f\n", flowid, prevseqend, prevtime);

				}
				numpkts++;
				/* if all is fine then I saw a packet */
				if( (thisseqinfo.seqstart - prevseqend) < MAGIC_NUMBER)  {
				prevseqbegin=thisseqinfo.seqstart;
				prevseqend=thisseqinfo.seqend;
				}
				prevflag=NUMPKTS;
			} else if(thisseqinfo.seqstart > prevseqend ) {

				holes=prevseqend;

#ifdef PRINTFS
				printf("begin holes = %lld thisseqinfo.seqstart = %lld guesspsize = %d \n", holes, thisseqinfo.seqstart, guesspsize);
#endif
				entireflight=0;
				if(((thisseqinfo.seqstart - prevseqend)/guesspsize) >= 4) {
					entireflight=1;
					extraholes=0;
					//fprintf(holesinfofile,"%s HOLE %lld %f\n", flowid, prevseqend, prevtime);
				}
				while (holes<thisseqinfo.seqstart) {
	//				if(entireflight!=1)
						fprintf(holesinfofile,"%s HOLE %lld %f\n", flowid, prevseqend, prevtime);
					prevseqbegin+=guesspsize;
					prevseqend+=guesspsize;
					holes=prevseqend;
					if(entireflight==1)
						extraholes++;
#ifdef PRINTFS
				printf("###########hole ... %lld\n", prevseqbegin);
#endif
				}
				numpkts++;

				if(entireflight==1)
					fprintf(holesinfofile,"%s EEE %d %lld %f\n", flowid,extraholes, thisseqinfo.seqstart, tm);
				entireflight=0;
				extraholes=0;

#ifdef PRINTFS
				printf("end holes = %lld thisseqinfo.seqstart = %lld guesspsize = %d \n", holes, thisseqinfo.seqstart, guesspsize);
				printf("hole \n");
#endif
				prevseqbegin=thisseqinfo.seqstart;
				prevseqend=thisseqinfo.seqend;
				prevflag=HOLE;
			} else if(thisseqinfo.seqend <= prevseqbegin) {
				if(((tm - prevpkttime) < 0.2) && (prevflag!=OOO) && ((prevseqend - thisseqinfo.seqend)< (guesspsize*3)) ) {
#ifdef PRINTFS
					printf("looks like OOO but in order numpkts so far %lld\n", numpkts);
#endif
					numpkts++;

				}
				else{
#ifdef PRINTFS
				printf("earlier semgment appears now\n");
#endif
				fprintf(holesinfofile,"%s OOO %lld %f\n", flowid, thisseqinfo.seqstart, tm);
				}
				prevflag=OOO;
			}
			else {

#ifdef PRINTFS
				printf("resend earlier segment\n");
#endif
				numpkts++;
				numpkts--;
				//fprintf(holesinfofile,"%s OOO %lld %f\n", flowid, thisseqinfo.seqstart, tm);

				//prevflag=OOO;
			}
	//		if(prevflag!=OOO){
				prevpkttime=tm;
	//		}
			
		} else {
			/* old over */
#ifdef PRINTF
			printf("old over \n");
#endif
			if(numpkts>0) fprintf(holesinfofile,"%s NUMPKTS %lld\n", prevflowid, numpkts);
			prevseqend=0;
			prevseqbegin=0;
			prevseqbegin=thisseqinfo.seqstart;
			prevseqend=thisseqinfo.seqend;
			prevtime=tm;
			numpkts=1;
			guesspsize=thisseqinfo.pkt_data;
			prevflag=NUMPKTS;
		}
		
getnextline:
#ifdef PRINTFS
				printf("prev flag is %d\n", prevflag);
#endif
		strcpy(prevflowid, flowid);
		prevtime=tm;
		prevack=ack;

	}
			fprintf(holesinfofile,"%s NUMPKTS %lld\n", prevflowid, numpkts);
			return 0;
}
	//fprintf(stderr, " read file 0 \n");;
