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

#include "hashtable_cwc22.h"
#include "hashtable_itr_cwc22.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* for memcmp */
#include <sys/time.h> /* for timeofday */
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<signal.h>
#include<sys/wait.h>
#include<errno.h>
#include<unistd.h>
#include<netdb.h>

char *tptr;
char * fptr;
int modifiedrsp;
char timescpy[1024];
char addcpy[1024];
/*
# define EXTRACT_TIMES_AND_ADD(arg, times, add)  { \
	strcpy(timescpy,arg); \
	tptr = strchr(timescpy, 't'); \
	if(tptr==NULL) { \
		times=atof(timescpy); \
		add=0.0; \
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
#define QUICKIE_STRCMP(a,b) (*(a)!=(*b) ? (int) ( (unsigned char ) *(a) - (unsigned char ) *(b) ) : strcmp((&a[1]),(&b[1])))
//#define QUICKIE_STRCMP(a,b) (strcmp(a,b))
//#define LOG_CNTS 1

#define TIMESRESP 1
#define MAXNUMCONN 1000
#define MAXINTERRRE (1000*1000)
#define MAXNUMRRE 1000
//#define LINESIZE 1024


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

float SWINGPROJECT_numpairs_TIMES, SWINGPROJECT_numconn_TIMES, SWINGPROJECT_REQ_TIMES, SWINGPROJECT_RSP_TIMES;
float SWINGPROJECT_numpairs_ADD, SWINGPROJECT_numconn_ADD, SWINGPROJECT_REQ_ADD, SWINGPROJECT_RSP_ADD;
float SWINGPROJECT_numpairs_FACTOR, SWINGPROJECT_numconn_FACTOR, SWINGPROJECT_REQ_FACTOR, SWINGPROJECT_RSP_FACTOR;
#include "workgen_header.h"

extern void generate_zipf(char *, char *, char *);
extern int genfromfile(char * , int, int **, int*);


int RANDSEED;

struct key
{
	char * class_name;
};

struct paramstruct {
	enum parametersnames kind;
	/* for file kind=0 */
	char filename[50];
	int *arrayentries;
	int numofentries;
	int currentindex;
	udp_req * udp_arrayentries;
	int seed;
	int medianval;
	
	/* for fixed kind=1*/

	int fixedvalue;

	/* for rand kind=2*/
	/* SEED is common from above */

	int maxrand;
	int minrand;
	/* for function kind=3*/
	char funcname[20];
	int numfuncparams;
	int * funcparams;
};

struct value
{
	struct paramstruct  table[numofparams];
	char class_name[30];
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
    return (0 == QUICKIE_STRCMP((( struct key *) k1)->class_name,(( struct key *)  k2)->class_name));
}

static int
vequalkeys(void *k1, void *k2)
{
	return( 0 == memcmp(k1,k2,sizeof(struct vkey)));
}


struct hashtable *h;
unsigned long dummy_port;
struct key * k ; 
struct hashtable *vh;
struct vkey *vk;
struct vvalue  *vv, *vfound;
struct value *v , *found;
int sessid=0;
enum protocols { UDP=-1 ,TCP=1} ;


udp_req getparamvalue_udp ( struct value * found, enum parametersnames param ) {
	struct paramstruct * myparam;
	udp_req toreturn;
	myparam=&(found->table[param]);
	//fprintf(stderr, "here \n");
	//fprintf(stderr, "param %d kidn %d \n",param,  myparam.kind);
	if(myparam->kind==file) {
		/* return the next value */
		toreturn.bytes = (myparam->udp_arrayentries[myparam->currentindex]).bytes;
		toreturn.pkts = myparam->udp_arrayentries[myparam->currentindex].pkts;
		toreturn.duration = myparam->udp_arrayentries[myparam->currentindex].duration;
		myparam->currentindex=(myparam->currentindex +1) % myparam->numofentries;
	} 
	return toreturn;
}


int getmedianvalue ( struct value * found, enum parametersnames param ) {

	struct paramstruct * myparam;
	int toreturn=-999;
	//int i;
	myparam=&(found->table[param]);
	if(param==param_brate) {
		//if(found->table[param_transport].fixedvalue==TCP)
			return 2222;
	}
	//fprintf(stderr, "here \n");
	//fprintf(stderr, "param %d kidn %d \n",param,  myparam.kind);
	if(myparam->kind==file) {
		/* return the median value */
		toreturn = myparam->medianval;
	} else {
			 toreturn=0;
	}
	return toreturn;

}

int getparamvalue ( struct value * found, enum parametersnames param ) {
	struct paramstruct * myparam;
	int toreturn=-999;
	//int i;
#ifdef DEBUG
	if(param==param_interconn)
		fprintf(stderr," param_interconn ");
#endif
	myparam=&(found->table[param]);
	if(param==param_brate) {
		//if(found->table[param_transport].fixedvalue==TCP)
			return 2222;
	}
	//fprintf(stderr, "here \n");
	//fprintf(stderr, "param %d kidn %d \n",param,  myparam.kind);
	if(myparam->kind==file) {
		/* return the next value */

#ifdef DEBUG
	if(param==param_interconn)
		fprintf(stderr," param_interconn file curindex=%d", myparam->currentindex);
#endif
		toreturn = myparam->arrayentries[myparam->currentindex];
 
#ifdef DEBUG
	if(param==param_interconn)
		fprintf(stderr," will advance cur inde ");
#endif
		myparam->currentindex=(myparam->currentindex +1) % myparam->numofentries;
#ifdef DEBUG
	if(param==param_interconn)
		fprintf(stderr," about to treturn from getparamvalue ");
#endif
		//if(param==param_REQ) {
		//	for(i=0;i<myparam->numofentries;i++)
		//		printf("%d\n", myparam->arrayentries[i]);
		//	exit(1);
		//}
	} else if(myparam->kind==randval ){
		toreturn=(int)(drand48()*(myparam->maxrand - myparam->minrand) + myparam->minrand);
		if(toreturn>myparam->maxrand) toreturn = myparam->minrand;
	} else if(myparam->kind==fixed ){
	//	fprintf(stderr, "it is fixed \n");
		toreturn = myparam->fixedvalue;
	} else if(myparam->kind==function){
	}
	if ( toreturn==0 ) { if (param==param_serverthink) return 0; 
		return 0; }
	else {
		if(param==param_RSP) {
			if(SWINGPROJECT_RSP_TIMES!=-1) {
				toreturn=toreturn*SWINGPROJECT_RSP_TIMES;
				toreturn=toreturn+SWINGPROJECT_RSP_ADD;
				toreturn=toreturn*SWINGPROJECT_RSP_FACTOR + ( 1 - SWINGPROJECT_RSP_FACTOR) * myparam->medianval;
				if (toreturn<=0) toreturn=100;
			}
			return toreturn*TIMESRESP;
		}
		else if(param==param_REQ){
			if(SWINGPROJECT_REQ_TIMES!=-1){
				toreturn=toreturn*SWINGPROJECT_REQ_TIMES;
				toreturn=toreturn+SWINGPROJECT_REQ_ADD;
				toreturn=toreturn*SWINGPROJECT_REQ_FACTOR + ( 1 - SWINGPROJECT_REQ_FACTOR) * myparam->medianval;
				if (toreturn<=0) toreturn=100;
			}
			return toreturn;
		}
		else return toreturn;
	}
}
	
	
void Usage(char * argv[]){
	printf("Usage: %s [timecommonfile] [repeattimes] [offset] [RAND SEED]\n", argv[0]);
	exit(-1);
	return ; 
}


char this_line[1024];

int populate(struct value *v, char *thisline) 
{
	char thisparam[20] , thiskind[10], thisfile[20], dummyseed[20];
	int thisvalue, thisseed;
	struct paramstruct * thisstruct;
	int min, max;
	char dummymin[20], dummymax[20];


	if(thisline[0]=='#') return -999;
	if(sscanf(thisline, "%s %s", thisparam, thiskind)!=2) return -999;
	/* assume below if wellformed */
	//thisstruct=v->table[thisparam];

	if(!QUICKIE_STRCMP(thisparam,"param_REQ"))
		thisstruct=&(v->table[param_REQ]);
	else if(!QUICKIE_STRCMP(thisparam,"param_RSP"))
		thisstruct=&(v->table[param_RSP]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_numpairs"))
		thisstruct=&(v->table[param_numpairs]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_serverthink"))
		thisstruct=&(v->table[param_serverthink]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_userthink"))
		thisstruct=&(v->table[param_userthink]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_brate"))
		thisstruct=&(v->table[param_brate]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_protocol"))
		thisstruct=&(v->table[param_protocol]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_pktsize"))
		thisstruct=&(v->table[param_pktsize]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_numconn"))
		thisstruct=&(v->table[param_numconn]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_numrre"))
		thisstruct=&(v->table[param_numrre]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_interRRE"))
		thisstruct=&(v->table[param_interRRE]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_interconn"))
		thisstruct=&(v->table[param_interconn]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_serverdist"))
		thisstruct=&(v->table[param_serverdist]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_clientdist"))
		thisstruct=&(v->table[param_clientdist]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_port"))
		thisstruct=&(v->table[param_port]);
	 else if(!QUICKIE_STRCMP(thisparam,"param_transport"))
		thisstruct=&(v->table[param_transport]);
	else {
		//fprintf(stderr, "unknon param ");
		return -999;
	}

	if(QUICKIE_STRCMP(thiskind,"fixed")==0) {
		if(sscanf(thisline, "%s %s %d", thisparam, thiskind, &thisvalue)!=3) return -999;
		thisstruct->fixedvalue=thisvalue;
		thisstruct->kind=fixed;
	//	fprintf(stderr," fixed val %d\n", thisstruct->fixedvalue);
	}
	else if(!QUICKIE_STRCMP(thiskind,"file")){
		if(sscanf(thisline,"%s %s %s %s %d",thisparam, thiskind, thisfile,dummyseed,  &thisseed)!=5) return -999;
		thisseed=RANDSEED;

		thisstruct->seed=thisseed;
		strcpy(thisstruct->filename, thisfile);
		if(!QUICKIE_STRCMP(v->class_name,"UDP_like") && (!QUICKIE_STRCMP(thisparam,"param_RSP") || !QUICKIE_STRCMP(thisparam,"param_REQ"))) {
	//		fprintf(stderr, "udplike genfrom file\n");

			thisstruct->numofentries= genfromfile_udp(thisstruct->filename, thisstruct->seed, &(thisstruct->udp_arrayentries));
			thisstruct->seed=RANDSEED;
		}
		else {
		thisstruct->numofentries= genfromfile(thisstruct->filename, thisstruct->seed, &(thisstruct->arrayentries), &(thisstruct->medianval));
		}
		if(thisstruct->numofentries>0) thisstruct->currentindex=((int)( drand48()*thisstruct->numofentries)) % thisstruct->numofentries;
		else thisstruct->currentindex=0;
		thisstruct->kind=file;
	//	fprintf(stderr, " SEED %d %d %d\n", thisstruct->seed,  thisstruct->currentindex, thisstruct->numofentries);
	}
	else if(!QUICKIE_STRCMP(thiskind, "randval")){
	//	fprintf(stderr, "randval\n");
		if(sscanf(thisline,"%s %s %s %d %s %d",thisparam, thiskind, dummymin, &min, dummymax ,&max)!=6) return -999;
		thisstruct->maxrand=max;
		thisstruct->minrand=min;
		thisstruct->kind=randval;
	//	fprintf(stderr, "min %d max %d rand \n", thisstruct->minrand, thisstruct->maxrand);
	}
	else {
		return -999;
	}
	if(!(QUICKIE_STRCMP(thisparam,"param_port"))) {
	//	fprintf(stderr, "this is port");
	//	fprintf(stderr, " kind %d val %d\n",thisstruct->kind, thisstruct->fixedvalue);
	//	fprintf(stderr, " kind %d val %d\n", v->table[param_port].kind, v->table[param_port].fixedvalue);
	}
//	fprintf(stderr, "%s %s\n", thisparam, thiskind);
	return 0;
}

int main(int argc, char *argv[]) {
	char * timecommonfile;
	char * newfilename;
	FILE * foriginal, *fnew, *fstartlistener;
	FILE * specsfile=fopen(SPECSFILE,"r") ; 
	FILE * vipfile=fopen(HOSTSFILE,"r") ; 
	int repeattimes;
	char new_line[ LINESIZE];
	char CLASS[20];

	int vnclientnum, vnservernum, offsetval;
	int offset,i;
	int numrre;
	int req, resp,times, numreq;
	int rretimes;
	int interRRE;
	int interconn;
	int conntimes;
	int numconn;
	int reqthink;
	int serverthink;
	int brate;
	int portnum;
#ifdef LOG_CNTS
	int reqrsp_logcnt=0;
	int numpairs_logcnt=0;
	int numrre_logcnt=0;
#endif
	enum protocols transport;

	if(argc<5)
		Usage(argv);
	timecommonfile=argv[1];
	repeattimes=atoi(argv[2]);
    offset=atoi(argv[3]);
	RANDSEED=atoi(argv[4]);

	if(argc>=6) {
		EXTRACT_TIMES_AND_ADD(argv[5],SWINGPROJECT_RSP_TIMES, SWINGPROJECT_RSP_ADD, SWINGPROJECT_RSP_FACTOR);
	} else {
		SWINGPROJECT_RSP_TIMES=-1;
	}
	if(argc>=7) {
		EXTRACT_TIMES_AND_ADD(argv[6],SWINGPROJECT_REQ_TIMES, SWINGPROJECT_REQ_ADD, SWINGPROJECT_REQ_FACTOR);
	} else {
		SWINGPROJECT_REQ_TIMES=-1;
	}
	if(argc>=8) {
		EXTRACT_TIMES_AND_ADD(argv[7],SWINGPROJECT_numconn_TIMES, SWINGPROJECT_numconn_ADD, SWINGPROJECT_numconn_FACTOR);
	} else {
		SWINGPROJECT_numconn_TIMES=-1;
	}
	if(argc>=9) {
		EXTRACT_TIMES_AND_ADD(argv[8],SWINGPROJECT_numpairs_TIMES, SWINGPROJECT_numpairs_ADD, SWINGPROJECT_numpairs_FACTOR);
	} else {
		SWINGPROJECT_numpairs_TIMES=-1;
	}

	//if(repeattimes==1) exit(0);

	newfilename=(char *) malloc((strlen(timecommonfile)+4)*sizeof(char));
	sprintf(newfilename,"%s.new",timecommonfile);
	foriginal=fopen(timecommonfile,"r");
	if(!foriginal) {
		perror("opening file timecommonfile");
		exit(-1);
	}
	fnew=fopen(newfilename,"w");
	//fstartlistener=fopen(STARTLISTENERFILENAME,"w");
	fstartlistener=fopen("startlistenerfile","w");
    if(!fstartlistener) {
		perror("opening file STARTLISTENERFILENAME");
		exit(-1);
	}
	if(!fnew) {
		perror("opening file timecommonfile.new");
		exit(-1);
	}

	if(!specsfile) {
		perror("opening file SPECSFILE ");
		exit(-1);
	}
	if(!vipfile) {
		perror("opening file HOSTSFILE ");
		exit(-1);
	}
    h = create_hashtable(160, 0.75, hashfromkey, equalkeys);
	vh=create_hashtable(160,0.75,vhashfromkey,vequalkeys);
    if (NULL == h) { perror("could not make hashtable "); exit(-1) ;} /*oom*/
	if (NULL == vh){ perror("could not make hashtable ");  exit(-1) ;} /* oom */

	//SEED=atoi(argv[2]);
	found=NULL;
	vfound=NULL;
	k=NULL;
	v=NULL;
	vk=NULL;
	vv=NULL;


	/* vip file start */
	fprintf(stderr, " vip file start ");
	while(fgets(this_line,80,vipfile))
	{
		if(strncmp(this_line,"client",5)==0)
		{
			vk = (struct vkey *) malloc(sizeof(struct vkey ));
			vv = (struct vvalue *) malloc(sizeof(struct vvalue));
			vv->vip=(char*)malloc(sizeof(char)*26);
			sscanf(this_line, "client%d %s # on unknown", &vk->vn_num,vv->vip);
			//fprintf(stderr, " vn %d ip %s \n", vk->vn_num, vv->vip);
#ifdef DEBUG
			fprintf(stderr, " VN %d\n", vk->vn_num);
#endif
			vfound=NULL;
			if (NULL==(vfound=vsearch_some(vh,vk))){
		//		printf("did not find it\n");
			}
			else {
				fprintf(stderr, "something wrong... already in hashtable \n");
			}
			if(!vinsert_some(vh,vk,vv)) { perror("could not insert in hashtable"); exit(-1); }
			if (NULL==(vfound=vsearch_some(vh,vk))){
				printf("did not find it\n");
			}
			else
			{
			//	if(vk->vn_num==601)
			//		fprintf(stderr, "vn %d vip %s %x %x \n", vk->vn_num, vfound->vip,&( vfound->vip), &vfound);
				//printf("vip of vn %d is %s \n", vk->vn_num, vv->vip);
			}

		}
	}
	fprintf(stderr, "\r vip file over          ");
	/* vip file end */
#if 0
	vk=(struct vkey*) malloc(sizeof(struct vkey));
	for(i=0;i<1000;i++)
	{
		vk->vn_num=i;
			if (NULL==(vfound=vsearch_some(vh,vk))){
				printf("did not find it\n");
			}
			else fprintf(stderr, "vn %d vip %s %x \n", i, vfound->vip, &(vfound->vip));
		
	}
	exit(0);
#endif
	/* specs file start */

	v=NULL;
	fprintf(stderr, "\r specs file begin        ");
	while(fgets(this_line,80,specsfile)) {
	//	fprintf(stderr, "this line %s\n", this_line);
		if(this_line[0]=='#') goto skipthislineforspecsfile;
		if(strncmp(this_line, "class", 5)==0)
		{
			if(v!=NULL){
				if(!insert_some(h,k,v)) { perror("could not insert in hashtable ") ; exit(-1); }
				if(NULL==(found=search_some(h,k))){
					printf("did not find it\n");
				}
				else {
					//printf("class %s listens on port %d numxchng_max %d\n",k->class_name, found->listen_port, found->numxchng_max);
				}
			}
			k = (struct key *) malloc(sizeof(struct key ));
			v = (struct value *) malloc (sizeof(struct value));
			v->transport=0;
			v->resp_zipfres=0;
			k->class_name = (char *)
				malloc(sizeof(char)*strlen(&this_line[6]) +1 );
			memset(k->class_name,0x0,1);
			sscanf(&(this_line[6]),"%s",k->class_name);

			strcpy(v->class_name, k->class_name);
	//		fprintf(stderr, "starting class %s\n", v->class_name);

		}
		else if(populate(v, this_line)==-999) {

	//		fprintf(stderr, "not successful %s \n", this_line);
		}
#if 0
		else if (strncmp(this_line,"port",4)==0)
		{
			sscanf(&(this_line[5]),"%lu",&dummy_port);
			v->listen_port=dummy_port;
			if(!insert_some(h,k,v)) { perror("could not insert in hashtable ") ; exit(-1); }
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
#endif
skipthislineforspecsfile:
		sleep(0);
	}
	fprintf(stderr, "\r specs file over         ");
	//fprintf(stderr, " here \n");
	goto skipthispart;	
//exit(1);
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
			if(!insert_some(h,k,v)) { perror("could not insert in hashtable ") ; exit(-1); }
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

	
	/* specs file end */


skipthispart:
	//exit(1);

	
	//fnew=stdout;

	//exit(0);
#ifdef LOG_CNTS
	reqrsp_logcnt=0;
	numpairs_logcnt=0;
	numrre_logcnt=0;
#endif
	for(i=0;i<repeattimes;i++) {
		printf("\r i=%d              ",i);
		while(fgets(new_line, LINESIZE, foriginal)){
			numrre=2;
			interRRE=2;
			interconn=0; /* at the moment this like interRRE will be in milliseconds */
			numconn=2;
			numreq=2;
			req=444;
			resp=33333;
			reqthink=1;
			portnum=9980;
			serverthink=10;
			brate=22222;
			transport=TCP;
			
			if(sscanf(new_line,"GENERATE %s vn%d client vn%d server TIME %d",CLASS, &vnclientnum , &vnservernum , &offsetval)!=4) {
				perror("cant read 4 fields from new generate line ");
				printf("%s", new_line);
				exit(1);
			}
#ifdef DEBUG
			fprintf(stderr, " %s", new_line);
#endif
			//printf("GENERATE %s vn%d client vn%d server TIME %d\n", CLASS,vnservernum, vnclientnum, offsetval);
			//fprintf(fnew,"GENERATE %s vn%d client vn%d server TIME %d numrre %d ", CLASS,vnclientnum, vnservernum, offsetval+offset*i, numrre);
	//		fprintf(stderr, "CLASS\n");
			k=(struct key *) malloc(sizeof(struct key));
			k->class_name=strdup(CLASS);
			if(NULL==(found=search_some(h,k))) {
				fprintf(stderr, "CLASS %s not found \n", k->class_name);
				exit(-1);
			} else {
			//	fprintf(stderr, "CLASS name %s port %d \n", k->class_name, getparamvalue(found, param_port));
			}
		//	fprintf(stderr, "vip\n");
			vk=(struct vkey *) malloc(sizeof(struct vkey));
			vk->vn_num=vnclientnum;
			if(NULL==(vfound=vsearch_some(vh,vk))) {
				fprintf(stderr, "could not find vn %d it in hashtable  for class\n", vnclientnum);
				exit(-1);
			}
		//	else fprintf(stderr, "vn %d ip %s \n", vnclientnum, vfound->vip);
			fprintf(fnew,"GENERATE sessid %d %s client %d ",sessid++, CLASS,vnclientnum );
			//fprintf(fnew,"GENERATE sessid %d %s client %s ",sessid++, CLASS,vfound->vip );
			numrre=getparamvalue(found, param_numrre);
			numrre=min(numrre, MAXNUMRRE);
			//numrre=(numrre>10?10:numrre);
			//changed
			//numrre=1;
	//		fprintf(stderr, "found numrre %d \n", numrre);
			fprintf(fnew,"TIME %d numrre %d ",(int)(offsetval+offset*i), numrre);
			for(rretimes=0;rretimes<numrre;rretimes++) {
#ifdef DEBUG
				fprintf(stderr, " RRE %d ", rretimes);
#endif
				if(rretimes >0 ) {
					interRRE=getparamvalue(found, param_interRRE);
					interRRE=min(interRRE,MAXINTERRRE);
					fprintf(fnew, "interRRE %d RRE %d ",interRRE,  rretimes+1);
				}
				else
					fprintf(fnew, "RRE %d ", rretimes+1);

				 numconn=getparamvalue(found, param_numconn);
#ifdef LOG_CNTS
				 numrre_logcnt++ ;
				 fprintf(stdout, " conn %d\n", numconn);
#endif
				 //numconn=(numconn>10?10:numconn);
				// changed
				//numconn=10 ;
				if(SWINGPROJECT_numconn_TIMES!=-1){
					numconn=numconn*SWINGPROJECT_numconn_TIMES;
					numconn=numconn+SWINGPROJECT_numconn_ADD;
					numconn=numconn*SWINGPROJECT_numconn_FACTOR + ( 1 - SWINGPROJECT_numconn_FACTOR) * getmedianvalue(found, param_numconn);
				}
				 numconn=min(numconn,MAXNUMCONN);
				fprintf(fnew,"NUMCONN %d ", numconn) ;
	//			fprintf(stderr,"num conn %d\n", numconn);
				for(conntimes=0; conntimes< numconn; conntimes++){
#ifdef DEBUG
				fprintf(stderr, " conn %d ", conntimes);
#endif
					vk=(struct vkey *) malloc(sizeof(struct vkey));
					vk->vn_num=vnservernum;
					if((vfound=vsearch_some(vh,vk))==NULL) {
						fprintf(stderr, "could not find %d in hashtable for numconn \n", vnservernum);
						perror("not found ");
						exit(-1);
					}
				//	else fprintf(stderr, "found %d %s %x %x \n", vk->vn_num, vfound->vip, &(vfound->vip), &vfound);
					fprintf(fstartlistener, "%s %d %d %d\n",CLASS, vnservernum, getparamvalue(found,param_port), getparamvalue(found, param_transport));
					//fprintf(fnew,"CONN %d SERVER %d ", conntimes+1, vnservernum);
					if(!QUICKIE_STRCMP(CLASS,"UDP_like"))
						fflush(fnew);
					if(conntimes>0) {
#ifdef DEBUG
						fprintf(stderr," interconn ? ");
#endif
						fprintf(fnew,"interconn %d ", getparamvalue(found,param_interconn));
#ifdef DEBUG
						fprintf(stderr,"\r done               ");
#endif
					}
					fprintf(fnew,"CONN %d SERVER %s ", conntimes+1, vfound->vip);
					numreq=getparamvalue(found, param_numpairs);
					if(SWINGPROJECT_numpairs_TIMES!=-1){
						numreq=numreq*SWINGPROJECT_numpairs_TIMES;
						numreq=numreq+SWINGPROJECT_numpairs_ADD;
					numreq=numreq*SWINGPROJECT_numpairs_FACTOR + ( 1 - SWINGPROJECT_numpairs_FACTOR) * getmedianvalue(found, param_numpairs);
					}
#ifdef LOG_CNTS
					numpairs_logcnt++;
#endif
	//				numreq=min(numreq, MAXNUMPAIRS);
					fprintf(fnew,"port %d numpairs %d TRANSPORT %d ",getparamvalue(found,param_port),  numreq,getparamvalue(found,param_transport)==1?TCP:UDP);
					// changed
					//fprintf(fnew,"port %d numpairs %d TRANSPORT %d ",getparamvalue(found,param_port),  numreq=1,getparamvalue(found,param_transport)==1?TCP:UDP);
					
				//	transport=transport*-1;
					/* loop for rre */	
					for(times=0;times<numreq;times++) {
						if(!QUICKIE_STRCMP(CLASS,"UDP_like")) {
							udp_req thisreq=getparamvalue_udp(found, param_REQ);
							udp_req thisresp=getparamvalue_udp(found, param_RSP);
#ifdef LOG_CNTS
							reqrsp_logcnt++;
							if((reqrsp_logcnt%100) == 0) fprintf(stderr, "\r reqrsp_logcnt %d ", reqrsp_logcnt);
#endif
	//						fprintf(stderr, " this is udp like \n");
							if(times>0) 
							fprintf(fnew,"reqthink %d REQ %d %d %d serverthink %d RSP %d %d %d brate %d ", reqthink=getparamvalue(found,param_userthink) , thisreq.bytes, thisreq.pkts, thisreq.duration, serverthink=getparamvalue(found, param_serverthink),  thisresp.bytes, thisresp.pkts, thisresp.duration, brate=getparamvalue(found,param_brate));
							else
							fprintf(fnew,"REQ %d %d %d serverthink %d RSP %d %d %d brate %d ", thisreq.bytes, thisreq.pkts, thisreq.duration, serverthink=getparamvalue(found, param_serverthink),  thisresp.bytes, thisresp.pkts, thisresp.duration, brate=getparamvalue(found,param_brate));

						} else {
							//fprintf(stderr, " this is not udp like \n");
						if(times>0)
							fprintf(fnew,"reqthink %d REQ %d serverthink %d RSP %d brate %d ", reqthink=getparamvalue(found,param_userthink) , getparamvalue(found, param_REQ), serverthink=getparamvalue(found, param_serverthink),  getparamvalue(found, param_RSP), brate=getparamvalue(found,param_brate));
						else
						{
							modifiedrsp=getparamvalue(found,param_RSP);
							/*
							 * modifiedrsp=6291456;
							 * basically 6 MB above for youtube-ish
							 */
							fprintf(fnew,"REQ %d serverthink %d RSP %d brate %d ", getparamvalue(found, param_REQ),serverthink=getparamvalue(found, param_serverthink),  modifiedrsp, brate=getparamvalue(found,param_brate));
							/*fprintf(fnew,"REQ %d serverthink %d RSP %d brate %d ", getparamvalue(found, param_REQ),serverthink=getparamvalue(found, param_serverthink),  getparamvalue(found, param_RSP), brate=getparamvalue(found,param_brate));
							 */
						}
							//changed
						//	fprintf(fnew,"REQ %d serverthink %d RSP %d brate %d ", getparamvalue(found, param_REQ),serverthink=1,   22333, brate=getparamvalue(found,param_brate));
						}
					}
				}
				
			}
#ifdef DEBUG
			fprintf(stderr, "\n");
#endif
			fprintf(fnew,"\n");
		}
		rewind(foriginal);
#ifdef LOG_CNTS
	fprintf(stderr, " totoal calls %d numparirssum %d numrre %d\n", reqrsp_logcnt, numpairs_logcnt, numrre_logcnt);
	reqrsp_logcnt=0; numpairs_logcnt=0; numrre_logcnt=0;
#endif
	}
	printf("\n");
	return 0;
}
