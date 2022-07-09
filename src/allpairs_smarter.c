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
# include <stdio.h>
# include <errno.h>
# include <stdlib.h>
# include <string.h>

/* 
<?xml version="1.0" encoding="ISO-8859-1"?>
<topology>
  <edges>
   <edge dbl_kbps="100000" dbl_plr="0" int_dst="1" int_delayms="10" int_src="0" int_idx="0" int_qlen="20"/>
   <edge dbl_kbps="100000" dbl_plr="0" int_dst="0" int_delayms="10" int_src="1" int_idx="1" int_qlen="20"/>
   <edge dbl_kbps="279.253" dbl_plr="0.000" int_dst="0" int_delayms="111.381" int_src="2" int_idx="2" int_qlen="20"/>
   <edge dbl_kbps="100000.000" dbl_plr="0.000" int_dst="2" int_delayms="1.000" int_src="0" int_idx="3" int_qlen="20"/>
   <edge dbl_kbps="279.253" dbl_plr="0.000" int_dst="1" int_delayms="111.381" int_src="3" int_idx="4" int_qlen="20"/>
   <edge dbl_kbps="100000.000" dbl_plr="0.000" int_dst="3" int_delayms="1.000" int_src="1" int_idx="5" int_qlen="20"/>
  </edges>
  <vertices>
   <vertex int_idx="0" dbl_x="4444" role="transit" dbl_y="5555" />
   <vertex int_idx="1" dbl_x="4444" role="transit" dbl_y="5555" />
   <vertex int_idx="2" role="virtnode" int_vn="0" />
   <vertex int_idx="3" role="virtnode" int_vn="1" />
  </vertices>
  <specs xmloutbug="workaround">
   <transit-transit dbl_plr="0" dbl_kbps="100000" int_delayms="1" int_qlen="100 " />
   <client-stub dbl_plr="0" dbl_kbps="10000" int_delayms="1" int_qlen="100" />
   <stub-stub dbl_plr="0" dbl_kbps="10000" int_delayms="1" int_qlen="100" />
   <stub-transit dbl_plr="0" dbl_kbps="100000" int_delayms="1" int_qlen="100" />
  </specs>
</topology>
*/
# define PRINTEXITERROR(a)  { fprintf (stderr, "Error: ( %s: %d: %s() ) ", __FILE__, __LINE__, __FUNCTION__); perror(a); exit(-1);}

void Usage(char * argv[]) {
	fprintf(stderr, "Usage: %s <assignedgraph> <mntcpdump> <cdfdirs...>\n", argv[0]);
	PRINTEXITERROR("could not open graph file for reading");
}

# define LINESIZE 1024
char thisline[LINESIZE];
int ts, td;
FILE * portsfile;
char * cdfdir;
int * vn_to, *vn_from;
int * vn2idx;
enum states {start, readingedges, edgesdone, readingvertices, verticesdone};
int mntcpdump;
enum states thistate;
int edgenum, vertexnum;
char dummy1[1024];
char d1[1024],d2[1024],d3[1024], d4[1024];
int idx;
int src;
float delay;
int vn;
int dst;
int thisvn;
int * direction;
int args;
int routedsrc;
FILE * thisfile;
int clientnum;
int s;
int servernum;
int *clients , * servers;
int main(int argc, char *argv[]) {
	FILE * infile;
	if(argc<3) Usage(argv);

	infile=fopen(argv[1],"r");
	if(infile==NULL) PRINTEXITERROR("could not open graph file for reading");
	thistate=start;
	while(fgets(thisline, LINESIZE, infile)!=NULL ) {
		if(thistate==start && strstr(thisline,"<edges>")) {
			thistate=readingedges;
			edgenum=0;
		} else if(thistate==readingedges ) {
			if(strstr(thisline,"</edges>")!=NULL ) {
				thistate=edgesdone;
			} else {
				edgenum++;
				/* do something smart with this edge  but in next scan */
			}
		} else if(thistate==edgesdone && strstr(thisline,"<vertices>")!=NULL) {
			thistate=readingvertices;
			vertexnum=0;
		} else if(thistate==readingvertices) {
			if(strstr(thisline,"</vertices>")!=NULL) {
				thistate=verticesdone;
			} else {
				vertexnum++;
			}
		}
	}
	fprintf(stderr, "read %d vertices and %d edges\n", vertexnum, edgenum);

	rewind(infile);
	/* vertexnum is vns */

	vn_to=(int*) malloc(sizeof(int)*vertexnum);
	vn_from=(int*) malloc(sizeof(int)*vertexnum);
	vn2idx=(int*)malloc(sizeof(int)*vertexnum);
	direction=(int*) malloc(sizeof(int)*vertexnum);
	direction[0]=0;
	direction[1]=1;
	if(vn_to==NULL || vn_from==NULL || vn2idx==NULL || direction==NULL) PRINTEXITERROR("not enough memore for vn_to and vn_from\n");
	thistate=start;
	while(fgets(thisline, LINESIZE, infile)!=NULL ) {
		if(thistate==start && strstr(thisline,"<edges>")) {
			thistate=readingedges;
		} else if(thistate==readingedges ) {
			if(strstr(thisline,"</edges>")!=NULL ) {
				thistate=edgesdone;
			} else {
				/* do something smart with this edge*/
				/* extract src dst and idx */
				/*
   <edge dbl_kbps="100000" dbl_plr="0" int_dst="1" int_delayms="0.0" int_src="0" int_idx="0" int_qlen="40"/>
   <edge dbl_kbps="100000" dbl_plr="0" int_dst="0" int_delayms="0.0" int_src="1" int_idx="1" int_qlen="40"/>
   <edge dbl_kbps="13714.286" dbl_plr="0.000" int_dst="0" int_delayms="395.585" int_src="2" int_idx="2" int_qlen="914"/>
   <edge dbl_kbps="100000.000" dbl_plr="0.000" int_dst="2" int_delayms="1.000" int_src="0" int_idx="3" int_qlen="26"/>
   <edge dbl_kbps="7609.385" dbl_plr="0.000" int_dst="0" int_delayms="12.569" int_src="3" int_idx="4" int_qlen="24"/>
   <edge dbl_kbps="100000.000" dbl_plr="0.000" int_dst="3" int_delayms="1.000" int_src="0" int_idx="5" int_qlen="26"/>
   <edge dbl_kbps="13714.286" dbl_plr="0.000" int_dst="0" int_delayms="11.238" int_src="4" int_idx="6" int_qlen="34"/>

   */
				if(sscanf(thisline, "%s %s %s int_dst=\"%d\" int_delayms=\"%f\" int_src=\"%d\" int_idx=\"%d\"",d1, d2, d3, &dst,&delay, &src,&idx)<7)
				{
					printf("%s", thisline);
					printf("%s %s %s %d %f %d %d\n", d1,d2, d3, dst,delay, src,idx);
					PRINTEXITERROR("cant read all fields from thisline\n");
				}
				vn_from[src]=idx;
				vn_to[dst]=idx;
			}
		} else if(thistate==edgesdone && strstr(thisline,"<vertices>")!=NULL) {
			thistate=readingvertices;
		} else if(thistate==readingvertices) {
			if(strstr(thisline,"</vertices>")!=NULL) {
				thistate=verticesdone;
			} else {
				/* do something smart with this vn */
				/*
   <vertex int_idx="2127" role="virtnode" int_vn="2125" />
   <vertex int_idx="2128" role="virtnode" int_vn="2126" />
   */			
				if(strstr(thisline,"int_vn")!=NULL ){
				if(sscanf(thisline,"%s int_idx=\"%d\" %s int_vn=\"%d\"", d1, &idx, d2, &vn)<4) {
					printf("%s", thisline);
					PRINTEXITERROR("cant read all fields from vn line \n");
				}
				vn2idx[vn]=idx;
				}
			}
		}
	}

	fclose(infile);
	/* lets do the direction stuff now */

	/* for all files... */
	routedsrc=0;
	printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
	printf("<allpairs>\n");
	mntcpdump=atoi(argv[2]);
	for (args=3;args<argc;args++) {
		
	//	fprintf(stderr, " arg %d %s \n", args-2, argv[args]);
		
		/* for each file fill in the info */

		thisfile=fopen(argv[args],"r");
	
		/* client 1 */

		if(fgets(thisline,LINESIZE,thisfile)==NULL) PRINTEXITERROR("reading lines from inputforgeneric file");

		if(sscanf(thisline,"client %d", &clientnum)!=1) PRINTEXITERROR("reading in format client <clientnum>");

		clients=(int *) malloc(sizeof(int)*clientnum);
		if(clients==NULL) PRINTEXITERROR("not enough memory\n");
		for(s=0;s<clientnum;s++) {
			if(s==0) {
				if(fscanf(thisfile,"vn%d", &thisvn)!=1) PRINTEXITERROR("reading clients from inputforgeneric file");
			}
			else {
				if(fscanf(thisfile," vn%d", &thisvn)!=1) PRINTEXITERROR("reading clients from inputforgeneric file");
			}
			clients[s]=thisvn;
			direction[clients[s]]=0;
		}
		/* now server */
		if(fgets(thisline,LINESIZE,thisfile)==NULL) PRINTEXITERROR("reading lines from inputforgeneric file");
		if(fgets(thisline,LINESIZE,thisfile)==NULL) PRINTEXITERROR("reading lines from inputforgeneric file");
		if(sscanf(thisline,"server %d", &servernum)!=1) PRINTEXITERROR("reading in format client <servernum>");
		servers=(int *) malloc(sizeof(int)*servernum);
		if(servers==NULL) PRINTEXITERROR("not enough memory\n");
		for(s=0;s<servernum;s++) {
			if(s==0) {
				if(fscanf(thisfile,"vn%d", &thisvn)!=1) PRINTEXITERROR("reading servers from inputforgeneric file");
			}
			else {
				if(fscanf(thisfile," vn%d", &thisvn)!=1) PRINTEXITERROR("reading servers from inputforgeneric file");
			}
			servers[s]=thisvn;
			direction[servers[s]]=1;
	//		fprintf(stderr, " vn %d ip %d dir %d\n", thisvn, vn2ip[thisvn], direction[vn2ip[thisvn]]);
		}

		/* get the next client line */
		/* do allpairs part 1 */
		
	//	fprintf(stderr, " clientnum %d %d->%d servernum %d %d->%d\n", clientnum, clients[0], clients[clientnum-1], servernum, servers[0], servers[servernum-1]);
		for(src=0;src<clientnum;src++) {
			routedsrc++;
			for(dst=0;dst<servernum;dst++) {
			//	printf( "--> src %d dst %d\n", src, dst);
				ts=clients[src];
				td=servers[dst];
	//			printf("trial path from %d -> %d %d (%d) %d %d (%d) %d \n",src, dst, vn_from[vn2idx[src]], direction[src], vn_to[vn2idx[dst]], vn_from[vn2idx[dst]] , direction[dst],  vn_to[vn2idx[src]]);
				if(mntcpdump>=2) {
					if(direction[ts]==0) {
						printf("  <path int_vndst=\"%d\" int_vnsrc=\"%d\" hops=\"%d 0 2 %d \" />\n",td, ts, vn_from[vn2idx[ts]], vn_to[vn2idx[td]]);
						printf("  <path int_vndst=\"%d\" int_vnsrc=\"%d\" hops=\"%d 3 1 %d \" />\n",ts, td, vn_from[vn2idx[td]], vn_to[vn2idx[ts]]);
					}
					else {
						printf("  <path int_vndst=\"%d\" int_vnsrc=\"%d\" hops=\"%d 3 1 %d \" />\n",td, ts, vn_from[vn2idx[ts]], vn_to[vn2idx[td]]);
						printf("  <path int_vndst=\"%d\" int_vnsrc=\"%d\" hops=\"%d 0 2 %d \" />\n",ts, td, vn_from[vn2idx[td]], vn_to[vn2idx[ts]]);
					}
				}
				else{
					printf("  <path int_vndst=\"%d\" int_vnsrc=\"%d\" hops=\"%d %d %d \" />\n",td, ts, vn_from[vn2idx[ts]], direction[ts], vn_to[vn2idx[td]]);
					printf("  <path int_vndst=\"%d\" int_vnsrc=\"%d\" hops=\"%d %d %d \" />\n",ts, td, vn_from[vn2idx[td]], direction[td], vn_to[vn2idx[ts]]);
				}
				/*

<?xml version="1.0" encoding="ISO-8859-1"?>
<allpairs>
  <path int_vndst="1" int_vnsrc="0" hops="2 5 " />
  <path int_vndst="2" int_vnsrc="0" hops="2 7 " />
  <path int_vndst="3" int_vnsrc="0" hops="2 9 " />
...
  <path int_vndst="2178" int_vnsrc="2182" hops="4366 4359 " />
  <path int_vndst="2179" int_vnsrc="2182" hops="4366 4361 " />
  <path int_vndst="2180" int_vnsrc="2182" hops="4366 4363 " />
  <path int_vndst="2181" int_vnsrc="2182" hops="4366 4365 " />
</allpairs>
				   */
			}
			if(routedsrc%10==0) 
				fprintf(stderr, "routing node %d\r",src);
		}
		free(clients);
		free(servers);
		
tryagain:
		if(fgets(thisline,LINESIZE,thisfile)==NULL) PRINTEXITERROR("reading lines from inputforgeneric file");
		if(sscanf(thisline,"client %d", &clientnum)!=1) goto tryagain;
		clients=(int *) malloc(sizeof(int)*clientnum);
		if(clients==NULL) PRINTEXITERROR("not enough memory\n");
		for(s=0;s<clientnum;s++) {
			if(s==0) {
				if(fscanf(thisfile,"vn%d", &thisvn)!=1) PRINTEXITERROR("reading clients from inputforgeneric file");
			}
			else {
				if(fscanf(thisfile," vn%d", &thisvn)!=1) PRINTEXITERROR("reading clients from inputforgeneric file");
			}
			clients[s]=thisvn;
			direction[clients[s]]=1;
	//		fprintf(stderr, " vn %d ip %d dir %d\n", thisvn, vn2ip[thisvn], direction[vn2ip[thisvn]]);
		}
		/* next server */
		if(fgets(thisline,LINESIZE,thisfile)==NULL) PRINTEXITERROR("reading lines from inputforgeneric file");
		if(fgets(thisline,LINESIZE,thisfile)==NULL) PRINTEXITERROR("reading lines from inputforgeneric file");
		if(sscanf(thisline,"server %d", &servernum)!=1) PRINTEXITERROR("reading in format client <servernum>");
		servers=(int *) malloc(sizeof(int)*servernum);
		if(servers==NULL) PRINTEXITERROR("not enough memory\n");
		for(s=0;s<servernum;s++) {
			if(s==0) {
				if(fscanf(thisfile,"vn%d", &thisvn)!=1) PRINTEXITERROR("reading servers from inputforgeneric file");
			}
			else {
				if(fscanf(thisfile," vn%d", &thisvn)!=1) PRINTEXITERROR("reading servers from inputforgeneric file");
			}
			servers[s]=thisvn;
			direction[servers[s]]=0;
	//		fprintf(stderr, " vn %d ip %d dir %d\n", thisvn, vn2ip[thisvn], direction[vn2ip[thisvn]]);
		}


	//	fprintf(stderr, " clientnum %d %d->%d servernum %d %d->%d\n", clientnum, clients[0], clients[clientnum-1], servernum, servers[0], servers[servernum-1]);
		for(src=0;src<clientnum;src++) {
			routedsrc++;
			for(dst=0;dst<servernum;dst++) {
				ts=clients[src];
				td=servers[dst];
	//			printf("trial path from %d -> %d %d (%d) %d %d (%d) %d \n",src, dst, vn_from[vn2idx[src]], direction[src], vn_to[vn2idx[dst]], vn_from[vn2idx[dst]] , direction[dst],  vn_to[vn2idx[src]]);
				if(mntcpdump>=2) {
					if(direction[ts]==0) {
						printf("  <path int_vndst=\"%d\" int_vnsrc=\"%d\" hops=\"%d 0 2 %d \" />\n",td, ts, vn_from[vn2idx[ts]], vn_to[vn2idx[td]]);
						printf("  <path int_vndst=\"%d\" int_vnsrc=\"%d\" hops=\"%d 3 1 %d \" />\n",ts, td, vn_from[vn2idx[td]], vn_to[vn2idx[ts]]);
					}
					else{
						printf("  <path int_vndst=\"%d\" int_vnsrc=\"%d\" hops=\"%d 3 1 %d \" />\n",td, ts, vn_from[vn2idx[ts]], vn_to[vn2idx[td]]);
						printf("  <path int_vndst=\"%d\" int_vnsrc=\"%d\" hops=\"%d 0 2 %d \" />\n",ts, td, vn_from[vn2idx[td]], vn_to[vn2idx[ts]]);
					}
				}
				else{
					printf("  <path int_vndst=\"%d\" int_vnsrc=\"%d\" hops=\"%d %d %d \" />\n",td, ts, vn_from[vn2idx[ts]], direction[ts], vn_to[vn2idx[td]]);
					printf("  <path int_vndst=\"%d\" int_vnsrc=\"%d\" hops=\"%d %d %d \" />\n",ts, td, vn_from[vn2idx[td]], direction[td], vn_to[vn2idx[ts]]);
				}
			}
			if(routedsrc%10==0) 
				fprintf(stderr, "routing node %d\r",src);
		}



		
		free(clients);
		free(servers);
		fclose(thisfile);
	}
	
	printf("</allpairs>\n");
	/*****************/
	return 0;
	for(src=0;src<vertexnum;src++)
		for(dst=0;dst<vertexnum;dst++)
		{
			if(src%10==0) {
				fprintf(stderr, "routing node %d\r",src);
			}
	printf("trial path from %d -> %d %d (%d) %d %d (%d) %d \n",src, dst, vn_from[vn2idx[src]], direction[src], vn_to[vn2idx[dst]], vn_from[vn2idx[dst]] , direction[dst],  vn_to[vn2idx[src]]);
		}
	fprintf(stderr, "\n");
	return 0;
}
