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
#  Last Modified : Thu Sep 18 22:48:33 PDT 2008
#
###################################################################################


if [ "$#" -lt "5" ]
then
	echo "Usage: $0 <swinghome> <outputdir> <topohome> <origtracedir> <outputdirpath>"
	exit
fi
SWINGHOME=$1
thisdir=$2
outputdir=$2
newthisdir=$2
TOPOHOME=$3
origtracedir=$4
outputdirpath=$5


mkdir -p ${TOPOHOME}
tracedir=${origtracedir}


thisdir=${outputdirpath}/${outputdir}
newthisdir=${thisdir}
createswingexpt.sh ${outputdirpath} ${newthisdir} ${origtracedir} ${SWINGHOME}


mkdir -p ${thisdir}


cd ${thisdir} || exit
source ./paramsforscript
thisdir=${newthisdir}
cd ${SWINGHOME}
cd ${thisdir} || exit

# for these three we have to first create the 
# JOBVARIABLES file and then only source it in
# while running the code.



source ./JOBVARIABLES



killtime=`echo ${runtime} | awk '{ printf "%d", $1*3}'`

if [ "$mntcpdump" == "" ]
then
	mntcpdump=0
fi



toposubstitutedir=${origtracedir}
cd ${SWINGHOME}
cd ${thisdir}
cat < /dev/null > create_topofiles_params


SECONDS=0;
testskip=0;
skip1=0;
runskip=0;
skipreboot=1;
deployskip=0;
topogenskip=0;
createtopofileskip=0 ;

#toposize=100

alltoposkip=0 ; # just for now for source code check
topogenskip=0 ; # just for now for source code check
deployskip=0  ; # just for now for source code check


echo "dilated=${dilated}" >> create_topofiles_params
echo "dilationfactor=${dilationfactor}" >>create_topofiles_params

bneck0lat=$lat
bneck1lat=$lat
#lat=${bneck0lat}
echo "bneck0lat=${bneck0lat}" >> paramsforscript
echo "bneck1lat=${bneck1lat}" >> paramsforscript

echo "qslot=${qslot}" >> create_topofiles_params
echo "qslot=${qslot}" >> paramsforscript

echo "variablemax=${variablemax}" >>paramsforscript
echo "maxmem=${maxmem}">>paramsforscript
echo "toposubstitutedir=${toposubstitutedir}" >> paramsforscript
echo "toposubstitutedir=${toposubstitutedir}" >> extraparams
echo "TRACE=${TRACE}" >> paramsforscript
##### wsize begin
twowaymax=1
if [ "$twowaymax" == "1" ]
then
	echo -ne "maxdef=">> paramsforscript
	(
	for  i in `cat ${tracedir}/ports | awk '{ if(NR>1) print $1 } END { print "othertcp"}'`
	do
		cat ${tracedir}/${i}/wsize_50
	done
	) | field 2 | getpercentile - 50 >> paramsforscript
	echo -ne "maxmem=">> paramsforscript
	(
	for  i in `cat ${tracedir}/ports | awk '{ if(NR>1) print $1 } END { print "othertcp"}'`
	do
		cat ${tracedir}/${i}/wsize_50
	done
	) | field 2 | getpercentile - 95 >>paramsforscript

fi


echo -n "Expt. starting time "  ; date
echo -n "Preparing expt. " ; date
cdfs=$cdfsbase
bw=2400000
bw=1000000
bw=${tracebw}
bneck0lat=${bneck0lat}
bneck1lat=${bneck1lat}
( cd ${tracedir} ; copy_to_swingdir ports ${TRACE} ${thisdir} ; cd -)
echo "bneck_bw=${bw}" > extraparams ; echo "bneck_bw0=${bw}" >> extraparams ; echo "bneck_bw1=${bw}" >> extraparams ; echo "bidirectional=1" >>extraparams
echo "mntcpdump=${mntcpdump}" >> extraparams;
echo "bneckplr=${bneckplr}" >> extraparams
echo "bneck0lat=${bneck0lat}">>extraparams; 
echo "bneck1lat=${bneck1lat}">>extraparams; 
echo "dilated=${dilated}">>extraparams;
echo "dilationfactor=${dilationfactor}">>extraparams;
#echo " Rtt ${rtt} emg ${emg} plr ${plr} "

if [ "${basetopodir}" == "" ]
then
	basetopodir=${tracedir}
fi


if [ "${toposubstitutedir}" == "" ]
then
	toposubstitutedir=${tracedir}
fi
echo "toposubstitutedir=${toposubstitutedir}" >> extraparams
echo "basetopodir=${basetopodir}" >> extraparams

tfile=${toposubstitutedir}/fileboth ; rttdir=${toposubstitutedir}/rtts/othertcp/rtttry
if [ "$ipmapsdir" == "4" ] ; then ipmaps=${toposubstitutedir}/ipmaps_numbers_new ; else ipmaps=${toposubstitutedir}/ipmaps_numbers ; fi
if [ "$alltoposkip" != "1" ] ; then
	for ports in `cat ${toposubstitutedir}/ports | awk '{ if(NR>1) print $1 } END { print "othertcp"}'`
	do
		(
		#   make the rttcdffiles
		cd ${toposubstitutedir}/${ports}
		echo -ne " port ${ports} ::  "
		indiv=1;
		ipbytesweight ip_bytesfrom ipmaps_${ports} ${indiv}> ipmaps_${ports}_numbers
		ipbytesweight ip_bytesfrom ipmaps_new_${ports} ${indiv}> ipmaps_new_${ports}_numbers
		if [ "$ports" == "othertcp" ]
		then
			dumyt=3
		else
			dumz=4
		fi
		cat < /dev/null > create_topofiles_params
		echo "defaultaparams0=\"1.00 2400000.00 0.00 1480\"" >> create_topofiles_params
		echo "defaultaparams1=\"1.00 2400000.00 0.00 1480\"" >> create_topofiles_params
		echo "defaultaparams0=\"1.00 1000000.00 0.00 1480\"" >> create_topofiles_params
		echo "defaultaparams1=\"1.00 1000000.00 0.00 1480\"" >> create_topofiles_params
		echo "defaultaparams0=\"1.00 ${tracebw} 0.00 1480\"" >> create_topofiles_params
		echo "defaultaparams1=\"1.00 ${tracebw} 0.00 1480\"" >> create_topofiles_params
		echo "defaultaparams2=\"1.00 ${tracebw} 0.00 1480\"" >> create_topofiles_params
		echo "defaultaparams3=\"1.00 ${tracebw} 0.00 1480\"" >> create_topofiles_params
		echo "defaults=${defaults}" >>create_topofiles_params
		echo "weightedsamplesperdir=${weightedsamplesperdir} " >> create_topofiles_params

		if [ "${harmonicmean_capacity}" == "1" ]
		then
			echo ""harmonicmean_capacity"=1" >> create_topofiles_params
		else
			echo ""harmonicmean_capacity"=0" >> create_topofiles_params
		fi
		if [ "${createtopofileskip}" != "1" ]
		then
			if [ "$psize" == "psize" ] ; then
				if [ "$ipmapsdir" == "4" ]
				then
					time bash -x create_topofiles ipmaps_new_${ports}_numbers ${toposize} $RANDOM ${ports} ${rtt} ${emg} ${plr} psize_50
				else
					create_topofiles ipmaps_${ports}_numbers ${toposize} $RANDOM ${ports} ${rtt} ${emg} ${plr} psize_50
				fi
				#create_topofiles ipmaps_${ports}_numbers 0 $RANDOM ${rtt} ${emg} ${plr} psize_50
				#echo "psize=\"max\"" > ${SWINGHOME}/psizeparam
				echo "psize=\"max\"" > ${thisdir}/psizeparam
			else # psize=psize
				if [ "$ipmapsdir" == "4" ]
				then
					create_topofiles ipmaps_new_${ports}_numbers ${toposize} $RANDOM ${ports} ${rtt} ${emg} ${plr} ${psize} 
				else
					create_topofiles ipmaps_${ports}_numbers ${toposize} $RANDOM ${ports} ${rtt} ${emg} ${plr} ${psize} 
				fi
				#echo "psize=\"cdf\"" > ${SWINGHOME}/psizeparam
				echo "psize=\"cdf\"" > ${thisdir}/psizeparam
			fi
		fi
		if [ "$copynwfromelsewhere" == "1" ]
		then
			echo "Copying network properties from dir $nwdir/$ports"
			cp ${nwdir}/${ports}/rttcdf_bw_data_* .
		fi

		#				read a
		if  [ "$dirsadjust" == "1" ]
		then
			echo "swapping dirs 0 and 1 because of MAC addresses "
			# client 0 from earlier becomes client 1 ...
			cp rttcdf_bw_data_client_0 bakuprtt
			cp rttcdf_bw_data_client_1 rttcdf_bw_data_client_0
			cp bakuprtt rttcdf_bw_data_client_1
			cp rttcdf_bw_data_server_0 bakuprtt
			cp rttcdf_bw_data_server_1 rttcdf_bw_data_server_0
			cp bakuprtt rttcdf_bw_data_server_1


			cp rttcdf_bw_data_client_0 rttcdf_bw_data_0
			cp rttcdf_bw_data_client_1 rttcdf_bw_data_2
			cp rttcdf_bw_data_server_0 rttcdf_bw_data_1
			cp rttcdf_bw_data_server_1 rttcdf_bw_data_3


		fi

		massage_nw_chars ${timesdelay} ${timeskbps} ${timesplr} ${timespsize}

		cd -
		)
	done
	cdfs=${cdfs}"plr_"${plr}"bw_"${emg}"_rtt"${rtt}"_bw"${bw}_${tracenameparam}

	if [ "$skip2" != "1" ]
	then
		#${SWINGHOME}/bin/script4_makeparamsforstartfromspecs.sh $tfile $RANDOM ${toposize} ${toposubstitutedir} ${SWINGHOME} > paramsforstartfromspecs.${tracenameparam}
		${SWINGHOME}/bin/script4_makeparamsforstartfromspecs.sh $tfile $RANDOM ${toposize} ${toposubstitutedir} ${thisdir} ${SWINGHOME}> paramsforstartfromspecs.${tracenameparam}
		ln -sf paramsforstartfromspecs.${tracenameparam} paramsforstartfromspecs
		if [ "${original_ipaddress_cdf}" == "1" ]
		then
			echo "original_ipaddress_cdf=1" >>paramsforstartfromspecs
		else
			echo "original_ipaddress_cdf=0" >>paramsforstartfromspecs
		fi
		echo "fixedratio=0; RATIO=$RATIO" >>paramsforstartfromspecs
		echo "mntcpdump=${mntcpdump}" >> paramsforstartfromspecs
		echo "multihops=${multihops}" >> paramsforstartfromspecs
		echo "applat=${applat}" >> paramsforstartfromspecs
		echo "topoprmcopynmbr=${topoprmcopynmbr}" >> paramsforstartfromspecs
		${SWINGHOME}/bin/script3_startfromspecsgen.sh paramsforstartfromspecs ${SWINGHOME} > ${TOPOHOME}/specs_file_${cdfs}
	fi # skip2

	cat ${TOPOHOME}/specs_file_${cdfs} > ${TOPOHOME}/specs_file
fi # alltoposkip

cd ${TOPOHOME}
tfile=${tracedir}/fileboth ; rttdir=${tracedir}/rtts/othertcp/rtttry
if [ "$ipmapsdir" == "4" ] ; then ipmaps=${tracedir}/ipmaps_numbers_new ; else ipmaps=${tracedir}/ipmaps_numbers ; fi

# write this_cmd_line_allpairs
allpairscmd="${SWINGHOME}/bin/allpairs_smarter assigned_topo_with_clients_and_link_specs.graph ${mntcpdump}"
ttlappclasses=1
for iports in `cat ${tracedir}/ports |awk '{ if(NR>1) print $3}'`
do
	#allpairscmd=${allpairscmd}" ${SWINGHOME}/cdfdir/inputforgeneric_${iports}"
	allpairscmd=${allpairscmd}" ${thisdir}/cdfdir/inputforgeneric_${iports}"
	ttlappclasses=$((ttlappclasses+1))
done
allpairscmd=${allpairscmd}" ${thisdir}/cdfdir/inputforgeneric_TCPOTHER"
allpairscmd=${allpairscmd}" ${thisdir}/cdfdir/inputforgeneric_UDP"
echo "allpairscmd=\"${allpairscmd}\"" > ${thisdir}/this_cmd_line_allpairs
echo "mntcpdump=\"${mntcpdump}\"" >>    ${thisdir}/this_cmd_line_allpairs
echo "dilation=\"${dilation}\"" >>      ${thisdir}/this_cmd_line_allpairs
if [ "$parallelapplication" != "0" ] || [ "$countingarrival" != "0" ] || [ "$parallelswing" != "0" ]
then
	echo "newallpairs=0" >> ${thisdir}/this_cmd_line_allpairs
fi

#echo $PATH
#which topo-XML
#if [ "$topogenskip" != "1" ] ; then bash topo-XML ${SWINGHOME} ${thisdir}; fi
if [ "$topogenskip" != "1" ] ; then bash generatetopo ${SWINGHOME} ${thisdir}; fi

#cat ${SWINGHOME}/topodir/hosts.xml  |grep host | awk -F '"' '{ print $2}' > machinelist
cat ${thisdir}/hosts.xml  |grep host | awk -F '"' '{ print $2}' > machinelist
cat < /dev/null > restartscript.sh
for vmi in `cat machinelist`
do
	echo "ssh ${vmi}  ' ${SWINGHOME}/bin/swinggexecrestart.sh '">>restartscript.sh
done
echo "Restarting gexec on expt. nodes"
bash ./restartscript.sh
echo "verifying ability to ping/ssh/gexec on expt. nodes"
pingvms.sh machinelist 2  || ( echo "Machines unreachable " ; exit 0 )
cd - # back from topo
echo "deploying topology"
if [ "$deployskip" != "1" ] 
then 
	deploy ${SWINGHOME}/bin/latest/example.model ${SWINGHOME}/bin/latest/example.route || ( echo " CAnnot deploy " ; exit 1 ) 
fi
#echo "press to proceed"
#read a

echo "Generating traffic"
if [ "$runskip" != "1" ]
then
	cd ${SWINGHOME}
	## start expt
	s=1;

	if [ "$apptimes" == "2" ]
	then
		apptimes=0
	fi
	echo " apptimes is $apptimes"
	numtries=0
	cd ${SWINGHOME}
	cd ${thisdir}
	while [ "$s" != "0" ]
	do
		bash ./restartscript.sh

		onlyonce=1
		onlyonce=0
		if [ "${onlyonce}" != "1" ]
		then

			echo "step L ; " ; ls ${SWINGHOME}/bin/latest/example.route
			runtimes=` echo $killtime $runtime | awk '{ printf "%d", 2+$1/$2}'`;
			bash ${SWINGHOME}/bin/runswingexpt.sh ${SWINGHOME} ${runtime} 1 ${runtimes} ${killtime} $RANDOM ${apptimes}
		else
			bash ${SWINGHOME}/bin/runswingexpt.sh ${SWINGHOME} ${runtime} 1 3 ${runtime} $RANDOM ${apptimes}
		fi
		s=$?;
		if [ "$s" != "0" ]
		then
			killthingsforendofrunswing;
		fi
		numtries=$((numtries + 1))
		if [ "$numtries" -ge "5" ]
		then
			echo "${numtries} already for this run so lets move on "
			if [ "$s" != "0" ]
			then
				mkdir -p ${thisdir}
				echo "STATE=abandoned" > ${thisdir}>>STATE
			fi
			s=0
		fi
	done
	apptimes=$((apptimes+1))
	sleep 10 
	killthingsforendofrunswing
fi # runskip

echo "Postprocessing"
mkdir -p ${thisdir}
cd $thisdir || exit
echo "$cdfs" >README
echo "
bw = ${emg}
plr= ${plr}
rtt=${rtt}
bneck bw = ${bw}
" >>README
set >>README

cat ${tracedir}/ports | awk '{ 
if(NR==1) {
	print "numports "$2+1
}
	else {
		print 20000+$1" "$2" "$3
	}
} END {
print "22222 tcp TCPOTHER"
}' > ports
SLEEPTIME=5
(
# some extra sutff for postanalysis
sleep $SLEEPTIME
ls ${SWINGHOME}/latest-trace/ > /dev/null
sleep $SLEEPTIME

skipinitialtime=$((killtime - runtime - 30))
if [ "$skipinitialtime" -le "0" ]
then
	skipinitialtime=""
fi

if [ "${onlyonce}" != "1" ]
then
	trace_get_analyse ${runtime} ${skipinitialtime} ${SWINGHOME}
else
	trace_get_analyse ${runtime} 0 ${SWINGHOME}
fi

)
# copy important things to the postanalysis dir after the postanalysis and
# before kicking off new runs

echo "saving a few intermediate files for possible post-run analysis"
cp ${SWINGHOME}/pktsizeSwing . && rm -rf ${SWINGHOME}/pktsizeSwing
cp -r ${SWINGHOME}/output . && rm -rf ${SWINGHOME}/output
cp -r ${SWINGHOME}/listener . && rm -rf ${SWINGHOME}/listener
cp ${SWINGHOME}/modelstat_output . && rm -rf ${SWINGHOME}/modelstat_output
cp ${SWINGHOME}/latest-trace/tlatest.*output . 
cp ${SWINGHOME}/common_list_file . && rm -rf ${SWINGHOME}/common_list_file



echo "cleanup"
( cd ${tracedir} ; remove_from_swingdir ports ${TRACE} ${thisdir} ; cd -)
echo "Expt. over `date`"
