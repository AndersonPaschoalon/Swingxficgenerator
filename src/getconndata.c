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

#include "getconndata.h"

/* RSP ACK ACK are the acks for responses
   RSP ACK are the acks that go along with the responses(i.e. whatever the server acks, for insance)
   */
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
double rsp_ack_ack_time, req_ack_ack_time;
long long int req_seq, req_ack, req_ack_ack , rsp_seq, rsp_ack, rsp_ack_ack, syn_seq , synack_seq,synack_ack, acksyn_ack, fin_seq, fin_ack, finack_seq, finack_ack, ackfin_seq, ackfin_ack;
long long int newack;
double rsp_end_time, req_end_time;
long long int req_bytes, rsp_bytes;
int pkt_data;
long long int ttlbytes=0;
char  client[50] ,  server[50], oclient[50], oserver[50];
int hasack;
int prev_rev;
int client0=0;
long long int latest_req_size, latest_rsp_size;
int nullip=0;
int finclient0;
int newconn;
int rspabovethreshold=0;
int req_printed, rsp_printed;
int adjusts=0, adjusts_rsp=0, adjusts_req=0;
long long int adjusts_rsp_seq_max=0, adjusts_rsp_seq_min=9999999;
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
#ifdef PRINTFS
			fprintf(stderr, " something illegal %s\n", data);
#endif
			//exit(0);
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
				//exit(0);

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
	fprintf(stderr, "Usage %s <tcpdumpfile>\n", argv[0]);
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
	int thispkts=0;
	int i;

	if(argc<2) Usage(argv);

	if((dumpfile=fopen(argv[1], "r"))==NULL) {
		perror("could not open tcpdump file");
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

	while(fgets(this_line,LINESIZE, dumpfile)!=NULL ) {
#ifdef PRINTFS
		fprintf(stderr, "%s", this_line);
		fprintf(stderr," rspbytes %ld\n", rsp_bytes);
		fprintf(stderr, " req ack %lld, rsp ack %lld req seq %lld, rsp seq %lld, req_ack_ack %lld, rsp_ack_ack %lld ttlbytes %lld\n",  req_ack, rsp_ack, req_seq, rsp_seq, req_ack_ack,
				rsp_ack_ack,ttlbytes );
		fprintf(stderr, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> req %lld acco. to seq %lld \n", req_bytes, req_seq);
		fprintf(stderr, "rspabovethreshold=%d\n", rspabovethreshold);
		fprintf(stderr, "adjusts_rsp=%d\n", adjusts_rsp);
		fprintf(stderr," adjusts_rsp_seq_min=%lld, adjusts_rsp_seq_max=%lld\n", adjusts_rsp_seq_min, adjusts_rsp_seq_max);
		fprintf(stderr, " rsp_printed=%d, req_printed=%d\n",rsp_printed,req_printed);
#endif
#ifdef PRINTTCPDUMPLINE
		fprintf(stderr, "%s", this_line);
#endif
		this_flag=0;
		if(sscanf(this_line, "%s %s %s %s %s %s %s %s %s %s %s", d[1],d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9], d[10], d[11])<8) {
			fprintf(stderr, "%s", this_line);
			perror("error reading from line");
			state=CONNSTATE_BAD;
			goto getnextline;
			//		exit(0);
		}
		rev=atoi(d[1]);
		if(rev<0 || rev>1) {
			fprintf(stderr, "rev %s outside 0/1 range\n", d[1]);
			exit(0);
		}
		tm=atof(d[2]);

		src=d[3];
		nullip=0;
		getips(src, srcip);
		if(nullip==1) goto getnextline;
		gt=d[4];
		if(strcmp(gt,">")!=0) {
			fprintf(stderr, " this symbol %s should have been >\n", d[4]);
			exit(-1);
		}
		dst=d[5];
		getips(dst, dstip);
		colon=d[6];
		if(strcmp(colon,":")!=0) {
			fprintf(stderr, " this symbol %s should have been :\n", d[6]);
			exit(-1);
		}

		flag=d[7];
		setflags(flag);
		if(this_flag == TCPFLG_NON) goto getnextline;
		hasack=0;
#ifdef PRINTFS
		//	fprintf(stderr, " hasack,%d flags %d\n", hasack, this_flag);
#endif
		if(strcmp(d[8],"ack")==0) hasack=1;
		else if(strcmp(d[9],"ack")==0) hasack=1;
		if(hasack==1) this_flag= this_flag|TCPFLG_ACK;
		sprintf(flowid, "%s#%s\0", src, dst);
		newconn=0;
		if(strcmp(flowid,prevflowid)!=0) {
#ifdef PRINTFS
			fprintf(stderr, " take care of old \n");
#endif

			if(req_printed==1)
			{
#ifdef TOLOG
				fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, oclient, oserver);
#endif
				req_printed=0;
			}

			if(rsp_printed==1)
			{
#ifdef TOLOG
				/* remove the below, at least for SIGCOMM */
				/* PUT it back now post sigcomm Fri Feb 17 10:09:58 PST 2006 */
				if(rspabovethreshold!=1 || (ADJUSTTHRESHOLDFRACTION*rsp_bytes>adjusts_rsp))
				fprintf(stderr, "TOLOG: RSPOVER %f sent %lld bytes client %s server %s\n", rsp_end_time, rsp_bytes, oclient, oserver);
#endif
				rsp_printed=0;
			}



#ifdef TOLOG
			fprintf(stderr, "TOLOG: PREV CONN OVER\n");

#endif

#ifdef PRINTFS
			fprintf(stderr, " *********************************************************************************************************************************\n");
			fprintf(stderr, " new conn ************************************************************************************************************************\n");
			fprintf(stderr, " *********************************************************************************************************************************\n");
#endif

			state=CONNSTATE_BAD;
			newconn=1;
			thispkts=1;
			adjusts=0;adjusts_req=0; adjusts_rsp=0;
			rsp_ack_ack=-1;rsp_ack=-1;req_ack_ack=-1;req_ack=-1;
			rsp_seq=-1;req_seq=-1;
			syn_seq=-1; synack_ack=-1; 
			syn_time=0; synack_time=0; acksyn_time=0; acksyn_ack=-1;
			adjusts_rsp_seq_max=ADJUSTRSPSEQMAX; adjusts_rsp_seq_min=ADJUSTRSPSEQMIN;
			client0=-1 ; latest_rsp_size=0; latest_req_size=0;
			rspabovethreshold=0;
		} /* strcmp flowid, prevflowid */ 
		else {
			thispkts++;
		}
		if(state==CONNSTATE_BAD) {

			if(newconn !=1 && (this_flag&TCPFLG_SYN != TCPFLG_SYN) ) {
#ifdef PRINTFS
				fprintf(stderr, " this conn already abandoned. waiting for new conn \n");
#endif 
#ifdef TOLOG
				fprintf(stderr, "TOLOG: JUNK DATA waiting for new conn \n");
#endif
				goto getnextline;
			}
		}


testallstates:
		/* STATETEST 0  TESTING FOR SYN */
		if((this_flag & TCPFLG_SYN) == TCPFLG_SYN) {
			if(hasack==0) {
				/* start of new conn */
#ifdef PRINTFS
				fprintf(stderr, " SYN \n");
#endif
				if(state!=CONNSTATE_BAD) {
					if((tm - syn_time )>=SYNTIMEOUT) {
#ifdef TOLOG
						fprintf(stderr, "TOLOG: syntimeout\n");
#endif
					} else {
#ifdef PRINTFS
						fprintf(stderr, " weird. 5 tuple repeats\n");
						fprintf(stderr, "%s", this_line);
#endif
						goto getnextline;
					}
					//	exit(0);
				}
				state=CONNSTATE_SYN;
				syn_time=tm;

				getseqnum(d[8]);
				if(thisseqinfo.seqstart>thisseqinfo.seqend ) {
					fprintf(stderr, " weird %s in SYN packet \n", d[8]);
	//				exit(0);
					goto getnextline;
				}
				syn_seq=thisseqinfo.seqstart;
				if(rev==0) {
					client0=1;
					strcpy(client, src);
					strcpy(server, dst);
				}else {
					client0=0;/* means when rev=0 then the client is the server */
					strcpy(client, dst);
					strcpy(server, src);
				}
#ifdef PRINTFS
				fprintf(stderr, " SYN %f client %s server %s seq %lld \n", syn_time, client, server, syn_seq);
#endif
#ifdef TOLOG
				fprintf(stderr, "TOLOG: SYN %f client %s server %s\n", syn_time, client, server);
#endif
				req_printed=0;
				rsp_printed=0;
			}
			else {
			/* syn does not have ack */
#ifdef PRINTFS
				fprintf(stderr, " SYN+ACK \n");
#endif
				
				if(state!=CONNSTATE_SYN){
					newack=getacknum(d[10]);
					getseqnum(d[8]);
					if(thisseqinfo.seqend<=acksyn_ack && state!=CONNSTATE_BAD) {
#ifdef PRINTFS
						fprintf(stderr, "ignore the duplicate synacka\n");
#endif
						state=CONNSTATE_BAD;
						goto testallstates;
#ifdef TOLOG
						/* NEEDRETRANSMIT */

LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d seqend %lld\n", tm , client, server,client0,rev,thisseqinfo.seqend);  */
#endif
						
						goto getnextline;
					} else if (state==CONNSTATE_BAD ) {
#ifdef PRINTFS
						fprintf(stderr, " maybe SYN was sent and nto seen and this is SYN/ACK after a RST \n");
#endif
						newack=getacknum(d[10]);
						getseqnum(d[8]);


						syn_time=tm;
						synack_time=tm;
						syn_seq=-1;
						synack_seq=newack-1;
						synack_ack=-1;


						if(rev==0) {
							client0=0; /* since we are pretending that this is ackSYN*/
							strcpy(server,src);
							strcpy(client,dst);
						} else { 
							client0=1;
							strcpy(server,dst);
							strcpy(client,src);
						}
#ifdef TOLOG
						fprintf(stderr,"TOLOG: did not see SYN and SYN+ACK set but setting it and trying\n");
						fprintf(stderr, "TOLOG: SYN %f client %s server %s\n", syn_time, client, server);
						fprintf(stderr, "TOLOG: SYNACK %f client %s server %s\n", synack_time, client, server);
#endif
						//this_flag=this_flag|TCPFLG_SYN;
						state=CONNSTATE_SYNACK;
						adjusts_rsp=0;adjusts_req=0; adjusts=0;






						
						
						goto getnextline;
						
					} else {
#ifdef PRINTFS
						fprintf(stderr, " weird, syn+ack not after syn %s", this_line);
						fprintf(stderr, " prev state %d\n", state);
#endif
#ifdef TOLOG
						fprintf(stderr, "TOLOG: ABANDON conn reasons 2 client %s server %s \n",  client, server);

#endif 
					//	state=CONNSTATE_BAD;
						goto getnextline;
						//					exit(0);
					}
				}
				state=CONNSTATE_SYNACK;
				synack_time=tm;
				getseqnum(d[8]);
				if(thisseqinfo.seqstart!=thisseqinfo.seqend || thisseqinfo.pkt_data!=0) {
#ifdef PRINTFS
					fprintf(stderr, " weird %s in SYN+ACK packet \n", d[8]);
#endif
					//exit(0);
				}
				synack_seq=thisseqinfo.seqstart;
				synack_ack=getacknum(d[10]);
				if(synack_ack!=syn_seq+1) {
#ifdef PRINTFS
					fprintf(stderr, " syn+ack not acking the correct syn %s ", this_line);
#endif
					//exit(0);
					goto getnextline;
				}
#ifdef PRINTFS
				fprintf(stderr, " SYN+ACK %f client %s server %s seq %lld ack %lld \n", synack_time, client, server, synack_seq, synack_ack);
#endif
#ifdef TOLOG
				fprintf(stderr, "TOLOG: SYNACK %f client %s server %s\n", synack_time, client, server);
						if(thispkts<5) 
				fprintf(stderr, "TOLOG: TIME SYN_SYNACK %f client %s server %s\n", synack_time - syn_time, client, server);
#endif

			}
		} /* if SYN is set */

		/* STATETEST 1  TESTING FOR FIN */
		else if ((this_flag & TCPFLG_FIN)== TCPFLG_FIN) {
			/*
			 */
			/* see if there is data */
			if(strcmp(d[8],"ack")!=0) {
				/* has data */
				getseqnum(d[8]);
				newack=getacknum(d[10]);
				if((state==CONNSTATE_REQ || state==CONNSTATE_REQACK)&& client0!=rev) {
					/* more of req along with FIN */
#ifdef PRINTFS
					fprintf(stderr, " Request along with FIN\n");
#endif
					if(thisseqinfo.seqstart>=req_seq && newack==req_ack) {
						req_bytes+=thisseqinfo.pkt_data;
						if(thisseqinfo.seqstart> req_seq && ((thisseqinfo.seqstart - req_seq) < MAGIC_NUMBER))
							req_bytes+=thisseqinfo.seqstart - req_seq;
						req_seq=thisseqinfo.seqend;
						req_end_time=tm;
						fin_time=tm;
						fin_seq=thisseqinfo.seqend;
						if(rev==0) finclient0=1;
						else finclient0=0;

#ifdef PRINTFS
						fprintf(stderr, " REQ END with FIN %f sent %lld bytes seq %lld ack %d\n", req_start_time,req_bytes, req_seq, req_ack);
						fprintf(stderr, " REQ END with FIN %f finseq %lld \n", fin_time, fin_seq);
#endif
						state=state|CONNSTATE_FIN;
#ifdef TOLOG
						if(req_printed==1)
						fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, client, server);
#endif
						req_printed=0 ;






					} 
					else {
#ifdef TOLOG
						/* NEEDRETRANSMIT */

LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d seqend %lld\n", tm , client, server,client0,rev, thisseqinfo.seqend);  */
#endif
					}
					


				} else if((state==CONNSTATE_RSP || state==CONNSTATE_RSPACK)&& client0==rev) {
					/* more of rsp along with FIN */
#ifdef PRINTFS
					fprintf(stderr, " Response along with FIN\n");
#endif

					if(thisseqinfo.seqstart>=rsp_seq&&newack==rsp_ack) {
						rsp_ack=newack;
						rsp_seq=thisseqinfo.seqend;
						rsp_bytes+=thisseqinfo.pkt_data;
						rsp_end_time=tm;
						fin_time=tm;
						fin_seq=thisseqinfo.seqend;
						if(rev==0)finclient0=1;
						else finclient0=0;
						//rsp_ack_ack=-1;
						//rsp_start_time=tm;
						//state=CONNSTATE_RSP;
#ifdef PRINTFS
						fprintf(stderr, " RSP END with FIN %f sent %lld bytes seq %lld ack %d\n", rsp_start_time,rsp_bytes, rsp_seq, rsp_ack);
						fprintf(stderr, " RSP END with FIN %f finseq %lld \n", fin_time, fin_seq);
#endif
						state=state|CONNSTATE_FIN;
#ifdef TOLOG
						if(rspabovethreshold!=1 || (ADJUSTTHRESHOLDFRACTION*rsp_bytes>adjusts_rsp))
						fprintf(stderr, "TOLOG: RSPOVER %f sent %lld bytes client %s server %s\n", rsp_end_time, rsp_bytes, client, server);
#endif
						rsp_printed=0;


					}
					else {
#ifdef TOLOG
						/* NEEDRETRANSMIT */

LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d seqend %lld\n", tm , client, server,client0,rev, thisseqinfo.seqend);  */
#endif
					}





				} else if((state&CONNSTATE_FIN)==CONNSTATE_FIN && finclient0==rev && thisseqinfo.pkt_data==0) {
					/* FINACK */
#ifdef PRINTFS
					fprintf(stderr," fin ack req_seq %lld\n",req_seq);
#endif
					newack=getacknum(d[10]);
					if(client0!=rev){
						if(thisseqinfo.seqend>=req_seq && newack>=fin_seq) {
							req_seq=thisseqinfo.seqend;
							finack_time=tm;
							req_ack=newack;
#ifdef PRINTFS
							fprintf(stderr, "FINACK time %f seq %lld ack %lld\n", finack_time, req_seq,req_ack);
#endif

							state=CONNSTATE_FINACK;
						}
						else {
#ifdef TOLOG
							/* NEEDRETRANSMIT */

LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  							fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d seqend %lld\n", tm , client, server,client0,rev, thisseqinfo.seqend);  */
#endif
						}
					}
					else {
#ifdef PRINTFS
						fprintf(stderr, " trialll\n");
#endif
						if(thisseqinfo.seqend==rsp_seq && newack>=rsp_ack) {
							rsp_ack=newack;
							finack_time=tm;
							state=CONNSTATE_FINACK;
						}
						else {
#ifdef TOLOG
							LOGRETRANSMIT(tm, client, server, client0, rev, thisseqinfo.seqend);
							/*
							fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d seqend %lld\n", tm , client, server,client0,rev, thisseqinfo.seqend);
							*/
#endif
						}
					}
				} else if(state==CONNSTATE_RSP && client0!=rev)	{
					/* client want to close its end of the connection */
#ifdef PRINTFS
					fprintf(stderr,"client closed conn \n");
#endif
					newack=getacknum(d[10]);
					getseqnum(d[8]);
					if(thisseqinfo.pkt_data==0 && newack>=rsp_ack_ack) {
						fin_seq=thisseqinfo.seqend;
						fin_time=tm;
						fin_ack=newack;
#ifdef PRINTFS
						fprintf(stderr, "client closed conn \n");
#endif
#ifdef TOLOG
						fprintf(stderr, "TOLOG: FIN %f \n", fin_time);
#endif
						if(rev==0) finclient0=1;
						else finclient0=0;
						state=CONNSTATE_FIN;
					}
					else {

#ifdef TOLOG
						/* NEEDRETRANSMIT */
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
					}
				} else if(state==CONNSTATE_RSPACK && client0!=rev){
					/* client is closing connection */
					/* perhaps it thinks it has got enuf data */
					newack=getacknum(d[10]);
					getseqnum(d[8]);
					if(rev==0) finclient0=1;
					else finclient0=0;
					state=CONNSTATE_FIN;
					fin_time=tm;
					fin_ack=newack;
					fin_seq=thisseqinfo.seqend;
					rsp_end_time=tm;
#ifdef PRINTFS
					fprintf(stderr, " RSP OVER probably, client sent FIN finclient0 %d\n", finclient0);
#endif
#ifdef PRINTFS
					fprintf(stderr,"RSP OVER probably %lld bytes \n", rsp_bytes);
#endif
#ifdef TOLOG
					if(rspabovethreshold!=1 || (ADJUSTTHRESHOLDFRACTION*rsp_bytes>adjusts_rsp))
						fprintf(stderr, "TOLOG: RSPOVER %f sent %lld bytes client %s server %s \n",  rsp_end_time, rsp_bytes, client, server);
						rsp_printed=0;
#endif

#if 0
						req_bytes=thisseqinfo.pkt_data;
						req_seq=thisseqinfo.seqend;
						req_start_time=tm;
						req_end_time=tm;
						req_ack = newack;
						req_ack_ack=-1;
#ifdef PRINTFS
						fprintf(stderr, " RSP OVERm new REQ %f %lld bytes\n", req_start_time, req_bytes);
#endif
#ifdef TOLOG
						fprintf(stderr, "TOLOG: RSPOVER %f sent %lld bytes client %s server %s \n",  rsp_end_time, rsp_bytes, client, server);
						rsp_printed=0;
#endif

#endif

					

				} else  if((state==CONNSTATE_REQ||state==CONNSTATE_REQACK) && client0==rev){
					/* server is closing conn*/
					getseqnum(d[8]);
					newack=getacknum(d[10]);
					if(rev==0) finclient0=1;
					else finclient0=0;
					state=CONNSTATE_FIN;
					fin_time=tm;
					fin_ack=newack;
					fin_seq=thisseqinfo.seqend;
#ifdef PRINTFS
					fprintf(stderr, " REQ OVER, client sent FIN finclient0 %d\n", finclient0);
#endif


				} else if(finclient0!=rev) {
					/* maybe repeat of FIN */
					getseqnum(d[8]);
					newack=getacknum(d[10]);
					if(thisseqinfo.seqend<=fin_seq) {
#ifdef PRINTFS
						fprintf(stderr, " resend FIN so ignore \n");
#endif
						if(thisseqinfo.seqstart<fin_seq){
#ifdef TOLOG
						/* NEEDRETRANSMIT */

LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  							fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
						}
#endif
						goto getnextline;
					}
				} else if(finclient0==rev && ( (state & CONNSTATE_FINACK)==CONNSTATE_FINACK ) ) {
					/* matybe repeat of FINACK */
					getseqnum(d[8]);
					newack=getacknum(d[10]);
					if(thisseqinfo.seqend<=finack_seq) {
#ifdef PRINTFS
						fprintf(stderr, " resend FINACL so ignore \n");
#endif
						goto getnextline;
					}
				} else if(finclient0==rev && state==CONNSTATE_FIN){
					/* whoever sent the FIN the other is pushing some data aling with its FIN */
#ifdef PRINTFS
					fprintf(stderr, " other side sent FIN this side sending FINACK wuth data \n");
#endif
					state=state|CONNSTATE_FINACK;
				}else {
#ifdef PRINTFS
					fprintf(stderr, "SHOULDNOTBEHERE 5 state %d\n", state);
#endif
#ifdef TOLOG
					fprintf(stderr, "TOLOG: SHOULDNOTBEHERE 5 state %d\n", state);
#endif

					state=CONNSTATE_FIN;
				}

			} else {
				/* fin but has  no data */
#ifdef PRINTFS
				fprintf(stderr, " no data \n");
#endif
#ifdef TOLOG
				fprintf(stderr, "TOLOG: SHOULDNOTBEHERE 2 state %d\n", state);
#endif
			}
		} /* if FIN is set */

		/* STATETEST 2  TESTING FOR RST */
		else if((this_flag & TCPFLG_RST)==TCPFLG_RST)  {
#ifdef PRINTFS
			//fprintf(stderr, "SHOULDNOTBEHERE\n");
			fprintf(stderr, "RST \n");

#endif
			if((state|CONNSTATE_FIN)==CONNSTATE_FIN  && rev!=finclient0)
			{
#ifdef PRINTFS
				fprintf(stderr, " cliuent had sent fin earlier and rst now so ignore this rst \n");
#endif
			}
			else
			state=CONNSTATE_BAD;
#ifdef TOLOG
			fprintf(stderr, "TOLOG: RST \n");
#endif
		}

		/* STATETEST 3  TESTING FOR ACK */
		else if((this_flag & TCPFLG_ACK)==TCPFLG_ACK) {
			/* has ack */
			if(strcmp(d[8],"ack")==0) {

					newack = getacknum(d[9]);
				/* it is .ack */
				if(state==CONNSTATE_SYNACK) {
					/* this is probably ack */
					acksyn_ack=getacknum(d[9]);
#ifdef PRINTFS
					fprintf(stderr, " synack_seq %lld acksyn_ack %lld\n", synack_seq, acksyn_ack);
#endif
					if(!(acksyn_ack!=(synack_seq -1) || acksyn_ack!=(synack_seq+1) || acksyn_ack!=synack_seq) && (acksyn_ack > MAGIC_NUMBER)) {
#ifdef PRINTFS
						fprintf(stderr, " weird ack does not ack syn+ack %s", this_line);
#endif
						state=CONNSTATE_BAD;
#ifdef PRINTFS
						fprintf(stderr, " WEIRD 1 \n");
#endif
#ifdef TOLOG
						fprintf(stderr, "TOLOG: ABANDON conn reason 1 syn %d state %d client %s server %s \n",syn_time,state, client , server);
#endif
					}

					state=CONNSTATE_ACKSYN;
					acksyn_time=tm;
					req_ack_ack=-1;
					req_ack=-1;
					req_seq=-1;
					rsp_ack_ack=-1;
					rsp_ack=-1;
					rsp_seq=-1;
#ifdef PRINTFS
					fprintf(stderr, "ACK SYN\n");
#endif
#ifdef TOLOG
					fprintf(stderr, "TOLOG: ACKSYN %f client %s server %s\n", acksyn_time, client, server);
					if( (acksyn_ack==(synack_seq+1)) || (acksyn_ack==synack_seq)|| (acksyn_ack==1))
						if(thispkts<5) 
					fprintf(stderr, "TOLOG: TIME SYNACK_ACKSYN %f client %s server %s\n", acksyn_time - synack_time, client , server);
#endif
				} /* state=CONNSTATE_SYNACK */  
				else  if(state==CONNSTATE_REQ && client0==rev) {
					/* ack to requests */
#ifdef PRINTFS
					fprintf(stderr, " ack requests \n");
#endif
					newack=getacknum(d[9]);
#ifdef PRINTFS
					fprintf(stderr, " newack %lld , req_ack_ack %lld\n", newack, req_ack_ack);

#endif
					if(newack- req_ack_ack> MAGIC_NUMBER) {
						/* perhaps late acksyn so ignore */
#ifdef TOLOG
						fprintf(stderr, "TOLOG: late ACKYN so ignore \n");

#endif
#ifdef TOLOG
						/* NEEDRETRANSMIT */

LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
						goto getnextline;
					}
					req_ack_ack=max(req_ack_ack, newack);


#ifdef PRINTFS
					fprintf(stderr, " REQ ACK %lld \n", req_ack_ack);
#endif
#ifdef TOLOG
					if((req_ack_ack==(req_seq +1 )) || (req_ack_ack==req_seq)) {
						fprintf(stderr, "TOLOG: TIME REQ_REQACK %f client %s server %s SIZE %lld\n", tm - req_end_time, client, server,latest_req_size);
					}
#endif

					req_ack_ack_time=tm;
					state=CONNSTATE_REQACK;
				}/* state=CONNSTATE_REQ && client0==rev*/ 
				else if(state==CONNSTATE_REQACK && client0==rev) {
					/* more acks to request */
#ifdef PRINTFS
					fprintf(stderr, " more ack to request \n");
#endif
					newack=getacknum(d[9]);
					req_ack_ack=max(req_ack_ack,newack);
#ifdef PRINTFS
					fprintf(stderr," REQ ACK CON %lld\n", req_ack_ack);
#endif
#ifdef TOLOG
					if((req_ack_ack==(req_seq +1 )) || (req_ack_ack==req_seq)) {
						fprintf(stderr, "TOLOG: TIME REQ_REQACK %f client %s server %s SIZE %lld\n", tm - req_end_time, client, server,latest_req_size);
					}
#endif

				}/* state==CONNSTATE_REQACK && client0==rev*/
				else if(state==CONNSTATE_RSP && client0!=rev) {
					/* ack to responses */
#ifdef PRINTFS
					fprintf(stderr, " ack responses \n");
#endif
					newack=getacknum(d[9]);
					rsp_ack_ack=max(rsp_ack_ack, newack);

#ifdef PRINTFS
					fprintf(stderr, " RSP ACK %d \n", rsp_ack_ack);
#endif
#ifdef TOLOG
					if((rsp_ack_ack==(rsp_seq +1)) || (rsp_ack_ack==rsp_seq)) 
					fprintf(stderr, "TOLOG: TIME RSP_RSPACK %f client %s server %s SIZE %lld\n", tm - rsp_end_time, client, server,latest_rsp_size);
#endif
					rsp_ack_ack_time=tm;

					state=CONNSTATE_RSPACK;


				} /* state==CONNSTATE_RSP && client0!=rev */
				else if((state & CONNSTATE_RSP)==CONNSTATE_RSP && client0!=rev) {
					/* ack response but other flag also set.. perhaps fin of server */
#ifdef PRINTFS
					fprintf(stderr, " ack repsonses with other flags too \n");
#endif
					newack=getacknum(d[9]);
					rsp_ack_ack=max(rsp_ack_ack, newack);
					state=(state^CONNSTATE_RSP)| CONNSTATE_RSPACK;
#ifdef PRINTFS
					fprintf(stderr, " RSP ACK with other flags ack %lld\n", rsp_ack_ack);
#endif
#ifdef TOLOG
	//				if((rsp_ack_ack==(rsp_seq +1)) || (rsp_ack_ack==rsp_seq)) 
	//				fprintf(stderr, "TOLOG: TIME RSP_RSPACK %f client %s server %s SIZE %lld\n", tm - rsp_end_time, client, server,latest_rsp_size);
#endif
				} else if(state==CONNSTATE_RSPACK && client0!=rev) {
					/* more rsp ack */
#ifdef PRINTFS
					fprintf(stderr, " more rsp acks \n");
#endif
					newack=getacknum(d[9]);
					if(newack==rsp_ack_ack) {
#ifdef PRINTFS
						fprintf(stderr, "repeatack\n");
						goto getnextline;
#endif
					}
					rsp_ack_ack=max(rsp_ack_ack, newack);
#ifdef PRINTFS
					fprintf(stderr, " RSP ACK continue ack %lld\n", rsp_ack_ack);
					fprintf(stderr, " rsp_seq was %lld\n", rsp_seq);
#endif

#ifdef TOLOG
					if((rsp_ack_ack==(rsp_seq +1)) || (rsp_ack_ack==rsp_seq))
					fprintf(stderr, "TOLOG: TIME RSP_RSPACK %f client %s server %s SIZE %lld\n", tm - rsp_end_time, client, server,latest_rsp_size);
#endif
					if(rsp_ack_ack>rsp_seq &&(rsp_ack_ack - rsp_bytes)<MAGIC_JUMP) {
						rsp_bytes+=(rsp_ack_ack - rsp_seq);
						
#ifdef PRINTFS
						fprintf(stderr, "adjust rsp for possibly onewaytrace add %d bytes \n", (int)(rsp_ack_ack - rsp_seq));
#endif
						adjusts+=(int)(rsp_ack_ack - rsp_seq);
						adjusts_rsp +=(int)(rsp_ack_ack - rsp_seq);
						adjusts_rsp_seq_min=min(adjusts_rsp_seq_min,rsp_ack_ack);
						adjusts_rsp_seq_max=max(adjusts_rsp_seq_max,rsp_ack_ack);
						rsp_seq=rsp_ack_ack;
#ifdef PRINTFS
							fprintf(stderr, " adjusts_rsp %d\n",adjusts_rsp);
#endif
						if(adjusts_rsp>ADJUST_THRESHOLD) {
#ifdef TOLOG
							fprintf(stderr, " cannot adjust, above threshold \n");
#endif
							rsp_printed=1;
							rspabovethreshold=1;
						}
						/* NEWCHANGE Mon May 23 20:43:01 PDT 2005 */
					}
				} else if((state&CONNSTATE_RSPACK)==CONNSTATE_RSPACK && client0!=rev) {
					/* rsp ack cont with other flags */
#ifdef PRINTFS
					fprintf(stderr, " rsp ack continue with other flags perhaps fin \n");
#endif
					newack=getacknum(d[9]);
					rsp_ack_ack=max(rsp_ack_ack, newack);
#ifdef PRINTFS
					fprintf(stderr, " RSP ACK continues with other flags ack %lld\n", rsp_ack_ack);
#endif


#ifdef TOLOG
	//				if((rsp_ack_ack==(rsp_seq +1)) || (rsp_ack_ack==rsp_seq))
	//				fprintf(stderr, "TOLOG: TIME RSP_RSPACK %f client %s server %s SIZE %lld\n", tm - rsp_end_time, client, server,latest_rsp_size);
#endif
				} else if(state==CONNSTATE_FINACK && finclient0!=rev) {
					/* ackfin */
#ifdef PRINTFS
					fprintf(stderr, " ack fin \n");
#endif
					newack=getacknum(d[9]);
					if(newack>finack_seq) {
						ackfin_time=tm;
						ackfin_ack=newack;
#ifdef PRINTFS
						fprintf(stderr, "ACKFIN time %f ack %lld\n",ackfin_time, ackfin_ack);
#endif
					}
					else {
#ifdef TOLOG
						/* NEEDRETRANSMIT */

LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
					}
				}else if(state==CONNSTATE_RSPACK && client0==rev) {
					/* rsp over , new req start */
#ifdef PRINTFS
					fprintf(stderr, " REQ ACK \n");
#endif
				//	newack=getacknum(d[10]);
				//	getseqnum(d[8]);
					newack=getacknum(d[9]);
					
					if(newack>=req_ack_ack || req_ack_ack==-1)
					{
						req_ack_ack=newack;
					req_ack_ack_time=tm;
						state=CONNSTATE_REQACK;
						
#ifdef PRINTFS
						fprintf(stderr, " REQ ACK time \n");
#endif
#ifdef TOLOG
						fprintf(stderr, "TOLOG: REQACK %lld\n",req_ack_ack) ;
#endif
					}
					else {
#ifdef TOLOG
						/* NEEDRETRANSMIT */
LOGRETRANSMITUPDATEVARIABLE(req_ack_ack_time,tm);

LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
					}
				} else if(state==CONNSTATE_RSP && client0==rev) {
					/* req ack  */
					newack=getacknum(d[9]);
#ifdef PRINTFS
					fprintf(stderr, " Req_ack_Ack %lld rspack %lld newack %lld\n", req_ack_ack, rsp_ack, newack);
#endif
					if(newack>req_ack_ack && newack > rsp_ack){
						req_ack_ack=newack;
					//	state=CONNSTATE_REQACK;
#ifdef TOLOG
						fprintf(stderr,"TOLOG: REQACK %lld\n", req_ack_ack);
#endif
					}
				} else if(state==CONNSTATE_BAD && newconn==1) {
					/* perhaps this is acksyn */






					/* probably it is a new conn but we did not see the SYN set */
					/* if the ack number is very high and then pretend that 
					   syn and syn+ack have gone and that this is acksyn */
#ifdef PRINTFS
					fprintf(stderr, " perhaps this is acksyn \n");
#endif
					getseqnum(d[8]);
					newack=getacknum(d[9]);
					if(newack>MAGIC_NUMBER ||thisseqinfo.seqstart>MAGIC_NUMBER )
					{
#ifdef PRINTFS
						fprintf(stderr, " good chance this is acksyn \n");
#endif
						/* fake that you have already seen SYn and syn+Ack */

						syn_time=tm;
						synack_time=tm;
						syn_seq=-1;
						synack_seq=newack-1;
						synack_ack=-1;


						if(rev==1) {
							client0=0; /* since we are pretending that this is ackSYN*/
							strcpy(server,src);
							strcpy(client,dst);
						} else { 
							client0=1;
							strcpy(server,dst);
							strcpy(client,src);
						}
#ifdef TOLOG
						fprintf(stderr,"TOLOG: did not see SYN and SYN+ACK set but setting it and trying\n");
						fprintf(stderr, "TOLOG: SYN %f client %s server %s\n", syn_time, client, server);
						fprintf(stderr, "TOLOG: SYNACK %f client %s server %s\n", synack_time, client, server);
#endif
						//this_flag=this_flag|TCPFLG_SYN;
						state=CONNSTATE_SYNACK;
						goto testallstates;
					} else {
						/* treat as ack */
#ifdef PRINTFS
						fprintf(stderr, " perhaps a connection tapped in between \n");
#endif
						if(!strcmp(d[8],"ack")) {
#ifdef PRINTFS
							fprintf(stderr, " old ack \n");
#endif
							newack=getacknum(d[9]);
							if(newack<RSPNUM) {
#ifdef PRINTFS
								fprintf(stderr, " old REQ ack %lld state %d\n" , newack, state);
#endif
								if(state==CONNSTATE_BAD) {
									state=CONNSTATE_ACKSYN;


									syn_time=tm;
									synack_time=tm;
									syn_seq=-1;
									synack_seq=newack-1;
									synack_ack=-1;
									
									if(rev==1) {
										client0=0; /* since we are pretending that this is ackSYN*/
										strcpy(server,src);
										strcpy(client,dst);
									} else { 
										client0=1;
										strcpy(server,dst);
										strcpy(client,src);
									}

#ifdef TOLOG
									fprintf(stderr,"TOLOG: did not see SYN and SYN+ACK set but setting it and trying\n");
									fprintf(stderr, "TOLOG: SYN %f client %s server %s\n", syn_time, client, server);
									fprintf(stderr, "TOLOG: SYNACK %f client %s server %s\n", synack_time, client, server);
#endif

								}
							}
							else {
#ifdef PRINTFS
								fprintf(stderr, " old RSP ack  %lld  state %d \n", newack, state);
#endif


								if(rev==1) {
									client0=0; /* since we are pretending that this is  RSP ack*/
									strcpy(server,src);
									strcpy(client,dst);
								} else { 
									client0=1;
									strcpy(server,dst);
									strcpy(client,src);
								}





							rsp_ack_ack_time=tm;	
								state=CONNSTATE_RSPACK;
							}
						}

					}

				} else if(state==CONNSTATE_SYN && client0==rev) {
					/* server is sending ack after a SYN */
#ifdef PRINTFS
					fprintf(stderr, " server sent ack after syn, ignore and wait for synack\n ");
#endif
				} else if(state==CONNSTATE_REQACK && client0!=rev) {
#ifdef PRINTFS
					fprintf(stderr, "old acks to take care of \n");
#endif
					newack = getacknum(d[9]);
					if(newack>req_ack && newack<MAGIC_NUMBER){
						req_ack=newack;
#ifdef PRINTFS
					fprintf(stderr, " new ack %lld\n", newack);
#endif
					}
					
					
					req_ack=newack;
				} else if(client0!=rev && state==CONNSTATE_SYN) {
#ifdef PRINTFS
					fprintf(stderr, " perhaps one way trace so seeing acksyn directly \n");
#endif


					state=CONNSTATE_ACKSYN;
					acksyn_time=tm;
					synack_time=tm;
					req_ack_ack=-1;
					req_ack=-1;
					req_seq=-1;
					rsp_ack_ack=-1;
					rsp_ack=-1;
					rsp_seq=-1;
#ifdef PRINTFS
					fprintf(stderr, "ACK SYN\n");
#endif
#ifdef TOLOG
					fprintf(stderr, "TOLOG: ACKSYN %f client %s server %s\n", acksyn_time, client, server);
						if(thispkts<5) 
					fprintf(stderr, "TOLOG: TIME SYNACK_ACKSYN %f client %s server %s\n", acksyn_time - synack_time, client , server);
#endif





				//}else if(client0!=rev && (acksyn_ack - newack)>MAGIC_NUMBER && newack < MAGIC_NUMBER  && state==CONNSTATE_ACKSYN ) {
				}else if(client0!=rev && (acksyn_ack - newack)>MAGIC_NUMBER && newack < MAGIC_NUMBER  && state==CONNSTATE_ACKSYN ) {
#ifdef PRINTFS 
					fprintf(stderr, " perhaps one sided so am seeing ack directly \n");
#endif
					if(newack>1) {
					req_start_time=tm;
					req_bytes=0;
					req_end_time=tm;
					rsp_start_time=tm;
					rsp_end_time=tm;
					rsp_bytes=newack;
#ifdef TOLOG
	
						
						fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, (int)req_bytes, req_bytes, client, server);
						fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, client, server);
						fprintf(stderr, "TOLOG: RSPSTART %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
#endif
						req_printed=0;
						rsp_printed=1;
						latest_rsp_size=rsp_bytes;
							rsp_ack_ack_time=tm;	
						state=CONNSTATE_RSPACK;
					}

				}
				else if(client0!=rev) {
#ifdef PRINTFS
					fprintf(stderr," some old ack from the client \n");
#endif

				}
				else if(client0==rev){
#ifdef PRINTFS
					fprintf(stderr, " ack from server \n");
#endif
					newack=getacknum(d[9]);
					if(newack>req_ack_ack && newack < MAGIC_NUMBER) {
						req_ack_ack=newack;
#ifdef PRINTFS
						fprintf(stderr, " new req ack %lld\n", req_ack_ack);
#endif
					}
				}
				else {
#ifdef PRINTFS
					fprintf(stderr, "SHOULDNOTBEHERE 3 \n");
#endif
#ifdef TOLOG
					fprintf(stderr, "TOLOG: SHOULDNOTBEHERE 3 state %d\n", state);
#endif
					state=CONNSTATE_BAD;
				}


				/* only ack */
			} else {
				/* data as well as ack */
#ifdef PRINTFS
				fprintf(stderr, "has seq as well as ack state %d client0 %d  rev %d\n", state, client0, rev);
#endif
				/* first check if this is possibly the start of a new conn using the pktthreshold heuristic */
			
					getseqnum(d[8]);
					newack=getacknum(d[10]);
				if((thisseqinfo.seqstart>MAGIC_NUMBER || newack>MAGIC_NUMBER) && thispkts<PKTDISCARDTHRESHOLD) {
#ifdef PRINTFS
					fprintf(stderr, " possibly a the start of a conn \n");
#endif
					if((thisseqinfo.seqstart>MAGIC_NUMBER) && (newack>MAGIC_NUMBER) ) {
#ifdef PRINTFS
						fprintf(stderr, " possibly this is syn+ack or acksyn with data state=%d\n", state);
						
#endif
						if(thisseqinfo.pkt_data>0 && state!=CONNSTATE_BAD) {
#ifdef PRINTFS
							fprintf(stderr, " possibly acksyn with req packets \n");
#endif

							syn_time=tm;
							synack_time=tm;
							acksyn_time=tm;
							acksyn_ack=newack;
							req_bytes=thisseqinfo.pkt_data;
							req_start_time=tm;
							req_end_time=tm;
							if(thisseqinfo.seqend<MAGIC_NUMBER)
							req_seq=thisseqinfo.seqend;
							else 
								req_seq=thisseqinfo.pkt_data -1;
							if(newack<MAGIC_NUMBER)
							req_ack=newack;
							else 
								newack=0;
							if(rev==0) {
								client0=1;
								strcpy(client,src);
								strcpy(server,dst);
							} else {
								client0=0;
								strcpy(client,dst);
								strcpy(server,src);
							}
							
							
							latest_req_size=req_bytes;
#ifdef TOLOG

						fprintf(stderr, "TOLOG: ACKSYN %f client %s server %s\n", acksyn_time, client, server);
						if(thispkts<5) 
						fprintf(stderr, "TOLOG: TIME SYNACK_ACKSYN %f client %s server %s\n", acksyn_time - synack_time, client , server);
						fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);

							
#endif
							state=CONNSTATE_ACKSYN;
							state=CONNSTATE_REQ;

						req_printed=1 ;
						latest_req_size=req_bytes;
						rsp_printed=0;
							
						}

						else{
							req_seq=-1; rsp_seq=-1;rsp_ack=-1;req_ack=-1;rsp_ack_ack=-1;req_ack_ack=-1;
							syn_time=tm;
							synack_time=tm;
							syn_seq=thisseqinfo.seqstart-1;
							synack_seq=newack-1;
							synack_ack=thisseqinfo.seqstart;

							syn_seq=newack-1;
							synack_seq=thisseqinfo.seqend;
							synack_ack=newack;



							if(rev==1) {
								client0=1; /* since we are pretending that this is SYNACK*/
								strcpy(server,dst);
								strcpy(client,src);
							} else { 
								client0=0;
								strcpy(server,src);
								strcpy(client,dst);
							}
#ifdef TOLOG
							fprintf(stderr, "TOLOG: SYN %f client %s server %s\n", syn_time, client, server);
							fprintf(stderr, "TOLOG: SYNACK %f client %s server %s\n", synack_time, client, server);
							//fprintf(stderr, "TOLOG: TIME: SYN_SYNACK %f client %s server %s\n", synack_time - syn_time, client, server);
#endif
							//this_flag=this_flag|TCPFLG_SYN;
							state=CONNSTATE_SYNACK;
							adjusts_rsp=0;adjusts_req=0; adjusts=0; adjusts_rsp_seq_max=ADJUSTRSPSEQMAX; adjusts_rsp_seq_min=ADJUSTRSPSEQMIN;
						}
						goto getnextline;
	//					goto testallstates;

					}
				}
				fprintf(stderr,"the state is %d\n", state);
	//			if(client0==rev) 
	//				fprintf(stderr,"client0 is rev \n");
	//			if(state==CONNSTATE_RSP) 
	//				fprintf(stderr,"state is CONNSTATE_RSP \n");
				if(state==CONNSTATE_ACKSYN) {
					getseqnum(d[8]);
					state=CONNSTATE_REQ;
					newack=getacknum(d[10]);
					if(rev==client0) {
						/* "server" is sending first set of data so call it response */

					state=CONNSTATE_RSP;
						rsp_ack=getacknum(d[10]);
						rsp_start_time=tm;
						req_start_time=tm;
						req_end_time=tm;
						req_seq=newack-1;
						req_ack=-1;
						req_ack_ack=-1;
						req_bytes=0;
						rsp_ack_ack=-1;
						rsp_end_time=tm;
						rsp_seq=thisseqinfo.seqend;
						rsp_bytes=thisseqinfo.pkt_data;
						if((thisseqinfo.seqstart!=0 || thisseqinfo.seqend!= thisseqinfo.pkt_data)&&thisseqinfo.seqend<MAGIC_NUMBER)   {
							rsp_bytes=thisseqinfo.seqend;
						}
						//	if(rev==0) client0=1;
						//	else client0=0;
						if(rsp_seq>MAGIC_NUMBER) rsp_seq=-1;
						if(rsp_ack>MAGIC_NUMBER) rsp_ack=-1;
						/*
						   fprintf(stderr, " ack %lld\n", req_ack);
						   fprintf(stderr, "reqseq %lld bytes %lld ack %lld\n", req_seq, req_bytes, req_ack)  ;
						   fprintf(stderr, " ack %lld\n", req_ack);
						 */
#ifdef PRINTFS
#ifndef WANT_PRINTF_LONG_LONG
						fprintf(stderr, " RSP START %f sent %lld bytes seq %lld ack %lld \n", rsp_start_time, rsp_bytes, rsp_seq, rsp_ack);
#else
						fprintf(stderr, " RSP START %f sent %lld bytes seq %lld ack %lld \n", rsp_start_time, rsp_bytes, rsp_seq, rsp_ack);
#endif
#endif
#ifdef TOLOG
						//fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %d bytes\n", req_start_time, thisseqinfo.pkt_data, req_bytes);
						fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, (int)req_bytes, req_bytes, client, server);
						//fprintf(stderr, "TOLOG: REQSTART %f sent %lld bytes total %lld bytes\n", req_start_time, req_bytes, req_bytes);
	//					fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes total %lld bytes\n", req_start_time, req_bytes, req_bytes);
						fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, client, server);
						//fprintf(stderr, "TOLOG: RSPSTART %f sent %lld bytes total %lld bytes\n", rsp_start_time, rsp_bytes, rsp_bytes);
						fprintf(stderr, "TOLOG: RSPSTART %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
#endif
						req_printed=0 ;
						rsp_printed=1;
						latest_rsp_size=rsp_bytes;











						
						
					}
					else {
						req_ack=getacknum(d[10]);
						req_start_time=tm;
						req_ack_ack=-1;
						req_end_time=tm;
						req_seq=thisseqinfo.seqend;
						req_bytes=thisseqinfo.pkt_data;
						if((thisseqinfo.seqstart!=0 || thisseqinfo.seqend!= thisseqinfo.pkt_data)&&thisseqinfo.seqend<MAGIC_NUMBER &&( (thisseqinfo.seqend - req_bytes)<MAGIC_JUMP))   {
							req_bytes=thisseqinfo.seqend;
						}
						//	if(rev==0) client0=1;
						//	else client0=0;
						if(req_seq>MAGIC_NUMBER) req_seq=-1;
						if(req_ack>MAGIC_NUMBER) req_ack=-1;
						/*
						   fprintf(stderr, " ack %lld\n", req_ack);
						   fprintf(stderr, "reqseq %lld bytes %lld ack %lld\n", req_seq, req_bytes, req_ack)  ;
						   fprintf(stderr, " ack %lld\n", req_ack);
						 */
#ifdef PRINTFS
#ifndef WANT_PRINTF_LONG_LONG
						fprintf(stderr, " REQ START %f sent %lld bytes seq %lld ack %lld \n", req_start_time, req_bytes, req_seq, req_ack);
#else
						fprintf(stderr, " REQ START %f sent %lld bytes seq %lld ack %lld \n", req_start_time, req_bytes, req_seq, req_ack);
#endif
#endif
#ifdef TOLOG
						//fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %d bytes\n", req_start_time, thisseqinfo.pkt_data, req_bytes);
						fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);
						//fprintf(stderr, "TOLOG: REQSTART %f sent %lld bytes total %lld bytes\n", req_start_time, req_bytes, req_bytes);
#endif
						req_printed=1;
						latest_req_size=req_bytes;
					}
				} else if(state==CONNSTATE_REQ && client0!=rev) {
					/* more belonging to request */
#ifdef PRINTFS
					fprintf(stderr, " more belonging tpo the request \n");
#endif
					getseqnum(d[8]);
					newack=getacknum(d[10]);
#ifdef PRINTFS
					fprintf(stderr, " req_ack %lld , newack %lld req_seq %lld\n", req_ack, newack, req_seq);
#endif



					if((thisseqinfo.seqend>req_seq && thisseqinfo.seqstart>=req_seq&&newack>=req_ack) || ( req_seq > MAGIC_NUMBER)) {
#ifdef PRINTFS
						fprintf(stderr, " just another marker for debug aa\n");
#endif
						if(newack<MAGIC_NUMBER) req_ack=newack;
						req_bytes+=thisseqinfo.pkt_data;
						if(thisseqinfo.seqstart>req_seq && req_seq!=-1 && (thisseqinfo.seqstart - req_bytes)<MAGIC_NUMBER &&( thisseqinfo.seqstart - req_seq)<ADJUST_THRESHOLD) 
							req_bytes+=thisseqinfo.seqstart - req_seq;
						req_seq=thisseqinfo.seqend;
						//rsp_ack_ack=-1;
						req_end_time=tm;
						//rsp_start_time=tm;
						//state=CONNSTATE_RSP;
#ifdef PRINTFS
						fprintf(stderr, " REQ CONTINUE %f sent %lld bytes seq %lld ack %d\n", req_start_time,req_bytes, req_seq, req_ack);
#endif
#ifdef TOLOG
						fprintf(stderr, "TOLOG: REQCONTINUE %f sent %d bytes total %lld\n", tm, thisseqinfo.pkt_data, req_bytes);
#endif

						req_printed=1;
						rsp_printed=0;
						req_ack=max(newack, req_ack);
					}
					else if((thisseqinfo.seqend>req_seq && newack>=req_ack) || ( req_seq > MAGIC_NUMBER)) {
#ifdef PRINTFS
						fprintf(stderr, " overlapping parts of REQ ... \n");
#endif
						req_end_time=tm;
						if(thisseqinfo.seqstart<5){
							/* consider this as begin of req */
							req_start_time=tm;
							req_seq=thisseqinfo.seqend;
							req_bytes=thisseqinfo.pkt_data;

						fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);

						} else {
							/* add req bytes to earlier version */
							req_bytes+=(thisseqinfo.seqend - req_seq);
							req_seq=thisseqinfo.seqend;

#ifdef PRINTFS
						fprintf(stderr, " REQ CONTINUE %f sent %lld bytes seq %lld ack %d\n", req_start_time,req_bytes, req_seq, req_ack);
#endif
#ifdef TOLOG
						fprintf(stderr, "TOLOG: REQCONTINUE %f sent %d bytes total %lld\n", tm, thisseqinfo.pkt_data, req_bytes);
#endif
						}
						req_ack=max(newack, req_ack);
						latest_req_size=thisseqinfo.pkt_data;
					}

					else if(newack>MAGIC_NUMBER || thisseqinfo.seqend>MAGIC_NUMBER) {
						/* fill this in */

						
					} else {
#ifdef TOLOG
						/* NEEDRETRANSMIT */
LOGRETRANSMITUPDATEVARIABLE(req_end_time,tm);
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
						goto getnextline;
					}







				} else if(state==CONNSTATE_REQACK && client0==rev) {
#ifdef PRINTFS
					fprintf(stderr, " stat= CONNSTATE_REQACK and client0==rev\n");
#endif
					/*response has started */
					if(rsp_printed!=1){
#ifdef PRINTFS
						fprintf(stderr, " response has strarted  \n");
#endif
						getseqnum(d[8]);
						newack=getacknum(d[10]);
#ifdef PRINTFS
						fprintf(stderr, " %lld %lld %lld %lld\n", thisseqinfo.seqstart, rsp_seq, newack, rsp_ack);
#endif
						if((thisseqinfo.seqstart>=rsp_seq  && newack>=rsp_ack) ||( rsp_ack > MAGIC_NUMBER)) {

							if(thisseqinfo.seqstart<MAGIC_JUMP && thisseqinfo.seqstart>1 && (rsp_seq < (thisseqinfo.seqstart-1)) )
								rsp_bytes+=thisseqinfo.seqstart;
							rsp_ack=newack;
							rsp_seq=thisseqinfo.seqend;
							rsp_bytes=thisseqinfo.pkt_data;
							rsp_end_time=tm;
							rsp_ack_ack=-1;
							rsp_start_time=tm;
							state=CONNSTATE_RSP;
#ifdef PRINTFS
							fprintf(stderr, " REQ OVER earlier \n");
#ifndef WANT_PRINTF_LONG_LONG
							fprintf(stderr, " RSP START %f sent %ld bytes seq %ld ack %d\n", rsp_start_time,rsp_bytes, rsp_seq, rsp_ack);
#else
							fprintf(stderr, " RSP START %f sent %lld bytes seq %lld ack %d\n", rsp_start_time,rsp_bytes, rsp_seq, rsp_ack);
#endif
#endif
#ifdef TOLOG
							fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, client, server);
							fprintf(stderr, "TOLOG: RSPSTART %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
							fprintf(stderr, "TOLOG: TIME REQ_RSP %f client %s server %s\n", rsp_start_time - req_end_time, client, server);
#endif
							req_printed=0;
							rsp_printed=1;
						latest_rsp_size=rsp_bytes;
						}
						else {
#ifdef TOLOG
							/* NEEDRETRANSMIT */
LOGRETRANSMITUPDATEVARIABLE(req_end_time,tm);
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  							fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
						}
					}else {
#ifdef PRINTFS
						fprintf(stderr, " response was never over so this is perhaps just a continuation \n");
#endif




						getseqnum(d[8]);
						newack=getacknum(d[10]);
#ifdef PRINTFS
						fprintf(stderr, " %lld %lld %lld %lld\n", thisseqinfo.seqstart, rsp_seq, newack, rsp_ack);
#endif
						if((thisseqinfo.seqstart>=rsp_seq  && newack>=rsp_ack) ||( rsp_ack > MAGIC_NUMBER)) {
							rsp_ack=newack;
							rsp_seq=thisseqinfo.seqend;
							rsp_bytes+=thisseqinfo.pkt_data;
							rsp_end_time=tm;
							rsp_ack_ack=-1;
							//rsp_start_time=tm;
							state=CONNSTATE_RSP;


#ifdef PRINTFS
						fprintf(stderr, " RSP CONTINUE %f sent %lld bytes seq %lld ack %lld\n", rsp_start_time,rsp_bytes, rsp_seq, rsp_ack);
#endif
#ifdef TOLOG
						fprintf(stderr, "TOLOG: RSPCONTINUE %f sent %d bytes total %lld\n", rsp_end_time, thisseqinfo.pkt_data, rsp_bytes);
#endif
							
							rsp_printed=1;
							req_printed=0;
						}
						else {
#ifdef TOLOG
							/* NEEDRETRANSMIT */
LOGRETRANSMITUPDATEVARIABLE(rsp_end_time, tm ); 
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  							fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
						}





						
						
					}

				} else if(state==CONNSTATE_REQ && client0==rev ) {
					/* ack of req as well as sending of response */
#ifdef PRINTFS
					fprintf(stderr, " REQ OVER AS WELL AS RSP START \n");
#endif
					newack=getacknum(d[10]);
					getseqnum(d[8]);
#ifdef PRINTFS
					fprintf(stderr, " newack %lld req_seq %lld thisseqinfo.seqend %lld req_ack_ack %lld\n", newack, req_seq, thisseqinfo.seqend, req_ack_ack);
#endif
					if(newack>=req_seq && thisseqinfo.seqend>=rsp_ack_ack) {
						rsp_bytes=thisseqinfo.pkt_data;
						rsp_seq=thisseqinfo.seqend;
						rsp_ack=newack;
						rsp_ack_ack=-1;
						rsp_start_time=tm;
						rsp_end_time=tm;
						state=CONNSTATE_RSP;
#ifdef PRINTFS
						fprintf(stderr, "REQ OVER RSP START %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
#endif
#ifdef TOLOG
						fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, client, server);
						fprintf(stderr, "TOLOG: RSPSTART %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
						fprintf(stderr, "TOLOG: TIME REQ_RSP %f client %s server %s\n", rsp_start_time - req_end_time, client, server);
#endif
						req_printed=0;
						rsp_printed=1;
						latest_rsp_size=rsp_bytes;
					} else if(thisseqinfo.seqend >= req_ack_ack ) {
						/* perhaps some potion of req not received thats why acks dont match */
						rsp_bytes=thisseqinfo.pkt_data;
						rsp_seq=thisseqinfo.seqend;
						rsp_ack=newack;
						rsp_ack_ack=-1;
						rsp_start_time=tm;
						rsp_end_time=tm;
						state=CONNSTATE_RSP;
#ifdef PRINTFS
						fprintf(stderr, "REQ OVER RSP START %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
#endif
#ifdef TOLOG
						fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, client, server);
						fprintf(stderr, "TOLOG: RSPSTART %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
						fprintf(stderr, "TOLOG: TIME REQ_RSP %f client %s server %s\n", rsp_start_time - req_end_time, client, server);
#endif
						req_printed=0;
						rsp_printed=1;
						latest_rsp_size=rsp_bytes;


					} else if(thisseqinfo.seqstart>=rsp_seq) {
#ifdef PRINTFS
						fprintf(stderr, " some portion of the request has not been acked yet \n");
						fprintf(stderr, "		so hopefully it iwll be acked in the future, lets for now mark this as begginging of resp\n");
#endif


						rsp_bytes+=thisseqinfo.pkt_data;
						rsp_seq=thisseqinfo.seqend;
						rsp_ack=newack;
						rsp_ack_ack=-1;
						rsp_start_time=tm;
						rsp_end_time=tm;
						req_end_time=tm;
						state=CONNSTATE_RSP;

#ifdef TOLOG
						fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, client, server);
						fprintf(stderr, "TOLOG: RSPSTART %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
						fprintf(stderr, "TOLOG: TIME REQ_RSP %f client %s server %s\n", rsp_start_time - req_end_time, client, server);
#endif
						req_printed=0;
						rsp_printed=1;
						latest_rsp_size=rsp_bytes;

						
					}
					else {
#ifdef TOLOG
						/* NEEDRETRANSMIT */
LOGRETRANSMITUPDATEVARIABLE(req_end_time,tm);
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
					}
				} else if(state==CONNSTATE_RSP && client0!=rev) {
					/* rsp over and req start */
#ifdef PRINTFS
					fprintf(stderr, " rsp over req start \n");
#endif
					getseqnum(d[8]);
					newack=getacknum(d[10]);
					if (thisseqinfo.seqend>=req_ack_ack) {
#ifdef PRINTFS
						fprintf(stderr, " new segments of request \n");
#endif
						req_seq=thisseqinfo.seqend;
						req_ack=newack;
						req_ack_ack=-1;
						req_bytes=thisseqinfo.pkt_data;
						req_start_time=tm;
						req_end_time=tm;
#ifdef TOLOG
						if(rspabovethreshold!=1 || (ADJUSTTHRESHOLDFRACTION*rsp_bytes>adjusts_rsp) )
						fprintf(stderr, "TOLOG: RSPOVER %f sent %lld bytes client %s server %s \n",  rsp_end_time, rsp_bytes, client, server);
						//fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %d bytes\n", req_start_time, thisseqinfo.pkt_data, req_bytes);
						fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);
						fprintf(stderr, "TOLOG: TIME RSP_REQ %f client %s server %s\n", req_start_time - rsp_end_time, client, server);
#endif
						rsp_printed=0;
						req_printed=1;
						latest_req_size=req_bytes;
						state=CONNSTATE_REQ;
					}
					else {
#ifdef PRINTFS
						fprintf(stderr  ,  "resend \n");
						fprintf(stderr  ,  "req_ack_ack %lld \n", req_ack_ack);
#endif

#ifdef TOLOG
						/* NEEDRETRANSMIT */
LOGRETRANSMITUPDATEVARIABLE(rsp_end_time,tm);
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
					}
				}  else if((state==CONNSTATE_RSP )&& (client0==rev )) {
					/* response continbuted */
#ifdef PRINTFS
					fprintf(stderr, "possibly rsp continued\n");

#endif
					getseqnum(d[8]);
					newack=getacknum(d[10]);
#ifdef PRINTFS
					fprintf(stderr, " %lld %lld %lld %lld \n", thisseqinfo.seqstart, rsp_seq, newack, rsp_ack);
#endif
					if((newack<=rsp_ack) || ((newack-rsp_ack)>MAGIC_JUMP))
					{
					if( (thisseqinfo.seqstart>=rsp_seq&&newack>=rsp_ack && ( thisseqinfo.seqstart - rsp_seq) < MAGIC_NUMBER)||(rsp_seq>MAGIC_NUMBER || rsp_ack > MAGIC_NUMBER)) {
						rsp_ack=newack;
						if(thisseqinfo.seqstart>adjusts_rsp_seq_max){
						rsp_bytes+=thisseqinfo.pkt_data;
						}else {
							//if(thisseqinfo.pkt_data<rsp_seq)
							adjusts_rsp-=thisseqinfo.pkt_data;
						}
						if(thisseqinfo.seqstart!=rsp_seq && thisseqinfo.seqstart<MAGIC_NUMBER &&(rsp_seq - thisseqinfo.seqstart)<MAGIC_JUMP && (thisseqinfo.seqstart - rsp_seq)<MAGIC_JUMP) 
							rsp_bytes+=thisseqinfo.seqstart - rsp_seq;
						rsp_seq=thisseqinfo.seqend;
						rsp_end_time=tm;
						//rsp_ack_ack=-1;
						//rsp_start_time=tm;
						//state=CONNSTATE_RSP;
#ifdef PRINTFS
						fprintf(stderr, " RSP CONTINUE %f sent %lld bytes seq %lld ack %lld\n", rsp_start_time,rsp_bytes, rsp_seq, rsp_ack);
#endif
#ifdef TOLOG
						fprintf(stderr, "TOLOG: RSPCONTINUE %f sent %d bytes total %lld\n", rsp_end_time, thisseqinfo.pkt_data, rsp_bytes);
#endif
						rsp_printed=1;
							req_printed=0;


					}
					else {
#ifdef PRINTFS
						fprintf(stderr, " adjust question \n");
#endif
						
						if(thisseqinfo.seqend<=adjusts_rsp_seq_max) {
							adjusts_rsp-=(thisseqinfo.pkt_data);
							adjusts_rsp_seq_min=max(adjusts_rsp_seq_min, thisseqinfo.seqend);
							if(adjusts_rsp_seq_min>=adjusts_rsp_seq_max) {
								adjusts_rsp_seq_max=ADJUSTRSPSEQMAX;
								adjusts_rsp_seq_min=ADJUSTRSPSEQMIN;
							}
						}

#ifdef TOLOG
						/* NEEDRETRANSMIT */

LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
					}
					}
					else{
#ifdef PRINTFS
						fprintf(stderr, " looks like we missed an entire request and only the req_ack advancement was seen \n");
#endif
						/* rspover, reqstart, rspover, rspstart */
						req_bytes=newack - rsp_ack;
						req_start_time=tm;
						req_end_time=tm;
						req_ack_ack=-1;
						req_ack=thisseqinfo.seqstart;
						req_seq=newack;
						state=CONNSTATE_REQ;
						rsp_end_time=tm;
#ifdef PRINTFS
						fprintf(stderr,"RSP over REQ start %f %lld bytes client %s server %s\n", req_start_time, req_bytes, client, server);
#endif
#ifdef TOLOG
				//		if(rspabovethreshold!=1 || (ADJUSTTHRESHOLDFRACTION*rsp_bytes>adjusts_rsp))
						fprintf(stderr, "TOLOG: RSPOVER %f sent %lld bytes client %s server %s\n", rsp_end_time, rsp_bytes, client, server);
						//fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %d bytes\n", req_start_time, thisseqinfo.pkt_data, req_bytes);
						fprintf(stderr, "TOLOG: REQSTART %f sent %lld bytes total %lld client %s server %s\n", req_start_time, req_bytes, req_bytes, client, server);
						fprintf(stderr, "TOLOG: TIME RSP_REQ %f client %s server %s\n", req_start_time - rsp_end_time, client, server);
						fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes total %lld client %s server %s\n", req_start_time, req_bytes, req_bytes, client, server);
#endif
						rsp_bytes=thisseqinfo.pkt_data;
						rsp_start_time=tm;
						rsp_end_time=tm;
						rsp_ack=newack;
						rsp_ack_ack=-1;
#ifdef TOLOG
							fprintf(stderr, "TOLOG: RSPSTART %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
#endif

							state=CONNSTATE_RSP;
							rsp_printed=1;
							req_printed=0;
						latest_rsp_size=rsp_bytes;

/*allover*/
					}
				}
				else if(state==CONNSTATE_RSPACK && client0==rev) {
#ifdef PRINTFS
					fprintf(stderr, " rsp continued, already received some ack for other side\n");

#endif
					/* add the new RSPACK_RSP timehack for estimating delay of 'server' */
					getseqnum(d[8]);
					newack=getacknum(d[10]);
#ifdef TOLOG
					//if((rsp_ack_ack!=-1) && (rsp_seq>=(rsp_ack_ack -1))) {}
					if((rsp_ack_ack!=-1) && (rsp_seq==(rsp_ack_ack -1) || rsp_seq==(rsp_ack_ack)) ) {

						fprintf(stderr, "TOLOG: TIME RSPACK_RSP %f client %s server %s\n", tm - rsp_ack_ack_time, client, server);
					}
#endif
#ifdef PRINTFS
					fprintf(stderr, "%lld %lld %lld %lld rspbytes %lld \n", thisseqinfo.seqstart, rsp_seq, newack, rsp_ack, rsp_bytes);
#endif
					if((thisseqinfo.seqstart>=rsp_seq && (newack>=rsp_ack || newack==1))||(rsp_seq> MAGIC_NUMBER || newack>MAGIC_NUMBER)|| (newack==1 && (rsp_seq>MAGIC_NUMBER || rsp_seq<=thisseqinfo.seqstart))) {
#ifdef PRINTFS
					fprintf(stderr, "rsp bytes %lld thisseqinfo.pkt_data %d\n",rsp_bytes,  thisseqinfo.pkt_data);
#endif
						if(adjusts_rsp_seq_max<thisseqinfo.seqstart) {
						rsp_bytes+=thisseqinfo.pkt_data;
						}else {
							if(thisseqinfo.seqstart>rsp_seq)
							adjusts_rsp-=thisseqinfo.pkt_data;
						}
						rsp_ack=newack;
#ifdef PRINTFS
					fprintf(stderr, "rsp bytes %lld thisseqinfo.pkt_data %d\n",rsp_bytes,  thisseqinfo.pkt_data);
#endif
						rsp_end_time=tm;
						if(thisseqinfo.seqstart!=rsp_seq && (rsp_seq < MAGIC_NUMBER) && ( (thisseqinfo.seqstart - rsp_seq)<MAGIC_JUMP)) 
						{
	//				fprintf(stderr, "rsp bytes %lld thisseqinfo.pkt_data %d\n",rsp_bytes,  thisseqinfo.pkt_data);
							if(thisseqinfo.seqend>rsp_bytes && ( thisseqinfo.seqstart - rsp_seq)<MAGIC_JUMP && ( rsp_seq - thisseqinfo.seqstart)<MAGIC_JUMP) rsp_bytes+=(thisseqinfo.seqstart - rsp_seq);
							rsp_seq=thisseqinfo.seqend;
						}
						rsp_seq=thisseqinfo.seqend;
	//				fprintf(stderr, "rsp bytes %lld thisseqinfo.pkt_data %d\n",rsp_bytes,  thisseqinfo.pkt_data);
#ifdef PRINTFS
						fprintf(stderr, " RSP CONTINUE %f sent %lld bytes seq %lld ack %lld\n", rsp_start_time, rsp_bytes, rsp_seq, rsp_ack);
#endif
#ifdef TOLOG
						fprintf(stderr, "TOLOG: RSPCONTINUE %f sent %d bytes total %lld\n", rsp_end_time, thisseqinfo.pkt_data, rsp_bytes);
#endif
						rsp_seq=thisseqinfo.seqend;
						state=CONNSTATE_RSP;
						rsp_printed=1;
							req_printed=0;
					}
					else {
						if(thisseqinfo.seqend<=adjusts_rsp_seq_max) {
							adjusts_rsp-=(thisseqinfo.pkt_data);
							adjusts_rsp_seq_min=max(adjusts_rsp_seq_min, thisseqinfo.seqend);
							if(adjusts_rsp_seq_min>=adjusts_rsp_seq_max) {
								adjusts_rsp_seq_max=ADJUSTRSPSEQMAX;
								adjusts_rsp_seq_min=ADJUSTRSPSEQMIN;
							}
						}


						
#ifdef TOLOG
						/* NEEDRETRANSMIT */
	//					fprintf(stderr, "99999999999933\n");
LOGRETRANSMITUPDATEVARIABLE(rsp_end_time,tm);
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
					}

				} else if(state==CONNSTATE_RSPACK && client0!=rev) {
					/* rsp over req starts */
#ifdef PRINTFS
					fprintf(stderr, "RSP over REQ starts \n");
#endif
					adjusts_rsp_seq_min=ADJUSTRSPSEQMIN;adjusts_rsp_seq_max=ADJUSTRSPSEQMAX;
					getseqnum(d[8]);
					newack=getacknum(d[10]);
					fprintf(stderr, "newack %lld rsp_ack_ack %lld thisseqinfo.seqend %d req_seq %lld\n", newack, rsp_ack_ack, thisseqinfo.seqend, req_seq);
					//if(newack==rsp_ack_ack && thisseqinfo.seqend>=req_seq  ) {
					//}
					if(newack>=rsp_ack_ack && thisseqinfo.seqend>=req_seq  ) {
						req_bytes=thisseqinfo.pkt_data;
						req_start_time=tm;
						req_end_time=tm;
						req_ack_ack=-1;
						req_ack=newack;
						req_seq=thisseqinfo.seqend;
						state=CONNSTATE_REQ;
#ifdef PRINTFS
						fprintf(stderr,"RSP over REQ start %f %lld bytes client %s server %s\n", req_start_time, req_bytes, client, server);
#endif
#ifdef TOLOG
						if(rspabovethreshold!=1 || (ADJUSTTHRESHOLDFRACTION*rsp_bytes>adjusts_rsp))
						fprintf(stderr, "TOLOG: RSPOVER %f sent %lld bytes client %s server %s\n", rsp_end_time, rsp_bytes, client, server);
						//fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %d bytes\n", req_start_time, thisseqinfo.pkt_data, req_bytes);
						fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);
						fprintf(stderr, "TOLOG: TIME RSP_REQ %f client %s server %s\n", req_start_time - rsp_end_time, client, server);
#endif
						req_printed=1;
						latest_req_size=req_bytes;
						rsp_printed=0;
					}
					else { 
#ifdef TOLOG
						/* NEEDRETRANSMIT */
LOGRETRANSMITUPDATEVARIABLE(rsp_end_time,tm);
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
					}
				} else if(state==CONNSTATE_REQACK && client0!=rev && req_printed==1) {
					/* req continues */
#ifdef PRINTFS
					fprintf(stderr, "REQ continues\n");
					fprintf(stderr, "req_ack %lld req_seq %lld\n", req_ack, req_seq);
#endif
#ifdef TOLOG
					if(req_seq>=(req_ack_ack-1) && ( req_ack_ack!=-1)) {
						fprintf(stderr, "TOLOG: TIME REQACK_REQ %f client %s server %s\n", tm - req_ack_ack_time, client, server);
					}
#endif
					if(req_seq>MAGIC_JUMP && thisseqinfo.seqstart<MAGIC_JUMP && thispkts < 5)  {
						/* just adjust stuff */
					/* 
					 * will not be considered a retransmit if the req_seq earlier is too high because
					 * data was sent during the three way handshake
					 */
						req_seq=thisseqinfo.seqstart;
					}
					newack=getacknum(d[10]);
					getseqnum(d[8]);
					if ((newack == req_ack && (thisseqinfo.seqend < req_seq) &&((req_seq - thisseqinfo.seqend)<MAGIC_JUMP)) ||(newack== req_ack && thisseqinfo.seqend==req_seq && thisseqinfo.pkt_data>0 )) {
#ifdef PRINTFS
						fprintf(stderr, " resend, ignore \n");
#endif

#ifdef TOLOG
						/* NEEDRETRANSMIT */
LOGRETRANSMITUPDATEVARIABLE(req_end_time,tm);
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
					} else if((thisseqinfo.seqstart  - req_seq) > MAGIC_NUMBER) {
#ifdef PRINTFS
						fprintf(stderr, " resend, ignore 2 \n");
#endif
#ifdef TOLOG
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  						fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif

					} else if((newack>=req_ack && thisseqinfo.seqend >= req_seq) || (  req_seq==-1 && thisseqinfo.seqstart==req_bytes)) {
#ifdef PRINTFS
						fprintf(stderr, " general log bb seqstart %lld reqseq %lld diff %lld\n", thisseqinfo.seqstart,req_seq, thisseqinfo.seqstart - req_seq);
#endif

						req_bytes+=thisseqinfo.pkt_data;

						if(thisseqinfo.seqstart!=req_seq && req_seq!=-1 &&( (thisseqinfo.seqstart - req_seq)<ADJUST_THRESHOLD  ||  ( (thisseqinfo.seqstart - req_seq) < ADJUST_THRESHOLD_RATIO*req_bytes )  )) { 
							req_bytes+=(thisseqinfo.seqstart - req_seq);
#ifdef PRINTFS
							fprintf(stderr, "adjust rsp for possibly onewaytrace add %d bytes \n", (int)(thisseqinfo.seqstart - rsp_seq));
#endif

						}
						req_seq=thisseqinfo.seqend;
						req_end_time=tm;
						req_ack = newack;
#ifdef PRINTFS
						fprintf(stderr, "REQ continues sent %d bytes total %lld \n", thisseqinfo.pkt_data, req_bytes);
#endif
#ifdef TOLOG
						fprintf(stderr,"TOLOG: REQCONTINUE %f sent %d bytes total %lld \n", req_end_time, thisseqinfo.pkt_data, req_bytes);
#endif
						state=CONNSTATE_REQ;
						rsp_printed=0;
						req_printed=1;

#if 0
					} else if(req_seq > thisseqinfo.seqstart && (req_seq - thisseqinfo.seqstart) > MAGIC_JUMP && (thisseqinfo.seqend - req_bytes)<MAGIC_JUMP) {
						req_bytes+=thisseqinfo.pkt_data;

		//				if(thisseqinfo.seqstart!=req_seq && req_seq!=-1 && (thisseqinfo.seqstart - req_seq) < MAGIC_JUMP) 
		//					req_bytes+=thisseqinfo.seqstart - req_seq;
						req_seq=thisseqinfo.seqend;
						req_end_time=tm;
						req_ack = newack;
#ifdef PRINTFS
						fprintf(stderr, "REQ continues sent %d bytes total %lld \n", thisseqinfo.pkt_data, req_bytes);
#endif
#ifdef TOLOG
						fprintf(stderr,"TOLOG: REQCONTINUE %f sent %d bytes total %lld \n", req_end_time, thisseqinfo.pkt_data, req_bytes);
#endif
						state=CONNSTATE_REQ;
						rsp_printed=0;
						req_printed=1;

#endif
					}
					else if(thisseqinfo.seqend<= req_seq ) {
#ifdef PRINTFS
							fprintf(stderr, " just resending request bytes \n");
#endif
							if(req_ack<newack && newack<MAGIC_NUMBER) req_ack=newack;
							else if(newack<req_ack) {
#ifdef PRINTFS
								fprintf(stderr, " Client decreasing ack sent with request from %lld to %lld \n", req_ack, newack);
#endif
								req_ack=newack;
							}

#ifdef TOLOG
							LOGRETRANSMITUPDATEVARIABLE(req_end_time,tm);
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  						fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif

						if(req_seq>MAGIC_NUMBER) req_seq=thisseqinfo.seqend;
					} else {


#ifdef PRINTFS
						fprintf(stderr, "SHOULDNOTBEHERE 6 state%d\n", state);
#endif

#ifdef TOLOG
						fprintf(stderr, "TOLOG: SHOULDNOTBEHERE 6 state%d\n", state);
#endif
					}

				} else if(state==CONNSTATE_REQACK && client0!=rev && rsp_printed==1) {
#ifdef PRINTFS
					fprintf(stderr, "RSPOVER and perhaps REQ Start\n");
#endif

						req_seq=max(thisseqinfo.seqend,req_seq);
						req_ack=max(newack,req_ack);
						req_ack_ack=-1;
						req_bytes=thisseqinfo.pkt_data;
						rsp_end_time=tm;
						req_start_time=tm;
						req_end_time=tm;
#ifdef TOLOG
						if(rspabovethreshold!=1 || (ADJUSTTHRESHOLDFRACTION*rsp_bytes>adjusts_rsp))
						fprintf(stderr, "TOLOG: RSPOVER %f sent %lld bytes client %s server %s \n",  rsp_end_time, rsp_bytes, client, server);
						//fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %d bytes\n", req_start_time, thisseqinfo.pkt_data, req_bytes);
						fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);
						fprintf(stderr, "TOLOG: TIME RSP_REQ %f client %s server %s\n", req_start_time - rsp_end_time, client, server);
#endif
						rsp_printed=0;
						req_printed=1;
						latest_req_size=req_bytes;
						state=CONNSTATE_REQ;









					
				} else if((state&CONNSTATE_RSPACK)==CONNSTATE_RSPACK && client0==rev) {
					
					/* rsp continues but server has sent FIN also already */
					/* means that client hasnt received some part of the response so server is trying to resend
					   it , in tis case we need to track(mor like confirm) if the data is a repetition..
					   in terms of the seq number.
					   also the seq num should be around the acks sent by the client */
					newack=getacknum(d[10]);
					getseqnum(d[8]);
					if(thisseqinfo.seqend <= fin_seq && newack==fin_ack) {
#ifdef PRINTFS
						fprintf(stderr, " retransmit \n");
#endif
#ifdef TOLOG
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  						fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
					}
				} else if(state==CONNSTATE_SYNACK && client0!= rev) {
#ifdef PRINTFS
					fprintf(stderr, "here 1 \n");
#endif
					/* perhaps ACKSYN and REQ are in the same packet */
					newack=getacknum(d[10]);
					getseqnum(d[8]);
					if(newack>=synack_seq && thisseqinfo.seqend >= synack_ack) {
#ifdef PRINTFS
						fprintf(stderr, " ACKSYN and REQ in one \n");
#endif
						req_bytes=thisseqinfo.pkt_data;
						req_start_time=tm;
						acksyn_time=tm;
						req_end_time=tm;
						req_seq=-1;
						req_ack=-1;
						req_ack_ack=-1;
						rsp_ack=-1;
						rsp_seq=-1;
						rsp_ack_ack=-1;

#ifdef TOLOG
						fprintf(stderr, "TOLOG: ACKSYN %f client %s server %s\n", acksyn_time, client, server);
						if(thispkts<5) 
						fprintf(stderr, "TOLOG: TIME SYNACK_ACKSYN %f client %s server %s\n", acksyn_time - synack_time, client , server);
						//fprintf(stderr, "TOLOG: REQSTART %f sent %lld bytes total %lld bytes\n", req_start_time, req_bytes, req_bytes);
						fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);

#endif
						req_printed=1;
						latest_req_size=req_bytes;

						state=CONNSTATE_REQ;
					} else if((newack<MAGIC_JUMP) && (newack>1)) {
						/* set client/server accordingly*/
#ifdef PRINTFS
						fprintf(stderr, "here 2 client %s server %s\n", src, dst);
#endif
							strcpy(client, src);
							strcpy(server, dst);
							req_bytes=newack-1;
							req_start_time=tm;
							req_end_time=tm;
							rsp_start_time=tm;
							rsp_end_time=tm;
							rsp_bytes=thisseqinfo.pkt_data;
							rsp_ack=newack;
							req_ack_ack=-1;
							rsp_ack_ack=-1;

							fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);
							fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, client, server);
							fprintf(stderr, "TOLOG: RSPSTART %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
							state=CONNSTATE_RSP;
							rsp_printed=1;
							req_printed=0;
						latest_rsp_size=rsp_bytes;

							
					}
#ifdef PRINTFS
						fprintf(stderr, "here 3 client %s server %s\n", src, dst);
#endif

				} 
 
				else if(state==CONNSTATE_FIN && finclient0==rev) {
					/* other side has sent fin but this side is pushing data */
#ifdef PRINTFS
					fprintf(stderr, "client has closed conn, server sending more data \n");
#endif
					newack=getacknum(d[10]);
					getseqnum(d[8]);
					if(newack>=rsp_ack && thisseqinfo.seqend>=rsp_seq) {
						rsp_bytes+=thisseqinfo.pkt_data;
						rsp_seq=thisseqinfo.seqend;
						rsp_end_time=tm;
#ifdef PRINTFS
						fprintf(stderr, "RSP continue %f sent %d bytes total %lld \n", rsp_end_time, thisseqinfo.pkt_data, rsp_bytes);
#endif
					}
					else {

#ifdef TOLOG
						/* NEEDRETRANSMIT */
LOGRETRANSMITUPDATEVARIABLE(rsp_end_time,tm);
LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
					}
				} else if(state==CONNSTATE_BAD && newconn==1) {
					/* probably it is a new conn but we did not see the SYN set */
					/* if the seq number is very high and there is payload then pretend that 
					   syn and syn+ack have gone and that this is ack along with request being sent */
#ifdef PRINTFS
					fprintf(stderr, " first packet has seq and ack both \n");

#endif
					newack=getacknum(d[10]);
					getseqnum(d[8]);
					if(thisseqinfo.seqstart>MAGIC_NUMBER && newack>MAGIC_NUMBER && thisseqinfo.pkt_data>0) {
#ifdef PRINTFS
						fprintf(stderr, " good chance this is syn ack but some request is already being sent by the server \n");
						fprintf(stderr, " so we will pretend that a req of size 1 has already been sent and that this is the response \n");
#endif
						
						req_bytes=1;
						rsp_bytes=thisseqinfo.pkt_data;
						rsp_ack=-1;
						req_ack_ack=-1;
						rsp_ack_ack=-1;
						rsp_seq=thisseqinfo.seqend;
						rsp_ack=newack;
						req_start_time=tm;
						rsp_start_time=tm;
						syn_time=tm;
						req_end_time=tm;

						if(rev==0) {
							client0=0;
							strcpy(server, src);
							strcpy(client, dst);
							
						} else {
							client0=1;
							strcpy(client, src);
							strcpy(server, dst);
						}

						state=CONNSTATE_RSP;
#ifdef TOLOG
							fprintf(stderr, "TOLOG: SYN %f client %s server %s\n", syn_time, client, server);
							fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);
							fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, client, server);
							fprintf(stderr, "TOLOG: RSPSTART %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
#endif
						req_printed=0 ;
						rsp_printed=1;
						latest_rsp_size=rsp_bytes;

					}
					else if(thisseqinfo.seqstart>MAGIC_NUMBER && thisseqinfo.pkt_data>0 && (newack<=1 || newack > MAGIC_JUMP))
					{
#ifdef PRINTFS
						fprintf(stderr, " good chance this is acksyn+req \n");
#endif
						// what if we just ignore ?
					//	goto getnextline;
						/* fake that you have already seen SYn and syn+Ack */

						if(newack>MAGIC_JUMP) {
#ifdef PRINTFS
							fprintf(stderr, " actually looks like this is synack+req\n");
#endif
						} 
						//else if(newack >1) {
						//	fprintf(stderr, "perhaps a small request already happened and this is a response...\n");

//						}
						syn_time=tm;
						synack_time=tm;
						syn_seq=thisseqinfo.seqstart-1;
						synack_seq=newack-1;
						synack_ack=thisseqinfo.seqstart;


						if(rev==1) {
							client0=0; /* since we are pretending that this is ackSYN*/
							strcpy(server,src);
							strcpy(client,dst);
						} else { 
							client0=1;
							strcpy(server,dst);
							strcpy(client,src);
						}
#ifdef TOLOG
						fprintf(stderr,"TOLOG: 11did not see SYN and SYN+ACK set but setting it and trying\n");
						fprintf(stderr, "TOLOG: SYN %f client %s server %s\n", syn_time, client, server);
						fprintf(stderr, "TOLOG: SYNACK %f client %s server %s\n", synack_time, client, server);
						//fprintf(stderr, "TOLOG: TIME: SYN_SYNACK %f client %s server %s\n", synack_time - syn_time, client, server);
#endif
						//this_flag=this_flag|TCPFLG_SYN;
						state=CONNSTATE_SYNACK;
						goto testallstates;
					}
					else {
						/* connection already in progress perhaps */
#ifdef PRINTFS
						fprintf(stderr, " perhaps req/resp already in progress \n");
#endif
	//					if(newack <=1 || newack>MAGIC_NUMBER ) {
						if(newack <=1 || newack>MAGIC_NUMBER || newack>thisseqinfo.seqend) {

#ifdef PRINTFS
							fprintf(stderr, " request\n");
#endif
							/* this is request */
							state=CONNSTATE_REQ;
							//						rsp_printed=0;


							//						req_printed=0;

							req_ack=getacknum(d[10]);
							req_start_time=tm;
							req_ack_ack=-1;
							req_end_time=tm;
							req_seq=thisseqinfo.seqend;
							req_bytes=thisseqinfo.pkt_data;
							if((thisseqinfo.seqstart!=0 || thisseqinfo.seqend!= thisseqinfo.pkt_data ) && newack<MAGIC_NUMBER && ( (thisseqinfo.seqend - req_bytes) < MAGIC_JUMP)) {
/*# Tue May 10 23:45:30 PDT 2005 magic jump added */
								req_bytes=thisseqinfo.seqend;
							}
							if(rev==0) {
								client0=1;
								strcpy(server, dst);
								strcpy(client, src);
							}
							else {
								client0=0;
								strcpy(server, src);
								strcpy(client, dst);
							}
							if(req_seq>MAGIC_NUMBER) req_seq=-1;
							if(req_ack>MAGIC_NUMBER) req_ack=-1;
							/*
							   fprintf(stderr, " ack %lld\n", req_ack);
							   fprintf(stderr, "reqseq %lld bytes %lld ack %lld\n", req_seq, req_bytes, req_ack)  ;
							   fprintf(stderr, " ack %lld\n", req_ack);
							 */
							syn_time=tm;
							syn_seq=MAGIC_NUMBER;
#ifdef TOLOG
							fprintf(stderr, "TOLOG: SYN %f client %s server %s\n", syn_time, client, server);
#endif
#ifdef PRINTFS
#ifndef WANT_PRINTF_LONG_LONG
							fprintf(stderr, " REQ START %f sent %lld bytes seq %lld ack %lld \n", req_start_time, req_bytes, req_seq, req_ack);
#else
							fprintf(stderr, " REQ START %f sent %lld bytes seq %lld ack %lld \n", req_start_time, req_bytes, req_seq, req_ack);
#endif
#endif
#ifdef TOLOG
						//	fprintf(stderr, "TOLOG: REQSTART %f sent %lld bytes total %lld bytes\n", req_start_time, req_bytes, req_bytes);
						fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);
#endif
							req_printed=1;
						latest_req_size=req_bytes;

							rsp_printed=0;







						}
						else {
#ifdef PRINTFS
							fprintf(stderr, " response\n");
#endif
							/* this is response */
							req_end_time=tm;
							syn_time=tm;
							syn_seq=-1;
							req_start_time=tm;
							if(newack<MAGIC_JUMP)req_bytes=newack-1;
							else req_bytes=thisseqinfo.pkt_data;
							req_seq=-1;req_ack=-1;req_ack_ack=-1;
							rsp_start_time=tm;
							rsp_end_time=tm;
							if(thisseqinfo.seqend<MAGIC_JUMP)rsp_bytes=thisseqinfo.seqend;
							else rsp_bytes=thisseqinfo.pkt_data;
							rsp_ack=newack;
							rsp_seq=thisseqinfo.seqend;
		//					fprintf(stderr, "here\n");
							if(rev==0) {
								client0=0;
								strcpy(server,src);
								strcpy(client,dst);
							}
							else {
								client0=1;
								strcpy(server,dst);
								strcpy(client,src);
							}
#ifdef TOLOG
					//		fprintf(stderr, " SYN %f client %s server %s seq %lld \n", syn_time, client, server, syn_seq);
							fprintf(stderr, "TOLOG: SYN %f client %s server %s\n", syn_time, client, server);
						//	fprintf(stderr, "TOLOG: REQSTART %f sent %lld bytes total %lld bytes\n", req_start_time, req_bytes, req_bytes);
						fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);
							fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, client, server);
							fprintf(stderr, "TOLOG: RSPSTART %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
#endif
							state=CONNSTATE_RSP;
							req_printed=0;
							rsp_printed=1;
						latest_rsp_size=rsp_bytes;
						}

					}
				} else if ( ((state& CONNSTATE_FIN)==CONNSTATE_FIN) && finclient0==rev  ){
					/* client has sent fin and server is first sending some data perhaps */
					getseqnum(d[8]);
					newack=getacknum(d[10]);
#ifdef PRINTFS
					fprintf(stderr, " client/server had send fin but server/client sending response/request first \n");

					fprintf(stderr, " seqstart %lld req_seq %lld rsp_seq %lld newack %lld rsp_ack %lld req_ack %lld rsp_ack_ack %lld req_ack_ack %lld\n", thisseqinfo.seqstart,req_seq, rsp_seq ,newack,rsp_ack, req_ack,rsp_ack_ack, req_ack_ack);
					if(rsp_printed==1) fprintf(stderr, " outstanding rsp to print \n");
#endif

					if(client0==rev) {
#ifdef PRINTFS
						fprintf(stderr, "server \n");

#endif
						/* this is the servers packe */

						if(( (thisseqinfo.seqstart>=rsp_seq)  && (newack>=rsp_ack)) ||( rsp_ack > MAGIC_NUMBER)) {
#ifdef PRINTFS
							fprintf(stderr, " inside server \n");
#endif
							rsp_ack=newack;
							rsp_seq=thisseqinfo.seqend;
							rsp_end_time=tm;
							rsp_start_time=tm;
							if((state&CONNSTATE_RSP)==CONNSTATE_RSP) {
								rsp_bytes+=thisseqinfo.pkt_data;

#ifdef PRINTFS
								fprintf(stderr, " RSP CONTINUE %f sent %lld bytes seq %lld ack %lld\n", rsp_start_time,rsp_bytes, rsp_seq, rsp_ack);
#endif
#ifdef TOLOG
								fprintf(stderr, "TOLOG: RSPCONTINUE %f sent %d bytes total %lld\n", rsp_end_time, thisseqinfo.pkt_data, rsp_bytes);
#endif
							req_printed=0;

							}
							else{
								rsp_bytes=thisseqinfo.pkt_data;
								rsp_ack_ack=-1;

#ifdef PRINTFS
								fprintf(stderr, " RSP START %f sent %ld bytes seq %ld ack %d\n", rsp_start_time,rsp_bytes, rsp_seq, rsp_ack);
#endif
								state=state|CONNSTATE_RSP;
#ifdef TOLOG
								fprintf(stderr, "TOLOG: RSPSTART %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);
#endif

							}
							req_printed=0;
							rsp_printed=1;
						latest_rsp_size=rsp_bytes;
						} else {
#ifdef TOLOG
							/* NEEDRETRANSMIT */

LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
						}

					}
					else {

#ifdef PRINTFS
						fprintf(stderr, " client \n");
#endif
						/* this is the client packet */

						if(( (thisseqinfo.seqstart>=req_seq)  && (newack>=req_ack || newack>=rsp_ack_ack)) ||( req_ack > MAGIC_NUMBER)) {
#ifdef PRINTFS
							fprintf(stderr, " inside client \n");
#endif
							req_ack=newack;
							req_seq=thisseqinfo.seqend;
							req_end_time=tm;
	//						req_start_time=tm;
							if((state&CONNSTATE_REQ)==CONNSTATE_REQ) {
								req_bytes+=thisseqinfo.pkt_data;

#ifdef PRINTFS
								fprintf(stderr, " REQ CONTINUE %f sent %lld bytes seq %lld ack %lld\n", req_start_time,req_bytes, req_seq, req_ack);
#endif
#ifdef TOLOG
								fprintf(stderr, "TOLOG: REQCONTINUE %f sent %d bytes total %lld\n", req_end_time, thisseqinfo.pkt_data, req_bytes);
#endif
						rsp_printed=0;

							}
							else{
								req_bytes=thisseqinfo.pkt_data;
								req_ack_ack=-1;
								req_seq=thisseqinfo.seqstart;
								req_start_time=tm;
								req_end_time=tm;

#ifdef PRINTFS
								fprintf(stderr, " REQ START %f sent %ld bytes seq %ld ack %d\n", req_start_time,req_bytes, req_seq, req_ack);
#endif
								state=state|CONNSTATE_REQ;
#ifdef TOLOG
								fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);
#endif

							}
							req_printed=1;
						latest_req_size=req_bytes;
							rsp_printed=0;
						} else {
#ifdef TOLOG
							/* NEEDRETRANSMIT */

LOGRETRANSMIT(tm,client,server,client0,rev,thisseqinfo.seqend);
/*  fprintf(stderr, "TOLOG: RETRANSMIT %f client %s server %s client0 %d rev %d\n", tm , client, server,client0,rev);  */
#endif
						}





					}





				} else if(state==CONNSTATE_SYN && client0!=rev) {
					/* perhaps only SYN seen and now you are seeing first data packet */
#ifdef PRINTFS
					fprintf(stderr, " first req packet perhaps after SYN \n");
#endif
					newack=getacknum(d[10]);
					getseqnum(d[8]);
					if(newack==1 && thisseqinfo.seqend<MAGIC_NUMBER) {
						state=CONNSTATE_REQ;



						req_start_time=tm;
						req_ack_ack=-1;
						req_ack=newack;
						req_end_time=tm;
						req_seq=thisseqinfo.seqend;
						req_bytes=thisseqinfo.pkt_data;
						
						if(thisseqinfo.seqstart!=0 || thisseqinfo.seqend!= thisseqinfo.pkt_data) {
							req_bytes=thisseqinfo.seqend;
						}
					//	if(rev==0) client0=1;
					//	else client0=0;
						if(req_seq>MAGIC_NUMBER) req_seq=-1;
						if(req_ack>MAGIC_NUMBER) req_ack=-1;
						/*
						   fprintf(stderr, " ack %lld\n", req_ack);
						   fprintf(stderr, "reqseq %lld bytes %lld ack %lld\n", req_seq, req_bytes, req_ack)  ;
						   fprintf(stderr, " ack %lld\n", req_ack);
						 */
#ifdef PRINTFS
#ifndef WANT_PRINTF_LONG_LONG
						fprintf(stderr, " REQ START %f sent %lld bytes seq %lld ack %lld \n", req_start_time, req_bytes, req_seq, req_ack);
#else
						fprintf(stderr, " REQ START %f sent %lld bytes seq %lld ack %lld \n", req_start_time, req_bytes, req_seq, req_ack);
#endif
#endif
#ifdef TOLOG
	//					fprintf(stderr, "TOLOG: REQSTART %f sent %lld bytes total %lld bytes\n", req_start_time, req_bytes, req_bytes);
						fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);
#endif
						req_printed=1;
						latest_req_size=req_bytes;














						
					}
				} else if ( ((state&CONNSTATE_FIN)==CONNSTATE_FIN ) && finclient0!=rev && client0==rev) {
#ifdef PRINTFS
					fprintf(stderr, "server had sent fin but looks like there is more data\n");
#endif
					if(thisseqinfo.seqstart<rsp_seq) {
#ifdef PRINTFS
						fprintf(stderr, " just a retransmit \n");
#endif
					}
					
				}
				
				else if(client0==rev) { 
					/* perhaps req rsp in progress and did not catch some packets in between   */
#ifdef PRINTFS
					fprintf(stderr, " client0 %d rev %d finclient0\n", client0, rev, finclient0);
					fprintf(stderr, " reqrsp in progress some packets missed \n");
#endif
					newack=getacknum(d[10]);
					getseqnum(d[8]);
					if(thisseqinfo.seqend<MAGIC_NUMBER){
						rsp_bytes=thisseqinfo.pkt_data;
						rsp_seq=thisseqinfo.seqend;
						rsp_ack_ack=-1;
						rsp_ack=newack;
						rsp_start_time=tm;
						rsp_end_time=tm;
						rsp_printed=1;
						/* adjust */
						if(thisseqinfo.seqend>(rsp_bytes +1 ) && (thisseqinfo.seqend - rsp_bytes ) < MAGIC_JUMP && ( thisseqinfo.seqend - rsp_bytes)<ADJUST_THRESHOLD) {
#ifdef PRINTFS
							fprintf(stderr, "adjust rsp for possibly onewaytrace add %d bytes \n", (int)(rsp_ack_ack - rsp_seq));
#endif
							rsp_bytes+=(thisseqinfo.seqend - rsp_bytes);
						}
						/* adjust over */
						state=CONNSTATE_RSP;
						if(newack<MAGIC_NUMBER){
							req_bytes=newack-1;
							req_seq=newack-1;
							req_start_time=tm;
							req_end_time=tm;
							req_ack=-1;
							req_ack_ack=-1;
#ifdef TOLOG
							fprintf(stderr, "TOLOG: REQSTART %f sent %d bytes total %lld client %s server %s\n", req_start_time, thisseqinfo.pkt_data, req_bytes, client, server);
							fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, client, server);
							req_printed=0;

#endif
						}

#ifdef TOLOG
						fprintf(stderr, "TOLOG: RSPSTART %f sent %d bytes total %lld client %s server %s\n", rsp_start_time, thisseqinfo.pkt_data, rsp_bytes, client, server);

						req_printed=0;
						rsp_printed=1;
						latest_rsp_size=rsp_bytes;
#endif
					}


				}
				
				
				
				
				
				
				
				
				
				
				else {

#ifdef PRINTFS
					fprintf(stderr, "SHOULDNOTBEHERE 4\n");
#endif
#ifdef TOLOG
					fprintf(stderr, "TOLOG: SHOULDNOTBEHERE 4 state %d\n", state);
#endif
					state=CONNSTATE_BAD;
				}


				/* it has seq num also */
			}
			//	fprintf(stderr, "SHOULDNOTBEHERE 4\n");

			//{} /* if ACK is thre */
		}

		/* STATETEST 4  TESTING FOR ERR */
		else {
#ifdef PRINTFS
			fprintf(stderr, "SHOULDNOTBEHERE 1 state %d\n", state);
#endif
#ifdef TOLOG
			fprintf(stderr,"TOLOG: SHOULDNOTBEHERE 1 state %d\n", state);
#endif
		}


		
getnextline:
	//	if(flowid!=NULL) {
			strcpy(prevflowid, flowid);
			prev_rev=rev;
	//}
#ifdef PRINTFS
				if(!strcmp(d[9],"ack")) {
					getseqnum(d[8]);
					fprintf(stderr, " pktLOG1 %d\n", thisseqinfo.pkt_data);
				ttlbytes+=thisseqinfo.pkt_data;
				fprintf(stderr, " ttlbytes %lld\n", ttlbytes);
				} else if(strcmp(d[8],"ack")!=0) {
					getseqnum(d[8]);
					fprintf(stderr, " pktLOG2 %d\n", thisseqinfo.pkt_data);
				}
#endif
		
		if(state==CONNSTATE_BAD)
		{
			if(req_printed==0 && rsp_printed ==0)
			{
#ifdef TOLOG
				fprintf(stderr, "TOLOG: NOTHING LOGGED \n");
#endif
#ifdef PRINTFS
				if(!strcmp(d[9],"ack")) {
					getseqnum(d[8]);
					fprintf(stderr, " pktleft %d\n", thisseqinfo.pkt_data);
				}
#endif

			} else {
				if(req_printed==1)
				{
#ifdef TOLOG
					fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, client, server);
#endif
					req_printed=0;
				}

				if(rsp_printed==1)
				{
#ifdef TOLOG
					if(rspabovethreshold!=1 || (ADJUSTTHRESHOLDFRACTION*rsp_bytes>adjusts_rsp))
					fprintf(stderr, "TOLOG: RSPOVER %f sent %lld bytes client %s server %s\n", rsp_end_time, rsp_bytes, client, server);
#endif
					rsp_printed=0;
				}
			}
		}
#ifdef PRINTFS
		fprintf(stderr, "----------------------------------------------------------\n");
#endif
		strcpy(oclient, client);
		strcpy(oserver, server);
	}

	

/****************************/
fprintf(stderr, " end of file ... \n");



/****************************/
			if(req_printed==1)
			{
#ifdef TOLOG
				fprintf(stderr, "TOLOG: REQOVER %f sent %lld bytes client %s server %s\n", req_end_time, req_bytes, oclient, oserver);
#endif
				req_printed=0;
			}

			if(rsp_printed==1)
			{
#ifdef TOLOG
				if(rspabovethreshold!=1 || (ADJUSTTHRESHOLDFRACTION*rsp_bytes>adjusts_rsp))
				fprintf(stderr, "TOLOG: RSPOVER %f sent %lld bytes client %s server %s\n", rsp_end_time, rsp_bytes, oclient, oserver);
#endif
				rsp_printed=0;
			}

#ifdef TOLOG
			fprintf(stderr, "TOLOG: PREV CONN OVER\n");

#endif
			return 0;
	}


