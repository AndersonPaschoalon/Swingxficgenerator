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
# include <string.h>
# include <stdlib.h>


# define LINESIZE (1024*10)

char     thisline[LINESIZE];
float    prev;
int      day;
double   thistm;
int      hr, min;
double   sec;
int      spaceindex;

int getindex(char * str, char chr) {

    int     idx;

    idx = 0;
    while(idx<LINESIZE && str[idx]!='\0') {
      if(str[idx] == chr) return idx;
      idx++;
    }
    return -1;
}

int main () {

    FILE     * infile;

    infile = fopen("/dev/stdin","r");

	int	sscanfret;
    
    day = 0;
    prev = 0;
    
    while(fgets(thisline, LINESIZE, infile)!=NULL) {
	    sscanfret=sscanf(thisline,"%d:%d:%lf", &hr,&min, &sec);
        if(sscanfret==3){
            thistm = ((hr*60)+min)*60+sec;
            thistm += day;
            if(prev > (thistm + 3600) ) {
                day += (3600*24);
                thistm += (3600*24);
            }
            spaceindex = getindex(thisline,' ');
            if(spaceindex != -1) {
                printf("%.6f", thistm);
                printf("%s", &(thisline[spaceindex]));
            }
        } else {
		  printf("%s", thisline);
		}
    }
    return 0;
}

