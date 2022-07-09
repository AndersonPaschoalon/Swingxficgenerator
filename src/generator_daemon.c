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
/**** *
 * NAME
 *   Source code for generators
 * AUTHOR
 *	Kashi Vishwanath
 ****/
#include "hashtable_cwc22.h"
#include "hashtable_itr_cwc22.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* for memcmp */
#include <sys/time.h> /* for timeofday */
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<signal.h>
#include<sys/wait.h>
#include<errno.h>
#include<unistd.h>
#include<math.h>
#include<netdb.h>
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#include "workgen_header.h"


#ifdef DEBUG_REQRSPSIZE
long DEBUG_REQRSPSIZE_RSP_sent, DEBUG_REQRSPSIZE_RSP_original, DEBUG_REQRSPSIZE_REQ_original, DEBUG_REQRSPSIZE_REQ_sent;
#endif
char * tptr;
char * fptr;
char timescpy[1024];
char addcpy[1024];
/*
# define EXTRACT_TIMES_AND_ADD(arg, times, add)  { \
	strcpy(timescpy,arg); \
	tptr = strchr(timescpy, 't'); \
	if(tptr==NULL) { \
		add=atof(timescpy); \
		times=0.0; \
	} else { \
		add=atof(tptr+1); \
		*tptr='\0'; \
		times=atof(timescpy); \
	}\
}
*/

# define EXTRACT_TIMES_AND_ADD(arg, times, add, factor)  { \
	strcpy(timescpy,arg); \
	tptr = strchr(timescpy, 't'); \
	if(tptr==NULL) { \
		add=atof(timescpy); \
		times=0.0; \
		factor=1.0; \
	} else { \
		*tptr='\0'; \
		times=atof(timescpy); \
		fptr=strchr(tptr+1,'t'); \
		if(fptr==NULL) { \
			add=atof(tptr+1); \
			factor=1.0; \
		} else{ \
			*fptr='\0'; \
			add=atof(tptr+1); \
			factor=atof(fptr+1); \
		} \
	}\
}

//# define DONTNEETD2
//#define NEWPRINTFS 1 
/* for params new way.. begin */
/****v* Swing/generator_params
    * NAME
	*	Global vars
	* SOURCE 
	*/
int PKTSIZE;
struct sockaddr_in server;
udp_req this_rsp, this_req;
enum protocols {UDP=-1, TCP=1};
	int vnclientnum, offsetval;
	char * vnservernum;
	int i;
	int tcpnodelayflag=1;
	int skiptimes;
	int numrre,rrenum;
	int req, resp,times, numreq;
	int rretimes;
	int interRRE;
	int interconn;
	long REQ, RSP, RSP2, wantrsp, wantreq;
	int conntimes;
	int numconn,connnum;
	int reqthink;
	long serverthink;
	long brate;
	int portnum;
	enum protocols transport;
	char tosendmessage[LINESIZE];
	char getmessage[LINESIZE];

	char Inline[LINESIZE];
	/***** PArams */
/* for params new way..  end */

extern void generate_zipf(char *, char *, char *);
extern int genfromfile(char * , int, int **);

int loops;
//typedef unsigned int uint32_t;
//typedef unsigned short uint16_t;
struct timeval tv,tv2, cdfstarttime;
int trial;

void Usage(char **args)
{
	printf("Usage:%s <indifile> <tstart> <psize> <REQTHINK> <INTERCONN> <INTERRRE>\n", args[0]);
	exit(1);
}


  fd_set readset, tmpset;
  int result;
/****f* Swing/getresponse
 * 	NAME
 * 		getresponse
 *	AUTHOR
 *		Kashi Vishwanath 
 *	SOURCE
 ****/
int getresponse(int sock,char * getmessage,enum protocols proto, struct sockaddr *client,int client_len, long expected) {
	int responsesize=0;
	int thisdone;
	int r;
	int retrysend=0;
	int done=0;
	int s;
	int getRSP;
	int totreceived;
	struct timeval tv1;
#ifdef NEWPRINTFS
//	fprintf(stderr, "waiting for response \n");
#endif
	if(proto==UDP) {

#ifndef NOPRINTFS
	//	fprintf(stderr, "to get response udp \n");
#endif
	//	fflush(stderr);
		totreceived=0;
		gettimeofday(&tv1, NULL);
		/* this is before any data is received */
waitformoredata:

		FD_ZERO(&readset);
		FD_SET(sock, &readset);
	//	fprintf(stderr, " select fdset for %d \n", sock);

		tmpset = readset;
		tv.tv_sec = TIMEOUTSECS;
		tv.tv_usec = 0;
#ifdef NEWPRINTFS
		fprintf(stderr, " waiting for data \n");
#endif
		result = select(FD_SETSIZE, &tmpset, NULL, NULL, &tv);
		//result = select(sock+1, &tmpset, NULL, NULL, &tv);

		if (result == 0) {
			if(totreceived==0 && retrysend==0 ) {
				retrysend=1;
				sprintf(Inline,"R 1 1 1 0 %ld %d %d 2222\0", this_rsp.bytes, this_rsp.pkts, this_rsp.duration);
	//			fprintf(stderr, " retrysend %d bytes \n", sendto(sock, Inline, strlen(Inline) , 0, client, client_len));
				
				goto waitformoredata;
			}
#ifndef NOPRINTFS
			fprintf(stderr,"%dTimed out so Exiting...\n", getpid());
#endif
#ifdef NEWPRINTFS
			fprintf(stderr,"%dTimed out so Exiting...\n", getpid());
#endif
		}                           
		else if (result == -1) {    
			fprintf(stderr, "%s\n", "Error....exiting");
//			fflush(stderr);
			exit(-1);
		}                           
		else {
	//		fprintf(stderr , "select's data \n");
	//		fflush(stderr);
			r=0;
			if (FD_ISSET(sock, &tmpset)) {   
				if((r=recvfrom(sock, Inline, LINESIZE, 0,client, &client_len))<0)
				{
					perror("recvfrom error in generator_daemon.c");
//					fflush(stderr);
				//	close(sock);
					exit(-1);
				}
#ifdef NEWPRINTFS
				fprintf(stderr, "RECVFROM: Inline before slash0 %s\n", Inline);
				fprintf(stderr, "RECVFROM: r %d\n",r );
#endif
				Inline[r]='\0';
#ifdef NEWPRINTFS
				fprintf(stderr, "RECVFROM: Inline after slash0 %s\n", Inline);
#endif
				

			}
			else {
				fprintf(stderr,"select said yes but no data avail\n");
//				fflush(stderr);
				//close(sock);
				exit(-1);
			}
			totreceived+=r;
#ifdef NEWPRINTFS
			fprintf(stderr, " received some data %d bytes \n",r);
#endif
			gettimeofday(&tv2,NULL);
			/* this is after the last byte of data is received */
			if(strstr(Inline, "ALLDONE")==NULL)
			{
#ifdef NEWPRINTFS
				fprintf(stderr, "notoveryet: %s\n", Inline);
#endif
				goto waitformoredata;
			}
#ifdef NEWPRINTFS
			fprintf(stderr, " ALLOVER\n");
#endif
		}

		r=totreceived;
#ifndef NOPRINTFS
	//	fprintf(stderr," GEt responsne by udp \n");
#endif
	} else {
#ifndef NOPRINTFS
		//fprintf(stderr," get response  by tcp \n");
#endif

			r=0;
			thisdone=0;
						while(thisdone!=1 && r<expected) {
							if(done!=1) s=recv(sock,&Inline[r], LINESIZE-r, 0);
							else s=recv(sock,Inline,LINESIZE,0);
							if(s<0) {
								perror("recv failed");
								close(sock);
//								fflush(stderr);
								exit(-1);
							}
							if(done!=1)
								if(strstr(Inline,TERM_STRING)!=NULL) {
									done=1;
									sscanf(Inline,"RSP %d", &getRSP);
								}
							r+=s;
							if(s==0) {
								//perror("Remote server has called close");
								thisdone=1;
							}
							if(done==1)
								if(r>=getRSP) thisdone=1;
#ifndef NOPRINTFS
							fprintf(stderr, "got %d out of %d bytes expecting %ld \n", r, getRSP, expected);
#endif
#ifdef NEWPRINTFS
							fprintf(stderr, "got %d out of %d bytes \n", r, getRSP);
#endif
						}
#ifndef NOPRINTFS
						
						fprintf(stderr, "got response  RSP %d bytes \n", r);
#endif

#ifdef NEWPRINTFS
						fprintf(stderr, "got response  RSP %d bytes \n", r);
#endif
		

	}

	return r;
}

/****f* Swing/sendmessage
  * NAME
  * this sends the message
  * INPUTS
  *		sock - the socket
  * SOURCE
  */
long	sendmessage(int sock,char * Inline,int brate, long resp_size_int,enum protocols proto,struct sockaddr* client,int client_len, udp_req thisreq, udp_req thisrsp){
	long sent=0;
	long r;
	int result, numtries;
	int pktnum;
	long ttltime;
	struct timeval udpstart, udpend, udpsleep;
    int factor;
	int pktssent=0;
	int pktstosend;
	if(proto==UDP) {
		pktstosend=thisreq.pkts;
#ifdef NEWPRINTFS
	//	fprintf(stderr, " udp... to send %d bytes %d pkts %d secs \n", thisreq.bytes, thisreq.pkts, thisreq.duration);
#endif
		thisreq.duration=max(thisreq.duration,1);
		thisreq.pkts=max(thisreq.pkts,1);
		memset(Inline,'1',LINESIZE-1);
		thisreq.pkts=(thisreq.bytes/thisreq.pkts > UDPPKTSIZEMAX ) ? (1+thisreq.bytes/UDPPKTSIZEMAX):thisreq.pkts;

		if(thisreq.bytes<=0) thisreq.bytes=1;
//		thisreq.pkts=(thisreq.bytes/thisreq.pkts < UDPPKTSIZEMIN ) ? (1+thisreq.bytes/UDPPKTSIZEMIN):thisreq.pkts;
		//if(thisreq.pkts>UDP_PKT_DURATION_THRESHOLD){}
#ifndef NORINTFS
	//	fprintf(stderr, " udp... to send %d bytes %d pkts %d secs \n", thisreq.bytes, thisreq.pkts, thisreq.duration);
	//	fprintf(stderr," UDP_PKT_THRESHOLD %d UDP_DURATION_THRESHOLD %d\n", UDP_PKT_THRESHOLD, UDP_DURATION_THRESHOLD);
#endif
		if(thisreq.pkts>UDP_PKT_THRESHOLD && thisreq.duration > UDP_DURATION_THRESHOLD){
			//thisreq.pkts=(thisreq.bytes/thisreq.pkts > UDPPKTSIZEMAX ) ? (1+thisreq.bytes/UDPPKTSIZEMAX):thisreq.pkts;

			factor=4;
			brate=thisreq.bytes/thisreq.duration;
#ifdef NEWPRINTFS
			fprintf(stderr, " should use duration \n");
			fprintf(stderr, " transmit %ld bytes in %ld pkts in %ld seconds \n", thisreq.bytes, thisreq.pkts, thisreq.duration);
//			fflush(stderr);
#endif
			gettimeofday(&udpstart,NULL);
			ttltime=0;
			sent=0;
			//		r=sendto(sock, Inline, 100, 0, client, client_len);
			//sent+=r;

			while(ttltime<=thisreq.duration && ((thisreq.duration<10)?(sent<thisreq.bytes):1 )) 
			{
#ifdef NEWPRINTFS
				fprintf(stderr, " ttltime %d secs, bytes sent %d \n", ttltime, sent);
#endif
				if(ttltime<thisreq.duration)
					r=sendto(sock, Inline, max(min(thisreq.bytes/factor/thisreq.duration,UDPPKTSIZEMAX),UDPPKTSIZEMIN), 0, client, client_len);
				else
					r=sendto(sock, Inline, max(min(thisreq.bytes/factor/thisreq.duration,UDPPKTSIZEMAX), UDPPKTSIZEMIN ), 0, client, client_len);

				sent+=r;
				pktssent++;
#ifdef NEWPRINTFS
				fprintf(stderr, "sent %ld bytes total %ld \n", r, sent);
#endif

				if(thisreq.bytes > sent )
				{
#ifdef NEWPRINTFS
					fprintf(stderr, " check1 \n");
//					fflush(stderr);
#endif
					udpsleep.tv_sec=(thisreq.duration - ttltime)/( 1.0*(thisreq.bytes-sent) / (thisreq.bytes/thisreq.pkts) );
#ifdef NEWPRINTFS
					fprintf(stderr, " check2 \n");
//					fflush(stderr);
#endif
					if((thisreq.bytes - sent )>0)
						udpsleep.tv_usec=1.0*(1.0* (thisreq.duration - ttltime)/((thisreq.bytes-sent)/(thisreq.bytes/thisreq.pkts)) - udpsleep.tv_sec)*1000000;
					else udpsleep.tv_usec=999999;
#ifdef NEWPRINTFS
					fprintf(stderr, " check3 \n");
//					fflush(stderr);
#endif
					//udpsleep.tv_sec=thisreq.duration/thisreq.pkts;
					//udpsleep.tv_usec=1.0*(1.0*thisreq.duration/thisreq.pkts - udpsleep.tv_sec)*1000000;
#ifdef NEWPRINTFS
					fprintf(stderr, "sleep for %ld %ld \n", udpsleep.tv_sec, udpsleep.tv_usec);
//					fflush(stderr);
#endif
				}
				udpsleep.tv_sec=0;
				udpsleep.tv_usec=1000000/factor;
				udpsleep.tv_usec=max(udpsleep.tv_usec, 0);
				udpsleep.tv_sec=max(udpsleep.tv_sec, 0);
				select(0,NULL,NULL,NULL,&udpsleep);
				gettimeofday(&udpend, NULL);
				ttltime=udpend.tv_sec - udpstart.tv_sec + ( udpstart.tv_usec - udpstart.tv_usec)/1000000 ;
#ifdef NEWPRINTFS
				fprintf(stderr, " ttltime %ld\n", ttltime);
#endif
			}
#ifdef NEWPRINTFS
			fprintf(stderr, "out of the loop\n");
//			fflush(stderr);
#endif
			sprintf(Inline,"R 0 0 0 0 %d %d %d 0", thisrsp.bytes, thisrsp.pkts,thisrsp.duration);
			//	sprintf(Inline,"RSPSEND %d %d %d \0", thisrsp.bytes, thisrsp.pkts,thisrsp.duration);
			r=sendto(sock, Inline, min(strlen(Inline), UDPPKTSIZEMIN), 0, client, client_len);
			sent+=r;
				pktssent++;
		}
		else {  
		//	thisreq.pkts=(thisreq.bytes/thisreq.pkts > UDPPKTSIZEMAX ) ? (1+thisreq.bytes/UDPPKTSIZEMAX):thisreq.pkts;
		//	thisreq.pkts=(thisreq.bytes/thisreq.pkts<UDPPKTSIZEMIN) ? 1:thisreq.pkts;
			memset(Inline,'1',LINESIZE-1);
#ifdef NEWPRINTFS
			fprintf(stderr, " should use bytes and pkts \n");
#endif
			//	while(sent<resp_size_int) {}
			for(pktnum=0;pktnum<thisreq.pkts;pktnum++)
			{
				if(pktnum==thisreq.pkts-1){
					sprintf(Inline,"R 0 0 0 0 %d %d %d 0", thisrsp.bytes, thisrsp.pkts,thisrsp.duration);
					//sprintf(Inline,"RSPSEND %d %d %d ", thisrsp.bytes, thisrsp.pkts,thisrsp.duration);
					Inline[strlen(Inline)]=' ';
#ifdef NEWPRINTFS
					fprintf(stderr, "sending %s\n", Inline);
#endif
					r=sendto(sock, Inline, min(max( (thisreq.bytes - sent), UDPPKTSIZEMIN), UDPPKTSIZEMAX) ,0, client, client_len);
				}
				else r=sendto(sock, Inline, min(thisreq.bytes/thisreq.pkts, UDPPKTSIZEMAX), 0, client, client_len);
				/*		if((resp_size_int - sent) <= UDPPKTSIZEMAX)
						r=sendto(sock,& Inline[sent], resp_size_int - sent, 0, client, client_len);
						else
						r=sendto(sock,& Inline[sent], UDPPKTSIZEMAX, 0, client, client_len);
				 */
				if(r<0) {
					perror("send failed");
					close(sock);
//	//				fflush(stderr);
					exit(-1);
				}
				sent+=r;
				pktssent++;
			}
			//{}
		}
#ifndef NOPRINTFS
	//	fprintf(stderr," Send by udp message %s\n", Inline);
#endif
		resp_size_int=sent;
		if(thisreq.bytes>0) goto canproceedtoreadudp;
//#ifdef NEWPRINTFS
		fprintf(stderr, " waiting for data to come...\n");
		numtries=0;
		while(numtries<5) {
			FD_ZERO(&readset);
			FD_SET(sock, &readset);
	//	fprintf(stderr, " select fdset for %d \n", sock);

		tmpset = readset;
		tv.tv_sec = TIMEOUTSECS_DURINGSEND;
		tv.tv_usec = 0;
#ifdef NEWPRINTFS
		fprintf(stderr, " waiting for data \n");
#endif
		result = select(FD_SETSIZE, &tmpset, NULL, NULL, &tv);
		if(result>0) {
#ifdef NEWPRINTFS
			fprintf(stderr, "got some data \n");
#endif
			goto canproceedtoreadudp;
		}
		else if(result<0) {
			fprintf(stderr, " error in select during send data and polling receive \n");
//			fflush(stderr);
			exit(1);
		}
		
			
			numtries++;
			sprintf(Inline,"RSPSEND %d %d %d \0", thisrsp.bytes, thisrsp.pkts,thisrsp.duration);
			r=sendto(sock, Inline, strlen(Inline), 0, client, client_len);
			pktssent++;
			if(r>0)
			{
				fprintf(stderr," retried sending %s\n", Inline);
			}
		}
canproceedtoreadudp:
		sleep(0);
#ifndef NOPRINTFS
	if(pktstosend>0)	fprintf(stderr, " UDP GENERATOR BYTES %d PKTS TOSEND %d sent %d diff %f \%\n", thisreq.bytes, pktstosend, pktssent, 100.0*(pktstosend - pktssent)/pktstosend);
	else fprintf(stderr, " UDP GENERATOR -ver pktstosend\n");
#endif
//#endif
	} else {
		while(sent<resp_size_int) {
#ifndef NOPRINTFS
			fprintf(stderr, "sent = %d resp_size_int=%d\n", sent, resp_size_int);
#endif
	//		if(sent<LINESIZE) {}
			if(MAGICLINESIZE> PKTSIZE &&  sent<MAGICLINESIZE) {
#ifdef NODELAYHACK
				//r=send(sock,& Inline[sent],min(min( resp_size_int - sent , LINESIZE - sent ), PKTSIZE) , 0);
				r=send(sock,& Inline[sent],min(min( resp_size_int - sent , MAGICLINESIZE - sent ), PKTSIZE) , 0);
#ifndef NOPRINTFS
				fprintf(stderr, " PKTSIZE %d\n", PKTSIZE);
#endif
#else
				//r=send(sock,& Inline[sent],min( resp_size_int - sent , LINESIZE - sent ) , 0);
				r=send(sock,& Inline[sent],min( resp_size_int - sent , MAGICLINESIZE - sent ) , 0);
#endif
			}
			else {
#ifdef NODELAYHACK
				r=send(sock, Inline, min(min(resp_size_int - sent, LINESIZE), PKTSIZE), 0);
#else
				r=send(sock, Inline, min(resp_size_int - sent, LINESIZE), 0);
#endif
			}
			if(r<0) {
				perror("send failed");
				strerror(errno);
				close(sock);
//				fflush(stderr);
				exit(-1);
			}
			sent+=r;
#ifndef NOPRINTFS
			fprintf(stderr, "sent another %d bytes\n", r);
#endif
		}
#ifndef NOPRINTFS
		fprintf(stderr," Send by tcp message %s\n", Inline);
#endif
	}

	if(proto==TCP) return sent;
	else return resp_size_int;
}

/*****/

/*****************************************************************************/
/*
struct key
{
    uint32_t one_ip; uint32_t two_ip; uint16_t one_port; uint16_t two_port;
};

struct value
{
    char *id;
};
*/

 struct timeval tv,sleeptv, newdebugtv;
  fd_set readset, tmpset;
  int result;


struct key
{
	char * class_name;
};

struct value
{
	uint16_t listen_port;
	int numxchng_min, numxchng_max;
	int req_size_min, req_size_max;
	int resp_size_min, resp_size_max;
	int process_min, process_max;
	int think_min, think_max;
	int numconn_min, numconn_max;
	int transport; /* 0-> TCP 1->UDP;*/
	int resp_zipfres;
	int resp_zipf_numtogen, resp_zipf_SEED, resp_zipf_maxfreq;
};

struct vkey
{
	int vn_num;
};

struct vvalue
{
	char * vip;
};

DEFINE_HASHTABLE_INSERT(insert_some, struct key, struct value);
DEFINE_HASHTABLE_SEARCH(search_some, struct key, struct value);
DEFINE_HASHTABLE_REMOVE(remove_some, struct key, struct value);

DEFINE_HASHTABLE_INSERT(vinsert_some, struct vkey, struct vvalue);
DEFINE_HASHTABLE_SEARCH(vsearch_some, struct vkey, struct vvalue);
DEFINE_HASHTABLE_REMOVE(vremove_some, struct vkey, struct vvalue);


/*****************************************************************************/

/*
static unsigned int
hashfromkey(void *ky)
{
    struct key *k = (struct key *)ky;
    return (((k->one_ip << 17) | (k->one_ip >> 15)) ^ k->two_ip) +
            (k->one_port * 17) + (k->two_port * 13 * 29);
}
*/


static unsigned int
hashfromkey(void *ky)
{
	int i,retval;
	struct key * k = (struct key *) ky;
	int len = strlen(k->class_name);
	retval=0;
	for (i=0;i<len;i++)
	{
		retval+=k->class_name[i];
	}
	return retval;
}

static unsigned int
vhashfromkey(void *ky)
{
	return (((struct vkey *) ky )->vn_num)%7;
}
static int
equalkeys(void *k1, void *k2)
{
    return (0 == strcmp((( struct key *) k1)->class_name,(( struct key *)  k2)->class_name));
}

static int
vequalkeys(void *k1, void *k2)
{
	return( 0 == memcmp(k1,k2,sizeof(struct vkey)));
}
/*****************************************************************************/
/****f* Swing/main_generator
 * SOURCE
 */
int
main(int argc, char **argv)
{
	int THISTIME;
	int sock;
	struct sockaddr_in server ;
//int SEED;
	struct timeval thistime, thistimestart;
	int server_len;

	//float rval;
	int retVal,cid;
	//int numxchng_int, numconn_int,  req_size_int, think_int, process_int;
	// int resp_size_int;
	//int max,min;
	int curlength;
	int clientvn;
#ifndef NOPRINTFTIMEOUTPUT
	struct timeval respstarttime, respendtime;
#endif
	struct timeval tv1,  tv2;
    struct key *k;
	int notprintedserveryet;
   struct value *v, *found;
  //  struct hashtable *h;

	struct vkey *vk;
	struct vvalue *vv, *vfound;
	//struct hashtable *vh;
    //struct hashtable_itr *itr;
    //unsigned int i;
	char this_line[TIMECOMMONLINESIZE];
	char class[20];
	int diff;
	int received;
	int client_vn, server_vn, offset;
	//char dummy_class_name[255];
	//int vn_num;
	//char *clnt, *srvr;
	unsigned long dummy_port;
	struct hostent * host_info;
	int gotsize;
	long totreceived, r;
	//FILE * specsfile=fopen(SPECSFILE,"r") ; 
//	FILE * classfile=fopen("/usr/research/home/kvv/failure/mystuff/class.hosts","r");
//	FILE * vipfile=fopen(HOSTSFILE,"r") ; 
    char * thistoken;
	FILE * indifile=fopen(argv[1],"r");
	int  t_to_start=atoi(argv[2]);

	char tosend_msg[LINESIZE];
	long sent,tosend;
	int sock_opt;
	int sock_type;
	//fprintf(stderr, " %s " , SPECSFILE);
	//FILE * errfile=fopen("/home/kvishwanath/Swing/swing_code/errfileconnforks","a");
	FILE * errfile=fopen(ERRFILECONNFORKS,"a");
	
	int flowid=0;
	int s ; 
	float SWINGPROJECT_REQTHINK_TIMES;
	float SWINGPROJECT_REQTHINK_ADD;
	float SWINGPROJECT_REQTHINK_FACTOR;
	float SWINGPROJECT_INTERCONN_TIMES;
	float SWINGPROJECT_INTERCONN_ADD;
	float SWINGPROJECT_INTERCONN_FACTOR;


	float SWINGPROJECT_INTERRRE_TIMES;
	float SWINGPROJECT_INTERRRE_ADD;
	float SWINGPROJECT_INTERRRE_FACTOR;

#ifdef NEWPRINTFS
	
	int thissess=0;
	int sessover=0;
	
#endif 
	int connforkpid, pid2fork;
#ifdef DEBUG_SLOWNESS
	struct timeval slowtv2, slowtv1;
#endif

	//fprintf(stderr, " heare \n");
	if(argc<3)
	{
		Usage(argv);
	}
	if(argc>=4) PKTSIZE=atoi(argv[3]) ;
	else PKTSIZE=PKTSIZE_DEFAULT;
	PKTSIZE=min(PKTSIZE, PKTSIZE_DEFAULT);
	if(argc>=5) {
			EXTRACT_TIMES_AND_ADD(argv[4], SWINGPROJECT_REQTHINK_TIMES, SWINGPROJECT_REQTHINK_ADD, SWINGPROJECT_REQTHINK_FACTOR);
			//fprintf(stderr, " the input %s times %f add %f \n", argv[4], SWINGPROJECT_REQTHINK_TIMES, SWINGPROJECT_REQTHINK_ADD);
			//SWINGPROJECT_REQTHINK_TIMES=atof(argv[4]);
	} else  {
		SWINGPROJECT_REQTHINK_TIMES=-1;
	}

	if(argc>=6) {
			EXTRACT_TIMES_AND_ADD(argv[5], SWINGPROJECT_INTERCONN_TIMES, SWINGPROJECT_INTERCONN_ADD, SWINGPROJECT_INTERCONN_FACTOR);
			//SWINGPROJECT_INTERCONN_TIMES=atof(argv[5]);
			//fprintf(stderr, " the input %s times %f add %f \n", argv[5], SWINGPROJECT_INTERCONN_TIMES, SWINGPROJECT_INTERCONN_ADD);
	} else SWINGPROJECT_INTERCONN_TIMES=-1;

	if(argc>=7) {
			EXTRACT_TIMES_AND_ADD(argv[6], SWINGPROJECT_INTERRRE_TIMES, SWINGPROJECT_INTERRRE_ADD, SWINGPROJECT_INTERRRE_FACTOR);
	} else SWINGPROJECT_INTERRRE_TIMES=-1;
	
#if DONTNEETD2/*{{{*/
    h = create_hashtable(160, 0.75, hashfromkey, equalkeys);
	vh=create_hashtable(160,0.75,vhashfromkey,vequalkeys);
//    if (NULL == h) { perror("could not make hashtable "); fflush(stderr) ; exit(-1) ;} /*oom*/
//	if (NULL == vh){ perror("could not make hashtable "); fflush(stderr);  exit(-1) ;} /* oom */

	//SEED=atoi(argv[2]);
	found=NULL;
	vfound=NULL;
	k=NULL;
	v=NULL;
	vk=NULL;
	vv=NULL;

	memset(tosend_msg,'1', LINESIZE);
	clnt=(char * ) malloc(sizeof(char)*80);
	srvr=(char * ) malloc(sizeof(char)*80);
//	if(clnt==NULL) { perror("out of memory"); fflush(stderr) ; exit(-1);}
//	if(srvr==NULL) { perror("out of memory"); fflush(stderr) ; exit(-1);}
	while(fgets(this_line,80,specsfile))
	{
		if(strncmp(this_line, "class", 5)==0)
		{
			k = (struct key *) malloc(sizeof(struct key ));
			v = (struct value *) malloc (sizeof(struct value));
			v->transport=0;
			v->resp_zipfres=0;
			k->class_name = (char *)
				malloc(sizeof(char)*strlen(&this_line[6]) +1 );
			memset(k->class_name,0x0,1);
			sscanf(&(this_line[6]),"%s",k->class_name);
			
		}
		else if (strncmp(this_line,"port=",5)==0)
		{
			sscanf(&(this_line[5]),"%lu",&dummy_port);
			v->listen_port=dummy_port;
//			if(!insert_some(h,k,v)) { perror("could not insert in hashtable ") ; fflush(stderr) ; exit(-1); }
			if(NULL==(found=search_some(h,k))){
				printf("did not find it\n");
			}
			else {
				//printf("class %s listens on port %d numxchng_max %d\n",k->class_name, found->listen_port, found->numxchng_max);
			}
		}
		else if (strncmp(this_line, "numxchng",8)==0)
		{
			sscanf(this_line,"numxchng min %d max %d", &v->numxchng_min, &v->numxchng_max);
		}
		else if (strncmp(this_line,"req",3)==0)
		{
			sscanf(this_line,"req min %d max %d", &v->req_size_min, &v->req_size_max);
		}
		else if (strncmp(this_line,"process",7)==0)
		{
			sscanf(this_line,"process min %d max %d", &v->process_min, &v->process_max);
		}
		else if (strncmp(this_line,"resp",4)==0)
		{

		    if(strncmp(&this_line[5],"min",3)==0)
				sscanf(this_line,"resp min %d max %d",&v->resp_size_min, &v->resp_size_max);
			else if (strncmp(&this_line[5],"zipf",4)==0)
			{
				v->resp_zipfres=1;
				sscanf(this_line,"resp zipf maxfreq %d numtogen %d SEED %d", &v->resp_zipf_maxfreq, &v->resp_zipf_numtogen , &v->resp_zipf_SEED);
			}
		}
		else if(strncmp(this_line,"think",5)==0)
		{
			sscanf(this_line,"think min %d max %d", &v->think_min, &v->think_max);
		}
		else if(strncmp(this_line, "conn", 4)==0)
		{
			sscanf(this_line, "conn min %d max %d",&v->numconn_min, &v->numconn_max);
		}
		else if(strncmp(this_line,"transport",9)==0)
		{
			sscanf(this_line, "transport %d",&v->transport);
		}
	}
	while(fgets(this_line,80,vipfile))
	{
		if(strncmp(this_line,"client",5)==0)
		{
			vk = (struct vkey *) malloc(sizeof(struct vkey ));
			vv = (struct vvalue *) malloc(sizeof(struct vvalue));
			vv->vip=(char*)malloc(sizeof(char)*16);
			sscanf(this_line, "client%d %s # on unknown", &vk->vn_num,vv->vip);
//			if(!vinsert_some(vh,vk,vv)) { perror("could not insert in hashtable");fflush(stderr);  exit(-1); }
			if (NULL==(vfound=vsearch_some(vh,vk))){
				printf("did not find it\n");
			}
			else
			{
				//printf("vip of vn %d is %s \n", vk->vn_num, vv->vip);
			}
		}
	}

	gettimeofday(&tv, NULL);
	k=(struct key *) malloc(sizeof(struct key));
	vk=(struct vkey *) malloc(sizeof(struct vkey));
#endif/*}}}*/

	signal(SIGCHLD, SIG_IGN);
	/* wait till the time to start */
	{
		struct timeval tv_st;
		gettimeofday(&tv_st, NULL);
		while ( tv_st.tv_sec < t_to_start ) 
		{
			gettimeofday(&tv_st, NULL);
			sleep(1);
			
		}
	//	fprintf(stderr, " Starting at gettimeofday %ld\n", tv_st.tv_sec);

	}
		gettimeofday(&thistimestart,NULL);
	while(fgets(this_line,TIMECOMMONLINESIZE - 2 ,indifile)) {
#ifndef NOPRINTFS
		fprintf(stderr,"%s", this_line);
		fprintf(stderr,"length is %d limit is %d\n", strlen(this_line), TIMECOMMONLINESIZE);

#endif
	//   fprintf(stderr, "%s", this_line);
		if(strlen(this_line)>TIMECOMMONLINESIZE) {
			fprintf(stderr,"too big a line \n");
			fprintf(stderr, "%s", this_line);
			fprintf(stderr, "next line \n");
			goto getnextline;
		}
//		//fflush(stderr);
		THISTIME=0;
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv2,NULL);
			fprintf(stderr, " read line  %ld %ld \n", slowtv2.tv_sec, slowtv2.tv_usec);
#endif
	//	if((pid2fork=fork())==0) {}
			/* this is the child */
			thistoken = strtok(this_line," ");
			if(strncmp(thistoken,"GENERATE",8)!=0) goto getnextline;
			if(thistoken==NULL) goto getnextline;
				/* thistoken has GENERATE as of now */

				thistoken=strtok(NULL," "); thistoken=strtok(NULL," ");
				notprintedserveryet=1;
#ifndef NOPRINTFS
				fprintf(stderr, "sessid %d ", atoi(thistoken));
#endif
				/* this is the sessid */

				thistoken=strtok(NULL," ");
				/* this now has the class */

				thistoken=strtok(NULL, " "); thistoken=strtok(NULL, " ");
#ifndef NOPRINTFS
				fprintf(stderr, "client %d ", atoi(thistoken));
#endif
				clientvn=atoi(thistoken);
				/* this has the client vn */

				thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); offset=atoi(thistoken);
				/* this has the time */
				gettimeofday(&thistime, NULL);
				while((thistime.tv_sec - thistimestart.tv_sec + ( thistime.tv_usec - thistimestart.tv_usec )/1000000) < offset/TIMESCALEFACTOR)
				{
					//sleep(1);
					sleeptv.tv_sec=0;
					sleeptv.tv_usec=1000;
					select(0,NULL,NULL,NULL,&sleeptv);
					gettimeofday(&thistime, NULL);
				}
				
#ifdef DEBUG_SLOWNESS
			gettimeofday(&thistime, NULL);
			fprintf(stderr, " before fork %ld %ld \n", thistime.tv_sec, thistime.tv_usec);
#endif
				
		if((pid2fork=fork())==0) {
			/* this is the child */
#ifdef DEBUG_SLOWNESS
			gettimeofday(&thistime, NULL);
			fprintf(stderr, " new process %ld %ld \n", thistime.tv_sec, thistime.tv_usec);
#endif

				thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); numrre=atoi(thistoken);
#ifndef NOPRINTFS
				fprintf(stderr,"numrre %d ", numrre);
#endif
				/* this has numrre */

				for(rretimes=0;rretimes<numrre;rretimes++) {
#ifdef DEBUG
					fprintf(stderr, " RRE=%d\n", rretimes);
#endif
#ifdef MACRODEBUGS
					fprintf(stderr, " RRE=%d out of %d\n", rretimes, numrre);
#endif
					if(rretimes >0 ) {
						/* sleep interRRE msecs */
						/* keep doing thistoken till you hit interRRE */
						while(strcmp(thistoken,"interRRE")!=0) {
							thistoken=strtok(NULL," ");
						}
						thistoken=strtok(NULL," "); interRRE=atoi(thistoken);
						interRRE=(interRRE<0?0:interRRE);
						/* TODO INTERRRE */

							if(SWINGPROJECT_INTERRRE_TIMES!=-1)
							{
								interRRE=interRRE*SWINGPROJECT_INTERRRE_TIMES;
								interRRE=interRRE+SWINGPROJECT_INTERRRE_ADD;
							}
	//					interRRE=interRRE*SWINGPROJECT_INTERRRE_TIMES;
#ifdef MACRODEBUGS
						fprintf(stderr, "interRRE %d msec\n", interRRE);
#endif
						/* this has interRRE time */

						thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); rrenum=atoi(thistoken);
						/* this has rrenum */
#ifndef NOPRINTFS
						fprintf(stderr, "interRRE %d RRE %d ",interRRE,  rrenum);
#endif
#ifdef DEBUG_SLOWNESS
						gettimeofday(&slowtv2,NULL);
						fprintf(stderr, "before sleep %d secs  %ld %ld \n", interRRE, slowtv2.tv_sec, slowtv2.tv_usec);
#endif
						if(interRRE>0) {
							/* TIMESCALEFACTOR is 1000 as the sleep time will be in milliseconds as we read it from a file */
							sleeptv.tv_sec=(long)(interRRE/TIMESCALEFACTOR);
							sleeptv.tv_usec = (long)(1000000*(1.0*interRRE/TIMESCALEFACTOR - sleeptv.tv_sec));
							//sleeptv.tv_sec=(long)(interRRE);
							//sleeptv.tv_usec = (long)(1000000*(1.0*interRRE - sleeptv.tv_sec));
#ifdef MACRODEBUGS
							fprintf(stderr, " select sleep %ld %ld \n", sleeptv.tv_sec, sleeptv.tv_usec);
#endif
							select(0,NULL,NULL,NULL,&sleeptv);
							//	sleep(interRRE);
						}
#ifdef DEBUG_SLOWNESS
						gettimeofday(&slowtv2,NULL);
						fprintf(stderr, "after sleep  %ld %ld \n", interRRE, slowtv2.tv_sec, slowtv2.tv_usec);
#endif
					} /* if rretimes > 0 */
					else {

						thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); rrenum=atoi(thistoken);
						/* this has rrenum */
#ifndef NOPRINTFS
		 				fprintf(stderr, "RRE %d ", rrenum);
#endif 
					} /* if rretimes > 0 . else */

					thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); numconn=atoi(thistoken);
					/* this has numconn */
#ifndef NOPRINTFS
					fprintf(stderr,"NUMCONN %d ", numconn);
#endif
				//	if(numconn>2) {
				//		printf("error in numconn\n");
				//		exit(-1);
				//	}
					for(conntimes=0; conntimes< numconn; conntimes++){

#ifdef DEBUG
						fprintf(stderr, " conntimes %d \n", conntimes);
#endif
#ifdef MACRODEBUGS
						fprintf(stderr, " this conn %d out of %d \n", conntimes, numconn);
#endif
						/* probably fork off for each conn and in the end the parent simply waits for all
						   the conns to be over 
						   When you fork off then how to read in info pertainging to you ?
						   One option is that since you know the value of conntimes when you fork off, in
						   the child. do a strtok so many times for CONN
						 */
						if(conntimes>0) {
							/* sleep interconn msecs */
							/* read the interconn */
							
							while(strcmp(thistoken,"interconn")!=0) {
								thistoken=strtok(NULL," ");
							}
							thistoken=strtok(NULL," "); interconn=atoi(thistoken);
							interconn=(interconn<0?0:interconn);
							if(SWINGPROJECT_INTERCONN_TIMES!=-1)
							{
								interconn=interconn*SWINGPROJECT_INTERCONN_TIMES;
								interconn=interconn+SWINGPROJECT_INTERCONN_ADD;
							}
							/* this has interRRE time */
							//thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); 
							//rrenum=atoi(thistoken);
#ifdef DEBUG
							fprintf(stderr, " interconn %d\n", interconn);
#endif
							/* this has rrenum */
							if(interconn>0) {
#ifdef DEBUG
								fprintf(stderr, " sleep %d msec\n", interconn);
#endif
								/* TIMESCALEFACTOR is 1000 as the sleep time will be in milliseconds as we read it from a file */
								sleeptv.tv_sec=(long)(interconn/TIMESCALEFACTOR);
								sleeptv.tv_usec = (long)(1000000*(1.0*interconn/TIMESCALEFACTOR - sleeptv.tv_sec));
								//sleeptv.tv_sec=(long)(interRRE);
								//sleeptv.tv_usec = (long)(1000000*(1.0*interRRE - sleeptv.tv_sec));
								select(0,NULL,NULL,NULL,&sleeptv);
								//	sleep(interRRE);
							}
						}
#ifndef HACK_CONN1
						if((connforkpid=fork())==0) {
#endif
#ifdef DEBUG
							fprintf(stderr, " start conn \n");
#endif
#ifdef NEWPRINTFS
							fprintf(stderr, " start conn num %d \n", conntimes);
#endif
#ifndef NOPRINTFS
							fprintf(stderr, " for conn %d \n", conntimes+1);
#endif
							skiptimes=0;
#ifndef HACK_CONN1
							while(skiptimes<=conntimes) {
#else
							while(skiptimes<=0) {
#endif
								
								thistoken=strtok(NULL," ");
								if(strcmp(thistoken,"CONN")==0) {
									skiptimes++;
									thistoken=strtok(NULL," "); connnum=atoi(thistoken);
									/* this has conn num */
								}
							}
#ifdef HACK_CONN1
						if((connforkpid=fork())==0) {
#endif
#ifndef NOPRINTFS
							fprintf(stderr, "got conn num %d ", connnum);
#endif
							/* this is for a conn */

							thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); vnservernum=strdup(thistoken);
							
							/* this has server num */

							if(rretimes==0 && conntimes==0) {
								if(notprintedserveryet==1)
								{
									notprintedserveryet=0;
									//gettimeofday(&cdfstarttime,NULL);
	//								fprintf(stderr, "CDFstats: client %d server %s %ld %ld\n", clientvn, vnservernum, cdfstarttime.tv_sec, cdfstarttime.tv_usec);
								}
							}
							
							thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); portnum=atoi(thistoken);
							/* this has port num */

							thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); numreq=atoi(thistoken);
							/* this has numreq */
	//						fprintf(stderr, " numreq %d\n", numreq);

							thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); transport=atoi(thistoken);
							/* this has transport */
							if(transport==1) transport=TCP; else transport=UDP;
#ifndef NOPRINTFS
							fprintf(stderr,"CONN %d SERVER %s port %d numpairs %d TRANSPORT %d ", connnum, vnservernum,portnum,  numreq,transport);
#endif
#ifdef NEWPRINTFS
							fprintf(stderr,"CONN %d SERVER %s port %d numpairs %d TRANSPORT %d ", connnum, vnservernum,portnum,  numreq,transport);
#endif
							/* make the conn here */
							if(transport==UDP) sock_type=SOCK_DGRAM ;
							else sock_type=SOCK_STREAM;

tryagain:
							/* open socket */
							sock = socket(AF_INET, sock_type,0);
							if (sock < 0)
							{
								fprintf(stderr," %s : ", vnservernum);
								perror("could not create stream socket");
//								fflush(stderr);
								exit(-1);
							}


/* set sockopt reuseaddr */
							sock_opt = 1;
							if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&sock_opt,
										sizeof (sock_opt)) == -1) {
								perror("setsockopt(SO_REUSEADDR)");
								(void) close(sock);
//								fflush(stderr);
								exit(-1);
							}
							/* tcpnodelay and mss hack */
if(sock_type==SOCK_STREAM)
{
#ifdef NODELAYHACK
							if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &tcpnodelayflag, sizeof(int))==-1) {
								perror("Setsockopt(TCP_NODELAY)");
//								fflush(stderr);
								exit(-1);
							}
#endif
							/* mss hack */
#ifdef MSSHACK
	sock_opt = PKTSIZE+12;
	if (setsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, (void *)&sock_opt,
				sizeof (sock_opt)) < 0) {
		perror("setsockopt(TCP_MAXSEG)");
		(void) close(sock);
//		fflush(stderr);
		exit(-1);
	}
#endif

}

					/* connect to the client and prepare */
							server.sin_family=AF_INET;
							server.sin_port=htons(portnum);
							memset(&(server.sin_zero), '\0', 8);
							//fprintf(stderr, "%s server \n", vnservernum);
							host_info=gethostbyname(vnservernum); 

							if(host_info==NULL)
							{
								fprintf(stderr," %s : ", vnservernum);
								perror("unkown host");
//								fflush(stderr);
								exit(2);
							}

							server.sin_family=host_info->h_addrtype;
							memcpy((char *) & server.sin_addr, host_info->h_addr, host_info->h_length);
							server_len=sizeof(server);
#ifndef NOPRINTFS
							fprintf(stderr, "before connect \n");
//		//					fflush(stderr);
#endif
							if(sock_type==SOCK_STREAM) {
								errno=0;
								if (connect(sock,(struct sockaddr *) & server , sizeof (server))<0)
								{
									//{ 
									//	char  thisstring[50];
									//	memset(thisstring, '\0', 50);
									//	strcat(thisstring, vnservernum);
									//	strcat(thisstring, ": connecting to the server") ;
									//	perror(thisstring);
								//	}
									if(errno==ECONNREFUSED) {
										/* even though code is in place below but
										   for now exit i.e. dont make 20 tries/200 secs etc.
										 */
#if 0
										trial++;
										if(trial<=20) 
										{
											fprintf(stderr,"too many open file, trying again trial %d\n",trial);
											sleep(10);
											goto tryagain;
										}
										else {
											fprintf(stderr,"too many tries to connect... now giving up\n");
											close(sock);
											exit(-1);
										}
#endif
										perror("conn refused");
									}
									
								//	fprintf(stderr,"errno is %d\n", errno);
//								//	fflush(stderr);
									else perror("connect to server");
									fprintf(stderr," server was %s ", vnservernum);
									close(sock);
//									fflush(stderr);
									exit(-1);
								}
#ifndef NOPRINTFS
								fprintf(stderr, "connected on sock %d\n", sock);
#endif
#ifdef NEWPRINTFS
							fprintf(stderr, " connected conn num %d \n", conntimes);
#endif
								/* you are connected now for TCP*/
							}
							/* loop for rre */	
							for(times=0;times<numreq;times++) {
								/* for each pair do the REQ/RSP */
#ifdef MACRODEBUGS
								fprintf(stderr, " numpair %d out of %d for conn %d of RRE %d\n", times, numreq, conntimes, rrenum);
#endif
#ifdef NEWPRINTFS
								fprintf(stderr, " times = %d out of %d for con %d\n", times, numreq, connnum);
#endif
#ifndef NOPRINTFS
								gettimeofday(&newdebugtv,NULL);
								fprintf(stderr, " numreq = %d out of %d for  conn %d tm %ld %ld ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n", times, numreq, connnum, newdebugtv.tv_sec, newdebugtv.tv_usec);
#endif
#ifdef NEWPRINTFS
								if(numreq>2000)
								{
									fprintf(stderr, " numreq = %d out of %d^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n", times, numreq);
//									fflush(stderr);
								}

#endif
								if(times>0) {
									thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); reqthink=atoi(thistoken);
									/* this has reqthink */
									if(SWINGPROJECT_REQTHINK_TIMES!=-1)
									{
										reqthink=reqthink * SWINGPROJECT_REQTHINK_TIMES;
										reqthink=reqthink + SWINGPROJECT_REQTHINK_ADD;
									}
#ifndef NOPRINTFS
									fprintf(stderr,"reqthink %d usleep ", reqthink );
#endif
									usleep((unsigned int) reqthink*1000); /* usleep of reqthink by user */
								}
								if(sock_type==SOCK_STREAM) {
									thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); REQ=atoi(thistoken);
									this_req.bytes=REQ;
								/* this has REQ */
								}
								else {
									server.sin_port=htons(portnum);
									thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); this_req.bytes=atoi(thistoken);
									thistoken=strtok(NULL," "); this_req.pkts=atoi(thistoken);
									thistoken=strtok(NULL," "); this_req.duration=atoi(thistoken);
									/* this has udp_req pair */
#ifdef NEWPRINTFS
									fprintf(stderr, " %d %d %d \n", this_req.bytes, this_req.pkts, this_req.duration);
#endif
									REQ=this_req.bytes;
								}
								

								thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); serverthink=atoi(thistoken);
								/* this has serverthink */

								if(sock_type==SOCK_STREAM) {
									thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); RSP=atoi(thistoken);
									this_rsp.bytes=RSP;
								/* this has RSP */
								} else {
									thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); this_rsp.bytes=atoi(thistoken);
									thistoken=strtok(NULL," "); this_rsp.pkts=atoi(thistoken);
									thistoken=strtok(NULL," "); this_rsp.duration=atoi(thistoken);
									/* this has udp_rsp pair */
									RSP=this_rsp.bytes;
#ifdef NEWPRINTFS
									fprintf(stderr, " %d %d %d \n", this_rsp.bytes, this_rsp.pkts, this_rsp.duration);
#endif

								}
#ifdef REQRSPSIZE100
								REQ=100 ; RSP=100;
#endif

								thistoken=strtok(NULL," "); thistoken=strtok(NULL," "); brate=atoi(thistoken);
								/* this has brate */
#ifndef NOPRINTFS
								fprintf(stderr,"REQ %ld serverthink %ld RSP %ld brate %ld ", REQ,serverthink,  RSP, brate);
//								fflush(stderr);
#endif
#ifdef DEBUG_REQRSPSIZE
								DEBUG_REQRSPSIZE_REQ_original=REQ;
								DEBUG_REQRSPSIZE_RSP_original=RSP;
#endif
								//RSP=max(RSP,1);
								/* make the message to send */
								memset(tosendmessage,'1',LINESIZE);
								tosendmessage[LINESIZE-1]='\0';
								if(times==numreq-1) {
									if(sock_type==SOCK_STREAM)
									sprintf(tosendmessage,"R %ld %ld %ld %ld close %s",REQ,serverthink,RSP,brate,TERM_STRING);
									else
									sprintf(tosendmessage,"R %ld %d %d %ld %ld %d %d %ld close %s",this_req.bytes, this_req.pkts, this_req.duration,serverthink,this_rsp.bytes, this_rsp.pkts, this_rsp.duration,brate,TERM_STRING);
								}
								else {
	//								sprintf(tosendmessage,"REQ %ld serverthink %ld RSP %ld brate %ld %s",REQ,serverthink,RSP,brate, TERM_STRING);
									if(sock_type==SOCK_STREAM)
									sprintf(tosendmessage,"R %ld %ld %ld %ld %s",REQ,serverthink,RSP,brate, TERM_STRING);
									else
									sprintf(tosendmessage,"R %ld %d %d %ld %ld %d %d %ld %s",this_req.bytes, this_req.pkts, this_req.duration, serverthink,this_rsp.bytes, this_rsp.pkts, this_rsp.duration,brate, TERM_STRING);

								}
								wantreq=REQ;
								wantrsp=RSP;
								//fprintf(stderr, "RSPtest %ld\n", RSP);
								//fprintf(stderr, " times = %d numreq-1 = %d sent %s \n",times, numreq-1,  tosendmessage);
								curlength=strlen(tosendmessage);
#ifndef NOPRINTFS
								fprintf(stderr,"\nsending message %s %d bytes \n", tosendmessage, curlength);
#endif
								/* handle the REQ=0 case later */
								if(REQ<LINESIZE) {
									if(REQ>curlength) {
#ifndef NOPRINTFS
										fprintf(stderr, " REQ > curlength\n");
#endif
										tosendmessage[curlength]='1';
										tosendmessage[REQ]='\0';

#ifndef NOPRINTFS
										fprintf(stderr, " hence tosendmessage = %s<-\n",tosendmessage);
#endif
									} else {
#ifndef NOPRINTFS
										fprintf(stderr, " REQ <= curlength\n");
#endif
	//									tosendmessage[REQ]='\0';
										if(sock_type!=SOCK_STREAM)
										{
										tosendmessage[curlength]='\0';
										REQ=curlength;

#ifndef NOPRINTFS
										fprintf(stderr, "changinre REQ to %ld\n", REQ);
#endif
										}
										/* encode response size in 1 byte */
										else{
										
										RSP2=(long)(1.0*log(RSP*1.0)/log(2.0)+0.001);
										/* the above +0.001 is just a hack to ensure that
										   truncation does now do weird things
										   */
										if(( RSP -(pow(2.0,RSP2*1.0)  ) ) > MAXTOLERATESIZEDIFF ) {
											/* no alteration will be done in thise case
											   as the potential loss of response size will be
											   more than the max I am willing to tolerate.
											   */
											fprintf(stderr, " more than MAXTOLERATESIZEDIFF \n");
											sprintf(tosendmessage,"%ld \0", RSP);
											REQ=strlen(tosendmessage);
#ifndef NOPRINTFS
	//										fprintf(stderr, " tosendmessage hack  %s\n", tosendmessage);
#endif
										}
										else{
#ifndef NOPRINTFS
												fprintf(stderr, " REQ %ld RSP %ld RSP2 %ld, log_2(%ld)=%f, long->%ld\n", REQ, RSP, RSP2, RSP, log(RSP*1.0)/log(2.0), (long)(log(RSP*1.0)/log(2.0)));
#endif
											//sprintf(tosendmessage,"%c R %ld %ld %ld %ld %s",(int)(log(1.0*RSP)/log(2.0))+'a', REQ,serverthink,RSP,brate, TERM_STRING);
											sprintf(tosendmessage,"%c R %ld %ld %ld %ld %s",(int)RSP2+'a', REQ,serverthink,RSP,brate, TERM_STRING);
	//										fprintf(stderr, " tosendmessage hack  %s\n", tosendmessage);
											tosendmessage[REQ]='\0';
#ifndef NOPRINTFS
	//										fprintf(stderr, " tosendmessage hack \n");
											fprintf(stderr, " tosendmessage hack  ::%s:: loss of %ld\n", tosendmessage, RSP - ( (long)(((pow(2.0, 1.0*RSP2)) + (pow(2.0,1.0*RSP2 +1)))/2  )));
		//									fprintf(stderr, "loss of %ld\n", RSP - ( (long)(((pow(2.0, 1.0*RSP2)) + (pow(2.0,1.0*RSP2 +1)))/2  )));
#endif
								//			RSP=(long)(log(RSP*1.0)/log(2.0));
								//			RSP=RSP2;
											RSP=(long)(((pow(2.0, 1.0*RSP2)) + (pow(2.0,1.0*RSP2 +1)))/2  );
										}
	//									RSP*=5;
										
										}
									}
								} else {
									tosendmessage[curlength]='1';
								}
#ifdef NEWPRINTFS
								fprintf(stderr, " to send message %s \n", tosendmessage);
#endif

#ifndef NOPRINTFS						
								fprintf(stderr, " REQ here is %ld\n", REQ);
#endif
								/* send the message */
								sent=0;
								tosend=REQ;
#ifdef NEWPRINTFS
								fprintf(stderr, " send %d bytes \n", REQ);
#endif
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv2,NULL);
			fprintf(stderr, " before send  %ld %ld \n", slowtv2.tv_sec, slowtv2.tv_usec);
#endif
								/*sent=REQ; */
								/* for testing. skipping */
								while(sent<tosend) {
#ifndef NOPRINTFS
									fprintf(stderr, " sock = %d  tosendmessage= %s brate=%d REQ=%ld transport=%d ", sock, tosendmessage, brate, REQ, transport);
#endif
									sent+=sendmessage(sock,tosendmessage, brate, REQ,(transport==TCP ? TCP:UDP), (struct sockaddr *)&  server , server_len, this_req, this_rsp );
									if(sent<LINESIZE) {
										memset(tosendmessage,'1',LINESIZE);
										tosendmessage[LINESIZE-1]='\0';
										if((sent+LINESIZE-1) > tosend) {
											tosendmessage[tosend - sent ]='\0';
										}
									}
#ifdef NEWPRINTFS
									fprintf(stderr, " sent everything %d bytes\n", sent);
#endif
									//exit(1);
#ifndef NOPRINTFS
								fprintf(stderr, "sent  bytes %ld to server by now  \n", sent);
#endif
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv2,NULL);
			fprintf(stderr, "sent  %ld %ld \n", slowtv2.tv_sec, slowtv2.tv_usec);
#endif
								if(transport==UDP) goto skipthistest;
								}
skipthistest:
#ifndef NOPRINTFS
								fprintf(stderr, "sent total bytes %ld to server \n", sent);
#endif
#ifdef DEBUG_REQRSPSIZE
								DEBUG_REQRSPSIZE_REQ_sent=sent;
#endif

								/* get the response */
#if 0
								if(transport==UDP) {
									int temp=0;


									
									fprintf(stderr, "UDPss\n");
//									fflush(stderr);
									if((temp=recvfrom(sock,getmessage, LINESIZE, 0, (struct sockaddr *) & server,& server_len))<0)
									{
										perror("error in recvfrom");
//										fflush(stderr);
										exit(-1);
									} else {
										getmessage[temp]='\0';
										fprintf(stderr, "received %s \n", getmessage);
//										fflush(stderr);
									}
								}
								else{
									fprintf(stderr, "TCP\n");
//									fflush(stderr);
#endif
#ifndef NOPRINTFTIMEOUTPUT
									gettimeofday(&respstarttime, NULL);
#endif
									/* if RSP is 0 then dont wait for RSP */
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv2,NULL);
			fprintf(stderr, "before response %ld %ld \n", slowtv2.tv_sec, slowtv2.tv_usec);
#endif
									if(RSP>0)
									RSP=getresponse(sock,getmessage, (transport==TCP?TCP:UDP), (struct sockaddr *) & server, server_len, RSP);
#ifdef DEBUG_REQRSPSIZE
									DEBUG_REQRSPSIZE_RSP_sent=RSP;
#endif
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv2,NULL);
			fprintf(stderr, "after response  %ld %ld \n", slowtv2.tv_sec, slowtv2.tv_usec);
#endif
#ifndef NOPRINTFTIMEOUTPUT
									gettimeofday(&respendtime, NULL);
#endif
	//							}
#ifdef NEWPRINTFS
									fprintf(stderr, "DEBUGresp: %d %d secs expected %d %d secs \n", RSP, respendtime.tv_sec - respstarttime.tv_sec, this_rsp.bytes, this_rsp.duration);
#endif
#ifndef NOPRINTFTIMEOUTPUT
								//fprintf(stderr, " got response %ld bytes %ld %ld %ld %ld req %ld client %d server %s \n", RSP, respstarttime.tv_sec, respstarttime.tv_usec, respendtime.tv_sec, respendtime.tv_usec, REQ, clientvn, vnservernum);
			//					fprintf(stderr, " got response %ld bytes %ld %ld %ld %ld req %ld client %d server %s \n", RSP, respstarttime.tv_sec, respstarttime.tv_usec, respendtime.tv_sec, respendtime.tv_usec,sent, clientvn, vnservernum);
#else
								fprintf(stderr, " got response %ld bytes client %d server %s \n", RSP, clientvn,vnservernum);
#endif
#ifdef MINIMALPRINTOUT
								fprintf(stderr, " got response %ld bytes %ld %ld %ld %ld req %ld client %d server %s \n", RSP, respstarttime.tv_sec, respstarttime.tv_usec, respendtime.tv_sec, respendtime.tv_usec,sent, clientvn, vnservernum);
#endif

//		//						fflush(stderr);
#ifndef NOPRINTFS
								fprintf(stderr, "match req %ld %ld rsp %ld %ld \n", wantreq, sent, wantrsp, RSP);
								//sleep(5);
#endif
								
#ifdef DEBUG_REQRSPSIZE
								fprintf(stderr, "DEBUGREQRSPSIZE: original/actual REQ: %ld %ld RSP: %ld %ld\n", DEBUG_REQRSPSIZE_REQ_original, DEBUG_REQRSPSIZE_REQ_sent, DEBUG_REQRSPSIZE_RSP_original, DEBUG_REQRSPSIZE_RSP_sent);
#endif
								free(vnservernum);
							} /* for loop num of req/resp per conn */

#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv2,NULL);
			fprintf(stderr, "before breaking this conn  %ld %ld \n", slowtv2.tv_sec, slowtv2.tv_usec);
#endif
							/* break the conn here */
#ifndef NOPRINTFS
							fprintf(stderr, "breaking sock conn %d\n", sock);
#endif
							/* At this stage the condition is the we have received as many bytes as required and in case of UDP we have timed out... */
#ifdef NEWPRINTFS
							fprintf(stderr, " closing sock conn %d\n", conntimes);
#endif
							close(sock);
							/* this child conn should exit */
#ifndef NOPRINTFS
							fprintf(stderr," conn %d over so exiting \n", connnum);
#endif
//								fflush(stderr);
								/*free everything */
	//							free(clnt);
	//							free(srvr);
							exit(0);
						}/* for conn. fork =0 */
						else {
							if(connforkpid<1)
							{
								fprintf(stderr, "could not create new conns\n");
								perror("new conn");
//								fflush(stderr);
								char hname[200];
								gethostname(hname,199);
								fprintf(errfile," conforkerr %d %s\n", errno, hname);
//								fflush(errfile);
								exit(-1);
							}
								
							/* nothing */
						}/* for conn. fork =0. else */

					} /* for loop conntimes */
#ifdef NEWPRINTFS
					fprintf(stderr, "all conns started at least \n");
//					fflush(stderr);
#endif

					while((cid=wait(&retVal))!=-1);
#ifdef NEWPRINTFS
					fprintf(stderr, "all conns joined \n");
//					fflush(stderr);
#endif
					/* wait for all conns to be over */

#ifdef MACRODEBUGS
					fprintf(stderr, " RREOVER=%d out of %d\n", rretimes, numrre);
#endif
				}/* for loop rretimes */
#ifdef NEWPRINTFS
				fprintf(stderr, "rre over \n");
//				fflush(stderr);
#endif
				//{} /* if (thistoken!=NULL) */
#ifndef NOPRINTFS
			fprintf(stderr, "\n");
#endif
	//		fprintf(stderr,"processing...\n");
			//sleep(5);
			//goto processit;

#ifndef NOPRINTFS
			fprintf(stderr, "done \n");
#endif
//								fflush(stderr);
#ifdef NEWPRINTFS
								fprintf(stderr, "this sess over %d \n", thissess);
//								fflush(stderr);
#endif
								/* fre everything */
	//							free(clnt);
	//							free(srvr);
							//	fprintf(stderr, "this sess over \n");
//								fflush(stderr);
								fclose(indifile);
								fclose(errfile);
			exit(0);

		} /* fork()==0 */
		else {/*{{{*/
			if(pid2fork<1) {
				fprintf(stderr, " could not fork off child ");
				perror(" big fork error");
//				fflush(stderr);
				char hname[200];
				gethostname(hname,199);
				fprintf(errfile," pid2fork %d %s\n", errno, hname);
//				fflush(errfile);
				exit(0);
			}
#ifdef NEWPRINTFS
			fprintf(stderr, " started sess %d \n", thissess++);
//			fflush(stderr);
#endif
			/* this is the parent */
			goto getnextline;
		} /* fork ()==0 . else *//*}}}*/
getnextline:
sleep(0);
	}/* while fgets */
#ifdef NEWPRINTFS
	fprintf(stderr, "all sess for this vn  %d started at least \n", thissess);
//	fflush(stderr);
#endif

	while((cid=wait(&retVal))!=-1){
#ifndef NOPRINTFS
		fprintf(stderr, "waiting for forked processes to join pid=%d \n",cid);
#endif
	}
#ifdef NEWPRINTFS
	fprintf(stderr, "all sess for this vn over %d out of %d sessions \n", sessover++, thissess);
//	fflush(stderr);
#endif
#ifndef NOPRINTFS
	fprintf(stderr,":application over\n");
//	fflush(stderr);
#endif
#ifdef MINIMALPRINTOUT
	fprintf(stderr, "allover\n");
#endif
//	fflush(stderr);
/* free everything here */

	//							free(clnt);
	//							free(srvr);
	//exit(0);
	fclose(indifile);
	fclose(errfile);
	return 0;
}
/****/

