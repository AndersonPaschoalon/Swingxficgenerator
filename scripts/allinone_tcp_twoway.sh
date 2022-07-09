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

rtttouse="rtt_50"
if [ "$#" -lt "6" ]
then
	echo "Usage $0 <logfile> <app> <fh> <ipmaps> <totaltime> <starttime> <optionalskip>
eg: $0 tlatest.tcp.9980 http Swing ipmaps 300 79200"
	exit
fi

lfile=$1
app=$2
fh=$3;
ipmaps=$4
totaltime=$5
starttime=$6
if [ "$7" != "" ]
then
	skip1=$7
else
	skip1=0
fi


function fail_prepareforgetconndata () {
	echo "fail_prepareforgetconndata" $?
	exit
}
function fail_getconndata () {
	echo "fail_getconndata" $?
	exit
}

function fail_getconninfo () {
	echo "fail_getconninfo" $?
	exit
}
ii=0;
SECONDS=0 ; 
if [ "$skip1" != "1" ]
then
	prepareforgetconndata.sh $lfile | filterforconndata > ${lfile}.forconndata
fi
getconndata ${lfile}.forconndata 2>&1  | grep TOLOG > ${lfile}.tolog
getconninfo ${lfile}.tolog 2>&1  | grep PAIRS > ${lfile}.conninfo
hostconnectivityip ${lfile}.tolog 1
getparamsfromconnanalysis ${lfile}.conninfo ${app} ${fh}
getparamsfromconnanalysis_perdir ${ipmaps} ${lfile}.conninfo ${app} ${fh}

cat < /dev/null > ipmaps_tmp
cat param_REQ_${app}_${fh}0ip | awk '{ dir[$1]=0 ; n[$1]++} END {for (ips in dir) {print ips, dir[ips], n[ips] >>"ipmaps_tmp"}}'
cat param_RSP_${app}_${fh}1ip | awk '{ dir[$1]=1 ;n[$1]++ } END {for (ips in dir) {print ips, dir[ips] , n[ips]>>"ipmaps_tmp"}}'
cat param_REQ_${app}_${fh}1ip | awk '{ dir[$1]=2 ; n[$1]++} END {for (ips in dir) {print ips, dir[ips] , n[ips]>>"ipmaps_tmp"}}'
cat param_RSP_${app}_${fh}0ip | awk '{ dir[$1]=3 ; n[$1]++} END {for (ips in dir) {print ips, dir[ips] , n[ips]>>"ipmaps_tmp"}}'
cat < /dev/null > ipmaps_new_${app}
cat ipmaps_tmp | awk -v app=$app '{ 
if(dir[$1]=="") {dir[$1]=$2;n[$1]=$3} 
		else {
			if($3>n[$1]) {
				dir[$1]=$2;
				n[$1]=$3;
			}
		}
	} END { 
	for (ips in dir) {
		print ips, dir[ips] >>"ipmaps_new_"app
	}
}'
ln -sf `pwd`/ipmaps_new_${app} `pwd`/../ipmaps_new_${app}

for params in `echo reqthink interRRE`
do
	massage_reqthink param_${params}initial_${app}_${fh}ip ${rtttouse} ipmaps_${app}_numbers 2
	massage_reqthink param_${params}initial_${app}_${fh}0ip ${rtttouse} ipmaps_${app}_numbers 0 
	massage_reqthink param_${params}initial_${app}_${fh}1ip ${rtttouse} ipmaps_${app}_numbers 1 
done
preparespecs.all ${app} ${fh} ${totaltime} ${starttime}
preparespecs.all.perdir ${app} ${fh} ${totaltime} ${starttime}

