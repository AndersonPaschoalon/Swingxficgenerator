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


typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

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
	printf("Usage:%s -s <specs_file> -h <timecommonfile>\n", args[0]);
	exit(1);
}

struct key
{
	char * class_name;
};

struct value
{
	unsigned int listen_port;
};

DEFINE_HASHTABLE_INSERT(insert_some, struct key, struct value);
DEFINE_HASHTABLE_SEARCH(search_some, struct key, struct value);
DEFINE_HASHTABLE_REMOVE(remove_some, struct key, struct value);


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
static int
equalkeys(void *k1, void *k2)
{
    return (0 == strcmp((( struct key *) k1)->class_name,(( struct key *)  k2)->class_name));
}

/*****************************************************************************/
int
main(int argc, char **argv)
{
    struct key *k;
	int dum_cl, dum_tm;
    struct value *v, *found;
    struct hashtable *h;
	char this_line[80];
	int vn_num;
	int i;
	char *clnt, *srvr;
	FILE * specsfile=NULL ;
	//=fopen("/usr/research/home/kvv/failure/mystuff/Generator/specs.all","r") ; 
	//FILE * classfile=fopen("/usr/research/home/kvv/failure/mystuff/class.hosts","r");
	FILE * hostport =NULL;
	//=fopen("./timecommonfile","r");
    h = create_hashtable(160, 0.75, hashfromkey, equalkeys);
    if (NULL == h) exit(-1); /*oom*/

	if(argc!=5)
		Usage(argv);
	for(i=1;i<argc;i++)
	{
		if (strcmp("-s", argv[i])==0)
			specsfile =fopen(argv[++i],"r") ; 
		else if (strcmp("-h", argv[i])==0)
			hostport =fopen(argv[++i],"r");
		else Usage(argv);
	}

	
	
	k=NULL;
	v=NULL;
	clnt=(char * ) malloc(sizeof(char)*80);
	srvr=(char * ) malloc(sizeof(char)*80);
	while(fgets(this_line,80,specsfile))
	{
		if(strncmp(this_line, "class", 5)==0)
		{
			k = (struct key *) malloc(sizeof(struct key ));
			v = (struct value *) malloc (sizeof(struct value));
			k->class_name = (char *)
				malloc(sizeof(char)*strlen(&this_line[6]) +1 );
			memset(k->class_name,0x0,1);
			sscanf(&(this_line[6]),"%s",k->class_name);
			
		}
		else if (strncmp(this_line,"param_port",10)==0)
		{
			sscanf(&(this_line[10+6]),"%u",&(v->listen_port));
			if(!insert_some(h,k,v)) exit(-1);
		}
	}
	while(fgets(this_line, 80, hostport))
	{
		if(strncmp(this_line,"GENERATE",8)==0)
		{
			sscanf(this_line,"GENERATE %s vn%d client vn%d server TIME %d", srvr, &dum_cl, &vn_num, &dum_tm);
			k=(struct key *) malloc(sizeof(struct key));
			k->class_name = srvr;

			if(NULL == (found = search_some(h,k))){
			}
			else
			{
				printf("LISTEN %s %d %d \n", k->class_name, vn_num, found->listen_port);
			}
		}
	}
	

    return 0;
}

