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



if [ "$#" -lt "2" ]
then
	echo "Usage $0 <params file> <swinghome>"
	exit
fi

paramsfile=$1
SWINGHOME=$2
applat=0;
source ${paramsfile}
source ./extraparams
source ./paramsforscript
newrttplr=1
tracefile=`echo $traceinfo|sed 's/.*\///'`

originaltopodir=${tracedir}
if [ "$toposubstitutedir" == "" ]
then
toposubstitutedir=${tracedir}
fi

if [ "$basetopodir" == "" ]
then
basetopodir=${tracedir}
fi



if [ "$updatecdfdir" != "1" ]
then
cdfdir=/dev/null
fi
mkdir -p ${cdfdir}
echo "qslot=\"${qslot}\""
echo "qslotedgemin=\"${qslot}\""
echo "pipes=\" \\"
echo "${pipe0} \\"
echo "${pipe1} \\"
startnode=0
endnode=1
if [ "$multihops" == "" ]
then
	multihops=2
fi
if [ "$multihops" -gt "1" ]
then
	nodebw=${bneck_bw}
	nodelat=${bneck0lat}
	nodeplr=${bneckplr}
	for((thishop=2;thishop<=multihops ; thishop++))
	do
		startnode=$((startnode+1))
		endnode=$((endnode+1))
		echo "${startnode}_${endnode}_${nodelat}_${nodebw}_${nodeplr} \\"
		echo "${endnode}_${startnode}_${nodelat}_${nodebw}_${nodeplr} \\"
	done
fi
if [ "$mntcpdump" -ge "2" ]
then
	echo "1_2_0_1000000_0.0 \\"
	echo "2_1_${bneck1lat}_${bneck_bw1}_${bneckplr} \\"
fi
echo "\""
echo "clients=\" \\"

leftattachnode=0
rightattachnode=1
if [ "$mntcpdump" -ge "2" ]
then
	rightattachnode=2
fi
s=0;

echo "0_${bneck_bw}_0_${applat}_${bneck_bw}_0_0 \\"
echo "0_${bneck_bw}_0_${applat}_${bneck_bw}_0_0 \\"
echo "0_${bneck_bw}_0_${applat}_${bneck_bw}_0_0 \\"
echo "0_${bneck_bw}_0_${applat}_${bneck_bw}_0_0 \\"
echo "0_${bneck_bw}_0_${applat}_${bneck_bw}_0_0 \\"

echo "${endnode}_${bneck_bw}_0_${applat}_${bneck_bw}_0_0 \\"
echo "${endnode}_${bneck_bw}_0_${applat}_${bneck_bw}_0_0 \\"
echo "${endnode}_${bneck_bw}_0_${applat}_${bneck_bw}_0_0 \\"
echo "${endnode}_${bneck_bw}_0_${applat}_${bneck_bw}_0_0 \\"
echo "${endnode}_${bneck_bw}_0_${applat}_${bneck_bw}_0_0 \\"
s=10;
udpplrhack=1;
defaultudpplr=0.0;
#######################class begin
cat < /dev/null> psizevals

thismulti=1;
for((thismulti=1;thismulti<=multihops;thismulti++))
do
leftattachnode=$((thismulti - 1))
rightattachnode=$((thismulti))

for((classnum=1; classnum<=numclasses;classnum++))
do
	client0=`extract_offset $classnum ${client0s}`
	server0=`extract_offset $classnum ${server0s}`
	client1=`extract_offset $classnum ${client1s}`
	server1=`extract_offset $classnum ${server1s}`
	class=`extract_offset $classnum ${class_s}`
	port=`extract_offset $classnum ${port_s}`
	conninfofile=`extract_offset $classnum ${conninfofile_s}`
	sessfile=`extract_offset $classnum ${sessfiles_s}`
	clientdistfile=`extract_offset $classnum ${clientdistfile_s}`
	serverdistfile=`extract_offset $classnum ${serverdistfile_s}`
	if [ "$bidirectional" == "1" ]
	then
	conninfofile1=`extract_offset $classnum ${conninfofile1_s}`
	sessfile1=`extract_offset $classnum ${sessfiles1_s}`
	clientdistfile1=`extract_offset $classnum ${clientdistfile1_s}`
	serverdistfile1=`extract_offset $classnum ${serverdistfile1_s}`
	fi

    if [ "$class" == "UDP" ]
	then
		useport="othertcp"
	else
		useport=$port
	fi
	cdffile0="${toposubstitutedir}/${useport}/rttcdf_bw_data_0"
	cdffile1="${toposubstitutedir}/${useport}/rttcdf_bw_data_1"
	cdffileclient0="${toposubstitutedir}/${useport}/rttcdf_bw_data_client_0"
	cdffileclient1="${toposubstitutedir}/${useport}/rttcdf_bw_data_client_1"
	cdffileserver0="${toposubstitutedir}/${useport}/rttcdf_bw_data_server_0"
	cdffileserver1="${toposubstitutedir}/${useport}/rttcdf_bw_data_server_1"

	
	cdffileclient0orig="${basetopodir}/${useport}/rttcdf_bw_data_client_0"
	cdffileclient1orig="${basetopodir}/${useport}/rttcdf_bw_data_client_1"
	cdffileserver0orig="${basetopodir}/${useport}/rttcdf_bw_data_server_0"
	cdffileserver1orig="${basetopodir}/${useport}/rttcdf_bw_data_server_1"
	if [ "$fixedratio" != "1" ]
	then

	RATIO=`getratio_client_server_bytes ${toposubstitutedir}/${useport}/ipmaps_${useport} ${conninfofile} | awk '{ printf "%.3f", $3 }'`
	fi
	

	if [ "$newrttplr" == "1" ]
	then
	if [ "$thismulti" == "1" ]
	then
		cat < /dev/null > ${cdfdir}/inputforgeneric_${class}
		fi
		udphackhere=0;if [ "$udpplrhack" == "1" ] ; then if [ "$port" == "otherudp" ] ; then udphackhere=1; fi; fi
		getrttbwplr_nums ${cdffileclient0} $RANDOM ${client0} ${rbw} ${rplr} ${rdel} ${leftattachnode} 1 psizevals ${cdfdir}/inputforgeneric_${class} $s ${udphackhere} ${cdffileclient0orig} ${topoprmcopynmbr}
		s=$((s+client0))
	else
		echo "client $client0" > ${cdfdir}/inputforgeneric_${class}
		for i in `seq 1 $client0`
		do
		vals=`getrttbwplr ${cdffile0} $RANDOM ` ; obw=`echo $vals | awk '{ print $2 }'` ; odel=`echo $vals | awk '{ printf "%.4f", $1 }'`; oplr=`echo $vals| awk '{ printf "%0.4f", $3}'`; 
		psize=`echo $vals|awk '{ printf "%d",$4}'`; echo "$s $vals" >>psizevals
		if [ "$udpplrhack" == "1" ]
		then
			if [ "$port" == "otherudp" ]
			then
				oplr=$defaultudpplr;
			fi
		fi
		echo "${leftattachnode}_${obw}_${oplr}_${odel}_${rbw}_${rplr}_${rdel} \\"
		echo -n vn$s" " >> ${cdfdir}/inputforgeneric_${class}
		s=$((s+1))
		done
		echo " " >> ${cdfdir}/inputforgeneric_${class}
	fi

	if [ "$newrttplr" == "1" ]
	then
		udphackhere=0;if [ "$udpplrhack" == "1" ] ; then if [ "$port" == "otherudp" ] ; then udphackhere=1; fi; fi
		getrttbwplr_nums ${cdffileserver1} $RANDOM ${server1} ${rbw1} ${rplr1} ${rdel1} ${rightattachnode} 0 psizevals ${cdfdir}/inputforgeneric_${class} $s ${udphackhere} ${cdffileserver1orig} ${topoprmcopynmbr}
		s=$((s+server1))
	else

		echo "server $server1" >> ${cdfdir}/inputforgeneric_${class}
		for i in `seq 1 $server1`
		do
		vals=`getrttbwplr ${cdffile1} $RANDOM ` ; obw1=`echo $vals | awk '{ print $2 }'` ; odel1=`echo $vals | awk '{ printf "%.4f", $1 }'`; oplr1=`echo $vals| awk '{ printf "%0.4f", $3}'` 
		psize=`echo $vals|awk '{ printf "%d",$4}'`; echo "$s $vals" >>psizevals
		if [ "$udpplrhack" == "1" ]
		then
			if [ "$port" == "otherudp" ]
			then
				oplr1=$defaultudpplr;
			fi
		fi
		echo "${rightattachnode}_${obw1}_${oplr1}_${odel1}_${rbw1}_${rplr1}_${rdel1} \\"
		#echo "2_${obw1}_${oplr1}_${odel1}_${rbw1}_${rplr1}_${rdel1} \\"
		echo -n vn$s" " >> ${cdfdir}/inputforgeneric_${class}
		s=$((s+1))
		done
		echo " " >> ${cdfdir}/inputforgeneric_${class}
	fi


	if [ "${original_ipaddress_cdf}" == "1" ]
	then
		cat ${clientdistfile} | wc -l | awk '{ printf "cdf %d\n",$1}' >>${cdfdir}/inputforgeneric_${class}
		cat ${clientdistfile} >>${cdfdir}/inputforgeneric_${class}
		cat ${serverdistfile} | wc -l | awk '{ printf "cdf %d\n",$1}'>>${cdfdir}/inputforgeneric_${class}
		cat ${serverdistfile}>>${cdfdir}/inputforgeneric_${class}
	else
	cat ${ratiofile} >> ${cdfdir}/inputforgeneric_${class}
	cat ${ratiofile} >> ${cdfdir}/inputforgeneric_${class}
	fi

	echo "ratio ${RATIO}" >> ${cdfdir}/inputforgeneric_${class}

	if [ "$newrttplr" == "1" ]
	then
		#getrttbwplr_nums ${cdffile1} $RANDOM ${client1} ${rbw1} ${rplr1} ${rdel1} 1 1 psizevals ${cdfdir}/inputforgeneric_${class} $s 0
		udphackhere=0;if [ "$udpplrhack" == "1" ] ; then if [ "$port" == "otherudp" ] ; then udphackhere=1; fi; fi
		getrttbwplr_nums ${cdffileclient1} $RANDOM ${client1} ${rbw1} ${rplr1} ${rdel1} ${rightattachnode} 1 psizevals ${cdfdir}/inputforgeneric_${class} $s ${udphackhere} ${cdffileclient1orig} ${topoprmcopynmbr}
		s=$((s+client1))
	else
		echo "client $client1" >> ${cdfdir}/inputforgeneric_${class}
		for i in `seq 1 $client1`
		do
		vals=`getrttbwplr ${cdffile1} $RANDOM ` ; obw1=`echo $vals | awk '{ print $2 }'` ; odel1=`echo $vals | awk '{ printf "%.4f", $1 }'`; oplr1=`echo $vals| awk '{ printf "%0.4f", $3}'` 
		psize=`echo $vals|awk '{ printf "%d",$4}'`; echo "$s $vals" >>psizevals
		if [ "$udpplrhack" == "1" ]
		then
			if [ "$port" == "otherudp" ]
			then
				oplr1=$defaultudpplr;
			fi
		fi
		echo "${rightattachnode}_${obw1}_${oplr1}_${odel1}_${rbw1}_${rplr1}_${rdel1} \\"
		#echo "2_${obw1}_${oplr1}_${odel1}_${rbw1}_${rplr1}_${rdel1} \\"
		echo -n vn$s" " >> ${cdfdir}/inputforgeneric_${class}
		s=$((s+1))
		done
		echo " " >> ${cdfdir}/inputforgeneric_${class}
    fi

	if [ "$newrttplr" == "1" ]
	then
		#getrttbwplr_nums ${cdffile0} $RANDOM ${server0} ${rbw} ${rplr} ${rdel} 0 0 psizevals ${cdfdir}/inputforgeneric_${class} $s 0
		udphackhere=0;if [ "$udpplrhack" == "1" ] ; then if [ "$port" == "otherudp" ] ; then udphackhere=1; fi; fi
		getrttbwplr_nums ${cdffileserver0} $RANDOM ${server0} ${rbw} ${rplr} ${rdel} ${leftattachnode} 0 psizevals ${cdfdir}/inputforgeneric_${class} $s ${udphackhere} ${cdffileserver0orig} ${topoprmcopynmbr}
		s=$((s+server0))
	else
		echo "server $server0" >> ${cdfdir}/inputforgeneric_${class}
		for i in `seq 1 $server0`
		do
		vals=`getrttbwplr ${cdffile0} $RANDOM ` ; obw=`echo $vals | awk '{ print $2 }'` ; odel=`echo $vals | awk '{ printf "%.4f", $1 }'`; oplr=`echo $vals| awk '{ printf "%0.4f", $3}'` 
		psize=`echo $vals|awk '{ printf "%d",$4}'`; echo "$s $vals" >>psizevals
		if [ "$udpplrhack" == "1" ]
		then
			if [ "$port" == "otherudp" ]
			then
				oplr=$defaultudpplr;
			fi
		fi
		echo "${leftattachnode}_${obw}_${oplr}_${odel}_${rbw}_${rplr}_${rdel} \\"
		echo -n vn$s" " >> ${cdfdir}/inputforgeneric_${class}
		s=$((s+1))
		done
		echo " " >> ${cdfdir}/inputforgeneric_${class}
	fi

	
	if [ "${original_ipaddress_cdf}" == "1" ]
	then
		if [ "${bidirectional}" == "1" ]
		then
		cat ${clientdistfile1} | wc -l | awk '{ printf "cdf %d\n",$1}'>>${cdfdir}/inputforgeneric_${class}
		cat ${clientdistfile1} >>${cdfdir}/inputforgeneric_${class}
		cat ${serverdistfile1} | wc -l | awk '{ printf "cdf %d\n",$1}'>>${cdfdir}/inputforgeneric_${class}
		cat ${serverdistfile1}>>${cdfdir}/inputforgeneric_${class}
		else
		cat ${clientdistfile} | wc -l | awk '{ printf "cdf %d\n",$1}'>>${cdfdir}/inputforgeneric_${class}
		cat ${clientdistfile} >>${cdfdir}/inputforgeneric_${class}
		cat ${serverdistfile} | wc -l | awk '{ printf "cdf %d\n",$1}'>>${cdfdir}/inputforgeneric_${class}
		cat ${serverdistfile}>>${cdfdir}/inputforgeneric_${class}
		fi
	else
	cat ${ratiofile} >> ${cdfdir}/inputforgeneric_${class}
	cat ${ratiofile}>> ${cdfdir}/inputforgeneric_${class}
	fi
	
done

done
echo "\""
#cat psizevals > ${psizevalstrace}
cat psizevals > psizevalstrace
exit
