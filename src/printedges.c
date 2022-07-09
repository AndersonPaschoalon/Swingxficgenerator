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
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <errno.h>
# include <math.h>

int singlequeue=1;
int defhops=100;

void Usage(char * argv[]) {
	printf("Usage: %s <cl> <nmamx> <enum> <qslot> [<defhops>]\n",argv[0]);
	exit(1);
}

char *cl;
int nmax,enumv,qslot;
int d1;
float mqueuef;
int mqueue;

float d2, d3, d4, d5 ,d6, d7;

int main (int argc, char *argv[]) {
	if(argc<4) Usage(argv);

	cl=argv[1];
	nmax=atoi(argv[2]);
	enumv=atoi(argv[3]);
	qslot=atoi(argv[4]);
	if(argc>=6) {
		defhops=atoi(argv[5]);
	}
	if(sscanf(cl, "%d_%f_%f_%f_%f_%f_%f", &d1,&d2,&d3,&d4,&d5,&d6,&d7)!=7) {
		perror("not enough fields");
		printf("%s\n", cl);
		exit(1);
	}
	/*
	   
	   	kbps  = d2 ;
		plr   = d3 ;
		delay = d4 ;
		
	   */
	mqueuef= (d2*d4 /8/1500) ;
	mqueue=(int) mqueuef;
	if (singlequeue==1) {
	printf("   <edge dbl_kbps=\"%.3f\" dbl_plr=\"%.6f\" int_dst=\"%d\" int_delayms=\"%.3f\" int_src=\"%d\" int_idx=\"%d\" int_qlen=\"%d\"/>\n", d2,d3,d1,d4,nmax,enumv,qslot);
	}
	else {
	printf("   <edge dbl_kbps=\"%.3f\" dbl_plr=\"%.6f\" int_dst=\"%d\" int_delayms=\"%.3f\" int_src=\"%d\" int_idx=\"%d\" int_qlen=\"%d\"/>\n", d2,d3,d1,d4,nmax,enumv,mqueue*2 + defhops);
	}
	enumv++;

	/*
	   
	   	kbps  = d5 ;
		plr   = d6 ;
		delay = d7 ;
		
	   */


	mqueuef= (d5*d7 /8/1500) ;
	mqueue=(int) mqueuef;

	
	if (singlequeue==1) {
	printf("   <edge dbl_kbps=\"%.3f\" dbl_plr=\"%.6f\" int_dst=\"%d\" int_delayms=\"%.3f\" int_src=\"%d\" int_idx=\"%d\" int_qlen=\"%d\"/>\n", d5,d6,nmax,d7,d1,enumv,qslot);
	} else {
	printf("   <edge dbl_kbps=\"%.3f\" dbl_plr=\"%.6f\" int_dst=\"%d\" int_delayms=\"%.3f\" int_src=\"%d\" int_idx=\"%d\" int_qlen=\"%d\"/>\n", d5,d6,nmax,d7,d1,enumv,mqueue*2 + defhops);
	}
	
}
