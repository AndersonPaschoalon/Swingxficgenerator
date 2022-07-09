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

#include "getconninfo.h"
/*
   This file is for taking in the TOLOG lines outputted by the getconndata and then produce one line for each connection summarizing syn time, fin time, client, server info, number of req/resp
   exchanges and the size and times of each */
#ifdef TOLOG
long long int rspovernum=0, reqovernum=0;
#endif
int prev_printed;
int numreqrsp;
int loops;
int numfields;
req_rsp_struct reqrsp[MAXNUMREQRSP+1];
int state;
char  * this_line;
char d[NUMFIELDS][FIELDSIZE];
int rev;
double tm;
char * src, *gt, *dst, *colon, *flag;
int this_flag;
int srcip[5],dstip[5];
char flowid[50], prevflowid[50];
double syn_time, synack_time, acksyn_time, req_start_time, rsp_start_time, fin_time, finack_time, ackfin_time;
long long int req_seq, req_ack, req_ack_ack , rsp_seq, rsp_ack, rsp_ack_ack, syn_seq , synack_seq,synack_ack, acksyn_ack, fin_seq, fin_ack, finack_seq, finack_ack, ackfin_seq, ackfin_ack;
long long int newack;
double rsp_end_time, req_end_time;
long long int req_bytes, rsp_bytes;
int pkt_data;
char  client[50] ,  server[50], oclient[50], oserver[50];
int hasack;
int prev_rev;
int client0=0;
int nullip=0;
int finclient0;
int newconn;
int req_printed, rsp_printed;
/* req_printed=0 always. when the size of request increase make it 1.
   wen you print the request make it 0
   this way when u abandon. if req_printed=0 then u dont have to print it
   since it means that req size has not increased since you last printed it */
seqinfo thisseqinfo;

long long int getacknum(char *data) {
	long long int toreturn=0;
	int l;
	for(l=0;l<strlen(data);l++) {
		toreturn=toreturn*10+data[l]-'0';
	}
#ifdef PRINTFS
	//fprintf(stderr, "ack %lld strlen %d\n", toreturn, strlen(data));
#endif
	return toreturn;
}
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

void getips(char * ip, int ips[5]) {
	int i=0;
	int curpos=0;
	int field=0;
	int numdots=0;
	int thisnum=0;
	int k=0;
	k=strlen(ip);
	for(i=0;i<k;i++) {
		if(ip[i]=='.') {
			(ips)[field]=thisnum;
			thisnum=0;
			field++;
			if(field>=5){
				fprintf(stderr, "too many fields in ip %s\n", ip);
				exit(0);
			}
		} else {
			thisnum=thisnum*10+ip[i]-'0';
		}
	}
	if(field!=4) {
		fprintf(stderr, " too little fields in ip %s\n", ip);
		nullip=1;
	//	exit(0);
	}
	(ips)[field]=thisnum;
	return;
	
}

void Usage(char *argv[]) {
	fprintf(stderr, "Usage %s <outputfile>\n", argv[0]);
	exit(0);
}

void setflags(char * flg){
	int k;
	int i=strlen(flg);
	for(k=0;k<i;k++){
	   switch(flg[k]){
		   case 'U': 
			//   fprintf(stderr, "flag urgent\n"); 
			   this_flag=this_flag|TCPFLG_URG;
			   break;
		   case 'A':
			 //  fprintf(stderr, "flag ack\n"); 
			   this_flag=this_flag|TCPFLG_ACK;
			   break;
		   case 'F':
			 //  fprintf(stderr, "flag fin \n"); 
			   this_flag=this_flag|TCPFLG_FIN;
			   break;
		   case 'S':
			 //  fprintf(stderr, "flag syn \n"); 
			   this_flag=this_flag|TCPFLG_SYN;
			   break;
		   case 'P':
			 //  fprintf(stderr, "flag push\n"); 
			   this_flag=this_flag|TCPFLG_PSH;
			   break;
		   case 'R':
			 //  fprintf(stderr, " flag  reset\n"); 
			   this_flag=this_flag|TCPFLG_RST;
			   break;
		   case '.':
			 //  fprintf(stderr, "no flag \n"); 
			   break;
		   default :
			   fprintf(stderr, " invalid flag %c \n", flg[k]);
			   fprintf(stderr, " ignore this line\n");
			   this_flag=TCPFLG_NON;
			   //goto getnextline
			  // exit(0);
					 
	   }

	}
	return ; 
};

int main(int argc, char *argv[]) {

	FILE * dumpfile;
	int i;

	if(argc<2) Usage(argv);

	if((dumpfile=fopen(argv[1], "r"))==NULL) {
		perror("could not open output file");
		exit(0);
	}
	this_line=(char *)malloc(sizeof(char)*LINESIZE);
	if(this_line==NULL) {
		perror("could not allocate enough memory for this_line");
	}
	if(d==NULL) {
		perror("could not allocate enough memory for d ");
		exit(0);
	}
	syn_time=0;

	while(fgets(this_line,LINESIZE, dumpfile)!=NULL ) {
#ifdef PRINTFS
		fprintf(stderr, "%s", this_line);
#endif
#ifdef PRINTTCPDUMPLINE
		fprintf(stderr, "%s", this_line);
#endif
		this_flag=0;
		if((numfields=sscanf(this_line, "%s %s %s %s %s %s %s %s %s %s %s %s", d[1],d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9], d[10], d[11],d[12]))<2) {
			fprintf(stderr, "%s", this_line);
			perror("error reading from line");
			state=CONNSTATE_BAD;
			goto getnextline;
	//		exit(0);
		}
		if(strcmp(d[1],"TOLOG:")!=0) {
			fprintf(stderr, " should start with TOLOG:\n");
			goto getnextline;
		}
		
			
testallstates:
			if(strcmp(d[2],"syntimeout")==0) {
				prev_printed=0;
			}
			else if(strstr(this_line," PREV CONN OVER")!=NULL) {
				if(prev_printed==1) {
#ifdef TOLOG
					fprintf(stderr, "PREVDATA\n");
					getips(client, srcip);
					getips(server,dstip);
					if(reqrsp[numreqrsp].rsp<=0) {
						reqrsp[numreqrsp].rsp=0;
						 //numreqrsp--;
#ifdef PRINTFS
						fprintf(stderr, " not printing %lld\n", reqrsp[numreqrsp+1].req);
#endif
					}
					if(numreqrsp>=1) {
					fprintf(stderr,"%d.%d.%d.%d %d %d.%d.%d.%d %d",srcip[0], srcip[1],srcip[2],srcip[3],srcip[4] , dstip[0],dstip[1], dstip[2], dstip[3], dstip[4]);
					fprintf(stderr, " SYN %f",syn_time); 
					fprintf(stderr, " PAIRS %d", numreqrsp);
					for(loops=1;loops<=numreqrsp;loops++)
					{
						fprintf(stderr," REQ %lld %f %f RSP %lld %f %f", reqrsp[loops].req, reqrsp[loops].reqstart, reqrsp[loops].reqend, reqrsp[loops].rsp, reqrsp[loops].rspstart, reqrsp[loops].rspend);
					}
					fprintf(stderr, " FIN %f %f\n", reqrsp[loops-1].rspend, reqrsp[loops-1].rspend);
#endif
					prev_printed=0;
					numreqrsp=0;
					}

				}
				syn_time=0;
				synack_time=0;
				acksyn_time=0;

			}
			else if(strcmp(d[2],"SYN")==0) {
#ifdef PRINTFS
				fprintf(stderr, "SYN\n");
#endif
				if(prev_printed==1) {
#ifdef TOLOG
					fprintf(stderr, "PREVDATA\n");
					getips(client, srcip);
					getips(server,dstip);
#ifdef PRINTFS
					fprintf(stderr, " numreqrsp %d \n", numreqrsp);
#endif
					if(reqrsp[numreqrsp].rsp<=0) {
						reqrsp[numreqrsp].rsp=0;
						
						//numreqrsp--;
#ifdef PRINTFS
						fprintf(stderr, " not printing %lld\n", reqrsp[numreqrsp+1].req);
#endif
					}
					if(numreqrsp>=1)
					{
					fprintf(stderr,"%d.%d.%d.%d %d %d.%d.%d.%d %d",srcip[0], srcip[1],srcip[2],srcip[3],srcip[4] , dstip[0],dstip[1], dstip[2], dstip[3], dstip[4]);
					fprintf(stderr, " SYN %f",syn_time); 
				fprintf(stderr, " PAIRS %d", numreqrsp);
				for(loops=1;loops<=numreqrsp;loops++)
				{
					fprintf(stderr," REQ %lld %f %f RSP %lld %f %f", reqrsp[loops].req, reqrsp[loops].reqstart, reqrsp[loops].reqend, reqrsp[loops].rsp, reqrsp[loops].rspstart, reqrsp[loops].rspend);
				}
				fprintf(stderr, " FIN %f %f\n", reqrsp[loops-1].rspend, reqrsp[loops-1].rspend);
					}
#endif
				prev_printed=0;
				syn_time=0;

				}
#ifdef PRINTFS
				else {
					fprintf(stderr, " nothing to print here \n");				
				}
#endif
				state=CONNSTATE_SYN;
				syn_time=atof(d[3]);
				strcpy(client, d[5]);
				strcpy(server, d[7]);
				numreqrsp=0;
				prev_printed=1;
#ifdef PRINTFS
				fprintf(stderr, "CONNINFO: SYN %f client %s server %s \n", syn_time, client, server);
#endif
				
			} /* if strcmp(d[2], "SYN")*/
			else if(strcmp(d[2],"REQSTART")==0) {
				/* see if req/rsp are over earlier */
				if(state!=CONNSTATE_SYN && state!=CONNSTATE_RSP) {

					fprintf(stderr, "CONNINFO:  error, reqstart state should not be here state %d\n", state);
				//	goto getnextline;
				}
				if(numreqrsp==MAXNUMREQRSP) {
					fprintf(stderr, "CONNINFO: PAIRS limit reached for number of pairs in a conn data structure \n");
					exit (-1);
				}
				if(numreqrsp==0) {
					if(strcmp(client, d[10])==0 && strcmp(server,d[12])==0) {
						fprintf(stderr, "client server match %s %s %s %s \n",client,d[10],server, d[12]);
					} else {
					 	fprintf(stderr, "client server do not match %s %s %s %s \n", client, d[10], server, d[12]);
						strcpy(client,d[10]);
						strcpy(server,d[12]);
					}
				}
				numreqrsp+=1;
				
				
				/* update the req/rsp data structure*/
				reqrsp[numreqrsp].req=atol(d[5]);
				reqrsp[numreqrsp].reqstart=atof(d[3]);
				reqrsp[numreqrsp].reqend  =atof(d[3]);
				reqrsp[numreqrsp].rsp=0;
				reqrsp[numreqrsp].rspstart=atof(d[3]);
				reqrsp[numreqrsp].rspend  =atof(d[3]);
				if(syn_time==0) syn_time=reqrsp[numreqrsp].reqstart;
				state=CONNSTATE_REQ;
#ifdef PRINTFS
				fprintf(stderr, "CONNINFO: numreqrsp %d req %lld time %f\n", numreqrsp, reqrsp[numreqrsp].req, reqrsp[numreqrsp].reqstart);
#endif
				
				prev_printed=1;
			} /* if strcmp(d[2],"REQSTART") */
			else if(strcmp(d[2],"REQOVER")==0) {
#ifdef TOLOG
				reqovernum++;
#endif
				if(state!=CONNSTATE_REQ) {
					fprintf(stderr, "CONNINFO: error, reqover state should not be here \n");
	//				goto getnextline;
				}
				reqrsp[numreqrsp].req=atol(d[5]);
				reqrsp[numreqrsp].reqend=atof(d[3]);
				reqrsp[numreqrsp].rspstart=atof(d[3]);
				reqrsp[numreqrsp].rsp= 0;
				reqrsp[numreqrsp].rspstart=atof(d[3]);
				reqrsp[numreqrsp].rspend  =atof(d[3]);

				if(syn_time==0) syn_time=reqrsp[numreqrsp].reqstart;
#ifdef PRINTFS
				fprintf(stderr, "CONNINFO: REQOVER numreqrsp %d req %lld time %f - %f\n", numreqrsp, reqrsp[numreqrsp].req, reqrsp[numreqrsp].reqstart, reqrsp[numreqrsp].reqend);
#endif
				prev_printed=1;
			} /* if strcmp(d[2],"REQOVER") */
			else if(!strcmp(d[2],"REQCONTINUE")) {
				state=CONNSTATE_REQ;
				prev_printed=1;
			} /* if strcmp(d[2],"REQCONTINUE) */
			else if(!strcmp(d[2],"RSPCONTINUE")) {
				state=CONNSTATE_RSP;
				prev_printed=1;
			} /* if strcmp(d[2],"RSPCONTINUE) */
			else if(strcmp(d[2],"RSPSTART")==0) {
				/* see if req is over earlier */
				if(state!=CONNSTATE_REQ) {
					fprintf(stderr, "CONNINFO:  error, rspstart state should not be here state %d\n", state);
				//	goto getnextline;
				}
				/* update the req/rsp data structure*/
				reqrsp[numreqrsp].rsp=atol(d[5]);
				reqrsp[numreqrsp].rspstart=atof(d[3]);
				reqrsp[numreqrsp].rspend  =atof(d[3]);
				state=CONNSTATE_RSP;
#ifdef PRINTFS
				fprintf(stderr, "CONNINFO: numreqrsp %d rsp %lld time %f\n", numreqrsp, reqrsp[numreqrsp].rsp, reqrsp[numreqrsp].rspstart);
#endif
				
				prev_printed=1;
			} /* if strcmp(d[2],"RSPSTART") */
			else if(strcmp(d[2],"RSPOVER")==0) {
#ifdef TOLOG
				rspovernum++;
#endif
				if(state!=CONNSTATE_RSP) {
					fprintf(stderr, "CONNINFO:  error, rspover state should not be here \n");
				//	goto getnextline;
				}
				reqrsp[numreqrsp].rsp=atol(d[5]);
				reqrsp[numreqrsp].rspend=atof(d[3]);
				fin_time=atof(d[3]);
#ifdef PRINTFS
				fprintf(stderr, "CONNINFO: RSPOVER numreqrsp %d rsp %lld time %f - %f\n", numreqrsp, reqrsp[numreqrsp].rsp, reqrsp[numreqrsp].rspstart, reqrsp[numreqrsp].rspend);
#endif
				prev_printed=1;
			} /* if strcmp(d[2],"RSPOVER") */
			else if(strcmp(d[2], "ACKSYN")==0) {
				if(syn_time==0) {syn_time=atof(d[3]);
				synack_time=atof(d[3]);}
				goto getnextline;
			}
				

		goto getnextline;
getnextline:
#ifdef PRINTFS
		fprintf(stderr, "%s", this_line);
#endif
		sleep(0);
	}
#ifdef TOLOG
	fprintf(stderr, "rspovernum %lld reqovernum %lld\n", rspovernum, reqovernum);
#endif


	if(prev_printed==1) {
#ifdef TOLOG
		fprintf(stderr, "PREVDATA\n");
		getips(client, srcip);
		getips(server,dstip);
#ifdef PRINTFS
		fprintf(stderr, " numreqrsp %d \n", numreqrsp);
#endif
		if(reqrsp[numreqrsp].rsp<=0) {
			reqrsp[numreqrsp].rsp=0;

			//numreqrsp--;
#ifdef PRINTFS
			fprintf(stderr, " not printing %lld\n", reqrsp[numreqrsp+1].req);
#endif
		}
		if(numreqrsp>=1)
		{
			fprintf(stderr,"%d.%d.%d.%d %d %d.%d.%d.%d %d",srcip[0], srcip[1],srcip[2],srcip[3],srcip[4] , dstip[0],dstip[1], dstip[2], dstip[3], dstip[4]);
			fprintf(stderr, " SYN %f",syn_time); 
			fprintf(stderr, " PAIRS %d", numreqrsp);
			for(loops=1;loops<=numreqrsp;loops++)
			{
				fprintf(stderr," REQ %lld %f %f RSP %lld %f %f", reqrsp[loops].req, reqrsp[loops].reqstart, reqrsp[loops].reqend, reqrsp[loops].rsp, reqrsp[loops].rspstart, reqrsp[loops].rspend);
			}
			fprintf(stderr, " FIN %f %f\n", reqrsp[loops-1].rspend, reqrsp[loops-1].rspend);
		}
#endif
		prev_printed=0;

	}





	return 0;
}




