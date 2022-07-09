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
#  Last Modified : Thu Sep 18 22:48:32 PDT 2008
#
###################################################################################

if [ "$#" -lt "6" ]
then
	echo "Usage $0 <tcpdumpfile> <RANDOM> <maxvn> <tracedir> <Swinghome> <realswinghome>"
	exit
fi

tcpdumpfile=$1
rval=$2
maxvn=$3
tracedir=$4
SWINGHOME=$5
realswinghome=$6
minside0=50
minside1=50
tcpdumpfilename=`echo $tcpdumpfile | sed 's/.*\///'`  
directory=`echo $tcpdumpfile | sed 's/\(.*\)\//\1#/' | sed 's/#.*//'`

bneck_bw="2000"
ipmapsfile=${directory}/ipmaps
cdfdir="cdfdir"
tracefile=${tcpdumpfile}
fixedratio=0;
updatecdfdir=1;
RATIO="0.07"

bneck_bw0=500000
bneck_bw1=500000
bneck0lat=20
bneck1lat=20
bneckplr=0.5;
qslot=20
source ${SWINGHOME}/paramsforscript
source ./extraparams
cd $directory
if [ "$mntcpdump" == "" ]
then
	mntcpdump=0
fi
if [ "$mntcpdump" -ge "2" ]
then
pipe0='"0_1_'${bneck0lat}'_'${bneck_bw0}'_'${bneckplr}' "'
pipe1='"1_0_0_1000000_0.0 "'
else
pipe0='"0_1_'${bneck0lat}'_'${bneck_bw0}'_'${bneckplr}' "'
pipe1='"1_0_'${bneck1lat}'_'${bneck_bw1}'_'0.00' "'
fi

obw=${bneck_bw0}
oplr=0.031
odel=1
#odel=0
rbw=$obw
rbw=${bneck_bw1}
rplr=$oplr
rplr=0.0
rdel=$odel

obw1=${bneck_bw1}
oplr1=0.031
odel1=1
#odel1=0
rbw1=$obw1
rbw1=${bneck_bw0}
rplr1=$oplr1
rplr1=0.0
rdel1=$odel1

rdel=1
rdel1=1
		
rdel=1
rdel1=1
odel1=1
odel=1
fh=${TRACE}
ratiofile=${realswinghome}/bin/ratiofile

numclasses=`cat ${tracedir}/ports | head -n 1 | awk '{printf "%d", $NF+2 }'`

class_s=""
port_s=""
conninfofile_s=""
sessfiles_s=""
clientdistfile_s=""
serverdistfile_s=""
cd ${tracedir}
export maxvn=${maxvn}

tracelength=`( head -n 1 fileboth ; tail -n 1 fileboth )  | awk '{ if(NR==1) star=$1; else en=$1 } END { printf "%d\n", en - star}'`
(
for files in `cat ports | awk '{ if(NR>1) print "fileboth.tcp."$1 } END { print "fileboth.tcp.other" ; print "fileboth.udp.other"}'`
do
echo -ne ${files} " "
thisline=`moving_app_bw ${files} ${tracelength}`
if [ "$thisline" == "" ]
then
	thisline="0 0 0 0.1 0"
fi
echo $thisline
done )| awk '{s[NR]=$4; sum+=$4 } END { for(nrs in s) print nrs, s[nrs]/sum, ENVIRON["maxvn"]*s[nrs]/sum}'|sort -k 1,1n|awk '{
	if(($NF)<1) printf "%d ", 1+$NF; 
	else printf "%d ",$NF
}'>vnvals
vnvals="`cat vnvals`"
cat < /dev/null > dumb-bell-vns
cat < /dev/null > clientservernums;
i=0;
for i in `seq 1 $((numclasses-2)) `
do
	pval=`getportinfo ports $i 1`
	pname=`getportinfo ports $i 3`
	
	class_s=${class_s}"${pname} "
	port_s=${port_s}"${pval} "
	if [ "${bidirectional}" == "1" ]
	then
		conninfofile_s=${conninfofile_s}"${directory}/${pval}/${tcpdumpfilename}.tcp.${pval}.conninfo "
		sessfiles_s=${sessfiles_s}"${directory}/${pval}/param_sessstart_${pval}_${fh}0ip "
		clientdistfile_s=${clientdistfile_s}"${directory}/${pval}/specs_param_clientdist_${pval}_${fh}0 "
		serverdistfile_s=${serverdistfile_s}"${directory}/${pval}/specs_param_serverdist_${pval}_${fh}0 "
		conninfofile_s=${conninfofile_s}"${directory}/${pval}/${tcpdumpfilename}.tcp.${pval}.conninfo "
		sessfiles_s=${sessfiles_s}"${directory}/${pval}/param_sessstart_${pval}_${fh}0ip "
		clientdistfile_s=${clientdistfile_s}"${directory}/${pval}/specs_param_clientdist_${pval}_${fh}0 "
		serverdistfile_s=${serverdistfile_s}"${directory}/${pval}/specs_param_serverdist_${pval}_${fh}0 "
		conninfofile1_s=${conninfofile1_s}"${directory}/${pval}/${tcpdumpfilename}.tcp.${pval}.conninfo "
		sessfiles1_s=${sessfiles1_s}"${directory}/${pval}/param_sessstart_${pval}_${fh}1ip "
		clientdistfile1_s=${clientdistfile1_s}"${directory}/${pval}/specs_param_clientdist_${pval}_${fh}1 "
		serverdistfile1_s=${serverdistfile1_s}"${directory}/${pval}/specs_param_serverdist_${pval}_${fh}1 "
	else
		conninfofile_s=${conninfofile_s}"${directory}/${pval}/${tcpdumpfilename}.tcp.${pval}.conninfo "
		sessfiles_s=${sessfiles_s}"${directory}/${pval}/param_sessstart_${pval}_${fh}ip "
		clientdistfile_s=${clientdistfile_s}"${directory}/${pval}/specs_param_clientdist_${pval}_${fh} "
		serverdistfile_s=${serverdistfile_s}"${directory}/${pval}/specs_param_serverdist_${pval}_${fh} "
	fi
	num=`extract_offset $i $vnvals`
	# num is total number we have access to;
	req0=`cat ${pval}/param_REQ_${pval}_${TRACE}0 |awk '{ a+=$1 }END { printf "%20d\n", a} '`
	if [ "$req0" == "" ]; then req0=0 ; fi
	req1=`cat ${pval}/param_REQ_${pval}_${TRACE}1 |awk '{ a+=$1 }END { printf "%20d\n", a} '`
	if [ "$req1" == "" ]; then req1=0 ; fi
	rsp0=`cat ${pval}/param_RSP_${pval}_${TRACE}0 |awk '{ a+=$1 }END { printf "%20d\n", a} '`
	if [ "$rsp0" == "" ]; then rsp0=0 ; fi
	rsp1=`cat ${pval}/param_RSP_${pval}_${TRACE}1 |awk '{ a+=$1 }END { printf "%20d\n", a} '`
	if [ "$rsp1" == "" ]; then rsp1=0 ; fi
	echo $num ${req0} ${rsp1} ${req1} ${rsp0} | awk '{
	#	print ;
		ttl=$2+$3+$4+$5;
		if(ttl>0)
		printf "%10d %10d %10d %10d\n", $1*$2/ttl, $1*$3/ttl , $1*$4/ttl, $1*$5/ttl >> "clientservernums";
		else
			print "0 0 0 0" >> "clientservernums";
		#; should be way to directly write client0 now.
	}'
	 (moving_app_bw ${tracedir}/file0.tcp.${pval} ${tracelength}; moving_app_bw ${tracedir}/fileboth.tcp.${pval} ${tracelength}) | awk -v num=$num '{ if(NR==1) star=$3; else en=$3} END{ printf "%d %d\n",num*star/en, num - num*star/en}' >>dumb-bell-vns
done
i=$((i+1))
# other tcp
	class_s=${class_s}"TCPOTHER "
	port_s=${port_s}"othertcp "
	if [ "${bidirectional}" == "1" ]
	then
		conninfofile_s=${conninfofile_s}"${directory}/othertcp/${tcpdumpfilename}.tcp.other.conninfo "
		sessfiles_s=${sessfiles_s}"${directory}/othertcp/param_sessstart_othertcp_${fh}0ip "
		clientdistfile_s=${clientdistfile_s}"${directory}/othertcp/specs_param_clientdist_othertcp_${fh}0 "
		serverdistfile_s=${serverdistfile_s}"${directory}/othertcp/specs_param_serverdist_othertcp_${fh}0 "
		conninfofile1_s=${conninfofile1_s}"${directory}/othertcp/${tcpdumpfilename}.tcp.other.conninfo "
		sessfiles1_s=${sessfiles1_s}"${directory}/othertcp/param_sessstart_othertcp_${fh}1ip "
		clientdistfile1_s=${clientdistfile1_s}"${directory}/othertcp/specs_param_clientdist_othertcp_${fh}1 "
		serverdistfile1_s=${serverdistfile1_s}"${directory}/othertcp/specs_param_serverdist_othertcp_${fh}1 "
	else
		conninfofile_s=${conninfofile_s}"${directory}/othertcp/${tcpdumpfilename}.tcp.other.conninfo "
		sessfiles_s=${sessfiles_s}"${directory}/othertcp/param_sessstart_othertcp_${fh}ip "
		clientdistfile_s=${clientdistfile_s}"${directory}/othertcp/specs_param_clientdist_othertcp_${fh} "
		serverdistfile_s=${serverdistfile_s}"${directory}/othertcp/specs_param_serverdist_othertcp_${fh} "
	fi
	num=`extract_offset $i $vnvals`


	pval=othertcp;
	req0=`cat ${pval}/param_REQ_${pval}_${TRACE}0 |awk '{ a+=$1 }END { printf "%20d\n", a} '`
	if [ "$req0" == "" ]; then req0=0 ; fi
	req1=`cat ${pval}/param_REQ_${pval}_${TRACE}1 |awk '{ a+=$1 }END { printf "%20d\n", a} '`
	if [ "$req1" == "" ]; then req1=0 ; fi
	rsp0=`cat ${pval}/param_RSP_${pval}_${TRACE}0 |awk '{ a+=$1 }END { printf "%20d\n", a} '`
	if [ "$rsp0" == "" ]; then rsp0=0 ; fi
	rsp1=`cat ${pval}/param_RSP_${pval}_${TRACE}1 |awk '{ a+=$1 }END { printf "%20d\n", a} '`
	if [ "$rsp1" == "" ]; then rsp1=0 ; fi
	echo $num ${req0} ${rsp1} ${req1} ${rsp0} | awk '{
	#	print ;
		ttl=$2+$3+$4+$5;
		if(ttl>0)
			printf "%10d %10d %10d %10d\n", $1*$2/ttl, $1*$3/ttl , $1*$4/ttl, $1*$5/ttl >> "clientservernums";
		else
			print "0 0 0 0" >> "clientservernums";
		# should be way to directly write client0 now.
	}'






	
	 (f1=`moving_app_bw ${tracedir}/file0.tcp.other ${tracelength}`;
	 f2=`moving_app_bw ${tracedir}/fileboth.tcp.other ${tracelength}`; 
	 if [ "$f1" == "" ]
	 then
	 	f1="0 0 0.1 0.1 0 "
	 fi
	 if [ "$f2" == "" ]
	 then
	 	f2="0 0 0.1 0.1 0 "
	 fi
	 echo $f1
	 echo $f2

	 ) | awk -v num=$num '{ if(NR==1) star=$3; else en=$3} END{ printf "%d %d\n",num*star/en, num - num*star/en}' >>dumb-bell-vns
	
# udp
i=$((i+1))
	class_s=${class_s}"UDP "
	port_s=${port_s}"otherudp "
	if [ "${bidirectional}" == "1" ]
	then
		conninfofile_s=${conninfofile_s}"${directory}/otherudp/${tcpdumpfilename}.udp.other.otherudp.sortedpairs.udpout.fullconnectionanalysisudp "
		sessfiles_s=${sessfiles_s}"${directory}/otherudp/${tcpdumpfile}.udp.other.otherudp.sortedpairs.udpout.fullconnectionanalysisudp "
		clientdistfile_s=${clientdistfile_s}"${directory}/otherudp/specs_param_clientdist_otherudp_${fh} "
		serverdistfile_s=${serverdistfile_s}"${directory}/otherudp/specs_param_serverdist_otherudp_${fh} "
		conninfofile1_s=${conninfofile1_s}"${directory}/otherudp/${tcpdumpfilename}.udp.other.otherudp.sortedpairs.udpout.fullconnectionanalysisudp "
		sessfiles1_s=${sessfiles1_s}"${directory}/otherudp/${tcpdumpfile}.udp.other.otherudp.sortedpairs.udpout.fullconnectionanalysisudp "
		clientdistfile1_s=${clientdistfile1_s}"${directory}/otherudp/specs_param_clientdist_otherudp_${fh} "
		serverdistfile1_s=${serverdistfile1_s}"${directory}/otherudp/specs_param_serverdist_otherudp_${fh} "
	else
		conninfofile_s=${conninfofile_s}"${directory}/otherudp/${tcpdumpfilename}.udp.other.otherudp.sortedpairs.udpout.fullconnectionanalysisudp "
		sessfiles_s=${sessfiles_s}"${directory}/otherudp/${tcpdumpfile}.udp.other.otherudp.sortedpairs.udpout.fullconnectionanalysisudp "
		clientdistfile_s=${clientdistfile_s}"${directory}/otherudp/specs_param_clientdist_otherudp_${fh} "
		serverdistfile_s=${serverdistfile_s}"${directory}/otherudp/specs_param_serverdist_otherudp_${fh} "
	fi
	num=`extract_offset $i $vnvals`


	# the foll causes div zero 'coz of udp
	# but we do not have udp in the current release neway
	# so comment it out
	# (moving_app_bw ${tracedir}/file0.udp.other ${tracelength}; moving_app_bw ${tracedir}/fileboth.udp.other ${tracelength}) | awk -v num=$num '{ if(NR==1) star=$3; else en=$3} END{ printf "%d %d %d %d\n",num*star/en/2, num*star/en/2 , (num - num*star/en)/2, (num - num*star/en)/2}' >> "clientservernums";
	 #(moving_app_bw ${tracedir}/file0.udp.other ${tracelength}; moving_app_bw ${tracedir}/fileboth.udp.other ${tracelength}) | awk -v num=$num '{ if(NR==1) star=$3; else en=$3} END{ printf "%d %d\n",num*star/en, num - num*star/en}' >>dumb-bell-vns

class_s=\"${class_s}\"
port_s=\"${port_s}\"
conninfofile_s=\"${conninfofile_s}\"
sessfiles_s=\"${sessfiles_s}\"
clientdistfile_s=\"${clientdistfile_s}\"
serverdistfile_s=\"${serverdistfile_s}\"
conninfofile1_s=\"${conninfofile1_s}\"
sessfiles1_s=\"${sessfiles1_s}\"
clientdistfile1_s=\"${clientdistfile1_s}\"
serverdistfile1_s=\"${serverdistfile1_s}\"

	cat clientservernums | awk -v minside0=${minside0} -v minside1=${minside1} '{
		c0=$1
		s0=$2
		c1=$3
		s1=$4
		if(c0<minside0) c0=minside0;
		if(s0<minside0) s0=minside0;
		if(c1<minside1) c1=minside1;
		if(s1<minside1) s1=minside1;
		client0s=client0s""c0" ";
		client1s=client1s""c1" ";
		server0s=server0s""s0" ";
		server1s=server1s""s1" ";
	} END{
		print "client0s=\""client0s"\"";
		print "client1s=\""client1s"\"";
		print "server0s=\""server0s"\"";
		print "server1s=\""server1s"\"";
	}' > tmpserversfile
	cd - > /dev/null
client0s="\"100 100 100 30\""
client1s="\"100 100 100 30\""
server0s="\"100 100 100 30\""
server1s="\"100 100 100 30\""
source ${tracedir}/tmpserversfile




skip=1
if [ "$skip" != "1" ]
then
if [ "$numclasses" == "3" ]
then
# not generic right now 

class_s="\"HTTP TCPOTHER UDP\""
port_s="\"80 othertcp otherudp\""
conninfofile_s="\"${directory}/80/${tcpdumpfilename}.tcp.80.conninfo ${directory}/othertcp/${tcpdumpfilename}.tcp.other.conninfo ${directory}/otherudp/${tcpdumpfilename}.udp.other.otherudp.sortedpairs.udpout.fullconnectionanalysisudp\""
sessfiles_s="\"${directory}/80/param_sessstart_80_${fh}ip ${directory}/othertcp/param_sessstart_othertcp_${fh}ip ${directory}/otherudp/${tcpdumpfilename}.udp.other.otherudp.sortedpairs.udpout.fullconnectionanalysisudp\""

clientdistfile_s="\"${directory}/80/specs_param_clientdist_80_${fh} ${directory}/othertcp/specs_param_clientdist_othertcp_${fh} ${directory}/otherudp/specs_param_clientdist_otherudp_${fh}\""
serverdistfile_s="\"${directory}/80/specs_param_serverdist_80_${fh} ${directory}/othertcp/specs_param_serverdist_othertcp_${fh} ${directory}/otherudp/specs_param_serverdist_otherudp_${fh}\""
#serverdistfile=`extract_offset $classnum ${serverdistfile_s}`

(
	cd ${tracedir}
	export maxvn=${maxvn}
	tracelength=`( head -n 1 fileboth ; tail -n 1 fileboth )  | awk '{ if(NR==1) star=$1; else en=$1 } END { printf "%d\n", en - star}'`
	
	for files in `cat ports | awk '{ if(NR>1) print "fileboth.tcp."$1 } END { print "fileboth.tcp.other" ; print "fileboth.udp.other"}'`
	do
		echo -ne ${files} " "
		moving_app_bw ${files} ${tracelength}
	done | awk '{ s[NR]=$4; sum+=$4 } END { for(nrs in s) print nrs, s[nrs]/sum, ENVIRON["maxvn"]*s[nrs]/sum}'|sort -k 1,1n|awk '{printf "%d ",$NF}'>vnvals

	vnvals="`cat vnvals`"

	(
# http

	num=`extract_offset 1 $vnvals`
	 (moving_app_bw file0.tcp.80 ${tracelength}; moving_app_bw fileboth.tcp.80 ${tracelength}) | awk -v num=$num '{ if(NR==1) star=$3; else en=$3} END{ printf "%d %d\n",num*star/en, num - num*star/en}'

#tcpother
	 num=`extract_offset 2 $vnvals`
	 (moving_app_bw file0.tcp.other ${tracelength}; moving_app_bw fileboth.tcp.other ${tracelength}) | awk -v num=$num '{ if(NR==1) star=$3; else en=$3} END{ printf "%d %d\n",num*star/en, num - num*star/en}'

#udpother
	 num=`extract_offset 3 $vnvals`
	 (moving_app_bw file0.udp.other ${tracelength}; moving_app_bw fileboth.udp.other ${tracelength}) | awk -v num=$num '{ if(NR==1) star=$3; else en=$3} END{ printf "%d %d\n",num*star/en, num - num*star/en}'

	) >dumb-bell-vns
	cat dumb-bell-vns | awk '{
		this0=$1
		this1=$2
		#if(this0<1) this0=1;
		#if(this1<1) this1=1;
		# below commented out only for job talk
		if(this0<10) this0=10;
		if(this1<10) this1=10;
		c0=(int(this0/2));
		s0=this0-c0;
		c1=(int(this1/2));
		s1=this1-c1;
		client0s=client0s""c0" ";
		client1s=client1s""c1" ";
		server0s=server0s""s0" ";
		server1s=server1s""s1" ";
	} END{
		print "client0s=\""client0s"\"";
		print "client1s=\""client1s"\"";
		print "server0s=\""server0s"\"";
		print "server1s=\""server1s"\"";
	}' > tmpserversfile
	cd -
)
client0s="\"100 100 30\""
client1s="\"100 100 30\""
server0s="\"100 100 30\""
server1s="\"100 100 30\""
source ${tracedir}/tmpserversfile
elif [ "$numclasses" == "4" ]
then

class_s="\"HTTP TCPOTHER P2P UDP\""
port_s="\"80 othertcp 6699 otherudp\""
conninfofile_s="\"${directory}/80/${tcpdumpfilename}.tcp.80.conninfo ${directory}/othertcp/${tcpdumpfilename}.tcp.other.conninfo ${directory}/6699/${tcpdumpfilename}.tcp.6699.conninfo ${directory}/otherudp/${tcpdumpfilename}.udp.other.otherudp.sortedpairs.udpout.fullconnectionanalysisudp\""
sessfiles_s="\"${directory}/80/param_sessstart_80_${fh}ip ${directory}/othertcp/param_sessstart_othertcp_${fh}ip ${directory}/6699/param_sessstart_6699_${fh}ip ${directory}/otherudp/${tcpdumpfilename}.udp.other.otherudp.sortedpairs.udpout.fullconnectionanalysisudp\""

clientdistfile_s="\"${directory}/80/specs_param_clientdist_80_${fh} ${directory}/othertcp/specs_param_clientdist_othertcp_${fh} ${directory}/6699/specs_param_clientdist_6699_${fh} ${directory}/otherudp/specs_param_clientdist_otherudp_${fh}\""
serverdistfile_s="\"${directory}/80/specs_param_serverdist_80_${fh} ${directory}/othertcp/specs_param_serverdist_othertcp_${fh} ${directory}/6699/specs_param_serverdist_6699_${fh} ${directory}/otherudp/specs_param_serverdist_otherudp_${fh}\""




(
	cd ${tracedir}
	export maxvn=${maxvn}
	tracelength=`( head -n 1 fileboth ; tail -n 1 fileboth )  | awk '{ if(NR==1) star=$1; else en=$1 } END { printf "%d\n", en - star}'`
	
	for files in `cat ports | awk '{ if(NR>1) print "fileboth.tcp."$1 } END { print "fileboth.tcp.other" ; print "fileboth.udp.other"}'`
	do
		echo -ne ${files} " "
		moving_app_bw ${files} ${tracelength}
	done | awk '{ s[NR]=$4; sum+=$4 } END { for(nrs in s) print nrs, s[nrs]/sum, ENVIRON["maxvn"]*s[nrs]/sum}'|sort -k 1,1n|awk '{printf "%d ",$NF}'>vnvals

	vnvals="`cat vnvals`"

	(
# http

	 num=`extract_offset 1 $vnvals`
	 (moving_app_bw file0.tcp.80 ${tracelength}; moving_app_bw fileboth.tcp.80 ${tracelength}) | awk -v num=$num '{ if(NR==1) star=$3; else en=$3} END{ printf "%d %d\n",num*star/en, num - num*star/en}'

#tcpother
	 num=`extract_offset 2 $vnvals`
	 (moving_app_bw file0.tcp.other ${tracelength}; moving_app_bw fileboth.tcp.other ${tracelength}) | awk -v num=$num '{ if(NR==1) star=$3; else en=$3} END{ printf "%d %d\n",num*star/en, num - num*star/en}'

#p2p
	 num=`extract_offset 3 $vnvals`
	 (moving_app_bw file0.tcp.6699 ${tracelength}; moving_app_bw fileboth.tcp.6699 ${tracelength}) | awk -v num=$num '{ if(NR==1) star=$3; else en=$3} END{ printf "%d %d\n",num*star/en, num - num*star/en}'

#udpother
	 num=`extract_offset 4 $vnvals`
	 (moving_app_bw file0.udp.other ${tracelength}; moving_app_bw fileboth.udp.other ${tracelength}) | awk -v num=$num '{ if(NR==1) star=$3; else en=$3} END{ printf "%d %d\n",num*star/en, num - num*star/en}'

	) >dumb-bell-vns
	cat dumb-bell-vns | awk '{
		this0=$1
		this1=$2
		#if(this0<1) this0=1;
		#if(this1<1) this1=1;
		# aboce for job talk
		if(this0<10) this0=10;
		if(this1<10) this1=10;
		c0=(int(this0/2));
		s0=this0-c0;
		c1=(int(this1/2));
		s1=this1-c1;
		client0s=client0s""c0" ";
		client1s=client1s""c1" ";
		server0s=server0s""s0" ";
		server1s=server1s""s1" ";
	} END{
		print "client0s=\""client0s"\"";
		print "client1s=\""client1s"\"";
		print "server0s=\""server0s"\"";
		print "server1s=\""server1s"\"";
	}' > tmpserversfile
	cd -
)
client0s="\"100 100 100 30\""
client1s="\"100 100 100 30\""
server0s="\"100 100 100 30\""
server1s="\"100 100 100 30\""
source ${tracedir}/tmpserversfile



fi
fi # for skip=1
#not generic above

echo "cdfdir=${cdfdir}"
echo "ipmapsfile=${ipmapsfile}"
echo "tracefile=${tracefile}"
echo "RANDOM=${rval}"
echo "fixedratio=${fixedratio}"
echo "updatecdfdir=${updatecdfdir}"
echo "RATIO=${RATIO}"
echo "qslot=${qslot}"
echo "pipe0=${pipe0}"
echo "pipe1=${pipe1}"
#echo "cdffile0=${cdffile0}"
#echo "cdffile1=${cdffile1}"
echo "oplr=${oplr}"
echo "oplr1=${oplr1}"
echo "odel=${odel}"
echo "odel1=${odel1}"
echo "rbw=${rbw}"
echo "rbw1=${rbw1}"
echo "rplr=${rplr}"
echo "rplr1=${rplr1}"
echo "rdel=${rdel}"
echo "rdel1=${rdel1}"
echo "ratiofile=${ratiofile}"
echo "class_s=${class_s}"
echo "port_s=${port_s}"
echo "conninfofile_s=${conninfofile_s}"
echo "sessfiles_s=${sessfiles_s}"
echo "clientdistfile_s=${clientdistfile_s}"
echo "serverdistfile_s=${serverdistfile_s}"
echo "conninfofile1_s=${conninfofile1_s}"
echo "sessfiles1_s=${sessfiles1_s}"
echo "clientdistfile1_s=${clientdistfile1_s}"
echo "serverdistfile1_s=${serverdistfile1_s}"
echo "client0s=\"${client0s}\""
echo "client1s=\"${client1s}\""
echo "server0s=\"${server0s}\""
echo "server1s=\"${server1s}\""
echo "numclasses=${numclasses}"
echo "tracedir=\"${tracedir}\""
echo "bidirectional=${bidirectional}"

# client0 and server0 are on side 0 
# client1 and server1 are on side 1
