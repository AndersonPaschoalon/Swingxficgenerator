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

# this will change the paramsforscript as appropriate and also call the
# sscript file
if [ "$#" -lt "4" ]
then
	echo "Usage: $0 <exptpath> <thisexptdir> <origtracedir> <SWINGHOME>"
	exit
fi
exptpath=$1
thisexptdir=$2
origtrcdir=$3
SWINGHOME=$4

exptdir=${thisexptdir}



done=0;

RANDOM=$RANDOM
RUNDESCRIPTION="testing with new bw thing"
tracestorun="mawi-casestudy-15-mins5"

bidirectional=1
thisbasedir=${exptpath}
thisdir=${exptdir}
mkdir -p ${thisdir}
for traces in `echo $tracestorun`

do

	cd ${exptdir} || exit "cannot cd to the expt dir"
	cat ${origtrcdir}/paramsforscript.template_entry > paramsforscript
	echo $RUNDESCRIPTION >rundescription
	echo "RANDOM=$RANDOM">>paramsforscript
	echo "bidirectional=${bidirectional}">>paramsforscript


	JOBDIRS=${SWINGHOME}/JOBDIRS
	HTTPPERCENTAGE=1.0;
	bneckplr=0.0;
	numhostss="12"
	cdfs="ratio_proper_auck_10mbps_rttmix_correct_capa_median_0_1"
	maxtimes=1; # for  starttimes <= i <= maxtimes 
	starttimes=0;
	runtime=120
	killtime=$((runtime*2+30))
	plrvalues="0.00 0.04 0.02 0.07"
	plrfile=0
	emgvalues="emg_50 emg_75 emg_90"
	emgvalues="emg_50 emg_90"
	plrvalues="0.04"
	plrvalues="0.04"
	rttvalues="rtt_5 rtt_90 rtt_90"
	bwvalues="100000 90000 75000 50000 25000 20000 10000"
	latencies="0"
	psizes="psize_90"
	apptimes=0;
	samerandom=0
	plrvalues="plr" 
	plrvalues="plr0"
	plrvalues="plr0"
	plrvalues="plr0"
	rttvalues="rtt_1 rtt_5" 
	rttvalues="rtt_50"
	bwvalues="100000" 
	emgvalues="emg_50 emg_90"  
	emgvalues="emg_50 emg_75 emg_90"
	emgvalues="emg_75"
	qslots="2000"
	weightedsamplesperdirs="1"
	weights="1" # XXXXXXXXX change this to 1 SOON.......
	cdfsbase="mawitraces" 
	tracenameparam="mawi"
	toposizes="1000"
	toposizes="2000"
	RATIOS="0.01"
	skip2=0;
	skip1=0;
	skip3=0;
	topogenskip=0;
	deployskip=0;
	runskip=0;
	alltoposkip=0;
	onlyonce=0 ; # => will use runtime and killtime if onlyonce=0
	ipmapsdir=4;
	TRACE="Auck"
	TRACE="mawi"
	original_ipaddress_cdf=0
	hacktopo=0;
	dilated=0;
	dilationfactor=10;
	defaults=0;
	timesplr=1;
	timespsize=1;
	timeskbps=1;
	timesdelay=1;
	delaytimes="1"
	delaytimes="1t0"
	reqthinks="1t0"
	interconns="1t0"
	interrres="1t0"
	delayplr=${delaytimes}
	delaypsize=${delaytimes}
	delaykbps=${delaytimes}
	delayplr="1t0 1.5t0 0.5t0" 
	reqthinks="1t0 0t0"
	interconns="1t0 0t0"
	interrres="1t0 0t0"

	delayplr="0t0.03"

	delayplr="1t0.02"
	interrres="1t0"
	interconns="1t0"
	reqthinks="1t0"
	NUMSESSION_TIMESs="10.0"

	parallelapplication="1"
	runswing="0"


	cs15run=0
	SPFrun=0
	variablemax=0
	twowaymax=1
	maxmems="16382 32768 65536 131070"
	maxmems="4096 32768 8192 16382"
	maxmems="110000 120000 130000"
	maxmems="65536 80000 100000"
	maxmems="4096 32768 8192 16382"
	maxmems="16382 8192"
	maxmems="16382 8192 4096"
	maxmems="32768"
	maxmems="131070 65536 32768"
	maxmems="4096 8192 16382 32768 65536 131070"

	maxmems="4096"
	maxdefs="4096"
	maxmems="65536"
	maxdefs="4096 8192 16382 32768 65536 131070"
	maxmems="32768"
	maxdefs="8192 4096 16382"
	maxdefs="16382"
	maxmems="4096"
	maxmems="65536"
	maxdefs="17520"
	maxmems="131070"

	if [ "${variablemax}" != "1" ]
	then
		maxmems="16382"
		maxdefs="16382"
	fi

	httpsquidnewratio="1"

	delaykbps="1t0" 
	delayplr="1t0" 
	delaytimes="1t0" 
	delaypsize="1t0" 
	numpairs="1t0" 
	numconns="1t0" 
	REQtimes="1t0" 
	RSPtimes="1t0"

	reqthinks="1t0"
	interconns="1t0"
	interrres="1t0"

	HTTPPERCENTAGEs="1.0"
	NUMSESSION_TIMESs="1.0"
	NUMSESSION_TIMESs="1.0"

	topoprmcopynmbr=5
	dirsadjust=0
	copyparamnumber=0

	qmonitor="0"
	qmonitorinterval="1"
	qmonitorhop="0"
	countingarrivals="0"
	poissonvsfixeds="2"
	if [ "$poissonvsfixeds" == "2" ]
	then
		rateofpoissonvsfixed="1"
		rateofpoissonvsfixed2="1"
		parallelswing="0"
	fi
	if [ "$countingarrivals" == "1" ]
	then
		timescalescounting="0.001"
	else
		timescalescounting="1"
	fi

	parallelapplications="0"
	runswings="1" 

	if [ "$parallelswing" != "0" ] 
	then 
		runswings="0" 
	fi
	allmultihops="2 3"



	if  test -f "paramsforscript" 
	then
		source ./paramsforscript
		fh=${TRACE}
		basedir=${tracedir}
	fi

	applat=41 # TSTPR1
	applat=10 # TSTPR2
	applat=132 # TSTPR3
	applat=3 # TSTPR5
	applat=9 # PLAB9's swing runs
	tracebws="30000"
	tracebws="12000"
	tracebws="60000"
	tracebws="44000"
	tracebws="14000" # for PLAB11's swing runs
	tracebws="12000" # PLAB12
	tracebws="11000" # PLAB13
	tracebws="35000" # new PLAB9

	tracebws="30000" ; applat="10" ; allmultihops="4" ; delaytimes="1t0" # plab4
	tracebws="20000" ; applat="40" ; allmultihops="4" ; delaytimes="1t0" # plab2
	tracebws="14000" ; applat="75" ; allmultihops="1" ; delaytimes="1t0" # plab11
	tracebws="50000" ; applat="3" ; allmultihops="1 4 2 3" ; delaytimes="1t0" # plab14
	tracebws="50000" ; applat="3" ; allmultihops="5 6" ; delaytimes="1t0" # plab14

	tracebws="82000" ; applat="3" ; allmultihops="1 4" ; delaytimes="1t0" # plab14
	tracebws="71000 80000" ; applat="3" ; allmultihops="4" ; delaytimes="1t0" # plab14
	tracebws="50000" ; applat="3" ; allmultihops="4" ; delaytimes="1t0" # plab14
	tracebws="50000" ; applat="3" ; allmultihops="4" ; delaytimes="1t0" # plab14
	tracebws="61000" ; applat="3" ; allmultihops="4" ; delaytimes="1t0" # plab14
	tracebws="80000" ; applat="3" ; allmultihops="4" ; delaytimes="1t0" # plab14
	tracebws="50000" ; applat="3" ; allmultihops="5" ; delaytimes="1t0" # plab14

	tracebws="100000" ; applat="3" ; allmultihops="1" ; delaytimes="1t0" # plab14
	allmultihops="1" ; applat="5";
	allmultihops="1" ; applat="2";
	#allmultihops="2 3"
	allmultihops="1"

	applat="2";
	applat="0";
	applat="10" ; allmultihops="1" ; # for multihop
	applat="4"; # for plpc postnsdi tsmatters validation
	applat="30"; # for httpvalidationofpredictionduringandafterjobtalk
	applat="15"; # for httpvalidationofpredictionduringandafterjobtalk
	applat="16"; # for httpvalidationofpredictionduringandafterjobtalk
	tracebws="12000 20000 50000 100000 200000 400000 600000" # for pathload
	tracebws="12000 20000 50000 100000 200000 400000 600000" # for pathchirp
	tracebws="50000" # for pathchirp/pathload burstiness vary
	delaytimes="1t0"
	delaytimes="1t0"
	delaytimes="1t0"
	tracebws="100000" # for pathchirp/pathload burstiness vary
	tracebws="10000" # for Casestudy of diff timescales effecting in diff ways.
	tracebws="20000" # for Fig 8 of orig sigcomm07 submission where all traces and their HB are with 20mbps links
	tracebws="10000" # for multihops
	tracebws="1000000" # for multihops
	tracebws="100000" # for multihops
	tracebws="100000" # for  PL/PC validate
	tracebws="10000" # for YAZ
	tracebws="100000" # for YAZ
	tracebws="10000" # for YAZ
	tracebws="50000" # for RP
	tracebws="4000" # for RP
	tracebws="10000" # for multihop
	tracebws="100000" # sigmetrics
	tracebws="10000" # for validation/prediction/during and after job talk
	tracebws="20000" # for fig4 redo of usenix08 cmra rdy.
	tracebws="100000" ; delaytimes="0t2"  # bwest redo of usenix08 cmra rdy.
	tracebws="100000" ; delaytimes="1t0"  # bwest redo of usenix08 cmra rdy.

	numhostsvary="0"
	numhostss="10"

	delaytimes="1t0"
	REQtimes="1t0"
	RSPtimes="1t0"

	allinoneforcs15="0"
	if [ "${allinoneforcs15}" == "1" ]
	then
		delaytimes="0.5t0"
		REQtimes="1.5t0"
		RSPtimes="1.5t0"
		delaykbps="2t10000"
		delaypsize="0t1460"
		numpairs="2t-1"
		reqthinks="0.5t0"
		NUMSESSION_TIMESs="10.0"

	fi

	hostclasses="1" # 1 => diff machines for diff classes.
	hostclasses="0" # 1 => diff machines for diff classes.
	qslots="100"
	qslots="1000"
	qslots="100"
	qslots="100"
	if [ "$poissonvsfixeds" != "2" ]
	then
		qslots="1000"
	fi
	toposizes="100"
	toposizes="50"
	toposizes="1000"
	#toposizes="1000"
	pathloadtolerance="1"
	diffbitratefile="realvideo10_256.rm"
	diffbitratefile="realvideo10_56.rm"
	diffbitratefile="realvideo10.rm"
	diffbitratefile=""
	diffbitratefile="30" # overloading for PCP
	tcpandnotudp=0
	bneckplrs="0.00"
	cat < /dev/null > create_topofiles_params
	paramsforscriptpath=`pwd`/paramsforscript


	if [ "$samerandom" == "1" ]
	then
		savedrandom=$RANDOM
	fi

	if [ "$plrfile" == "1" ]
	then
		plr="plr"
	fi

	SECONDS=0

	echo "dilated=${dilated}" >> create_topofiles_params
	echo "dilationfactor=${dilationfactor}" >>create_topofiles_params


	harmonicmean_capacity=1
	mntcpdumps="0"
	if [ "$mntcpdumps" == "" ]
	then
		mntcpdumps=$mntcpdump
	fi

	for hc in `echo ${hostclasses}`
	do
		for HTTPPERCENTAGE in `echo ${HTTPPERCENTAGEs}`
		do
			for poissonvsfixed in `echo ${poissonvsfixeds}`
			do
				for countingarrival in `echo ${countingarrivals}`
				do
					for timescalecounting in `echo ${timescalescounting}`
					do
						for runswing in `echo ${runswings}`
						do
							for parallelapplication in `echo ${parallelapplications}`
							do
								for numhosts in `echo ${numhostss}`
								do
									for mntcpdump in `echo ${mntcpdumps}`
									do
										for bneckplr in `echo ${bneckplrs}`
										do
											for tracebw in `echo $tracebws`
											do

												bw=${tracebw}


												if [ "$poissonvsfixed" == "0" ] || [ "${poissonvsfixed}" == "1" ] || [ "$poissonvsfixed" == "5" ] || [ "${poissonvsfixed}" == "11" ] || [ "${poissonvsfixed}" == "15" ]
												then
													delaykbps="0t${tracebw}"
													delayplr="0t0"
													delaytimes="0t1"
												fi



												if [ "$dilated" != "0" ]
												then
													tracebw=$((tracebw/dilationfactor))
													bneck0lat=$((bneck0lat*dilationfactor))
													bneck1lat=$((bneck1lat*dilationfactor))
												fi
												bwoftrace=${tracebw}

												for lat in `echo $latencies`
												do
													bneck0lat=$lat
													bneck1lat=$lat
													#lat=${bneck0lat}
													echo "bneck0lat=${bneck0lat}" >> paramsforscript
													echo "bneck1lat=${bneck1lat}" >> paramsforscript
													for multihops in `echo ${allmultihops}`
													do
														for qslot in `echo $qslots`
														do
															echo "qslot=${qslot}" >> create_topofiles_params
															echo "qslot=${qslot}" >> paramsforscript

															for maxmem in `echo ${maxmems}`
															do
																for maxdef in `echo ${maxdefs}`
																do
																	for REQtime in `echo $REQtimes`
																	do

																		for RSPtime in `echo $RSPtimes`
																		do

																			for numconn in `echo $numconns`
																			do
																				for numpair in `echo $numpairs`
																				do
																					for INTERRRE in `echo $interrres`
																					do
																						for NUMSESSION_TIMES in `echo $NUMSESSION_TIMESs`
																						do
																							for INTERCONN in `echo $interconns`
																							do	
																								for REQTHINK in `echo $reqthinks`
																								do
																									for toposize in `echo $toposizes`
																									do
																										for RATIO in `echo $RATIOS`
																										do
																											for psize in `echo $psizes`
																											do
																												for emg in `echo $emgvalues`
																												do
																													for weightedsamplesperdir in `echo $weightedsamplesperdirs`
																													do
																														for plr in `echo $plrvalues`
																														do
																															for rtt in `echo $rttvalues`
																															do
																																for timesdelay in `echo ${delaytimes}` 
																																do
																																	for timeskbps in `echo ${delaykbps}` 
																																	do
																																		for timesplr in `echo ${delayplr}` 
																																		do
																																			for timespsize in `echo ${delaypsize}` 
																																			do
																																				if [ "$samerandom" == "1" ] ; then RANDOM=$savedrandom ; fi
																																				for ((times=starttimes;times<maxtimes;times++))
																																				do
																																					copyparamnumber=${times}
																																					copyparamnumber=0
																																					copyparamnumber=15
																																					cdfs=$cdfsbase
																																					bneck0lat=${bneck0lat}
																																					bneck1lat=${bneck1lat}

																																					thisbasedir=${exptpath}
																																					if [ "$runswing" != "0" ] || [ "$parallelapplication" != "0" ] || [ "${parallelswing}" != "0" ]
																																					then
																																						echo "experiment details/results in $thisdir"
																																						set| grep -v SSH | grep -v PPID | grep -v BASH | grep -v UID | grep -v SHELL | grep -v GROUP > ${thisdir}/JOBVARIABLES
																																						if [ "$hc" == "1" ]
																																						then
																																							echo "numhostsvary=1" >> ${thisdir}/JOBVARIABLES
																																						fi
																																						echo "NUMSESSION_TIMES=${NUMSESSION_TIMES} " >> ${thisdir}/paramsforscript
																																						echo "state=jobcreated" > ${thisdir}/STATE
																																						echo ${thisdir} >> ${JOBDIRS}
																																					fi

																																				done # for loop
																																			done # for psize
																																		done # for plr
																																	done # for kbps
																																done # for delay
																															done   # rtt
																														done   # plrvalues
																													done # weights
																												done # emg
																											done # psizes
																										done # RATIO
																									done # toposizes
																								done # interconns
																							done # NUMSESSION_TIMESs
																						done #reqthinks
																					done #lat
																				done #qslot
																			done #multihops
																		done   # bwvalues

																	done # bneckplr
																done #mntcpdumps
															done # interrres
														done # numpairs
													done # numconns
												done # RSPtimes
											done # R2Qtimes
										done # maxdefs
									done # maxmems
								done # numhosts
							done # parallelapplications
						done # runswings
					done # timescalescounting
				done # countingarrivals
			done # poissonvsfixeds
		done # HTTPPERCENTAGE
	done # hostclasses


	echo -ne "creation of metadata for ${thisdir} done " # from top 3 lines
	date                  # from top 3 lines
done                      # from top 3 lines

