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

# define LINESIZE 1024


char thisline[LINESIZE];
char d1[LINESIZE],d2[LINESIZE],d3[LINESIZE],d4[LINESIZE],d5[LINESIZE];

char *src;
char *dst;
char *colonpos;
short strln;

double floatval;
int dir;
void Usage(char *argv[]) {
	printf("Usage: %s <infile>\n");
	exit(1);
}

int main(int argc, char *argv[]) {
	FILE * infile;
	if(argc<2) Usage(argv);
	if(!strcmp(argv[1],"-"))
		infile=fopen("/dev/stdin","r");
	else
		infile=fopen(argv[1],"r");
	if (infile==NULL) {
		perror("error opening infile");
		exit(1);
	}

	while (fgets(thisline, LINESIZE, infile)!=NULL) {
		if (sscanf(thisline,"%s %s %s %s",d1,d2,d3,d4)<4) {
			perror("cant read enough fields");
			continue;
		}
		colonpos=strchr(thisline,':');
		if(colonpos==NULL) continue;
		strln=strlen(d4);
		if (d3[0]!='>' || d3[1]!='\0'|| d4[strln-1]!=':') continue;
		floatval=atof(d1);
		if(strcmp(d2,d4)>0) {
			dir=1;
			src=d4;
			dst=d2;
		}
		else { 
			src=d2;
			dst=d4;
			dir=0;
		}
		
		d4[strln-1]='\0';
		*colonpos++;
		printf("%d %.6f %s > %s : %s", dir, floatval,src, dst, colonpos);
	}
	
	return 0;
}
