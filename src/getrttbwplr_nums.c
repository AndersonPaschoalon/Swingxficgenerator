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
#include<errno.h>

#define LINESIZE 1024
char thisline[LINESIZE];
/*
    x is percentile values
	y is the corresponding values
*/
FILE * cdffile, *fdummy;
int i,num, offset, origoffset, orignum;
float *x,*y,*z,*w,rval;
float *ox,*oy,*oz,*ow;
double dblread;
float thisrand;
int numfields;


void Usage(char *argv[]) {
	fprintf(stderr, "Usage %s <cdffile> [<randseed>] <numof nodes> <rbw> <rplr> <rdel> <dir 0/1> <client/server ie 0/1> <psizefile> <cdfdirfile> <vnstartingnum> <udphack>\n", argv[0]);
	exit(-1);

}


int getnumfields(char *line){
	int i=0;
	float dummy[50][50];
	if(sscanf(line,"%f %f %f %f %f %f %f", &dummy[0], &dummy[1], &dummy[2],&dummy[3],&dummy[4],&dummy[5],&dummy[6],&dummy[7])==7) {
		return 7;
	} else 
	if(sscanf(line,"%f %f %f %f %f %f", &dummy[0], &dummy[1], &dummy[2],&dummy[3],&dummy[4],&dummy[5],&dummy[6])==6) {
		return 6;
	} else 
	if(sscanf(line,"%f %f %f %f %f", &dummy[0], &dummy[1], &dummy[2],&dummy[3],&dummy[4],&dummy[5])==5) {
		return 5;
	} else 
	if(sscanf(line,"%f %f %f %f", &dummy[0], &dummy[1], &dummy[2],&dummy[3],&dummy[4])==4) {
		return 4;
	} else 
	if(sscanf(line,"%f %f %f", &dummy[0], &dummy[1], &dummy[2],&dummy[3])==3) {
		return 3;
	} else 
	if(sscanf(line,"%f %f", &dummy[0], &dummy[1], &dummy[2])==2) {
		return 2;
	} else  return 0;
}

int getoffset(float f) {
	int i;
	i=1;
	//while(i<=num && x[i]<(f*((x[num]-x[1])))) {
	while(i<=num && x[i]<f) {
		i++;
	}
	return i;

}
int main (int argc, char *argv[]) {
	int numtogenerate=0;
	int flipit=0;
	double rbw, rdel, rplr;
	int client_q, dir;
	FILE * psizefile, * cdfdirfile, *origcdffile;
	int vnnum;
	int udphack;
	int topoprmcopynmbr;
	
	if(argc<13) Usage(argv);
	cdffile=fopen(argv[1],"r");
	if(cdffile==NULL) {
		perror("could not open file");
		exit(1);
	}
	if(fscanf(cdffile,"%d",&num)!=1){
		perror("cannot read line 1 from the cdffile");
		exit(-1);
	}

	if(argc>2)
	srand48(atol(argv[2]));
	else srand48(442);
	if(argc>3)
		numtogenerate=atoi(argv[3]);
	else
		numtogenerate=1;

	rbw=atof(argv[4]);
	rplr=atof(argv[5]);
	rdel=atof(argv[6]);

	dir=atoi(argv[7]);
	client_q=atoi(argv[8]);
	psizefile=fopen(argv[9],"a");
	cdfdirfile=fopen(argv[10],"a");
	vnnum=atoi(argv[11]);
	udphack=atoi(argv[12]);
	origcdffile=fopen(argv[13],"r");
	if(origcdffile==NULL) {
		perror("could not open file");
		exit(1);
	}
	if(fscanf(origcdffile,"%d",&orignum)!=1){
		perror("cannot read line 1 from the cdffile");
		exit(-1);
	}
	topoprmcopynmbr=atoi(argv[14]);




/* orig topo additions begin */

	
	i=1;
	ox=(float *) malloc(sizeof(float)*(orignum+1));
	oy=(float *) malloc(sizeof(float)*(orignum+1));
	oz=(float *) malloc(sizeof(float)*(orignum+1));
	if(oz==NULL) {
		perror("cant allocate mem for oz");
		exit(1);
	}
	ow=(float *) malloc(sizeof(float)*(orignum+1));
	if(ox==NULL || oy==NULL||oz==NULL || ow==NULL) {
		printf("cannot allocate enough memory\n");
		exit(1);
	}
	ox[0]=0;
	oy[0]=0;
	oz[0]=0;
	ow[0]=0;

	//while(fgets(thisline,LINESIZE, origcdffile)!=NULL) {
	//	printf("from file %s", thisline);
	//}
	fgets(thisline, LINESIZE, origcdffile);
	if(!strcmp(thisline,"\0")){
		perror("Cant readin in line from origcdffile");
		fprintf(stderr, "origfile is %s\n", argv[13]);

		exit(1);
	}
	while(i<=orignum) {
		fgets(thisline, LINESIZE, origcdffile);
		if(thisline[0]=='\0'){
			//if(fgets(thisline, LINESIZE, origcdffile)==NULL) {
			perror("Cant readin in line from origcdffile");
			exit(1);
		}
	//	printf("thisline %s", thisline);
		if(sscanf(thisline,"%f %f %f %f", &ox[i],&oy[i],&oz[i], &ow[i])!=4) {
			perror("cannot read in specified format");
			exit(-1);
		}
	//	printf("orig [%d] = x %f   y %f   z %f\n", i, ox[i], oy[i],oz[i]);
		//origoffset=2; 
		//printf(" oz[%d]=%f\n",origoffset,oz[origoffset]);
		if(udphack!=0) oz[i]=0.0;
	//	printf("orig [%d] = x %f   y %f   z %f\n", i, ox[i], oy[i],oz[i]);
		//if(w[i]>1500) printf("->>>>>>>>>>>>>>>>>>>>>>>>>>>>>%s",thisline);
		//	y[i]=dblread;
		//	printf("%f %f %f %d\n", x[i],y[i],z[i],(int)w[i]);
		i++;
		}

	//	 origoffset=2; printf(" oz[%d]=%f\n",origoffset,oz[origoffset]);
/* orig topo additions end */





	
	i=1;
	x=(float *) malloc(sizeof(float)*(num+1));
	y=(float *) malloc(sizeof(float)*(num+1));
	z=(float *) malloc(sizeof(float)*(num+1));
	w=(float *) malloc(sizeof(float)*(num+1));
	x[0]=0;
	y[0]=0;
	z[0]=0;
	w[0]=0;

	/* get number of fields */
	/*
	fdummy=fopen(argv[1],"r");
	fgets(thisline, LINESIZE, fdummy);
	fgets(thisline, LINESIZE, fdummy);
	numfields=getnumfields(thisline);
	printf("num fields %d \n", numfields);
	fclose(fdummy);
	*/
	/* got number of fields */
	if(fgets(thisline, LINESIZE, cdffile)==NULL) {
		perror("Cant readin in line from cdffile");
		exit(1);
	}
	while(i<=num) {
		if(fgets(thisline, LINESIZE, cdffile)==NULL) {
			perror("Cant readin in line from cdffile");
			exit(1);
		}
		if(sscanf(thisline,"%f %f %f %f", &x[i],&y[i],&z[i], &w[i])!=4) {
			perror("cannot read in specified format");
			exit(-1);
		}
		if(udphack!=0) z[i]=0.0;
		//if(w[i]>1500) printf("->>>>>>>>>>>>>>>>>>>>>>>>>>>>>%s",thisline);
	//	y[i]=dblread;
	//	printf("%f %f %f %d\n", x[i],y[i],z[i],(int)w[i]);
		i++;
	}

	i=0;
	//while(i<10000) { 
	//}

	if(client_q==0) {
		fprintf(cdfdirfile,"server %d\n", numtogenerate);
	}
	else {
		fprintf(cdfdirfile,"client %d\n", numtogenerate);
	}
		
	for(i=0; i<numtogenerate; i++) {
		thisrand=drand48();
		offset=drand48()*num;
		offset++;
		origoffset=thisrand*orignum;
		origoffset++;
	//	printf("%.3f %.3f %.3f %d\n",x[offset],y[offset],z[offset],(int) w[offset]);
		/* x is del y is bw and z is plr */
		/* the output i.e. for specs_file */
		flipit=1; /* this will be 1 when topoprmcopynmbr = 1 will keep all from new
					 and only rtt from orig. this for use along with sess in project */
 if (flipit==1){
	 if(topoprmcopynmbr==1) {
		 /* only rtt is new
		  * */
	//	 printf(" fliip it %d topoprmcopynmbr %d\n", flipit, topoprmcopynmbr);
	//	 printf("instead of ");
	//	 printf("%d_%.3f_%.6f_%.3f_%.3f_%.3f_%.3f \\\n",dir,y[offset],z[offset],x[offset], rbw, rplr,rdel);
	//	 printf("it will be ");
		 printf("%d_%.3f_%.6f_%.3f_%.3f_%.3f_%.3f \\\n",dir,y[offset],z[offset],ox[origoffset], rbw, rplr,rdel);
		 // lat is orig rest is new
	 }
	 else if(topoprmcopynmbr==2) {
		 /* only capa is new
		  * */
		 printf("%d_%.3f_%.6f_%.3f_%.3f_%.3f_%.3f \\\n",dir,oy[origoffset],z[offset],x[offset], rbw, rplr,rdel);
		 // capa is orig rest is new
	 }
	 else if(topoprmcopynmbr==3) {
		 /* only plr is new
		  * */
		 printf("%d_%.3f_%.6f_%.3f_%.3f_%.3f_%.3f \\\n",dir,y[offset],oz[origoffset],x[offset], rbw, rplr,rdel);
		 // plr is orig rest is new
	 }
	 else {
		 printf("%d_%.3f_%.6f_%.3f_%.3f_%.3f_%.3f \\\n",dir,y[offset],z[offset],x[offset], rbw, rplr,rdel);
		 // all is new
	 }
	 /* psize */
	 //	fprintf(psizefile, "%d %d\n", vnnum+i, w[offset]);
	 //
	 /* cdfdir inputforgeneric file */
	 if(topoprmcopynmbr==1 || topoprmcopynmbr==2 || topoprmcopynmbr==3)
		 /*do not substitue psize */
		 fprintf(psizefile, "%d %.3f %.3f %.3f %d\n",vnnum+i, x[offset],y[offset],z[offset],(int) w[offset]);
	 else
		 /* substitue psize*/
		 fprintf(psizefile, "%d %.3f %.3f %.3f %d\n",vnnum+i, x[offset],y[offset],z[offset],(int) ow[origoffset]);
		 // psize is orig rest is new





 }
 else{


	 if(topoprmcopynmbr==1) {
		 /* only rtt is new
		  * */
	//	 printf(" fliip it %d topoprmcopynmbr %d offset=%d origoffset=%d\n", flipit, topoprmcopynmbr, offset, origoffset);
	//	 printf("instead of ");
	//	 printf("%d_%.3f_%.6f_%.3f_%.3f_%.3f_%.3f \\\n",dir,y[offset],z[offset],x[offset], rbw, rplr,rdel);
	//	 printf("it will be ");
		 printf("%d_%.3f_%.6f_%.3f_%.3f_%.3f_%.3f \\\n",dir,oy[origoffset],oz[origoffset],x[offset], rbw, rplr,rdel);
		 // only lat is new
	//	 printf("oy[%d]=%f oz[%d]=%f\n",origoffset,oy[origoffset],origoffset,oz[origoffset]);
	//	 printf("oy[2]=%f oz[2]=%f\n",oy[2],origoffset,oz[2]);
	//	 origoffset=2;
	//	 printf(" oz[%d]=%f\n",origoffset,oz[origoffset]);
	//	 printf("oy[2]=%f oz[2]=%f\n",oy[2],oz[2]);
	 }
	 else if(topoprmcopynmbr==2) {
		 /* only capa is new
		  * */
		 printf("%d_%.3f_%.6f_%.3f_%.3f_%.3f_%.3f \\\n",dir,y[offset],oz[origoffset],ox[origoffset], rbw, rplr,rdel);
	 }
	 else if(topoprmcopynmbr==3) {
		 /* only plr is new
		  * */
		 printf("%d_%.3f_%.6f_%.3f_%.3f_%.3f_%.3f \\\n",dir,oy[origoffset],z[offset],ox[origoffset], rbw, rplr,rdel);
	 }
	 else {
		 printf("%d_%.3f_%.6f_%.3f_%.3f_%.3f_%.3f \\\n",dir,oy[origoffset],oz[origoffset],ox[origoffset], rbw, rplr,rdel);
	 }
	 /* psize */
	 //	fprintf(psizefile, "%d %d\n", vnnum+i, w[offset]);
	 //
	 /* cdfdir inputforgeneric file */
	 if(topoprmcopynmbr==1 || topoprmcopynmbr==2 || topoprmcopynmbr==3)
		 /*do not substitue psize */
		 fprintf(psizefile, "%d %.3f %.3f %.3f %d\n",vnnum+i, x[offset],y[offset],z[offset],(int) ow[origoffset]);
	 else
		 /* substitue psize*/
		 fprintf(psizefile, "%d %.3f %.3f %.3f %d\n",vnnum+i, x[offset],y[offset],z[offset],(int) w[offset]);
 }

		fprintf(cdfdirfile,"vn%d ", vnnum+i);
	}
	i++;
	fprintf(cdfdirfile,"\n");
	fflush(psizefile);
	fflush(cdfdirfile);


	fclose(cdfdirfile);
	fclose(psizefile);
	free(x);
	free(y);
	free(z);
	free(w);
	free(ox);
	free(oy);
	free(oz);
	free(ow);
	//{ 
	//}
	return 0;
}
