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
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#define numperclass 2
#define MAXLINELENGTH 200

	int multihops;
char this_line[MAXLINELENGTH];

class generic_class {
	public :
		void setName(char * name) ;
		void setcdffile_client(char * name );
		char * class_name;
		
		int servernum;
		char *** server_list;
		int clientnum;
		char *** client_list;
		
		char * cdffile_client;
		float * cdfclientx;
		float * cdfclienty;
		int cdfclientnum;

		float * cdfserverx;
		float * cdfservery;
		int cdfservernum;

		int isdumbell; /* 0/1 */
		float ratio ; /* [0,1] */
		int current; /* 0/1 */
		int bidirectional;
		int servernum2;
		char *** server_list2;
		int clientnum2;
		char *** client_list2;
		
		char * cdffile_client2;
		float * cdfclientx2;
		float * cdfclienty2;
		int cdfclientnum2;

		float * cdfserverx2;
		float * cdfservery2;
		int cdfservernum2;


		
		virtual int random_deviate(char *);
};

void generic_class::setName(char * name) {
				  class_name=name;
}

void generic_class::setcdffile_client(char * name) {
				  cdffile_client=name;
}




class HTTP_class: public generic_class {
};
 
#if 0
int generic_class::random_deviate(char * name) {
	/*if(!strcmp(class_name,"HTTP_like"))
	{
		printf("HTTP\n");
		return 1;
		return ((HTTP_class *) this )->random_deviate(name);
	}
	return 0;*/
	return 0;
}
#endif
int search(float searchnum, float * cdfclienty, int cdfclientnum) {
	int i;
	for (i=0;i<cdfclientnum;i++) {
		if(cdfclienty[i]>searchnum) return (i);
	}
	return i-1;
}
#if 0
int HTTP_class::random_deviate(char * name) {
	if (!strcmp(name,"client")) {
		//printf("for client\n");
		/* use the cdffile_client[x|y]. X is hosts
		   and Y is requests
		*/
		/* basically generate a y variate as a random number and then do a search for it in the 
		   array.
		   i.e. locate the greatest y columns such that u<=y
		   then take the X value of that and the X value of the previous.
		   multiply both of these by the max num of hosts to gets
		   hostsmin and hostsmax.
		   next generate and random number between hostsmin and hostsmax.
		   this gives the index of the hosts number to be used for this process.
		   so then simply lookup that host number and print it.
		   this function should just return the offset and not the actual host.
		*/
		float u = drand48();
		//printf(" u is %f\n", u);
		int foundindex = search(u,cdfclienty,cdfclientnum);
		int maxhosts=cdfclientx[foundindex]*clientnum;
		int minhosts=0;
		if(foundindex>0) 
			cdfclientx[foundindex-1]*clientnum;
		return drand48()*(maxhosts - minhosts)+minhosts+1;
	}
	return 0;
}
int HTTP_class::random_deviate(char * name ){
	return generic_class::random_deviate(name);
}
#endif
void printarray(float * ff, int n) {
	for(int i=0;i<n;i++)
		printf("array[%d]=%f\n",i,ff[i]);
}
int getdeviate( float * cdfclienty, int cdfclientnum, float * cdfclientx, int clientnum) {
		int toreturn;
		float u = drand48();
		int foundindex = search(u,cdfclienty,cdfclientnum);
		//int maxhosts=(int)(cdfclientx[foundindex]*(clientnum -1) +0.5);
		int minhosts=(int)(cdfclientx[foundindex-1]*(clientnum ));
		int maxhosts=(int)(cdfclientx[foundindex]*(clientnum ));
		int toreturnhost=minhosts;

		if(minhosts<maxhosts) toreturnhost = (int)(minhosts + (maxhosts - minhosts ) *1.0* ( (u  - cdfclienty[foundindex-1]) *1.0/ (cdfclienty[foundindex] - cdfclienty[foundindex-1]))+0.5);
	//	fprintf(stderr,  " \n u %f idx %d maxhosts %d \n", u, foundindex, maxhosts);
		if(toreturnhost>=clientnum) toreturnhost=clientnum -1;
		if(toreturnhost<0) toreturnhost=0;
		return toreturnhost;
		if(maxhosts>=clientnum) maxhosts=clientnum-1;
		if(maxhosts<0) maxhosts=0;
		return maxhosts;
	//	printf("search for %f , found index %d clientnum %d\n", u,foundindex,clientnum);
	//	printarray(cdfclientx, cdfclientnum);
	//	int minhosts=0;
		if(foundindex>0) 
			minhosts=(int)(cdfclientx[foundindex-1]*clientnum);
		printf("minhosts=%d, maxhosts=%d\n",minhosts,maxhosts);
		toreturn = (int) (drand48()*(maxhosts - minhosts)+minhosts);
		if(toreturn<0) toreturn=0;
		if(toreturn>=clientnum) toreturn=clientnum-1;
		return toreturn;

}
int generic_class::random_deviate(char * name) {
	if (!strcmp(name,"client")) {
		if(isdumbell==1 && current==1)
			return getdeviate(cdfclienty2, cdfclientnum2, cdfclientx2, clientnum2);
		else
			return getdeviate(cdfclienty, cdfclientnum, cdfclientx, clientnum);
	}
	else if (!strcmp(name,"server")) {
		if(isdumbell==1 && current==1)
			return getdeviate(cdfservery2, cdfservernum2, cdfserverx2, servernum2);
		else
			return getdeviate(cdfservery, cdfservernum, cdfserverx, servernum);

	}
	return 0;
}


/* new add here */
class HTTPS_class: public generic_class {
};
class AZUREUS_class: public generic_class{
};
class IMAP_class: public generic_class {
};
class HTTPWEIRD_class: public generic_class {
};
class IBMSSD_class: public generic_class {
};
class NETIQMC_class: public generic_class {
};
class iRDMI_class: public generic_class {
};
class RSYNC_class: public generic_class {
};
class NETBIOS_class: public generic_class {
};
class SQUID_class: public generic_class {
};
class KAZAA_class: public generic_class {
};
class EDONKEY_class: public generic_class {
};
class WMEDIA_class: public generic_class {
};
class RTSP_class: public generic_class {
};
class NAPSTER_class: public generic_class {
};
class BITTORRENT_class: public generic_class {
};
class UDP_class: public generic_class {
};
class TCPOTHER_class: public generic_class {
};
class FTP_class: public generic_class {
};
class P2P_class: public generic_class {
};
class SMTP_class: public generic_class {
};
class NNTP_class: public generic_class {
};
class MMSTREAM_class: public generic_class{
};




/* new add here */
HTTP_class *httpc=NULL;
IMAP_class *imapc=NULL;
AZUREUS_class *azureusc=NULL;
HTTPWEIRD_class *httpweirdc=NULL;
IBMSSD_class *ibmssdc=NULL;
NETIQMC_class *netiqmcc=NULL;
iRDMI_class *irdmic=NULL;
RSYNC_class *rsyncc=NULL;
KAZAA_class *kazaac=NULL;
EDONKEY_class *edonkeyc=NULL;
NAPSTER_class *napsterc=NULL;
BITTORRENT_class *bittorrentc=NULL;
HTTPS_class *httpsc=NULL;
P2P_class *p2pc=NULL;
SQUID_class *squidc=NULL;
NETBIOS_class *netbiosc=NULL;
FTP_class *ftpc=NULL;
RTSP_class *rtspc=NULL;
SMTP_class *smtpc=NULL;
WMEDIA_class *wmediac=NULL;
NNTP_class *nntpc=NULL;
MMSTREAM_class *mmstreamc=NULL;
TCPOTHER_class *tcpotherc=NULL;
UDP_class *udpc=NULL;
void process_application(generic_class * ptr) {
	if(ptr!=NULL) {
		//char * class_name=ptr->class_name;
		/* for client first */
		FILE * openfile=fopen(ptr->cdffile_client,"r");
        char token[50];

		strcpy(token,"null");
		int multiloop=1;
		ptr->client_list=(char***)malloc(sizeof (char **)*multihops);
		ptr->server_list=(char***)malloc(sizeof (char **)*multihops);
		ptr->client_list2=(char***)malloc(sizeof (char **)*multihops);
		ptr->server_list2=(char***)malloc(sizeof (char **)*multihops);
		for(multiloop=1; multiloop<=multihops; multiloop++)
		{
	//		printf("readig loop %d\n", multiloop);
		//	printf(" this num 1 \n");

		while(strcmp(token,"client")!=0) {
			fscanf(openfile,"%s",token);
		}
		fscanf(openfile,"%d",&ptr->clientnum);
			//printf(" this num 2 \n");
		ptr->client_list[multiloop-1]=(char **) malloc(sizeof (char *) * ptr->clientnum); 
	//		printf(" this num 3 \n");
		for(int i=0;i<ptr->clientnum;i++) {
			ptr->client_list[multiloop-1][i]=(char *) malloc(sizeof(char)*7);
			fscanf(openfile,"%s", ptr->client_list[multiloop-1][i]);
		}
		//	printf(" this num 4 \n");
		while(strcmp(token,"server")!=0){
			fscanf(openfile,"%s",token);
		}
		fscanf(openfile,"%d",&ptr->servernum);
		ptr->server_list[multiloop-1]=(char **) malloc(sizeof (char *) * ptr->servernum); 
			//printf(" this num 5 \n");
		for(int i=0;i<ptr->servernum;i++) {
			ptr->server_list[multiloop-1][i]=(char *) malloc(sizeof(char)*7);
			fscanf(openfile,"%s", ptr->server_list[multiloop-1][i]);
		}
			//printf(" this num 6 \n");
		while(strcmp(token,"cdf")!=0){
			fscanf(openfile,"%s",token);
		}
	//		printf(" this num 7 \n");
		fscanf(openfile,"%d",&ptr->cdfclientnum);
		ptr->cdfclientx=(float *) malloc(sizeof(float)*ptr->cdfclientnum);
		ptr->cdfclienty=(float *) malloc(sizeof(float)*ptr->cdfclientnum);
		for(int i=0;i<ptr->cdfclientnum;i++) {
			fscanf(openfile,"%f %f", &ptr->cdfclientx[i], &ptr->cdfclienty[i]);
		}
		/* for server next */
			//printf(" this num 8 \n");

		strcpy(token,"null");
		while(strcmp(token,"cdf")!=0){
			fscanf(openfile,"%s",token);
		}
		//	printf(" this num 9 \n");
		fscanf(openfile,"%d",&ptr->cdfservernum);
		ptr->cdfserverx=(float *) malloc(sizeof(float)*ptr->cdfservernum);
		ptr->cdfservery=(float *) malloc(sizeof(float)*ptr->cdfservernum);
		for(int i=0;i<ptr->cdfservernum;i++) {
			fscanf(openfile,"%f %f", &ptr->cdfserverx[i], &ptr->cdfservery[i]);
		}
		//	printf(" this num 10 \n");
		char ratio[100];
	//	printf("here \n");
		if(fgets(ratio, 99, openfile)!=NULL) {
		fgets(ratio, 99, openfile);
	//	if(fscanf(openfile,"ratio %f", &(ptr->ratio))==1) {}
	//	if(fscanf(openfile,"ratio %f", &(ptr->ratio))==1) {}
		    sscanf(ratio, "ratio %f",  &(ptr->ratio));
		//	printf("got ratio %s<-\n", ratio);
			ptr->isdumbell=1;
	//		fscanf(openfile,"%f", &ptr->ratio);


/*********************************/

			strcpy(token,"null");
			while(strcmp(token,"client")!=0) {
				fscanf(openfile,"%s",token);
			}
			fscanf(openfile,"%d",&ptr->clientnum2);
			ptr->client_list2[multiloop-1]=(char **) malloc(sizeof (char *) * ptr->clientnum2); 
		//	printf(" this num 11 \n");
			for(int i=0;i<ptr->clientnum2;i++) {
				ptr->client_list2[multiloop-1][i]=(char *) malloc(sizeof(char)*7);
				fscanf(openfile,"%s", ptr->client_list2[multiloop-1][i]);
			}
			while(strcmp(token,"server")!=0){
				fscanf(openfile,"%s",token);
			}
			fscanf(openfile,"%d",&ptr->servernum2);
			ptr->server_list2[multiloop-1]=(char **) malloc(sizeof (char *) * ptr->servernum2); 
			for(int i=0;i<ptr->servernum2;i++) {
				ptr->server_list2[multiloop-1][i]=(char *) malloc(sizeof(char)*7);
				fscanf(openfile,"%s", ptr->server_list2[multiloop-1][i]);
			}
			while(strcmp(token,"cdf")!=0){
				fscanf(openfile,"%s",token);
			}
			fscanf(openfile,"%d",&ptr->cdfclientnum2);
			ptr->cdfclientx2=(float *) malloc(sizeof(float)*ptr->cdfclientnum2);
			ptr->cdfclienty2=(float *) malloc(sizeof(float)*ptr->cdfclientnum2);
			for(int i=0;i<ptr->cdfclientnum2;i++) {
				fscanf(openfile,"%f %f", &ptr->cdfclientx2[i], &ptr->cdfclienty2[i]);
			}
			/* for server next */
		//	printf(" this num 12 \n");

			strcpy(token,"null");
			while(strcmp(token,"cdf")!=0){
				fscanf(openfile,"%s",token);
			}
			fscanf(openfile,"%d",&ptr->cdfservernum2);
			ptr->cdfserverx2=(float *) malloc(sizeof(float)*ptr->cdfservernum2);
			ptr->cdfservery2=(float *) malloc(sizeof(float)*ptr->cdfservernum2);
			for(int i=0;i<ptr->cdfservernum2;i++) {
				fscanf(openfile,"%f %f", &ptr->cdfserverx2[i], &ptr->cdfservery2[i]);
			}
	//		printf(" this num 13 \n");

/**********************************/
			
		} else {
			/* only 1 server/client notion is there */
			ptr->isdumbell=0;
		}

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
	char *cpyclass;
	int len;
	int bidirectional=0;
	int thisdirection=0;
	generic_class * ptr;
	//printf(" class %s\n", this_class);
	len=strlen(this_class);
	cpyclass=(char *) malloc(sizeof(char)*(len+2));
	strcpy(cpyclass,this_class);
	if(this_class[len-6]=='0' || this_class[len-6]=='1') {
		/* cud be bidirectional */
		thisdirection=this_class[len-6]-'0';
		strcpy(&this_class[len-6],"_like");
		this_class[len-1]='\0';
		bidirectional=1;
	}
	//printf(" class %s\n", this_class);
	if(!strcmp("HTTP_like",this_class))  
	     ptr=httpc;
		
	/*else if(!strcmp("FTP",this_class)) ptr=ftpc;
	else if(!strcmp("SMTP",this_class)) ptr=smtpc;
	else if(!strcmp("P2P",this_class)) ptr=p2pc;*/

/* new add here */
	else if (!strcmp("SQUID_like",this_class))
		ptr=squidc;
	else if (!strcmp("IBMSSD_like",this_class))
		ptr=ibmssdc;
	else if (!strcmp("AZUREUS_like",this_class))
		ptr=azureusc;
	else if (!strcmp("HTTPWEIRD_like",this_class))
		ptr=httpweirdc;
	else if (!strcmp("IMAP_like",this_class))
		ptr=imapc;
	else if (!strcmp("NETIQMC_like",this_class))
		ptr=netiqmcc;
	else if (!strcmp("iRDMI_like",this_class))
		ptr=irdmic;
	else if(!strcmp("RSYNC_like", this_class))
		ptr=rsyncc;
	else if (!strcmp("RTSP_like",this_class))
		ptr=rtspc;
	else if (!strcmp("NETBIOS_like",this_class))
		ptr=netbiosc;
	else if (!strcmp("FTP_like",this_class))
		ptr=ftpc;
	else if (!strcmp("KAZAA_like",this_class))
		ptr=kazaac;
	else if (!strcmp("EDONKEY_like",this_class))
		ptr=edonkeyc;
	else if (!strcmp("NAPSTER_like",this_class))
		ptr=napsterc;
	else if (!strcmp("BITTORRENT_like",this_class))
		ptr=bittorrentc;
	else if (!strcmp("SMTP_like",this_class))
		ptr=smtpc;
	else if (!strcmp("WMEDIA_like",this_class))
		ptr=wmediac;
	else if (!strcmp("NNTP_like",this_class))
		ptr=nntpc;
	else if (!strcmp("P2P_like",this_class))
		ptr=p2pc;
	else if (!strcmp("HTTPS_like",this_class))
		ptr=httpsc;
	else if (!strcmp("MMSTREAM_like", this_class))
		ptr=mmstreamc;
	else if (!strcmp("TCPOTHER_like", this_class))
		ptr=tcpotherc;
	else if (!strcmp("UDP_like", this_class))
		ptr=udpc;
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

		if(ptr->isdumbell==1) {
			if (bidirectional==0){
			 if(drand48()>ptr->ratio) ptr->current = 1 ;
			 else ptr->current=0;
			} else {
				ptr->current=thisdirection;
			//	printf(" this direction %s %d \n", this_class, thisdirection);
			}
		}
		int client=ptr->random_deviate("client");
		int server=ptr->random_deviate("server");
		//printf("GENERATE %d client %d server \n",client,server);
		int multiloop=1;
		for(multiloop=1 ;multiloop<=multihops;multiloop++)
		{
		if(ptr->isdumbell==1 && ptr->current==1) 
			printf("GENERATE %s %s client %s server TIME %d\n",cpyclass, ptr->client_list2[multiloop-1][client],ptr->server_list2[multiloop-1][server],offset);
		else
			printf("GENERATE %s %s client %s server TIME %d\n",cpyclass, ptr->client_list[multiloop-1][client],ptr->server_list[multiloop-1][server],offset);
		//printf("client_reserved = %d server_reserved = %d \n", client_reserved, server_reserved);
		}
		client_reserved--;
		server_reserved--;
		
	}
}
void Usage(char * argv[]) {
	printf("Usage %s: <generatefile> <RANDSEED> <multihops>\n",argv[0]);
	printf("-class <classname> <cdffile_clients to read from> \n");
	printf("This assumes that prior to running this script \n");
	printf("we have individual cdfs for each application\n");
	printf("\n\n generatefile: This is the file generated by the genhosts script as of now\n");
	exit(1);
}

int main (int argc , char * argv[]) {

    
	int i;
	FILE *generatefile;
	char * classname;
	generic_class * ptr;
	if (argc<3) 
		Usage(argv);
	generatefile=fopen(argv[1],"r");

	srand48(atoi(argv[2]));
	multihops=atoi(argv[3]);
	for(i=4;i<argc;i++) {
		//if(i+numperclass >=argc ) Usage(argv);
		if(!strcmp(argv[i],"-class")) {
			classname=argv[++i];
			if(!strcmp(classname,"HTTP")){
				httpc=new HTTP_class;
				httpc->setName("HTTP");
				httpc->setcdffile_client(argv[++i]);
				ptr=httpc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"FTP")) {
				ftpc=new FTP_class;
				ftpc->setName("FTP");
				ftpc->setcdffile_client(argv[++i]);
				ptr=ftpc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"IMAP")) {
				imapc=new IMAP_class;
				imapc->setName("IMAP");
				imapc->setcdffile_client(argv[++i]);
				ptr=imapc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"NETIQMC")) {
				netiqmcc=new NETIQMC_class;
				netiqmcc->setName("NETIQMC");
				netiqmcc->setcdffile_client(argv[++i]);
				ptr=netiqmcc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"IBMSSD")) {
				ibmssdc=new IBMSSD_class;
				ibmssdc->setName("IBMSSD");
				ibmssdc->setcdffile_client(argv[++i]);
				ptr=ibmssdc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"iRDMI")) {
				irdmic=new iRDMI_class;
				irdmic->setName("iRDMI");
				irdmic->setcdffile_client(argv[++i]);
				ptr=irdmic;
				ptr->bidirectional=0;
			}

			
			else if (!strcmp(classname,"P2P")) {
				p2pc=new P2P_class;
				p2pc->setName("P2P");
				p2pc->setcdffile_client(argv[++i]);
				ptr=p2pc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"WMEDIA")) {
				wmediac=new WMEDIA_class;
				wmediac->setName("WMEDIA");
				wmediac->setcdffile_client(argv[++i]);
				ptr=wmediac;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"SMTP")) {
				smtpc=new SMTP_class;
				smtpc->setName("SMTP");
				smtpc->setcdffile_client(argv[++i]);
				ptr=smtpc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"NNTP")) {
				nntpc=new NNTP_class;
				nntpc->setName("NNTP");
				nntpc->setcdffile_client(argv[++i]);
				ptr=nntpc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"BITTORRENT")) {
				bittorrentc=new BITTORRENT_class;
				bittorrentc->setName("BITTORRENT");
				bittorrentc->setcdffile_client(argv[++i]);
				ptr=bittorrentc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"HTTPS")) {
				httpsc=new HTTPS_class;
				httpsc->setName("HTTPS");
				httpsc->setcdffile_client(argv[++i]);
				ptr=httpsc;
				ptr->bidirectional=0;
			}
/* new add here */
			else if (!strcmp(classname,"KAZAA")) {
				kazaac=new KAZAA_class;
				kazaac->setName("KAZAA");
				kazaac->setcdffile_client(argv[++i]);
				ptr=kazaac;
				ptr->bidirectional=0;
			}

			else if (!strcmp(classname,"AZUREUS")) {
				azureusc=new AZUREUS_class;
				azureusc->setName("AZUREUS");
				azureusc->setcdffile_client(argv[++i]);
				ptr=azureusc;
				ptr->bidirectional=0;
			}

			else if (!strcmp(classname,"HTTPWEIRD")) {
				httpweirdc=new HTTPWEIRD_class;
				httpweirdc->setName("HTTPWEIRD");
				httpweirdc->setcdffile_client(argv[++i]);
				ptr=httpweirdc;
				ptr->bidirectional=0;
			}

			else if (!strcmp(classname,"RSYNC")) {
				rsyncc=new RSYNC_class;
				rsyncc->setName("RSYNC");
				rsyncc->setcdffile_client(argv[++i]);
				ptr=rsyncc;
				ptr->bidirectional=0;
			}

			else if (!strcmp(classname,"RTSP")) {
				rtspc=new RTSP_class;
				rtspc->setName("RTSP");
				rtspc->setcdffile_client(argv[++i]);
				ptr=rtspc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"NETBIOS")) {
				netbiosc=new NETBIOS_class;
				netbiosc->setName("NETBIOS");
				netbiosc->setcdffile_client(argv[++i]);
				ptr=netbiosc;
				ptr->bidirectional=0;
			}	else if (!strcmp(classname,"SQUID")) {
				squidc=new SQUID_class;
				squidc->setName("SQUID");
				squidc->setcdffile_client(argv[++i]);
				ptr=squidc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"EDONKEY")) {
				edonkeyc=new EDONKEY_class;
				edonkeyc->setName("EDONKEY");
				edonkeyc->setcdffile_client(argv[++i]);
				ptr=edonkeyc;
				ptr->bidirectional=0;
			}

			else if (!strcmp(classname,"NAPSTER")) {
				napsterc=new NAPSTER_class;
				napsterc->setName("NAPSTER");
				napsterc->setcdffile_client(argv[++i]);
				ptr=napsterc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"MMSTREAM")) {
				mmstreamc=new MMSTREAM_class;
				mmstreamc->setName("MMSTREAM");
				mmstreamc->setcdffile_client(argv[++i]);
				ptr=mmstreamc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"TCPOTHER")) {
				tcpotherc=new TCPOTHER_class;
				tcpotherc->setName("TCPOTHER");
				tcpotherc->setcdffile_client(argv[++i]);
				ptr=tcpotherc;
				ptr->bidirectional=0;
			}
			else if (!strcmp(classname,"UDP")) {
				udpc=new UDP_class;
				udpc->setName("UDP");
				udpc->setcdffile_client(argv[++i]);
				ptr=udpc;
				ptr->bidirectional=0;
			}
			else {
				printf("Unknown class %s\", classname", classname);
				Usage(argv);
			}
			
		}
		else if(!strcmp(argv[i],"-dir")) {
			ptr->bidirectional=1;
			ptr->current=atoi(argv[++i]);
		}
		else {
			Usage(argv);
		}
	//srand48(4322);
	}
	/* first get in all the class files */
		
	/* first for HTTP */
    process_application(httpc);
    process_application(napsterc);
    process_application(kazaac);
/* new add here */
    process_application(edonkeyc);
	process_application(azureusc);
	process_application(httpweirdc);
    process_application(irdmic);
    process_application(ibmssdc);
	process_application(imapc);
    process_application(netiqmcc);
    process_application(rtspc);
    process_application(rsyncc);
	process_application(wmediac);
    process_application(netbiosc);
    process_application(squidc);
    process_application(bittorrentc);
    process_application(httpsc);
    process_application(ftpc);
    process_application(smtpc);
    process_application(nntpc);
    process_application(p2pc);
	process_application(mmstreamc);
	process_application(tcpotherc);
	process_application(udpc);
	
	//printstats_application(httpc);
	/* done with all the class files */
	/* then read in the generate file */
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
