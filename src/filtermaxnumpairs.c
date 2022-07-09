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
# include <stdlib.h>
# include <string.h>
# include <errno.h>

/*
# 10.0.0.10 1220 10.0.0.1 80 SYN 57871.233234 FIN 57872.910156 57872.910156 PAIRS 1 REQ 1201 57871.417969 57871.417969 RSP 21253 57871.679688 57872.910156
#    1        2      3     4  5        6       7       8          9           10  11 12  13    14            15         16    17      18          19
   */

//#define DEBUG 1
typedef struct {
        long long int req;
        long long int rsp;
        double reqstart;
        double reqend;
        double rspstart;
        double rspend;
} req_rsp_struct;

# define MAXNUMPAIRS (1024)
# define LINESIZE (1024)
char srcip[LINESIZE];
char srcport[LINESIZE];
char dstip[LINESIZE];
char dstport[LINESIZE];
double syn;
double finstart;
double finend;
int  numpairs;
int done;
int toprint=0;
int i;
int j;
int maxnumpairs;
int thispairs=0;
req_rsp_struct reqrsppairs[MAXNUMPAIRS];

int main (int argc, char *argv[]) {
    FILE * infile;

    if (!strcmp(argv[1],"-")) {
        infile=fopen("/dev/stdin","r");
    } else 
        infile=fopen(argv[1],"r");
    if(infile==NULL) {
        perror("could not open file");
        exit(1);
    }
    maxnumpairs=atoi(argv[2]);
#ifdef DEBUG
	printf(" maxnumpairs %d\n", maxnumpairs);
#endif
    done=0;
    while (done==0) {
        numpairs=0;
		toprint=0;
#ifdef DEBUG
		printf("----------------------------------------------------------------------------\n");
#endif
        if(fscanf(infile,"%s %s %s %s SYN %lf FIN %lf %lf PAIRS %d", srcip,srcport,dstip, dstport, &syn, &finstart, &finend, &numpairs)!=8) {
#ifdef DEBUG
            perror("could not read 8 fields");
#endif
			done=1;
			goto alldone;
        }
		thispairs=0;
		for(i=0;i<numpairs;i++) {
			if(fscanf(infile," REQ %lld %lf %lf RSP %lld %lf %lf",&reqrsppairs[thispairs].req, &reqrsppairs[thispairs].reqstart, &reqrsppairs[thispairs].reqend, &reqrsppairs[thispairs].rsp, &reqrsppairs[thispairs].rspstart, &reqrsppairs[thispairs].rspend)!=6) {
				perror("could not read the entire req/rsp struct in ");
				exit(1);
			}
			if(((int)syn)==-1) { 
				syn=reqrsppairs[thispairs].reqstart;
#ifdef DEBUG
				printf(" syn -1 so now syn = %lf\n", syn);
#endif
			}
			finstart=reqrsppairs[thispairs].rspend;
			finend=reqrsppairs[thispairs].rspend;
			thispairs++;
			toprint=1;
			if(thispairs>=maxnumpairs) {
#ifdef DEBUG
				printf("in between \n");
#endif
				printf("%s %s %s %s SYN %lf FIN %lf %lf PAIRS %d",srcip, srcport, dstip, dstport, syn, finstart, finend, thispairs);
				for(j=0;j<thispairs;j++)
					printf(" REQ %lld %lf %lf RSP %lld %lf %lf", reqrsppairs[j].req, reqrsppairs[j].reqstart, reqrsppairs[j].reqend, reqrsppairs[j].rsp, reqrsppairs[j].rspstart,  reqrsppairs[j].rspend);
				printf("\n");
				toprint=0;
				thispairs=0;
				syn=-1;
			}
		}
		if(toprint==1) {
			for(j=0;j<thispairs;j++) 
			{
				printf("%s %s %s %s SYN %lf FIN %lf %lf PAIRS %d",srcip, srcport, dstip, dstport, syn, finstart, finend, thispairs);
				for(j=0;j<thispairs;j++)
					printf(" REQ %lld %lf %lf RSP %lld %lf %lf", reqrsppairs[j].req, reqrsppairs[j].reqstart, reqrsppairs[j].reqend, reqrsppairs[j].rsp, reqrsppairs[j].rspstart,  reqrsppairs[j].rspend);
				printf("\n");
				toprint=0;
				thispairs=0;
			}
		}
    }
alldone:
	return 0;
}
