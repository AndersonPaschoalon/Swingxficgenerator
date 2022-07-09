#!/bin/bash -x
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





HTTPPERCENTAGE="1.0"
SWINGHOME=$1
swinginitiate.sh ${SWINGHOME}
source params.file

function quitscript {
	echo 
	echo "exitting prematurely due to errors..."
	exit -1
}

# the script ...


KILLTIME=""
TIMEOUT=4000

RANDOM=3335
RANDOM=3395
if [ "$#" -lt "4" ]
then
	echo "Usage: $0 <swinghome> <duration> <skipperiod> <repeattimes> [<killtime> <random>] "
	echo " [31;43m duration   : [m period of the trace                    (secs) 
 [31;43m skipperiod : [m granularity of session arrivals        (msec) 
 [31;43m repeattime : [m is as it suggests                      (secs)
 [31;43m killtime   : [m when will a kill be issued
 [31;43m random     : [m a RANDOM SEED"
	exit
fi

DURATION=1
if [ "$2" != "" ]
then
	DURATION=$2
fi
SKIPPERIOD=1
if [ "$3" != "" ]
then
	SKIPPERIOD=$3
fi
REPEATTIMES=1
if [ "$4" != "" ]
then
	REPEATTIMES=$4
fi
if [ "$5" != "" ]
then
	KILLTIME=$5
fi

if [ "$6" != "" ]
then
	RANDOM=$6
fi
if [ "$7" != "" ]
then
	EXTRAPARAM=$7
else
	EXTRAPARAM=0
fi
tparamm=$((EXTRAPARAM+1))
DURATION=$((DURATION*1000))
source ./paramsforscript
source ./extraparams
source ./JOBVARIABLES


echo "starting for duration $DURATION msecs, skipperiod $SKIPPERIOD secs 
thus a repeat of $REPEATTIMES times"
echo "kill all and log me out too prior to runs"
#hostrun ${modelfile} ./killandlogout > /dev/null || quitscript
hostrun ${modelfile} kill -9 -1 > /dev/null || quitscript
echo clean up everything
${SWINGHOME}/bin/killthingsforswing ${SWINGHOME}> /dev/null || quitscript

echo clean up everything....................................done



cdfdir=${tracedir}
numports=`cat ${tracedir}/ports | head -n 1|awk '{printf "%d",$2}'`
if [ "$bidirectional" == "1" ]
then
 for dirc in `echo 0 1`
 do
   for i in `seq 1 $numports`
   do
	 pval=`getportinfo ${tracedir}/ports $i 1`
	 pname=`getportinfo ${tracedir}/ports $i 3`
	 set ${pname}${dirc}port=$pval
	 set ${pname}${dirc}cdfdir=${cdfdir}/${pval}
   done
 done # dirc 0/1
else
 for i in `seq 1 $numports`
 do
	pval=`getportinfo ${tracedir}/ports $i 1`
	pname=`getportinfo ${tracedir}/ports $i 3`
	set ${pname}port=$pval
	set ${pname}cdfdir=${cdfdir}/${pval}
 done
fi
httpport=80
p2pport=6699
nntpport=119
smtpport=25
sshport=22
tcpotherport=othertcp
udpotherport=otherudp
httpcdfdir=${cdfdir}/${httpport}
ftpcdfdir=${cdfdir}/20
smtpcdfdir=${cdfdir}/25
p2pcdfdir=${cdfdir}/${p2pport}
nntpcdfdir=${cdfdir}/${nntpport}
smtpcdfdir=${cdfdir}/${smtpport}
sshcdfdir=${cdfdir}/${sshport}
tcpothercdfdir=${cdfdir}/${tcpotherport}
udpothercdfdir=${cdfdir}/${udpotherport}
mkdir -p cdfdir
echo -n "preparing cdf files"

numclasses=`cat ${tracedir}/ports | head -n 1 | awk '{printf "%d", $NF+2 }'`
if [ "$numclasses" == "4" ]
then
	p2pport=`cat ${tracedir}/ports | tail -n 1 | awk '{printf "%d", $1}'`
fi

testing=yes
testing=no




if [ "$testing" != "yes" ]
then

app=${TRACE}
fh=${app}

echo "....................................done"
#exit
if [ "$fh" == "Mawi" ]
then
borrowclassescdf=1
else
borrowclassescdf=0
fi
borrowclassescdf=0
if [ "${borrowclassescdf}" != "1" ]
then
cat < /dev/null> classes.cdf.${fh}
nohackapp=1 
# 0 means hackapp i.e. selectively 1 by 1
# can put here as a variant of what already exists for
# nohackapp for toposubstitutedir
if [ "$toposubstitutedir" == "" ]
then
	toposubstitutedir=${tracedir}
fi
if [ "$paramsubstitutedir" == "" ]
then
	paramsubstitutedir=${tracedir}
fi


if [ "$bidirectional" == "1" ]
then
  for dirc in `echo 0 1`
  do
	for i in `seq 1 $numports`
	do
		if [ "$nohackapp" == "1" ]
		then
		tparamm=$i
		fi
		if [ "$i" == "${tparamm}" ]
		then
		pval=`getportinfo ${tracedir}/ports $i 1`
		pname=`getportinfo ${tracedir}/ports $i 3`
		set ${pname}port=$pval
		set ${pname}cdfdir=${cdfdir}/${pval}
		pwd
		makeclasses.cdf ${cdfdir} ${pval} ${fh}${dirc} ${pname}${dirc}_like ${totaltime} ${starttime} >> classes.cdf.${fh}
		fi
	done
  done # for dirc 0 / 1
else
	for i in `seq 1 $numports`
	do
		if [ "$nohackapp" == "1" ]
		then
		tparamm=$i
		fi
		if [ "$i" == "${tparamm}" ]
		then
		pval=`getportinfo ${tracedir}/ports $i 1`
		pname=`getportinfo ${tracedir}/ports $i 3`
		set ${pname}port=$pval
		set ${pname}cdfdir=${cdfdir}/${pval}
		makeclasses.cdf ${cdfdir} ${pval} ${fh} ${pname}_like ${totaltime} ${starttime}>> classes.cdf.${fh}
		fi
	done
fi


if  [ "$swingtest" != "1" ]
then

if [ "$bidirectional" == "1" ]
then
	makeclasses.cdf ${cdfdir} ${tcpotherport} ${fh}0 TCPOTHER0_like ${totaltime} ${starttime} >>classes.cdf.${fh}
	makeclasses.cdf ${cdfdir} ${tcpotherport} ${fh}1 TCPOTHER1_like ${totaltime} ${starttime} >>classes.cdf.${fh}
else
	makeclasses.cdf ${cdfdir} ${tcpotherport} ${fh} TCPOTHER_like ${totaltime} ${starttime} >>classes.cdf.${fh}
fi
else
	cp ${tracedir}/classes.cdf.${fh} .
fi # swingtest
fi
if [ "$borrowclassescdf" == "1" ]
then
	cp classes.cdf.Mawi.cs6 classes.cdf.Mawi
fi
cat classes.cdf.${fh} > classes.cdf
cat specs.all.${fh} > specs.all

# only for now... remove later XXX
#cp specs.all /home/kvishwanath/Swing/swing_code
#cp specs.all ${SWINGHOME}


echo -n generating jobs file

inter_arrival_session_cdf=1
if [ "$multihops" == "" ]
then
	multihops=1
fi
genjobs classes.cdf  $DURATION $SKIPPERIOD ${inter_arrival_session_cdf} ${RANDOM} ${NUMSESSION_TIMES} ${HTTPPERCENTAGE} > tmpoutfile || quitscript 
if [ "$swingtest" != "1" ]
then
thiscmdline="generic tmpoutfile $RANDOM ${multihops} -class UDP cdfdir/inputforgeneric_UDP -class TCPOTHER cdfdir/inputforgeneric_TCPOTHER -dir 1"
else
thiscmdline="generic tmpoutfile $RANDOM ${multihops}"
fi
if [ "$bidirectional" == "1" ]
then
	for i in `seq 1 $numports`
	do
		if [ "$nohackapp" == "1" ]
		then
	 tparamm=$i
	 fi
	 if [ "$i" == "${tparamm}" ]
	 then
		pval=`getportinfo ${tracedir}/ports $i 1`
		pname=`getportinfo ${tracedir}/ports $i 3`
		thiscmdline=${thiscmdline}" -class ${pname} cdfdir/inputforgeneric_${pname} -dir 1"
	 fi
	done
else
	for i in `seq 1 $numports`
	do
		if [ "$nohackapp" == "1" ]
		then
	tparamm=$i
	fi
	 if [ "$i" == "${tparamm}" ]
	 then
		pval=`getportinfo ${tracedir}/ports $i 1`
		pname=`getportinfo ${tracedir}/ports $i 3`
		thiscmdline=${thiscmdline}" -class ${pname} cdfdir/inputforgeneric_${pname}"
	 fi
	done
fi
echo $thiscmdline
echo "EXTRAPARAM $EXTRAPARAM"
skip4=1
if [ "${skip4}" != "1" ]
then
if [ "$EXTRAPARAM" == "0" ]
then
	${thiscmdline} | grep "HTTP\|UDP"  > timecommonfile 2>generic_Err || quitscript
elif [ "$EXTRAPARAM" == "1" ]
then
	${thiscmdline} | grep "NAPSTER\|UDP"  > timecommonfile 2>generic_Err || quitscript
elif [ "$EXTRAPARAM" == "2" ]
then
	${thiscmdline} | grep "SMTP\|UDP"  > timecommonfile 2>generic_Err || quitscript
elif [ "$EXTRAPARAM" == "3" ]
then
	${thiscmdline} | grep "EDONKEY\|UDP"  > timecommonfile 2>generic_Err || quitscript
elif [ "$EXTRAPARAM" == "4" ]
then
	${thiscmdline} | grep "KAZAA\|UDP"  > timecommonfile 2>generic_Err || quitscript
fi
fi
echo $thiscmdline
${thiscmdline}  | grep -v UDP  > timecommonfile 2>generic_Err || quitscript

if [ "$swingtest" == "1" ]
then
	cat timecommonfile.swingtest > timecommonfile
fi

echo "...................................done"
echo -n making timecommonfile repeat $REPEATTIMES times
cp timecommonfile timecommonfile.bak || quitscript

repeattimecommonfile timecommonfile $REPEATTIMES $DURATION $RANDOM ${RSPtime} ${REQtime} ${numconn} ${numpair} || quitscript
	cp timecommonfile.new timecommonfile || quitscript
else
	echo in else part 
fi
echo "...................done"

echo -n generating indifiles centrally
${SWINGHOME}/bin/analysetimecommonfile  > /dev/null || quitscript 
echo " ........................done "
cat < /dev/null > pktsizeSwing
echo -n "starting tcpdump file on cores "
stopdumpfile > /dev/null || quitscript
startdumpfile > /dev/null || quitscript 
echo "........................done"

if [ "$runswing" == "1" ]
then
echo  starting listeners
startlisteners || quitscript
echo "starting listeners ....................................done"
fi
echo -n "preparing linux guys .................."
preparelinuxguys > /dev/null || quitscript
echo    "................done"
killallinseconds $KILLTIME


if [ "$runswing" != "0" ]
then

echo kickingoff-clients
kickoff-clients $KILLTIME || quitscript 
echo "kicking-off clients....................................done"
fi
done=1

while [ "$done" == "0" ]
do
	numprocs=`ps aux | grep gexec_script | wc -l`
	if [ "$numprocs" -le "1" ] 
	then
		done=1
	fi
	if [ "$SECONDS" -gt "$TIMEOUT" ]
	then
	    echo timeout
		done=1
	fi
	echo sleep 10 seconds more
	sleep 10
done

date


echo -n "kill everything in the end"
(killthingsforendofrunswing > /dev/null || quitscript ) &
f=$!
sleep 30
echo "            done"
kill -9 ${f}
echo -n "stop tcpdumpfile"
stopdumpfile > /dev/null || quitscript 
echo "            done"
echo -n "gather tcpdumpfiles"
gathertcpdumpfile
echo "            done"
date
echo "Traffic generation completed successfully..................."
