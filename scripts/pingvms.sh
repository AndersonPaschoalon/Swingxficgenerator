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
#  Last Modified : Thu Sep 18 22:48:31 PDT 2008
#
###################################################################################

if [ "$#" -lt "1" ]
then
	echo "Usage: $0 <vmiplist> [PING/SSH/GEXEC 3digitbinary]>"
	exit
fi

vmiplist=$1
physicallist=plist

if [ "$2" == "" ]
then
	tdf=1
else
	tdf=$2
fi

boolno='X'
boolyes='O'
boolno=' '
boolyes=' '
declare -a macnames
declare -a pings
declare -a sshs
declare -a gexecs

declare -a pingsy
declare -a sshsy
declare -a gexecsy
totmachines=`wc -l ${vmiplist} | field 1`

for i in `seq 1 ${totmachines}`
do
	thisnm=`head -n ${i} ${vmiplist}| tail -n 1`
	issysnet=`echo ${thisnm} | grep sysnet | wc -l | field 1 `
	if [ "$issysnet" == "1" ]
	then
	macnames[$i]=`echo ${thisnm} | sed 's/\..*//' | sed 's/sysnet//'`
	else
	macnames[$i]=`echo ${thisnm} | sed 's/.*\.//' `
	fi
	macnames[$i]=`echo ${thisnm} | sed 's/\..*//' `
	 pings[$i]="[37;41m${boolno}[m"
	  sshs[$i]="[37;41m${boolno}[m"
	gexecs[$i]="[37;41m${boolno}[m" # instd of 0
	 pingsy[$i]=0
	  sshsy[$i]=0
	gexecsy[$i]=0 # instd of 0
done
for i in `seq 1 ${totmachines}`
do
	echo -ne "${macnames[i]} "
done
echo " "
numreqd=`wc -l $vmiplist | field 1`
echo -ne " INIT:  0/${numreqd} "
for i in `seq 1 ${totmachines}`
do
	echo -ne "${pings[$i]}"
done
echo " "

# TRY PING
thisdone=0;
while [ "$thisdone" == "0" ]
do
	thisdone=1;
	thisnum=1
	succ=0
	numreqd=`wc -l $vmiplist | field 1`
	cat < /dev/null > trialpingfile
	cat < /dev/null > killpidfile
	for ips in `cat $vmiplist`
	do
		(
		lossfound=`ping  $ips -c 1 -w ${tdf} 2>/dev/null | grep loss`
		if [ "$lossfound" != "" ]
		then
			rcvd=0;
			rcvd=`echo $lossfound| awk '{ print $4 }'`
			if [ "$rcvd" -ge "1" ]
			then
				echo ${thisnum} >> trialpingfile
			fi
		else
			rcvd=0
		fi
		) &
		killpid=$!
		echo "${killpid}" >> killpidfile
		thisnum=$((thisnum+1))



	done
	machineup=`wc -l trialpingfile| field 1`

	if [ "$machineup" == "$numreqd" ]
	then
	echo -ne "\r PING: [37;42m${machineup}/${numreqd}[m "
	else
	echo -ne "\r PING: ${machineup}/${numreqd} "
	fi
	for i in `seq 1 ${totmachines}`
	do
		echo -ne "${pings[$i]}"
	done
	for j in `seq 1 ${machineup}`
	do
		thisname=`head -n ${j} trialpingfile| tail -n 1`
		pingsy[${thisname}]=1
		pings[${thisname}]="[37;42m${boolyes}[m" # instd of 1
	done

	SECONDS=0

	while [ "$SECONDS" -lt "$((2*tdf))" ] && [ "$machineup" -lt "${numreqd}" ]
	do

		machineup=`wc -l trialpingfile| awk '{ print $1}'`

	if [ "$machineup" == "$numreqd" ]
	then
	echo -ne "\r PING: [37;42m${machineup}/${numreqd}[m "
	else
	echo -ne "\r PING: ${machineup}/${numreqd} "
	fi

		for j in `seq 1 ${machineup}`
		do
			thisname=`head -n ${j} trialpingfile| tail -n 1`
			pingsy[${thisname}]=1
			pings[${thisname}]="[37;42m${boolyes}[m" # instd of 1
		done
		for i in `seq 1 ${totmachines}`
		do
			echo -ne "${pings[$i]}"
		done
		printed=0
		for i in `seq 1 ${totmachines}`
		do
			if [ "${pingsy[$i]}" != "1" ]
			then
				if [ "$printed" -lt "5" ]
				then
					if [ "$printed" == "0" ]
					then
						echo -ne " ["
					fi
					echo -ne " ${macnames[$i]} "
					printed=$((printed+1))
				else
					echo -ne "."
				fi
			fi
		done
		if [ "$printed" != "0" ]
		then
			echo -ne "]"
		fi
		for ((jj=printed;jj<15;jj++))
		do
			echo -ne " "
		done
		echo -ne "    "

	done







	if [ "$machineup" == "${numreqd}" ]
	then
		thisdone=1
	else
		thisnum=1
		for k in `cat killpidfile`
		do
			if [ "${pings[$thisnum]}" != "1" ]
			then
				kill -9 ${k} 2>/dev/null >/dev/null
			fi
			thisnum=$((thisnum+1))
		done

		thisdone=0
	fi

	if [ "$thisdone" == "0" ]
	then
		sleep 2
	fi

done
echo ""
# TRY SSH
thisdone=0
while [ "$thisdone" == "0" ]
do
	thisdone=1
	numup=0
	cat < /dev/null > trialsshfile
	cat < /dev/null > killpidfile
	numreqd=`wc -l $vmiplist | field 1`
	thisnum=1;
	for ips in `cat $vmiplist`
	do
		machineup=""
		(
		ssh $ips echo ${thisnum} 2>/dev/null | field 1 | grep -w ${thisnum}  >>trialsshfile 2>/dev/null 
		) 2>/dev/null &
		killpid=$!
		echo "${killpid}" >> killpidfile
		thisnum=$((thisnum+1))
	done
	machineup=`wc -l trialsshfile| awk '{ print $1}'`
	SECONDS=0

	while [ "$SECONDS" -lt "$((2*tdf))" ] && [ "$machineup" -lt "${numreqd}" ]
	do

		machineup=`wc -l trialsshfile| awk '{ print $1}'`
	if [ "$machineup" == "$numreqd" ]
	then
	echo -ne "\r  SSH: [37;42m${machineup}/${numreqd}[m "
	else
	echo -ne "\r  SSH: ${machineup}/${numreqd} "
	fi

		for j in `seq 1 ${machineup}`
		do
			thisname=`head -n ${j} trialsshfile| tail -n 1`
			sshsy[${thisname}]=1
			sshs[${thisname}]="[37;42m${boolyes}[m" # instd of 1
		done
		for i in `seq 1 ${totmachines}`
		do
			echo -ne "${sshs[$i]}"
		done
		printed=0
		for i in `seq 1 ${totmachines}`
		do
			if [ "${sshsy[$i]}" != "1" ]
			then
				if [ "$printed" -lt "5" ]
				then
				if [ "$printed" == "0" ]
				then
					echo -ne " ["
				fi
				echo -ne " ${macnames[$i]} "
				printed=$((printed+1))
				else
					echo -ne "."
				fi
			fi
		done
		if [ "$printed" != "0" ]
		then
			echo -ne "]"
		fi
		for ((jj=printed;jj<15;jj++))
		do
			echo -ne "  "
		done

	done
	echo -ne "     "
	if  [ "$machineup" -lt "${numreqd}" ]
	then
		thisnum=1
		for k in `cat killpidfile`
		do
	#		echo "findout for mac${thisnum} pid ${k} ${sshs[$thisnum]} ${macnames[$thisnum]}"
			if [ "${sshs[$thisnum]}" != "1" ]
			then
				kill -9 ${k} 2>&1 > /dev/null
			fi
			thisnum=$((thisnum+1))
		done
	fi
	if [ "$machineup" == "$numreqd" ]
	then
		thisdone=1
	else
		machineup=""
		thisdone=0
		sleep 2
	fi

done
echo -ne "                    "
echo " "
#echo "CAN SSH to the virtual machines"

# TRY GEXEC
thisdone=0
while [ "$thisdone" == "0" ]
do
	thisdone=1
	cat < /dev/null > gexectrialfile
	echo  /dev/null >killpidfile
	thisnum=1
	for ips in `cat $vmiplist`
	do
		(
		GEXEC_SVRS=$ips gexec -n 0 echo ${thisnum} 2>/dev/null | field 2 | grep -w ${thisnum}  >>gexectrialfile 
		) 2>/dev/null&
		killpid=$!
		echo "${killpid}" >> killpidfile
		thisnum=$((thisnum+1))
	done

	machineup=`wc -l gexectrialfile| awk '{ print $1}'`
	SECONDS=0
	while [ "$SECONDS" -lt "$((2*tdf))" ] && [ "$machineup" -lt "${numreqd}" ]
	do

		machineup=`wc -l gexectrialfile| awk '{ print $1}'`
	if [ "$machineup" == "$numreqd" ]
	then
	echo -ne "\rGEXEC: [37;42m${machineup}/${numreqd}[m "
	else
	echo -ne "\rGEXEC: ${machineup}/${numreqd} "
	fi


		for j in `seq 1 ${machineup}`
		do
			thisname=`head -n ${j} gexectrialfile| tail -n 1`
			gexecsy[${thisname}]=1
			gexecs[${thisname}]="[37;42m${boolyes}[m" # instd of 1
		done


		for i in `seq 1 ${totmachines}`
		do
			#echo -ne "${pings[$i]}${sshs[$i]}${gexecs[$i]} "
			echo -ne "${gexecs[$i]}"
		done
	printed=0
	for i in `seq 1 ${totmachines}`
	do
		if [ "${gexecsy[$i]}" != "1" ]
		then
			if [ "$printed" -lt "5" ]
			then
				if [ "$printed" == "0" ]
				then
					echo -ne " ["
				fi
				echo -ne " ${macnames[$i]} "
				printed=$((printed+1))
			else
				echo -ne "."
			fi
		fi
	done
	if [ "$printed" != "0" ]
	then
		echo -ne "]"
	fi
	for ((jj=printed;jj<15;jj++))
	do
		echo -ne "   "
	done

	#sleep 1
done
	if  [ "$machineup" -lt "${numreqd}" ]
	then
		#		echo -ne "\r machines that do not have GEXEC up ....... "
		thisnum=1
		for k in `cat killpidfile`
		do
			if [ "${gexecsy[$thisnum]}" != "1" ]
			then

				kill -9 ${k} 2>/dev/null
				#echo -ne "${macnames[$thisnum]}  "
			fi
		done
	fi
	if [ "$machineup" == "$numreqd" ]
	then
		thisdone=1
	else
		machineup=""
		thisdone=0
	fi
done
echo ""

#echo "CAN GEXEC to the virtual machines"
rm -rf trialpingfile
rm -rf trialpidfile
rm -rf killpidfile
rm -rf gexectrialfile
rm -rf trialsshfile
