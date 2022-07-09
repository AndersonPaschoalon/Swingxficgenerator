#!/bin/bash
###################################################################################
# 
#  Copyright (C) 2008 Kashi Venkatesh Vishwanath <swingxficgenerator@gmail.com>
# 
#  Permission is hereby granted, free of charge, to any person obtaining a copy of
#  this software and associated documentation files (the "Software"), to  deal  in
#  the Software without restriction, including without limitation  the  rights  to
#  use, copy, modify, merge, publish, distribute, sublicense, and/or  sell  copies
#  of the Software, and to permit persons to whom the Software is furnished to  do
#  so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included in  all
#  copies of the Software and its documentation and acknowledgment shall be  given
#  in the documentation  and  software  packages  that  this  Software  was  used.
#  
#  You should have received a copy of the GNU Library General Public License along
#  with this library; if not, write to the  Free  Software  Foundation,  Inc.,  59
#  Temple Place, Suite 330, Boston, MA 02111-1307, USA.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY  OF  ANY  KIND,  EXPRESS  OR
#  IMPLIED, INCLUDING BUT  NOT  LIMITED  TO  THE  WARRANTIES  OF  MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO  EVENT  SHALL  THE
#  AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR  OTHER  LIABILITY,  WHETHER  IN  AN
#  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF  OR  IN  CONNECTION
#  WITH  THE  SOFTWARE  OR  THE  USE  OR   OTHER   DEALINGS  IN   THE   SOFTWARE.
#
#
#
#  Last Modified : Thu Sep 18 22:48:26 PDT 2008
#
###################################################################################

# SUMMARY: takes in two files
# in ASCII format (from tcpdump)
# and then produces ipmaps file

if [ "$#" -lt "2" ]
then
	echo "Usage $0 <file1> <file2>"
	exit
fi

file1=$1
file2=$2
s=0;
SECONDS=0;
cat < /dev/null > ipmap
for files in `echo $file1 $file2`
do

cat $files | awk -v s=$s '{
	if($3==">") {
		split($2, sips,/\./);
		if(sips[5]!="") {
			srcip=sips[1]"."sips[2]"."sips[3]"."sips[4];
			src[srcip]++;

		}

		split($4, dips,/\./);
		if(dips[5]!="") {
			dstip=dips[1]"."dips[2]"."dips[3]"."dips[4];

			dst[dstip]++;

		}
	}
} END {
	for(ip in src)
	{
		if(s==0) {
			print ip, src[ip], 0>> "ipmap";
		} else {
			print ip,src[ip], 1 >> "ipmap";
		}
	}

	for(ip in dst)
	{
		if(s==0) {
			print ip, dst[ip], 1>> "ipmap";
		} else {
			print ip,dst[ip], 0 >> "ipmap";
		}
	}

}'
s=1;

done

cat < /dev/null>ipmaps
cat < /dev/null>ipconflicts
cat ipmap | awk '{ 

	if (d[$1]!=$3) {
		if (d[$1]=="") {
			d[$1]=$3;
			n[$1]=$2;
		} else {
			conflicts[$1]=1;
			if (n[$1]<$2) {
				n[$1]=$2
					d[$1]=$3
			}
		}
	}

} END{ 
	for( i in d)
		printf "%s %d\n", i, d[i] >>"ipmaps";
	for (i in conflict)
		print i >> "ipconflicts";
}'
