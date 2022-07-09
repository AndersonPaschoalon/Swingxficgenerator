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
#include "hashtable_cwc22.h"
#include "hashtable_itr_cwc22.h"
#include <stdlib.h>
#include <stdio.h>
//#include "memcheck.h"
#include <math.h>
#include <string.h> /* for memcmp */
#include <sys/time.h> /* for timeofday */
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<signal.h>
#include<errno.h>
#include<unistd.h>
#include<sys/uio.h>
#include<math.h>

//#define DEBUGLATEST 1
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#include "workgen_header.h"
int PKTSIZE;
udp_req thisrsp,this_req;
int tcpnodelayflag=1;
struct timeval udpstart, udpend, udpsleep;
int pktnum, ttltime;
//#define NEWPRINTFS 1
//#define DONTNEETD1 1

int diff_in_time_in_usec(struct timeval *tnew, struct timeval told, int bytes, int brate_int) ;
long	sendmesgbyudp(int sock,char * Inline,long resp_size_int,int flg,struct sockaddr* client,int client_len);
enum protocols {UDP=-1, TCP=1};
enum protocols transport;
char tosendmessage[LINESIZE];
int curlength;
int sent,tosend;

/****f* Swing/listener_sendmessage
  * NAME
  * 	send message
  * SOURCE
  *****/
long	sendmessage(int sock,char * Inline,int brate, long resp_size_int,enum protocols proto,struct sockaddr* client,int client_len){
	long sent=0;
	long r=0;
	struct timeval timenow;
	int loop;
    int time_to_play;
	struct timeval current_time, new_time;

	if(proto==UDP) {

	/* send resp_size_int bytes of data at brate bytes per second CBR */
	/* as of now dont put any itelligence here...*/
	loop=resp_size_int/LINESIZE;
	sent=0;

		
		time_to_play=resp_size_int/brate;
		sent=0;
		memset(Inline,'1',LINESIZE_UDP);
		Inline[LINESIZE_UDP]='\0';
		gettimeofday(&timenow, NULL);
		while ( resp_size_int > sent )
		{

		//	fprintf(stderr,  "sent %d bytes %d bytes still left\n", sent, resp_size_int);
			gettimeofday(& current_time , NULL);
			if ((resp_size_int - sent ) > brate)
				r=sendmesgbyudp(sock,Inline,brate,0,client,client_len);
			else
				r=sendmesgbyudp(sock, Inline, resp_size_int - sent , 0, client, client_len);

			gettimeofday( &new_time, NULL );
			if (r<0) {
				perror("error when calling sendmesgbyudp");
				//fflush(stderr);
				exit(-1);
			}
			else {
		//		printf("sent %d bytes \n", r);
			}
			//sent+=(resp_size_int - sent > brate ? brate:resp_size_int - sent);
			sent+=r;

			if (resp_size_int > sent ) diff_in_time_in_usec(&new_time,timenow , sent, brate);
		}





#ifndef NOPRINTFS
		printf("sent everything\n");
		fprintf(stderr," Send by udp message %s\n", Inline);
#endif
	} else {
#ifdef NEWPRINTFS
		fprintf(stderr, " send response by TCP %d bytes %s \n", resp_size_int, Inline);
#endif
#ifndef NOPRINTFS
		fprintf(stderr, " send response by TCP %d bytes %s \n", resp_size_int, Inline);
#endif
		/* first send the size of the response */
		sent=0;
		while(resp_size_int>0 && sent<resp_size_int) {
	//		if(sent<500)
	//			r=send(sock,Inline,min(TCPMAXSIZE, resp_size_int-sent), 0);
	//		else {
				//r=send(sock,Inline ,min(min(TCPMAXSIZE, resp_size_int-sent),PKTSIZE), 0);
				//r=send(sock,Inline ,min(PKTSIZE, resp_size_int-sent), 0);


#ifndef NOPRINTFS
				fprintf(stderr, " resp_size_int=%ld sent=%ld LINESIZE=%ld PKTSIZE=%ld MAGICLINESIZE=%ld\n", resp_size_int, sent, LINESIZE, PKTSIZE, MAGICLINESIZE);
#endif

			if(MAGICLINESIZE>PKTSIZE && sent<MAGICLINESIZE) {
#ifdef NODELAYHACK
				//r=send(sock,& Inline[sent],min(min( resp_size_int - sent , LINESIZE - sent ), PKTSIZE) , 0);
				r=send(sock,& Inline[sent],min(min( resp_size_int - sent , MAGICLINESIZE - sent ), PKTSIZE) , 0);

#ifndef NOPRINTFS
				fprintf(stderr, " PKTSIZE %d sent %d bytes\n", PKTSIZE, r);
#endif
#else
				//r=send(sock,& Inline[sent],min( resp_size_int - sent , LINESIZE - sent ) , 0);
				r=send(sock,& Inline[sent],min( resp_size_int - sent , MAGICLINESIZE - sent ) , 0);

#ifndef NOPRINTFS
				fprintf(stderr, " PKTSIZE %d sent %d bytes\n", PKTSIZE, r);
#endif
#endif
			}
			else {
#ifdef NODELAYHACK
				r=send(sock, Inline, min(min(resp_size_int - sent, LINESIZE), PKTSIZE), 0);

#ifndef NOPRINTFS
				fprintf(stderr, " PKTSIZE %d sent %d bytes\n", PKTSIZE, r);
#endif
#else
				r=send(sock, Inline, min(resp_size_int - sent, LINESIZE), 0);

#ifndef NOPRINTFS
				fprintf(stderr, " PKTSIZE %d sent %d bytes\n", PKTSIZE, r);
#endif
#endif
			}

/* comment out below */
/*
#ifdef NODELAYHACK
				r=send(sock,Inline ,min( (resp_size_int-sent),PKTSIZE), 0);
#else
				r=send(sock,Inline ,resp_size_int-sent, 0);
#endif
*/
/* comment out above */
		//}
			if(r<0) {
				perror("send failed");
	//			fflush(stderr);
				close(sock);
				exit(-1);
			}
			sent+=r;
#ifdef NEWPRINTFS 
			fprintf(stderr, " sent %d bytes out of %d\n", sent, resp_size_int );
#endif
		}
#ifndef NOPRINTFS
		fprintf(stderr," Send by tcp message %s\n", Inline);
#endif
	}

	return sent;
}


/*
# define TERM_STRING "22over22"
#define MY_OWN_PORT 22222
#define LINESIZE 1024*8
#define LINESIZE_UDP 1024
#define SEED 17687
*/

extern void generate_zipf(char *, char *, char *);
extern int genfromfile(char * , int, int **);


/* for sending send in chunks of 8K only 
   so have linesize = 8K */

int flowid;
int tryingagain;



//typedef unsigned int uint32_t;
//typedef unsigned short uint16_t;
struct timeval tv,tv2, testtv2 , testtv1 , timenew, timenow, tmf;
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


void Usage(char **args)
{
	printf("Usage:%s -p <port> -c <class> [-t 1 ] [-m PKTSIZE]\n", args[0]);
	printf("-t is for transport. Default is 1 => TCP but -1=> UDP\n");
	fflush(stderr);
	exit(1);
}

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
	int resp_zipfres;
	int resp_fileres;
	int brate;
	FILE * resp_file;
	int resp_SEED;
	int brate_SEED;
	int resp_counter;
	int resp_numtogen;
	int brate_counter;
	int brate_numtogen;
	int * resp_file_permute_items;
	int * brate_permute_items;
	int transport; /* 0-> default TCP, 1-> udp */
	int resp_zipf_numtogen, resp_zipf_SEED, resp_zipf_maxfreq;
	int * resp_zipf_permute_items;
	int resp_zipf_counter;
};

long	sendmesgbyudp(int sock,char * Inline,long resp_size_int,int flg,struct sockaddr* client,int client_len){
	/* send r bytes by udp... use Inline */
	long sent=0;
	long r=0;



	//memset(Inline,0x1,LINESIZE_UDP);
#ifndef NOPRINTFS
	fprintf(stderr, "to send %ld bytes file on %d \n", resp_size_int, sock );
#endif
	while(sent<resp_size_int) {
		
	//	fprintf(stderr, "sendmesgbyudp: sent %d reqd %d\n",sent,resp_size_int);
		if (resp_size_int-sent>LINESIZE_UDP) {
#ifndef NOPRINTFS
			fprintf(stderr, "sending %d bytes of %s ", LINESIZE_UDP, Inline);
#endif
			r=sendto(sock, Inline, LINESIZE_UDP,0,client,client_len);
#ifndef NOPRINTFS
			fprintf(stderr, " sent %ld bytes\n", r);
			//fflush(stderr);
#endif
		}
		else 
		{
#ifndef NOPRINTFS
			fprintf(stderr, "sending %ld bytes of %s ", resp_size_int-sent, Inline);
#endif
			r=sendto(sock, Inline, resp_size_int-sent, 0, client, client_len);
#ifndef NOPRINTFS
			fprintf(stderr, " sent %ld bytes\n", r);
			//fflush(stderr);
#endif
		}

		if(r<0) {
			perror("error in sendto in sendmesgbyudp");
	//		fflush(stderr);
			exit(-1);
		}
		sent+=r;
		//sleep(1);
//		printf("sent %d bytes total %d message\n", r, sent);

	}
#ifndef NOPRINTFS
	fprintf(stderr, "sending over\n");
#endif

	return sent;
}


int diff_in_time_in_usec(struct timeval *tnew, struct timeval told, int bytes, int brate_int) {
	/* returns positive time remaining so that select may sleep */
		struct timeval tv = *tnew;
		int diffinsec=(tv.tv_sec - told.tv_sec);
		if(diffinsec<=0) { sleep(1) ; return 0;}
		/*
		fprintf(stderr, "in here\n");
		fprintf(stderr," brate shud be %d but it is %d\n", brate_int, bytes/diffinsec);
		fflush(stderr);
		sleep(1);
		return 0;
		*/
		while( (bytes/diffinsec) > brate_int) {
			/*fprintf(stderr, "sleeping ..\n");
			printf(" brate shud be %d but it is %d\n", brate_int, bytes/diffinsec);
			fflush(stdout);*/
			sleep (1);
			gettimeofday(&tv, NULL);
		    diffinsec=(tv.tv_sec - told.tv_sec);
		}
		/*
		if( diffinusec <usecsleep) {
			(*tnew).tv_sec=0;
			(*tnew).tv_usec=diffinusec;
			printf("sleeping for %d usec\n", diffinusec);
			select(0,NULL, NULL, NULL,tnew); 
		}
		*/
		return 0;
}



int send_udp( int sock, char * Inline, long resp_size_int, int brate,  struct sockaddr * client , int client_len) {
	long r=0;
	long sent;
	int loop;
//	int i;
    int time_to_play;
	struct timeval current_time, new_time;
	/* send resp_size_int bytes of data at brate bytes per second CBR */
	/* as of now dont put any itelligence here...*/
	loop=resp_size_int/LINESIZE;
	sent=0;
	//memset(Inline,0x1,LINESIZE);
	
#if 0
	for(i=0;i<loop;i++) {
		if (i!=loop-1) r=sendto(sock, Inline, r,0,client,client_len);
		else 
		{
			r=resp_size_int- LINESIZE*(resp_size_int/LINESIZE);
			r=sendto(sock, Inline, r, 0, client, client_len);
		}

		if(r<0) {
			perror("error in sendto");
			fflush(stderr);
			exit(-1);
		}

		sent+=r;
	}
#endif
	time_to_play=resp_size_int/brate;
	sent=0;
	memset(Inline,'1',LINESIZE_UDP);
    gettimeofday(&timenow, NULL);
	while ( resp_size_int > sent )
	{
	    fprintf(stderr,  "sent %ld bytes %ld bytes still left\n", sent, resp_size_int);
		gettimeofday(& current_time , NULL);
		if ((resp_size_int - sent ) > brate)
			r=sendmesgbyudp(sock,Inline,brate,0,client,client_len);
		else
			r=sendmesgbyudp(sock, Inline, resp_size_int - sent , 0, client, client_len);

		gettimeofday( &new_time, NULL );
		if (r<0) {
			perror("error when calling sendmesgbyudp");
	//		fflush(stderr);
			exit(-1);
		}
		else {
			printf("sent %ld bytes \n", r);
		}
		//sent+=(resp_size_int - sent > brate ? brate:resp_size_int - sent);
		sent+=r;

		if (resp_size_int > sent ) diff_in_time_in_usec(&new_time,timenow , sent, brate);
	}
	printf("sent everything\n");
	
	return sent;
}
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
/****f* Swing/main_listener
  * NAME 
  *   The code all listeners use
  * AUTHOR
  *   Kashi Vishwanath
  * INPUTS
  *   -p port
  *   -c class
  *	  -t [0/1]
  *   -m PKTSIZE
  * SOURCE
  */
int
main(int argc, char **argv)
{

	int sock,fd, client_len, server_len;
	struct sockaddr_in server, client;
	struct timeval tprint ;
	udp_req thisreq;
	int i,port=0;
	int done ,closeit,alldone,thisdone;
	long r=0;
	long REQ, RSP, serverthink, brate;
	char Inline[LINESIZE];

	float rval;
	//int numxchng_int, numconn_int, resp_size_int, req_size_int, think_int, process_int;
	int brate_int;
	//int max,min;
	

 //   struct key *k;
   // struct value *v, *found;
    //struct hashtable *h;

	//int retVal,cid;
//	struct vkey *vk;
	//struct vvalue *vv, *vfound;
	//struct hashtable *vh;

	struct timeval start_tv,current_tv ;
	char this_line[80];
	//char *clnt, *srvr;
	char *thisclass=NULL;
	unsigned long dummy_port;
	int s;
	char * tosendsize;
	int totsent;
	int totreceived;
	int sentsize;
#ifndef NOPRINTFS
	int pairnumber=0;
#endif
	int sock_opt;
#if DONTNEETD1
	FILE * specsfile=fopen(SPECSFILE,"r") ; 
	FILE * vipfile=fopen(HOSTSFILE,"r") ; 
#endif
	int sock_type;

#ifdef DEBUG_SLOWNESS
	struct timeval slowtv2, slowtv1,slowtv3;
#endif
	int pktstosend, pktssent;
	fd=0;
	brate_int=0;
	tosendsize=(char * ) malloc (sizeof(char) * 20);
#ifdef DEBUGLATEST
	printf("argc=%d\n",argc);
#endif
	if (argc<7)
		Usage(argv);
	PKTSIZE=0;
	for(i=1;i<argc;i++)
	{
		if (strcmp("-p", argv[i])==0)
			port=atoi(argv[++i]);
		else if (strcmp("-c", argv[i])==0)
			thisclass=argv[++i];
		else if (strcmp("-t", argv[i])==0)
		{
			sock_type=atoi(argv[++i]);
			if(sock_type==TCP) sock_type=SOCK_STREAM;
			else sock_type=SOCK_DGRAM;
		}
		else if(strcmp("-m", argv[i])==0)
			PKTSIZE=atoi(argv[++i]);
		else Usage(argv);
	}
	if(PKTSIZE==0) PKTSIZE=PKTSIZE_DEFAULT;
	PKTSIZE=min(PKTSIZE, PKTSIZE_DEFAULT);
	#ifdef DEBUG_LISTENER
	printf("DEBUG_LISTENER test\n");
	//fflush(stdout);
	#endif
#if DONTNEETD1
    h = create_hashtable(160, 0.75, hashfromkey, equalkeys);
	vh=create_hashtable(160,0.75,vhashfromkey,vequalkeys);
    if (NULL == h) { exit(-1); }; /*oom*/
	if (NULL == vh){  exit(-1); } /* oom */

	k=NULL;
	v=NULL;
	vk=NULL;
	vv=NULL;
#endif
	//clnt=(char * ) malloc(sizeof(char)*80);
	//srvr=(char * ) malloc(sizeof(char)*80);
#if DONTNEETD1
	while(fgets(this_line,80,specsfile))
	{
		//printf("new line %s\n",this_line);
		if(strncmp(this_line, "class", 5)==0)
		{
		 /* initialize */
			k = (struct key *) malloc(sizeof(struct key ));
			v = (struct value *) malloc (sizeof(struct value));
			v->resp_zipfres=0;
			v->brate=0;
			v->brate_counter=0;
			v->resp_fileres=0;
			v->resp_zipf_counter=0;
			v->resp_counter=0;
			v->transport=0;
			k->class_name = (char *)
				malloc(sizeof(char)*strlen(&this_line[6]) +1 );
			memset(k->class_name,0x0,1);
			sscanf(&(this_line[6]),"%s",k->class_name);
			
		}
		else if (strncmp(this_line,"port=",5)==0)
		{
			sscanf(&(this_line[5]),"%lu",&dummy_port);
			v->listen_port=dummy_port;
			if(!insert_some(h,k,v)) {
				printf("listener.c: insert_some failed\n");
	//			fflush(stdout);
				fflush(stderr);
				exit(-1);
			}
			if(NULL==(found=search_some(h,k))){
				printf("listener.c: did not find it line %d\n",__LINE__);
				fflush(stdout);
				fflush(stderr);
				exit(-1);
			}
		}
		/* have matching for file to pickup distribution from first */
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
				sscanf(this_line,"resp zipf maxfreq %d numtogen %d SEED %d",&v->resp_zipf_maxfreq,&v->resp_zipf_numtogen,&v->resp_zipf_SEED);
				char * freq=(char * )malloc ( sizeof(char)*10);
				char * numgen=(char *)malloc( sizeof(char)*10);
				sprintf(freq,"%d", v->resp_zipf_maxfreq);
				sprintf(numgen,"%d", v->resp_zipf_numtogen);
				generate_zipf(freq, numgen,"zipf.input");
				genfromfile("zipf.input", v->resp_zipf_SEED, &v->resp_zipf_permute_items);
			}
			else if (strncmp(&this_line[5],"file",4)==0)
			{
				v->resp_fileres=1;
				char dummyfilename[30];
				sscanf(this_line,"resp file %s SEED %d", dummyfilename, &v->resp_SEED);
				v->resp_numtogen=genfromfile(dummyfilename, v->resp_SEED, &v->resp_file_permute_items);
				v->resp_counter=(int) drand48()*v->resp_numtogen;
				if(v->resp_counter>=v->resp_numtogen) v->resp_counter=0;
			}

		} /* if (strncmp(this_line,"resp",4)==0) */
		else if(strncmp(this_line,"brate",5)==0)
		{
			v->brate=1;/* no checks as of now for TCP/UDP etc and it has to be a file */
			char dummyfilename[30];
			sscanf(this_line,"brate file %s SEED %d", dummyfilename, &v->brate_SEED);
			v->brate_numtogen=genfromfile(dummyfilename, v->brate_SEED, &v->brate_permute_items);
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
			sscanf(this_line,"transport %d",&v->transport);
		}
	} /* while(fgets(this_line,80,specsfile)) */
#endif
	#ifdef DEBUG_LISTENER
	printf("done reading specs file\n");
	fflush(stdout);
	#endif
#if DONTNEETD1
	while(fgets(this_line,80,vipfile))
	{
		if(strncmp(this_line,"client",5)==0)
		{
			vk = (struct vkey *) malloc(sizeof(struct vkey ));
			vv = (struct vvalue *) malloc(sizeof(struct vvalue));
			vv->vip=(char*)malloc(sizeof(char)*16);
			sscanf(this_line, "client%d %s # on unknown", &vk->vn_num,vv->vip);
			if(!vinsert_some(vh,vk,vv)) {
				perror("could not perform vinsert_some");
				fflush(stderr);
				exit(-1);
			}
			if (NULL==(vfound=vsearch_some(vh,vk))){
				printf("listener.c: did not find it%d\n",__LINE__);
				perror("listener.c: did not find it vsearch_some");
				fflush(stderr);
				exit(-1);
			}
		}
	} /* while(fgets(this_line,80,vipfile))*/
#endif
	//k=(struct key *) malloc(sizeof(struct key));
	//k->class_name=thisclass;
#if DONTNEETD1
    sock_type=SOCK_STREAM;
	if(NULL==(found=search_some(h,k))) {
		printf("listener.c: could not find class %s in line %d\n",thisclass,__LINE__);
		exit(-1);
	}
	if (found->transport==1) sock_type=SOCK_DGRAM;
	else sock_type=SOCK_STREAM;
#endif
#ifndef NOPRINTFS
	fprintf(stderr, "before socket \n");
	if(sock_type==SOCK_STREAM) fprintf(stderr, "SOCK STREAM\n");
	else if(sock_type==SOCK_DGRAM) fprintf(stderr,"SOCK DGRAM\n");
	else fprintf(stderr, "neith SOCK_DGRAM nor SOCK_STREAM\n");
	fflush(stderr);
#endif
	
	sock = socket(AF_INET, sock_type,0);
	if (sock < 0)
	{
		perror("could not create stream socket");
		fflush(stderr);
		exit(-1);
	}
	sock_opt = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&sock_opt,
				sizeof (sock_opt)) == -1) {
		perror("setsockopt(SO_REUSEADDR)");
		(void) close(sock);
		fflush(stderr);
		exit(-1);
	}
if(sock_type==SOCK_STREAM) {
#ifdef NODELAYHACK
	if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&tcpnodelayflag, sizeof(int))<0) {
		perror("setsockopt(TCP_NODELAY)");
		fflush(stderr);
		exit(-1);
	}
#endif
#ifdef MSSHACK
	sock_opt = PKTSIZE+12;
	if (setsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, (void *)&sock_opt,
				sizeof (sock_opt)) < 0) {
		perror("setsockopt(TCP_MAXSEG)");
		(void) close(sock);
		fflush(stderr);
		exit(200);
	}
#endif
}

	
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	if (bind(sock,(struct sockaddr*) &server,sizeof(server)) < 0)
	{
		perror("error binding the socket");
		fflush(stderr);
		exit(-1);
	}

	listen(sock,1500);
    done=0;
	srand48(SEED);
	gettimeofday(&start_tv , NULL);
	#ifdef DEBUG_LISTENER
	printf("before signal\n");
	fflush(stdout);
	#endif
	signal(SIGCHLD, SIG_IGN);
#ifndef NOPRINTFS
	fprintf(stderr, "server\n");
	fflush(stderr);
#endif
	while(!done)
	{
tryagain:
		//	while((cid=(waitpid(-1, &retVal, WNOHANG)))>0);
		//	if(cid<0) perror("waitpid: ");
			tryingagain=0;
#ifndef NOPRINTFS
			fprintf(stderr, "before choosing\n");
#endif
		if (sock_type==SOCK_STREAM) {
#ifdef DEBUG_LISTENER
			printf("tcp...\n");
#endif

#ifndef NOPRINTFS
			fprintf(stderr, "tcp..\n");
#endif

			rval=drand48();
			client_len = sizeof(client);
#ifndef NOPRINTFS
			fprintf(stderr, " before accept \n");
#endif
			if((fd = accept(sock,(struct sockaddr *) &client, &client_len)) < 0 )
			{
				perror("could not accept connection");
	//			fflush(stderr);
				/* close for now even though i have code below
				in place to retry upto 3 times / 15 secs */
			//	close(sock);
			//	exit(-1);
				tryingagain++;
				//if(tryingagain < 3 ) {
					sleep (1);
					goto tryagain;
			//	}
			//	else {
			//		printf("too many tries but accpet fails\n");
			//		close(sock);
			//		exit(-1);
			//	}
				
			}
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv1, NULL);
			fprintf(stderr, " conn accpeted : %ld %ld \n", slowtv1.tv_sec, slowtv1.tv_usec);
#endif
			tryingagain=0;
#ifndef NOPRINTFS
			fprintf(stderr, "connection accepted\n");
#endif
#ifndef NOLOGGING
			printf("connection accepted\n");
#endif
		} // if sock_type == SOCK_STREAM
		else { // i.e. if sock_type = SOCK_DGRAM
#ifndef NOPRINTFS
			fprintf(stderr, "Udp...\n");
		//	fflush(stderr);
#endif
			client_len=sizeof(client);
#ifndef NOPRINTFS
			fprintf(stderr, "before recvfrom first time \n");
	//		fflush(stderr);
#endif
			r=recvfrom(sock, Inline, LINESIZE, 0, (struct sockaddr *)&client, &client_len);
			if(r<0)
			{
				perror("recvfrom error in listener.c");
				//fflush(stderr);
				tryingagain++;
				sleep(5);
				goto tryagain;
			//	exit(-11);
			}
#ifndef NOPRINTFS
			fprintf(stderr, "after recvfrom first time \n");
		//	fflush(stderr);
#endif
			fd=sock;
			Inline[r]='\0';
	//		fprintf(stderr, "received %d bytes \n", r);
#ifndef NOPRINTFS
			fprintf(stderr, "received %ld bytes UDP %d sock \n", r, fd);
	//		fflush(stderr);
#endif
		}
#if 0
		if(NULL==(found=search_some(h,k))) {
			printf("listener.c: could not find it %d\n",__LINE__);
			exit(-1);
		}
		if(found->resp_zipfres==1) {
			found->resp_zipf_counter++;
			if (found->resp_zipf_counter >= found->resp_zipf_numtogen)
				found->resp_zipf_counter=0;
		}
		else if(found->resp_fileres==1) {
			found->resp_counter++;
			if (found->resp_counter >= found->resp_numtogen)
				found->resp_counter=0;

		}
		if (found->brate==1) {
			found->brate_counter++;
			if (found->brate_counter >= found->brate_numtogen)
				found->brate_counter=0;
		}
#endif // need to see if this if0 is really true
#ifndef NOPRINTFS
		fprintf(stderr, "before fork \n");
#endif
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv2, NULL);
			fprintf(stderr, " before fork %ld %ld \n", slowtv2.tv_sec, slowtv2.tv_usec);
#endif
		if (fork()==0)
		{
			gettimeofday(&tprint,NULL);
#ifdef DEBUGLATEST
			fprintf(stderr, "%ld %ld \n", tprint.tv_sec, tprint.tv_usec);
#endif
			if(sock_type==SOCK_STREAM) close(sock);
#ifndef NOPRINTFS
			fprintf(stderr, "in child ... \n");
#endif

#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv1, NULL);
			fprintf(stderr, " in child after fork %ld %ld \n", slowtv1.tv_sec, slowtv1.tv_usec);
#endif
#ifndef NOLOGGING
			printf("this is a child\n");
#endif
			thisdone=0;
#if 0
			if(NULL==(found=search_some(h,k))){
				printf("listener.c: could not find it %d\n",__LINE__);
				exit(-1);
			}
#endif
		
#if 0
			rval=drand48();
			max=found->numxchng_max;
			min=found->numxchng_min;
			numxchng_int=(rval * ( max - min ) + min + 0.5 ) / 1 ;

			rval=drand48();
			max=found->req_size_max;
			min=found->req_size_min;
			req_size_int=(rval * ( max - min ) + min + 0.5 ) / 1 ;

			rval=drand48();
			max=found->process_max;
			min=found->process_min;
			process_int=(rval * ( max - min ) + min + 0.5 ) / 1 ;

			if(found->resp_zipfres==0 && found->resp_fileres==0) {
				rval=drand48();
				max=found->resp_size_max;
				min=found->resp_size_min;
				resp_size_int=(rval * ( max - min ) + min + 0.5 ) / 1 ;
			}
			else if(found->resp_fileres==1){
				resp_size_int=found->resp_file_permute_items[found->resp_counter];
				resp_size_int=resp_size_int*50;
				//resp_size_int=resp_size_int/2000;
				//resp_size_int=200;
			}
			else {
			//	printf("counter is %d\n", found->resp_zipf_counter);
				resp_size_int=found->resp_zipf_permute_items[found->resp_zipf_counter];
			}
			/* KV hack below */
			if (resp_size_int==0) resp_size_int=1;

			if(found->brate==1) {
				brate_int=found->brate_permute_items[found->brate_counter];
	//			brate_int=1024;// just for now
			}

			rval=drand48();
			max=found->think_max;
			min=found->think_min;
			think_int=(rval * ( max - min ) + min + 0.5 ) / 1 ;

			rval=drand48();
			max=found->numconn_max;
			min=found->numconn_min;
			numconn_int=(rval * ( max - min ) + min + 0.5 ) / 1 ;
#endif		
			while(thisdone!=1) {
			//	Inline[0]='\0';
				totreceived=0;
				errno=0;
				
				if(sock_type==SOCK_DGRAM) {
				 /* listen for connections*/
					//if(strstr(Inline,"RSPSEND")==NULL ) {
					if(Inline[0]!='R' ) {
	//					perror("no RSPSEND in Inline SOCK_DGRAM");
	//					fflush(stderr);
	//					fprintf(stderr, "sock %d has to be closed \n", sock);
						//close(sock);
						exit(1);
					}
#ifdef NEWPRINTFS
					gettimeofday(&tmf, NULL);
						fprintf(stderr,"got final packet %s %ld %ld\n", Inline,tmf.tv_sec, tmf.tv_usec );
#endif
	//					sscanf(Inline,"R %d %d %d",  &d1&thisrsp.bytes, &thisrsp.pkts,& thisrsp.duration);

						sscanf(Inline,"R %ld %d %d %ld %ld %d %d %ld ",&this_req.bytes, &this_req.pkts,& this_req.duration, &serverthink,&thisrsp.bytes,& thisrsp.pkts, &thisrsp.duration,&brate);
#ifdef NEWPRINTFS
						fprintf(stderr, " bytes %d pkts %d duration %d \n", thisrsp.bytes, thisrsp.pkts, thisrsp.duration);
						fprintf(stderr, " client %s\n",inet_ntoa(client.sin_addr));
	//					client.sin_port=htons(udpportnum);
		fprintf(stderr, " udp... to send back  %d bytes %d pkts %d secs \n", thisrsp.bytes, thisrsp.pkts, thisrsp.duration);
#endif
#if 0
		sock = socket(AF_INET, SOCK_DGRAM,0);
		if (sock < 0)
		{
			perror("could not create stream socket");
			fflush(stderr);
			exit(-1);
		}
		sock_opt = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&sock_opt,
					sizeof (sock_opt)) == -1) {
			perror("setsockopt(SO_REUSEADDR)");
			(void) close(sock);
			fflush(stderr);
			exit(-1);
		}
#endif
		pktstosend=thisrsp.pkts;
		pktssent=0;
		memset(Inline,'1',LINESIZE_UDP);
		if(thisrsp.duration<1) thisrsp.duration=1;
		if(thisrsp.pkts<1 && thisrsp.bytes>0) thisrsp.pkts=1;
		thisrsp.pkts=(thisrsp.bytes/thisrsp.pkts > UDPPKTSIZEMAX ) ? (1+thisrsp.bytes/UDPPKTSIZEMAX):thisrsp.pkts;
		//thisrsp.pkts=(thisrsp.bytes/thisrsp.pkts < UDPPKTSIZEMIN ) ? (1+thisrsp.bytes/UDPPKTSIZEMIN):thisrsp.pkts;
		//if(thisrsp.pkts>UDP_PKT_DURATION_THRESHOLD){ }
		if(thisrsp.pkts>UDP_PKT_THRESHOLD && thisrsp.duration > UDP_DURATION_THRESHOLD){
		thisrsp.pkts=(thisrsp.bytes/thisrsp.pkts > UDPPKTSIZEMAX ) ? (1+thisrsp.bytes/UDPPKTSIZEMAX):thisrsp.pkts;
			brate=thisrsp.bytes/thisrsp.duration;
		fprintf(stderr, " here rrr\n");
	//	fflush(stderr);
#ifdef NEWPRINTFS
			fprintf(stderr, " should use duration\n");
	//		fflush(stderr);
#endif
			gettimeofday(&udpstart,NULL);
			ttltime=0;
			sent=0;
			while(ttltime<=thisrsp.duration && ( (thisrsp.duration<10)?(sent<thisrsp.bytes):1)) 
			{
#ifdef NEWPRINTFS
				fprintf(stderr, " new iter \n");
				fprintf(stderr, " ttltime %d secs, bytes sent %d \n", ttltime, sent);
	//			fflush(stderr);
#endif
				if(ttltime<thisrsp.duration)
				r=sendto(sock, Inline, max(min(UDPPKTSIZEMAX,thisrsp.bytes/thisrsp.duration),UDPPKTSIZEMIN), 0,(struct sockaddr *) & client, client_len);
				else
				r=sendto(sock, Inline, max(min(UDPPKTSIZEMAX,thisrsp.bytes/thisrsp.duration),UDPPKTSIZEMIN), 0,(struct sockaddr *) & client, client_len);
				//r=sendto(sock, Inline, min(min(UDPPKTSIZEMAX,thisrsp.bytes/thisrsp.pkts),thisrsp.bytes-sent-8), 0,(struct sockaddr *) & client, client_len);

#ifdef NEWPRINTFS
					fprintf(stderr, "sent %d bytes of %s\n",r, Inline);
#endif
				sent+=r;
				pktssent++;
#ifdef NEWPRINTFS
					fprintf(stderr, "sent %d bytes of %d duration %d ttltime %d pkts %d \n",r, thisrsp.bytes, thisrsp.duration, ttltime, thisrsp.pkts);
#endif
	if(thisrsp.bytes>sent){			
		udpsleep.tv_sec=(thisrsp.duration - ttltime)/(1.0*(thisrsp.bytes-sent)/(thisrsp.bytes/thisrsp.pkts));
#ifdef NEWPRINTFS
		fprintf(stderr, " herer\n");
	//	fflush( stderr);
#endif
				udpsleep.tv_usec=1.0*(1.0* (thisrsp.duration - ttltime)/((thisrsp.bytes-sent)/(thisrsp.bytes/thisrsp.pkts)) - udpsleep.tv_sec)*1000000;
#ifdef NEWPRINTFS
				fprintf(stderr, "sleep for %ld %ld \n", udpsleep.tv_sec, udpsleep.tv_usec);
	//			fflush(stderr);
#endif
	}
					udpsleep.tv_sec=1;
					udpsleep.tv_usec=0;
				udpsleep.tv_usec=max(udpsleep.tv_usec,0);
				udpsleep.tv_sec=max(udpsleep.tv_sec,0);
				select(0,NULL,NULL,NULL,&udpsleep);
				gettimeofday(&udpend, NULL);
				ttltime=udpend.tv_sec - udpstart.tv_sec + ( udpstart.tv_usec - udpstart.tv_usec)/1000000 ;
#ifdef NEWPRINTFS
	if(ttltime>0)		{	
		int bratenow=thisrsp.bytes/thisrsp.duration;
		fprintf(stderr, " duration %d bytes %d brate %d current brate %d reqd %d\n", thisrsp.duration, thisrsp.bytes, bratenow, sent/ttltime, thisrsp.bytes/thisrsp.duration);
		fprintf(stderr, " thisrsp.durationd = %d thisrsp.bytes= %d \n", thisrsp.duration, thisrsp.bytes);
	}
				fprintf(stderr, " end iter \n");
#endif
			}
			
		sprintf(Inline,"ALLDONE \0");
		r=sendto(sock, Inline, strlen(Inline), 0,(struct sockaddr *) & client, client_len);
		sent+=r;
				pktssent++;
		fprintf(stderr, " duration over\n");
		//fflush(stderr);
		}
		else { 
	//	thisrsp.pkts=(thisrsp.bytes/thisrsp.pkts > UDPPKTSIZEMAX ) ? (1+thisrsp.bytes/UDPPKTSIZEMAX):thisrsp.pkts;
#ifdef NEWPRINTFS
			fprintf(stderr, " should use bytes and pkts \n");
	//		fflush(stderr);
#endif
			sent=0;
		memset(Inline,'1',LINESIZE_UDP);
	//	thisrsp.pkts=(thisrsp.bytes/thisrsp.pkts < UDPPKTSIZEMAX) ? thisrsp.pkts:thisrsp.bytes/UDPPKTSIZEMAX;
	//	while(sent<thisrsp.bytes) {
			for(pktnum=0;pktnum<thisrsp.pkts ;pktnum++)
			{
				if(pktnum==thisrsp.pkts-1) {
					sprintf(Inline,"ALLDONE");
					Inline[7]='1';
					
#ifdef NEWPRINTFS
					fprintf(stderr, " bytes %d pkts %d this pky=%d already sent %d \n", thisrsp.bytes, thisrsp.pkts,pktnum,  sent); 
					//sleep(1);

#endif
					
				 r=sendto(sock, Inline, max(min( thisrsp.bytes/thisrsp.pkts,UDPPKTSIZEMAX),8), 0, (struct sockaddr *) &client, client_len);
				}
				else r=sendto(sock, Inline, min( thisrsp.bytes/thisrsp.pkts,UDPPKTSIZEMAX), 0, (struct sockaddr *) &client, client_len);
#ifdef NEWPRINTFS
					//fprintf(stderr, "sent %d bytes of %s\n",r, Inline);
					fprintf(stderr, "sent %d bytes \n",r );
					if(pktnum==thisrsp.pkts-1){
						fprintf(stderr, " last one sent %s\n", Inline);
						}
#endif
			if(r<0) {
				perror("send failed");
				close(sock);
	//			fflush(stderr);
				exit(-1);
			}
			sent+=r;
				pktssent++;
#ifdef NEWPRINTFS
			fprintf(stderr, "sent %d bytes out of %d \n", sent, thisrsp.bytes);
#endif
			}
		//}
		}
#ifdef NEWPRINTFS
	//	fprintf(stderr, "sent all %d bytes \n", sent);
		fprintf(stderr, "sent %d bytes expected %d \n", sent, thisrsp.bytes);
#endif
#ifndef NOPRINTFS
	if(pktstosend>0)	fprintf(stderr, " UDP LISTENER BYTES %d PKTS TOSEND %d sent %d diff %f \%\n", thisrsp.bytes, pktstosend, pktssent, 1.0*(pktstosend - pktssent)/pktstosend*100);
	else fprintf(stderr, " UDP LISTENER -ve pktstosend\n");
	//	fflush(stderr);
#endif
					exit(1);
					
#ifndef NOPRINTFS
					fprintf(stderr, "in child of UDP \n");
	//				fflush(stderr);
#endif
					alldone=0;
					done=0;

#ifndef NOPRINTFS
					fprintf(stderr, "before starting anything %s Inline\n", Inline);
#endif
					if(done!=1)
						if(strstr(Inline,TERM_STRING)!=NULL) {
#ifndef NOPRINTFS
							fprintf(stderr, "all reveived allready for UDP \n");
#endif
							done=1;
							if(strstr(Inline,"close")!=NULL) {
								closeit=1;
							}
							sscanf(Inline,"REQ %ld serverthink %ld RSP %ld brate %ld", &REQ, &serverthink, &RSP, &brate);
							if(done==1)
								if(r>=REQ) thisdone=1;
						}
#ifndef NOPRINTFS
					 fprintf(stderr, "received till now %ld bytes \n", r);
					 pairnumber=0;
#endif
					
					while(alldone==0) {
						fprintf(stderr, " 111 \n");
						//r=0; /* r initialized when calling recvfrom for the first time */
					//	thisdone=0;
						while(thisdone!=1) {
						fprintf(stderr, " 222 \n");
#ifndef NOPRINTFS
							fprintf(stderr, "before recvfrom \n");
	//						fflush(stderr);
#endif
							if(done!=1) s=recvfrom(fd, &Inline[r], LINESIZE-r, 0, (struct sockaddr *)&client, &client_len);
							else s=recvfrom(fd,Inline, LINESIZE, 0, (struct sockaddr *) &client,& client_len);
							if(s<0) {
								perror("recv failed");
	//							fflush(stderr);
							//	close(fd);
								exit(-1);
							}
							if(s==0) {
								/* remote server called a close */
								done=1;
								closeit=1;
#ifndef NOPRINTFS
								fprintf(stderr, " remote site has closed \n");
#endif
								goto alldonehere;

							}
#ifndef NOPRINTFS
							fprintf(stderr, "after recvfrom \n");
	//						fflush(stderr);
#endif

							if(done!=1)
								if(strstr(Inline,TERM_STRING)!=NULL) {
									done=1;
									if(strstr(Inline,"close")!=NULL) {
										closeit=1;
									}
									sscanf(Inline,"REQ %ld serverthink %ld RSP %ld brate %ld", &REQ, &serverthink, &RSP, &brate);
								}
							r+=s;
#ifndef NOPRINTFS
							fprintf(stderr, "received done=%d alldone=%d closeit=%d\n", done, alldone, closeit);
							fprintf(stderr, "received %ld bytes out of %ld \n", r, REQ);
#endif
							if(done==1)
								if(r>=REQ) thisdone=1;

						}
#ifndef NOPRINTFS
						fprintf(stderr, " received from %d \n", fd);
						fprintf(stderr, "got message  REQ %ld serverthink %ld RSP %ld brate %ld \n", REQ, serverthink, RSP, brate);
	//					fflush(stderr);
#endif
					
						/* just keep streaming now till you want... */
						/* the receiver just times out...
						   but for debugging also send the size of the response */



						memset(tosendmessage,'1',LINESIZE);
						tosendmessage[LINESIZE-1]='\0';
						sprintf(tosendmessage,"RSP %ld %s ",RSP, TERM_STRING);
						curlength=strlen(tosendmessage);
#ifndef NOPRINTFS
						fprintf(stderr,"\nsending response %s %d bytes \n", tosendmessage, curlength);
#endif
						if(RSP<LINESIZE-1) {
							if(RSP>curlength) {
								tosendmessage[curlength]='1';
								tosendmessage[RSP]='\0';
							} else {
								RSP=curlength;
#ifndef NOPRINTFS
								fprintf(stderr, "changinre RSP to %ld\n", RSP);
#endif
							}
						} else {
							tosendmessage[curlength]='1';
						}
#ifndef NOPRINTFS
						fprintf(stderr, " RSP here is %ld\n", RSP);
	//					fflush(stderr);
#endif
						/* send the response */
						sent=0;
						tosend=RSP;
						transport=UDP;
#ifndef NOPRINTFS
						fprintf(stderr, "sending on %d\n",fd);
#endif
						while(RSP>0 && sent<tosend) {
							sent+=sendmessage(fd,tosendmessage, brate, RSP*brate,(transport==TCP ? TCP:UDP), (struct sockaddr *)&  client , client_len );
							if(sent<LINESIZE) {
								memset(tosendmessage,'1',LINESIZE);
								tosendmessage[LINESIZE-1]='\0';
								if(sent+LINESIZE-1 > tosend) {
									tosendmessage[tosend - sent ]='\0';
								}
							}
#ifndef NOPRINTFS
							fprintf(stderr, " sent %ld out of %ld \n", sent, tosend);
#endif
						}
#ifndef NOPRINTFS
						fprintf(stderr, "sent total bytes %d to server \n", sent);
	//					fflush(stderr);
#endif
						
alldonehere:
					//	exit(0);
						thisdone=0;r=0;done=0;
						if(closeit==1) alldone=1;
#ifndef NOPRINTFS
						fprintf(stderr, "this is pair number %d\n", pairnumber++);
#endif
						
					}
#if 0
					
					//fflush(stdout);
					resp_size_int=resp_size_int/1000;
				//	printf("received %d bytes brate=%d ", r, brate_int); 
				//	printf("sock is %d\n", sock);
	//				resp_size_int=200;
				//	fprintf(stderr, "to send %d bytes\n", resp_size_int);
					r=send_udp(sock, Inline, resp_size_int, brate_int, (struct sockaddr *) &client, client_len);
					gettimeofday(&timenew,NULL);
#ifdef DEBUG_LISTENER
					printf("udpmsgsent: sent %d bytes wanted brate= %d to client times %ld %ld %ld %ld \n", r,brate_int,  timenow.tv_sec, timenow.tv_usec, timenew.tv_sec, timenew.tv_usec); 
					printf("bpsudp=%d\n", r*8/(int)(timenew.tv_sec - timenow.tv_sec) );
#endif
					close(sock);
				//	fflush(stdout);
				  
#endif
				}/* if clause of sock_type==DGRAM */
				else { /* i.e. sock_type=TCP */
#ifdef DEBUG_LISTENER 
					flowid=-999;
#endif
#ifndef NOPRINTFS
					fprintf(stderr, "before receive \n");
	//				fflush(stderr);
#endif
				//	r=recv(fd,Inline,LINESIZE,0);
					alldone=0; closeit=0;done=0;
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv1,NULL);
			fprintf(stderr, " before receive  %ld %ld \n", slowtv1.tv_sec, slowtv1.tv_usec);
#endif
#ifndef NOPRINTFS
					pairnumber=0;
#endif
					while(alldone==0) {
#ifndef NOPRINTFS
						fprintf(stderr, " 8222\n");
						fprintf(stderr, " numpairs = %d \n", pairnumber);
#endif
						r=0;
						thisdone=0;
						while(thisdone!=1) {
							if(done!=1) s=recv(fd,&Inline[r], LINESIZE-r, 0);
							else s=recv(fd,Inline, LINESIZE, 0);
							if(s<0) {
								perror("recv failed");
	//							fflush(stderr);
								close(fd);
								exit(-1);
							}
							if(s==0) {
#ifndef NOPRINTFS
								fprintf(stderr, " remote client has calle3d close so we should close too \n");
#endif
								closeit=1; REQ=0;RSP=0; serverthink=0;brate=22222;done=1;
							}
#ifndef NOPRINTFS
							fprintf(stderr, " received ->%s<-\n", Inline);
							if(strlen(Inline)<LINESIZE) Inline[LINESIZE]='\0';
							fprintf(stderr, " hance received ->%s<-\n", Inline);
#endif
							if(done!=1)
								if(strstr(Inline,TERM_STRING)!=NULL) {
									done=1;
									if(strstr(Inline,"close")!=NULL) {
										closeit=1;
									}
									errno=0;
									if(sscanf(Inline,"R %ld %ld %ld %ld", &REQ, &serverthink, &RSP, &brate)!=4) {
	//									if(errno!=0) {
										if(Inline[0]>=(0+'0') && Inline[0]<=(9+'0')) {
											/* response has been sent directly */

#ifndef NOPRINTFS
										perror("response has been sent directly ");
#endif
										Inline[s]='\0';
										if(sscanf(Inline,"%ld", &RSP)!=1) {
											perror("canot read RSP directly ");
	//										fflush(stderr);
											exit(-1);
										}
#ifndef NOPRINTFS
										fprintf(stderr, " generate size %ld\n", RSP);
#endif
										REQ=strlen(Inline);
										serverthink=0;
										brate=22222;
										done=1;
										r=REQ;
										
										}
										else {
#ifndef NOPRINTFS
										perror("Inline does nto start with REQ");
										
											fprintf(stderr, "corrputrequest, should generate response of %ld bytes %ld \n", Inline[0], (long)(pow(2.0,1.0*(Inline[0]-'a'))));
#endif
											/* HACKED UP */
											done=1;
											REQ=20;
											r=20;
											serverthink=0;
											brate=22222;
											//RSP=(long)(pow(2.0,1.0*(Inline[0]-'a')));
											RSP=(long)( ((pow(2.0,1.0*(Inline[0]-'a'))) + ( pow(2.0,1.0*(Inline[0]-'a')+1))   )/2);
	//										RSP*=5;
#ifndef NOPRINTFS
											fprintf(stderr, " Generate hacked %ld bytes type 1 \n",RSP);
#endif
										}
											/*****/
	//									}
									}
								}
								else if(s>0) {

											if(Inline[0]!='R'&&(Inline[0]<(0+'0')||Inline[0]>(9+'0')) ){
#ifndef NOPRINTFS
											fprintf(stderr, "->%s<-", Inline);
											fprintf(stderr, "-> %c <-\n", Inline[0]);
											fprintf(stderr, "smallrequest, should generate response of %c bytes %ld \n", Inline[0],(long)( pow(2.0,(1.0)*(Inline[0]-'a'))));
#endif
											/* HACKED UP */
											done=1;
											REQ=20;
											r=20;
											serverthink=0;
											brate=22222;
											RSP=(long)( ((pow(2.0,1.0*(Inline[0]-'a'))) + ( pow(2.0,1.0*(Inline[0]-'a')+1))   )/2);
		//									RSP*=5;
#ifndef NOPRINTFS
											//fprintf(stderr, " Generate %ld bytes type 1 \n",RSP);
											fprintf(stderr, " Generate hacked %ld bytes type 2 \n",RSP);
#endif
											/*****/
											} else {




												if(Inline[0]>=('0'+0) && Inline[0]<=('0'+9)) {
													/* response has been sent directly */

#ifndef NOPRINTFS
													perror("response has been sent directly ");
#endif
													Inline[s]='\0';
													if(sscanf(Inline,"%ld", &RSP)!=1) {
														perror("canot read RSP directly ");
	//													fflush(stderr);
														exit(-1);
													}
#ifndef NOPRINTFS
													fprintf(stderr, " generate size %ld\n", RSP);
#endif
													REQ=strlen(Inline);
													serverthink=0;
													brate=22222;
													done=1;
													r=REQ;

												}
												else {
													perror("neither bytes sent nor code nor REQ ");
	//												fflush(stderr);
													exit(-1);
												}






											}


									
								}
							r+=s;
#ifndef NOPRINTFS
							fprintf(stderr, "received done=%d alldone=%d closeit=%d\n", done, alldone, closeit);
							fprintf(stderr, "received %ld bytes out of %ld \n", r, REQ);
#endif
							if(done==1)
								if(r>=REQ) thisdone=1;
							
						}
					//	REQ*=20;
					//	RSP*=20;
						
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv2,NULL);
			fprintf(stderr, " receive  over %ld %ld \n", slowtv2.tv_sec, slowtv2.tv_usec);
#endif

#ifndef NOPRINTFS
						fprintf(stderr, "got message  REQ %ld serverthink %ld RSP %ld brate %ld \n", REQ, serverthink, RSP, brate);
#endif
#ifdef NEWPRINTFS
						fprintf(stderr, "got message  REQ %ld serverthink %ld RSP %ld brate %ld \n", REQ, serverthink, RSP, brate);
#endif    

#ifdef DEBUG_LISTENER
						gettimeofday( &current_tv, NULL);
#endif
						/* maybe we want to completely get rid of server
						   logging */
#ifndef NOLOGGING
						printf("recd %d bytes process %d secs ",totreceived , process_int/1000 );
#endif
						//if (process_int>1000) sleep(process_int/1000);
						/* make the response */


						memset(tosendmessage,'1',LINESIZE);
						tosendmessage[LINESIZE-1]='\0';
						sprintf(tosendmessage,"RSP %ld %s ",RSP, TERM_STRING);
						curlength=strlen(tosendmessage);
#ifndef NOLOGGING
						fprintf(stderr,"\nsending response %s %d bytes \n", tosendmessage, curlength);
#endif
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv2,NULL);
			fprintf(stderr, " before send  %ld %ld \n", slowtv3.tv_sec, slowtv2.tv_usec);
#endif

						if(RSP>0 && RSP<LINESIZE) {
							if(RSP>curlength) {
#ifndef NOPRINTFS
								fprintf(stderr, " RSP > curlength\n");
#endif
								tosendmessage[curlength]='1';
								tosendmessage[RSP]='\0';
							} else {
#ifndef NOPRINTFS
								fprintf(stderr, " RSP <= curlength\n");
#endif
								tosendmessage[RSP]='\0';
								
								//RSP=curlength;
#ifndef NOPRINTFS
								fprintf(stderr, "changinre RSP to %ld\n", RSP);
#endif
								
								if(sock_type==SOCK_STREAM) {
									/* hacl */
#ifndef NOPRINTFS
									fprintf(stderr, " hack to send response \n");
#endif
									tosendmessage[RSP]='\0';
								}
							}
						} else {

#ifndef NOPRINTFS
								fprintf(stderr, " RSP > LINESIZE\n");
#endif
							tosendmessage[curlength]='1';
							tosendmessage[LINESIZE-1]='\0';
						}
#ifndef NOPRINTFS
						fprintf(stderr, " RSP here is %ld %s\n", RSP, tosendmessage);
#endif
						if(serverthink>=1) sleep(serverthink); /* sleep for serverthink secs */
						/* send the response */
						sent=0;
						tosend=RSP;
						transport=TCP;
						while(sent<tosend) {
							//sent+=sendmessage(fd,tosendmessage, brate, min((tosend - sent),min(PKTSIZE,LINESIZE)),(transport==TCP ? TCP:UDP), (struct sockaddr *)&  server , server_len );
							//sent+=sendmessage(fd,tosendmessage, brate, min((tosend - sent),max(PKTSIZE,LINESIZE)),(transport==TCP ? TCP:UDP), (struct sockaddr *)&  server , server_len );
							sent+=sendmessage(fd,tosendmessage, brate, tosend,(transport==TCP ? TCP:UDP), (struct sockaddr *)&  server , server_len );
							if(sent<LINESIZE) {
								memset(tosendmessage,'1',LINESIZE);
								tosendmessage[LINESIZE-1]='\0';
								if((sent+LINESIZE-1) > tosend) {
									tosendmessage[tosend - sent ]='\0';
								}
							}
#ifndef NOPRINTFS
							fprintf(stderr, "sent %ld bytes out of %ld \n", sent, tosend);
#endif
#ifdef NEWPRINTFS
							fprintf(stderr, "sent %ld bytes out of %ld \n", sent, tosend);
#endif
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv2,NULL);
			fprintf(stderr, " an iteration of  send  %ld %ld \n", slowtv2.tv_sec, slowtv2.tv_usec);
#endif
						}
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv2,NULL);
			fprintf(stderr, " sent  %ld %ld \n", slowtv2.tv_sec, slowtv2.tv_usec);
#endif
#ifndef NOPRINTFS
						fprintf(stderr, "sent total bytes %ld to server \n", sent);
#endif
#ifdef NEWPRINTFS
						fprintf(stderr, "sent total bytes %ld to server \n", sent);
#endif
#ifdef DEBUG_REQRSPSIZE
								fprintf(stderr, "DEBUGREQRSPSIZE: original/actual RSP: %ld %ld\n", RSP, sent);
#endif


					//	exit(0);

						

					  if(closeit==1) alldone=1;
					  else done=0;
#ifndef NOPRINTFS
					  fprintf(stderr, "this is pairnumber %d\n", pairnumber++);
#endif
#ifdef NEWPRINTFS
					  pairnumber++;
#endif
					}
#ifdef DEBUG_SLOWNESS
			gettimeofday(&slowtv2,NULL);
			fprintf(stderr, " closing socket  %ld %ld \n", slowtv2.tv_sec, slowtv2.tv_usec);
#endif

					//perror("r is negative");
					//fflush(stderr);
#ifdef DEBUG_LISTENER
					perror(" r is negative ");
					fprintf(stderr,"closing socket for flowid %d\n", flowid);
#endif
	if(transport==TCP)	{
#ifndef NOPRINTFS
		fprintf(stderr, "closing this connection\n");
#endif
		close(fd);
	}
				} // if sock_type== SOCK_DGRAM ... else clause
closeit:
#ifndef NOLOGGING
				printf("remote conn closed:\n");
#endif
#ifdef DEBUG_LISTENER
				printf("remote conn closed:\n");
#endif
				//fflush(stdout);
//thisonde:
				thisdone=1;

			}
#ifndef NOLOGGING
			printf("child over\n");
	//		fflush(stdout);
#endif
#ifdef DEBUG_LISTENER
			printf("child over\n");
#endif
			//close(fd);
			/* cleanup */
		//	free(k);
		//	free(srvr);
		//	free(clnt);
			free(tosendsize);
			/* cleanup over */
	//		print_memory_leaks();
			exit(1);
		}
		else
		{
#ifdef DEBUG_LISTENER
			printf("this is parent doing nothing\n");
#endif
			if(sock_type==SOCK_STREAM) close(fd);
		//	while((cid=(waitpid(-1, &retVal, WNOHANG)))>0);
		//	if(cid<0) perror("waitpid: ");
			//sleep(1);
			//fflush(stdout);
		}
	} /* while (!done) */
	/* should never reach here but anyway.... */
	printf("closing socket \n");
	//fflush(stdout);
 //	while((cid=wait(&retVal))!=-1)
//	{	
//		fprintf(stderr,"waiting for child to join pid=%d \n",cid);
		//sleep(1);
//	}
	close(sock);

	exit(0);
    //return 0;
}

/****/
