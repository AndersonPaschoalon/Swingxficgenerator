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
#  Last Modified : Thu Sep 18 22:48:27 PDT 2008
#
###################################################################################


if [ "$#" -lt "3" ]
then
	echo "Usage: $0 <dir> <tagnamefortrace> <origtraceclass...>" 
	exit
fi

tdir=$1
tagname=$2
tname=$3


tnamesmall=`echo $tname | tr A-Z a-z`
starttime=`head -n 1 fileboth| field 1 | awk '{ printf "%d",$1}'`
totaltime=`(head -n 1 fileboth; tail -n 1 fileboth ) | awk '{ if(NR==1) s=$1; else t=$1 } END { printf "%d", t -s }'`
runtime=${totaltime}
killtime=`echo $runtime | awk '{ printf "%d ", $1+$1/2+30}'`
pipe0="0_1_1_100000_0"
pipe1="1_0_1_100000_0"
bneck1lat="0"
bneck0lat="0"
bneckplrs="0"
lat="0";
latencies="1"
toposizes="1000"
qslots="1000"
rttvalues="rtt_50"
bw="100000"
tracebw="100000"
tracebws=100000
bw=100000
mntcpdumps=0

echo  "thistagname=\"$tagname\";
TRACE=\"$tname\";
tracedir=$tdir;
cdfsbase=\"${tname}traces\";
tracenameparam=\"${tnamesmall}\";
runtime=$runtime;
bw=$bw;
tracebw=$tracebw;
killtime=$killtime;
pipe0=\"${pipe0}\";
pipe1=\"${pipe1}\";
bneck1lat=${bneck1lat};
bneck0lat=${bneck0lat};
bneckplrs=\"${bneckplrs}\" ;
lat=${lat};
latencies=\"${latencies}\";
toposizes=\"${toposizes}\";
qslots=\"${qslots}\";
rttvalues=\"${rttvalues}\";
totaltime=$totaltime;
starttime=$starttime;
tracebws=\"${tracebws}\"
bw=${bw};
mntcpdumps=\"$mntcpdumps\";
numhostsvary=\"0\";
dilated=\"0\";
numhostss=\"12\";
" > paramsforscript.template_entry
