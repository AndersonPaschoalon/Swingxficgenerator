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
#include<iostream>
#define numperclass 2
#define MAXLINELENGTH 200
#define LINESIZE MAXLINELENGTH

/* in classes.cdf first column is probability i.e. interarrivaltime i think */
float NUMSESSION_TIMES=1.0;
float PRCHTTP=1.0;
float effectivenumsession=1.0;
//#define DEBUG 1
char this_line[MAXLINELENGTH];
char classname[MAXLINELENGTH];
float	perc_req, perc_resp, time_factor;
int num_users;
float default_time;
int skipperiod;
int generatefromcdf;
class generate_class {
	public :
		void setName(char * name) ;
		char * class_name;
		
		float perc_req, perc_resp,time_factor;
		int num_users;
		float * cdfx;
		float * cdfy;
		float  starttime;
		float  totaltime;
		float lasttime;
		int idx;
		int cdfnum;

};

void generate_class::setName(char * name) {
				  class_name=name;
}


generate_class * gen_class=NULL;

#if 0

int search(float searchnum, float * cdfclienty, int cdfclientnum) {
	int i;
	for (i=0;i<cdfclientnum;i++) {
		if(cdfclienty[i]>searchnum) return (i);
	}
	return i-1;
}
void printarray(float * ff, int n) {
	for(int i=0;i<n;i++)
		printf("array[%d]=%f\n",i,ff[i]);
}

gen_class *gclass=NULL;
void process_application(generic_class * ptr) {
	if(ptr!=NULL) {
		//char * class_name=ptr->class_name;
		/* for client first */
		FILE * openfile=fopen(ptr->cdffile_client,"r");
        char token[50];

		strcpy(token,"null");
		while(strcmp(token,"client")!=0) {
			fscanf(openfile,"%s",token);
		}
		fscanf(openfile,"%d",&ptr->clientnum);
		ptr->client_list=(char **) malloc(sizeof (char *) * ptr->clientnum); 
		for(int i=0;i<ptr->clientnum;i++) {
			ptr->client_list[i]=(char *) malloc(sizeof(char)*7);
			fscanf(openfile,"%s", ptr->client_list[i]);
		}
		while(strcmp(token,"server")!=0){
			fscanf(openfile,"%s",token);
		}
		fscanf(openfile,"%d",&ptr->servernum);
		ptr->server_list=(char **) malloc(sizeof (char *) * ptr->servernum); 
		for(int i=0;i<ptr->servernum;i++) {
			ptr->server_list[i]=(char *) malloc(sizeof(char)*7);
			fscanf(openfile,"%s", ptr->server_list[i]);
		}
		while(strcmp(token,"cdf")!=0){
			fscanf(openfile,"%s",token);
		}
		fscanf(openfile,"%d",&ptr->cdfclientnum);
		ptr->cdfclientx=(float *) malloc(sizeof(float)*ptr->cdfclientnum);
		ptr->cdfclienty=(float *) malloc(sizeof(float)*ptr->cdfclientnum);
		for(int i=0;i<ptr->cdfclientnum;i++) {
			fscanf(openfile,"%f %f", &ptr->cdfclientx[i], &ptr->cdfclienty[i]);
		}
		/* for server next */

		strcpy(token,"null");
		while(strcmp(token,"cdf")!=0){
			fscanf(openfile,"%s",token);
		}
		fscanf(openfile,"%d",&ptr->cdfservernum);
		ptr->cdfserverx=(float *) malloc(sizeof(float)*ptr->cdfservernum);
		ptr->cdfservery=(float *) malloc(sizeof(float)*ptr->cdfservernum);
		for(int i=0;i<ptr->cdfservernum;i++) {
			fscanf(openfile,"%f %f", &ptr->cdfserverx[i], &ptr->cdfservery[i]);
		}
		fclose(openfile);
	}
}

void printstats_application(generic_class * ptr) {
	if(ptr!=NULL) {

		printf("client %d\n",ptr->clientnum);
		for(int i=0;i<ptr->clientnum;i++) {
			printf("%s ",ptr->client_list[i]);
		}
		printf("\nserver %d\n",ptr->servernum);
		for(int i=0;i<ptr->servernum;i++) {
			printf("%s ", ptr->server_list[i]);
		}
		printf("\ncdf %d\n",ptr->cdfclientnum);
		for(int i=0;i<ptr->cdfclientnum;i++) {
			printf("%2.2f %2.2f\n", ptr->cdfclientx[i], ptr->cdfclienty[i]);
		}
	}
}

void generate_requests(char * this_class,int client_reserved, int client_normal,int  server_reserved,int  server_normal,int offset) {
    /* there are client_reserved clients at the time being and
	  server_reserved servers.
	  these figures will match for now at least 
      so pick one of each and print it
	 */
	/* get the ptr */
	generic_class * ptr;
	if(!strcmp("HTTP_like",this_class))  
	     ptr=httpc;
		
	/*else if(!strcmp("FTP",this_class)) ptr=ftpc;
	else if(!strcmp("SMTP",this_class)) ptr=smtpc;
	else if(!strcmp("P2P",this_class)) ptr=p2pc;*/
	else if (!strcmp("FTP_like",this_class))
		ptr=ftpc;
	else if (!strcmp("SMTP_like",this_class))
		ptr=smtpc;
	else if (!strcmp("P2P_like",this_class))
		ptr=p2pc;
	else if (!strcmp("MMSTREAM_like", this_class))
		ptr=mmstreamc;
	else { 
		return;
	}
	if (ptr==NULL ) {
		fprintf(stderr, "error: input files for %s not available to create requests\n",this_class);
		return;
	}
	int done=0;
	while(!done) {
		if(client_reserved==0 && client_normal ==0 && server_reserved==0 && server_normal ==0 ) { 
			done=1;
			continue;
		}

		int client=ptr->random_deviate("client");
		int server=ptr->random_deviate("server");
		//printf("GENERATE %d client %d server \n",client,server);
		printf("GENERATE %s %s client %s server TIME %d\n",this_class, ptr->client_list[client],ptr->server_list[server],offset);
		//printf("client_reserved = %d server_reserved = %d \n", client_reserved, server_reserved);
		client_reserved--;
		server_reserved--;
		
	}
}
#endif


int process_class(generate_class * gen_class) {
	/* for now ignore perc_resp, perc_resp time_Factor  etc. */
	int i;
	int seconds=0;
	int generated=0;
	for(i=0;i<gen_class->cdfnum;i++) {
		gen_class->cdfx[i]=gen_class->cdfx[i]*default_time;
		gen_class->cdfy[i]=gen_class->cdfy[i]*gen_class->num_users;
		if(gen_class->cdfx[i] - seconds > skipperiod || i==(gen_class->cdfnum -1 )) {
			//printf("%f %f\n", gen_class->cdfx[i], gen_class->cdfy[i]);
			seconds=(int) gen_class->cdfx[i];
			generated=(int) gen_class->cdfy[i] - generated;
			printf("GENERATE %s %d normal %d reserved %d normal_req %d reserved_req %d total offset %d Sat Dec 6 19:31:14 EST 2003\n", gen_class->class_name,0, generated, 0, generated, generated, seconds);

			/*
			   GENERATE  HTTP_like 0 normal 50 reserved 0 normal_req 50 reserved_req 50 total offset 2 Sat Dec 6 19:31:14 EST 2003
			   */
			generated=(int) gen_class->cdfy[i];
		}
	}
	return generated;
}

int searchcdf(double xval,float * cdfx, int cdfnum) {
	int i=0;
	//return 1+ (int)( xval*(cdfnum-2) );
	return (int)(1.5+  xval*(cdfnum-2) );
	
#if 0
	for(i=0;i<cdfnum-1;i++) {
		if(cdfx[i]>=xval) return i;
	}
	return i;
#endif
}

int process_class_cdf(generate_class * gen_class) {
	/* for now ignore perc_resp, perc_resp time_Factor  etc. */
	int i;
	int seconds=0;
	int sess_generated=0;
	int generated=0;
	int starttime=0;
	int thisx;
	float thisrand;
	float diff=0;
	int loopout=0;
	float timebin=0;
	float getx, gety;
	int less_sess=1;
	//while(timebin<=((int)default_time)) 
	//{
	//while(timebin<=(default_time*gen_class->lasttime) || less_sess==1) {
	//while(timebin<=(default_time*gen_class->lasttime) ) {
	printf(" -----------------------------------------------------------\n timebin %f default_time %f \n", timebin, default_time);
	//while(timebin<=(default_time)   ) {}
	while(timebin<(default_time)   ) 
	{
			fprintf(stdout,"START new entry*********default_time %f timebin = %f\n", default_time, timebin);
		printf(" less_sess %d sess_generated %d num_users %d ", less_sess, sess_generated, gen_class->num_users - 2);
		printf(" originally perhaps %d sessions \n", gen_class->num_users -2);
		printf(" lasttime %f def time  %f %f\n",gen_class->lasttime, default_time* gen_class->lasttime , default_time);
		thisrand=drand48();
		//thisx=searchcdf(thisrand, gen_class->cdfx, gen_class->cdfnum);
		if(!strncmp(gen_class->class_name, "HTTP",4)) {
			thisx=searchcdf(thisrand, gen_class->cdfy, gen_class->num_users/NUMSESSION_TIMES/PRCHTTP);
		} else {
			thisx=searchcdf(thisrand, gen_class->cdfy, gen_class->num_users/NUMSESSION_TIMES);
		}
		/* put some PRCHTTP here */
		getx=gen_class->cdfx[thisx];
		/* X Axis is time -> looks like */
			fprintf(stdout,"*********default_time %f\n", default_time);
		printf(" searched at random thisrand %f thisx %d gen_class->cdfx[thisx] %f gen_class->cdfy[thisx] %f \n", thisrand, thisx, gen_class->cdfx[thisx], gen_class->cdfy[thisx]);
		printf(" will advance by %f fraction of %f = %f \n", getx, default_time, 1.0*getx*default_time);
		//thisx=(int)(thisrand*gen_class->cdfnum);
		if(thisx!=(gen_class->cdfnum -1)){
			/* getx = time
			   gety = sessions */
			getx=(gen_class->cdfx[thisx+1] - gen_class->cdfx[thisx])*default_time;
			gety=(gen_class->cdfy[thisx+1] - gen_class->cdfy[thisx])*gen_class->num_users;
		//	printf(" %f sessions in %f time \n", gety, getx);
			if(getx>0){
				//generated=(int) (skipperiod*gety/getx);
				//diff=diff+(skipperiod* gety/getx - generated);
				generated=(int) gety;
				diff=diff+(gety - generated);
			}
			else generated=1;
		}
		else {
			getx=0;
			generated=1;
		}
		if(diff>1) {
			generated+=1;
			diff=diff-1;
		}

		//if((timebin+getx*default_time)<=default_time*gen_class->lasttime) {}

			fprintf(stdout,"*********default_time%f\n", default_time);
			fprintf(stdout,"*********generated=%d\n", generated);
			fprintf(stdout,"*********sess_generated + generated %d  (gen_class->num_users) %d\n", sess_generated+generated, gen_class->num_users);
			fprintf(stdout,"********* ( timebin + getx*default_time) %f <= (?) default_time %f\n", timebin+getx*default_time, default_time);

			if(!strncmp(gen_class->class_name, "HTTP",4)) {
				effectivenumsession=NUMSESSION_TIMES*PRCHTTP;
			} else {
				effectivenumsession=NUMSESSION_TIMES;
			}
		if((timebin+getx*default_time/effectivenumsession)<=default_time) {
			printf("yes\n");
			fprintf(stdout, " timebin %f getx*default_time %f <=  default_time %f\n", timebin, getx*default_time, default_time);

			printf("time bin before %f \n", timebin);
	//		printf("name=>%s\n", gen_class->class_name);
	//		if(!strncmp(gen_class->class_name, "SQUID",5)) {
			if(!strncmp(gen_class->class_name, "HTTP",4)) {
				printf(" squid numsess \n");
				printf(" instd of %f we will advance time by %f \n", (1.0*getx*default_time )/ (1.0), (1.0*getx*default_time )/ (1.0*NUMSESSION_TIMES*PRCHTTP) );
				printf("so a timebin of %f secs becomes ", timebin/1000);
				timebin+=(1.0*getx*default_time )/ (1.0*NUMSESSION_TIMES*PRCHTTP) ;

			} else {
				printf(" squid numsess \n");
				printf(" instd of %f we will advance time by %f \n", (1.0*getx*default_time )/ (1.0), (1.0*getx*default_time )/ (1.0*NUMSESSION_TIMES) );
				printf("so a timebin of %f secs becomes ", timebin/1000);
				timebin+=(1.0*getx*default_time )/ (1.0*NUMSESSION_TIMES) ;
			}
		//	else{
		//	timebin+=1.0*(getx)*default_time;
		//	}
			printf("time bin after %f \n", timebin/1000);
			seconds=(int) timebin;
			generated=1;
			fprintf(stdout,"generated=%d\n", generated);
			fprintf(stdout,"sess_generated + generated %d (gen_class->num_users) %d\n", sess_generated+generated, gen_class->num_users);
			//if((sess_generated + generated ) >(1+0.05)*(gen_class->num_users-2)) {}
	//		if((sess_generated + generated ) >(gen_class->num_users)) {}
			if((sess_generated + generated ) >(1+0.1)*(gen_class->num_users)) {
				fprintf(stdout,"will not print generate \n");
				generated=0;
				goto thisappover;
			} 
				else {
					fprintf(stdout, " will print generate\n");
				}
			if(generated>0) printf("GENERATE %s %d normal %d reserved %d normal_req %d reserved_req %d total offset %d Sat Dec 6 19:31:14 EST 2003\n", gen_class->class_name,0, generated, 0, generated, generated, seconds);
			sess_generated+=generated;
			if(sess_generated >=(1-0.05)*(gen_class->num_users -2 )) less_sess=0;
			loopout=0;
		}
		else {
			loopout++;
			printf("no\n");
			printf("loopout\n");
			fprintf(stdout, " timebin %f getx*default_time %f >  default_time %f\n", timebin, getx*default_time, default_time);
			if(loopout==10) goto thisappover;
		}
		printf(" time bin %f\n", timebin);
		printf("----------------------------------------------------------------------------\n");
		timebin+=getx;
	}
		printf(" time bin no longer < default_time\n");
thisappover:
	//sleep(0);
		printf(" -----------------------------------------------------------\n timebin %f default_time %d \n", timebin, default_time);
}

void Usage(char * argv[]) {
	printf("Usage %s: <generatefile> <default> <skipperiod> <0/1 for generating interarrival of sessionsfrom a cdf> <RANDSEED> <numsesstimes>\n",argv[0]);
	exit(1);
}

int initialize_class(char * thline) {
	float d1, d2;
//	printf("initializing class %s \n",thline);
	sscanf(thline,"class %s %f %f %f %d %f %f", classname, &perc_req, &perc_resp, &time_factor, &num_users, &d1, &d2);
//	printf("class %s %f %f %f %d\n", classname, perc_req, perc_resp, time_factor, num_users);
	gen_class=new generate_class;
	gen_class->setName(classname);
	gen_class->perc_req=perc_req;
	gen_class->perc_resp=perc_resp;
	gen_class->time_factor=time_factor;
	gen_class->num_users=num_users;
#ifdef DEBUG
	fprintf(stderr, "numusers <- %d ", gen_class->num_users);

#endif
	//if(!strncmp(classname, "SQUID", 5)) gen_class->num_users=(int) 1.0*gen_class->num_users*NUMSESSION_TIMES;
	gen_class->num_users=(int) 1.0*gen_class->num_users*NUMSESSION_TIMES;
	if(!strncmp(classname, "HTTP", 4)) gen_class->num_users=(int) 1.0*gen_class->num_users*PRCHTTP;
#ifdef DEBUG
	fprintf(stderr, " -> %d\n", gen_class->num_users);
#endif
	gen_class->totaltime=d1;
	gen_class->starttime=d2;
	gen_class->lasttime=0.0;
#ifdef DEBUG
	printf("class initialized totaltime %f %f\n", d1,gen_class->totaltime);
#endif
		
	return 1;
}
int process_file(FILE * generatefile) {
	while(fgets(this_line, LINESIZE, generatefile)) {
		if(this_line[0]=='#')
		{
			//printf("%s",this_line);
		}
		else if(!strncmp(this_line,"class", 5))
			initialize_class(this_line);
		else if(gen_class!=NULL) {
			if(!strncmp(this_line,"cdf",3))
			{
				sscanf(this_line,"cdf %d",&gen_class->cdfnum);
				gen_class->cdfnum=gen_class->cdfnum;
				gen_class->cdfx=(float *) malloc(sizeof(float) * gen_class->cdfnum);
				gen_class->cdfy=(float *) malloc(sizeof(float) * gen_class->cdfnum);
				gen_class->idx=0;
		//		printf("cdf %d\n",gen_class->cdfnum);

			}
			else {
				sscanf(this_line,"%f %f", &gen_class->cdfx[gen_class->idx],&gen_class->cdfy[gen_class->idx]);
				gen_class->cdfx[gen_class->idx]=gen_class->cdfx[gen_class->idx];
			//	printf("%f %f\n", gen_class->cdfx[gen_class->idx],gen_class->cdfy[gen_class->idx]);
				gen_class->idx++;
				if(gen_class->idx < gen_class->cdfnum) gen_class->lasttime+=gen_class->cdfx[gen_class->idx-1];
#ifdef DEBUG
				printf("lasttime %.6f x=%.6f y=%.6f i=%d\n", gen_class->lasttime, gen_class->cdfx[gen_class->idx-1], gen_class->cdfy[gen_class->idx-1], gen_class->idx-1);
#endif
				if(gen_class->idx==gen_class->cdfnum){
#ifdef DEBUG
					printf(" this class over \n");
#endif
					if(generatefromcdf==1) {
#ifdef DEBUG
						printf(" process from cdf \n");
#endif
						gen_class->cdfx[gen_class->idx-1]=1-gen_class->lasttime;
#ifdef DEBUG
						printf("gen_class->cdfx[last]=%.6f idx %d\n", gen_class->cdfx[gen_class->idx-1], gen_class->idx-1);
#endif
						process_class_cdf(gen_class);
					}
					else {
						process_class(gen_class);
					}
					delete gen_class;
				}
			}
		}
	}
	return 1;
}

int main (int argc , char * argv[]) {

    
	int i;
	FILE *generatefile;
	char * classname;
	srand48(444);
	if (argc<5) 
		Usage(argv);
	
	generatefile=fopen(argv[1],"r");
	default_time=atof(argv[2]);
	skipperiod=  atoi(argv[3]);
	if(argc>=5)generatefromcdf=atoi(argv[4]);
	else generatefromcdf=0;
	if(argc>=6) srand48(atol(argv[5]));
	if(argc>=7) NUMSESSION_TIMES=atof(argv[6]);
	if(argc>=8) PRCHTTP=atof(argv[7]);
	//fprintf(stderr,"numsess %f\n", NUMSESSION_TIMES);

	/* first get in all the class files */
		
	/* first for HTTP */
	process_file(generatefile);
	printf("DONE OFFSET 999999\n");
    exit(0);
	//printstats_application(httpc);
	/* done with all the class files */
	/* then read in the generate file */
#if 0
    char command[10];
	char this_class[10];
	int client_reserved, client_normal, server_reserved, server_normal,offset,total;
	while(fgets(this_line,MAXLINELENGTH,generatefile)!=NULL) {
		if(this_line[0]=='#')  continue;
		sscanf(this_line,"%s",command);
		if(!strcmp(command,"DONE")) break;
		if(!strcmp(command,"GENERATE")) {
			sscanf(this_line,"GENERATE %s %d normal %d reserved %d normal_req %d reserved_req %d total offset %d", this_class, &server_normal, &server_reserved,&client_normal,&client_reserved,&total,&offset);
			generate_requests(this_class,client_reserved, client_normal, server_reserved, server_normal,offset);
		}
	}
#endif
}

/*
GENERATE  HTTP_like 0 normal 50 reserved 0 normal_req 50 reserved_req 50 total offset 2 Sat Dec 6 19:31:14 EST 2003
GENERATE  FTP_like 0 normal 50 reserved 0 normal_req 50 reserved_req 50 total offset 2 Sat Dec 6 19:31:15 EST 2003
GENERATE  P2P_like 0 normal 50 reserved 0 normal_req 50 reserved_req 50 total offset 3 Sat Dec 6 19:31:16 EST 2003
GENERATE  SMTP_like 0 normal 50 reserved 0 normal_req 50 reserved_req 50 total offset 3 Sat Dec 6 19:31:17 EST 2003
GENERATE  HTTP_like 0 normal 50 reserved 0 normal_req 50 reserved_req 50 total offset 10 Sat Dec 6 19:31:17 EST 2003
GENERATE  SMTP_like 0 normal 50 reserved 0 normal_req 50 reserved_req 50 total offset 9 Sat Dec 6 19:31:18 EST 2003
GENERATE  P2P_like 0 normal 50 reserved 0 normal_req 50 reserved_req 50 total offset 10 Sat Dec 6 19:31:18 EST 2003
GENERATE  FTP_like 0 normal 50 reserved 0 normal_req 50 reserved_req 50 total offset 11 Sat Dec 6 19:31:19 EST 2003
DONE offset 9999999



GENERATE FTP_like vn72 client vn70 server TIME 1
GENERATE FTP_like vn72 client vn70 server TIME 1
GENERATE FTP_like vn72 client vn70 server TIME 1
GENERATE FTP_like vn72 client vn70 server TIME 1
GENERATE FTP_like vn72 client vn70 server TIME 1
GENERATE FTP_like vn72 client vn70 server TIME 1
GENERATE FTP_like vn72 client vn70 server TIME 1
GENERATE FTP_like vn72 client vn70 server TIME 1
GENERATE FTP_like vn73 client vn70 server TIME 1
GENERATE FTP_like vn73 client vn70 server TIME 1

*/
