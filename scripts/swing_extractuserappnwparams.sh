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


function quitseparate() {
	echo "Separate fails"
	cat errseparate
	exit

}

#if [ "$#" -lt "6" ]
if [ "$#" -lt "2" ]
then
	#echo "Usage: $0 <file> <ports> <fh> <swing/nonswing> <totaltime> <starttime>"
	echo "Usage: $0 <fh> <swing/nonswing>"
	exit
fi

portsfile="ports"
logfile="tlatest"
logfile="fileboth"
fh="Swing"
totaltime=300
starttime=0
nonswing=""
#logfile=$1

if ! test -f ${logfile}
then
  echo -ne "File ${logfile} not found, creating it from file0 and file1 "
  if ! test -f file0
  then
    echo ""
	echo "File file0 not found, make sure you have oneway traces as file0 and file1"
	exit
  fi
  if ! test -f file1
  then
    echo ""
	echo "File file1 not found, make sure you have oneway traces as file0 and file1"
	exit
  fi
  sort -nm file0 file1 > fileboth
  echo " ... done"
fi


if ! test -f ${portsfile}
then
  	echo "File ${portsfile} not found, Initializing it as follows"
	echo "numports 1" > ${portsfile}
	echo "80 tcp HTTP">> ${portsfile}
	cat ${portsfile}
fi



fh=$1
nonswing=$2

totaltime=`( head -n 1 fileboth ; tail -n 1 fileboth ) | awk '{ if(NR==1) st=$1 ; else printf "%d\n", $1 - st}'`
starttime=`head -n 1  fileboth | awk '{ printf "%4f\n", $1}'`
#echo ${totaltime} ${starttime}
echo "fileboth is of duration ${totaltime} secs"


if [ "$fh" == "Swing" ]
then
	fnames="tlatest."
	fileboth=tlatest
	file0=tlatest.0
	file1=tlatest.1
else
	fnames="file"
	fileboth=fileboth
	file0=file0
	file1=file1
fi
dontskipbidirectionalforswing=1
tcpdumpfile=${logfile}
ipmaps="ipmaps"

directory=`dirname ${tcpdumpfile}`
tcpdumpfilename=`basename ${tcpdumpfile}`
SECONDS=0
echo -ne "separating into per-port file ... "
separateintoport_files $logfile $portsfile port 2>errseparate  || quitseparate
#if [ "$fh" != "Swing" ] || [ "${dontskipbidirectionalforswing}" == "1" ]
if  [ "${dontskipbidirectionalforswing}" == "1" ]
then
	SECONDS=0
	(separateintoport_files ${file0} ports port 2>errseparate0 )&
	(separateintoport_files ${file1} ports port 2>errseparate1 )&
	wait
	for ports in `cat ports | awk '{ if(NR>1) print $1 }'`
	do
		(
			mkdir -p ${ports}
			client_server_dumbbell_assignment.sh ${file0}.tcp.${ports} ${file1}.tcp.${ports} 
			cat ipmaps > ipmaps_${ports} 
			ln -sf `pwd`/ipmaps_${ports} ${ports}/ipmaps_${ports}
		)
	done
	(
		mkdir -p othertcp
		client_server_dumbbell_assignment.sh file0.tcp.other file1.tcp.other  2>/dev/null
		cat ipmaps > ipmaps_othertcp 
		ln -sf `pwd`/ipmaps_othertcp othertcp/ipmaps_othertcp
	) 
	(
		mkdir -p otherudp
		client_server_dumbbell_assignment.sh ${file0}.udp.other ${file1}.udp.other  2>/dev/null
		cat ipmaps > ipmaps_otherudp 
		ln -sf `pwd`/ipmaps_otherudp otherudp/ipmaps_otherudp
	)
	wait
fi
echo ".. done"
SECONDS=0
for ports in `cat ${portsfile} | awk '{ if(NR>1) print $1"#"$2 }' `
do
	port=`echo $ports| sed 's/#.*//'`
	proto=`echo $ports|sed 's/.*#//'`
	mkdir -p $port
	cd $port
	ln -sf ../${logfile}.${proto}.${port} .
	cd -
done
mkdir -p othertcp
cd othertcp
ln -sf ../${logfile}.tcp.other .
cd ..
mkdir -p otherudp
cd otherudp
ln -sf ../${logfile}.udp.other .
cd ..
cat < /dev/null > ipmaps_new
cat < /dev/null > ipmaps_new_tmp
for ports in `cat ${portsfile} | awk '{ if (NR>1) print $1"#"$2 } '`
do
	(

		port=`echo $ports| sed 's/#.*//'`
		proto=`echo $ports|sed 's/.*#//'`
		#echo $port 
		cd $port
		app=${port}
		(cat ${logfile}.${proto}.${port} | convert_date_precision > ${logfile}.${proto}.${port}.converted_date)&
		allinone_tcp_twoway.sh ${logfile}.${proto}.${port} ${app} ${fh} ipmaps_${app} ${totaltime} ${starttime}  2>/dev/null 
		cat ipmaps_new_${port} >> ipmaps_new_tmp;
		#echo " next step ${port} " 
		wait
		cd -
	)  &

done
(
	if [ "$fh" != "Swing" ]
	then
		#echo othertcp
		cd othertcp
		port="other"
		proto="tcp"
		app="othertcp"
		#filterothertcp ${logfile}.${proto}.${port}
		(cat ${logfile}.${proto}.${port} | convert_date_precision >  ${logfile}.${proto}.${port}.converted_date)&
		allinone_tcp_twoway.sh ${logfile}.${proto}.${port} ${app} ${fh} ipmaps_${app} ${totaltime} ${starttime}  2>/dev/null
		cat ipmaps_new_othertcp >> ipmaps_new_tmp;
		wait
		cd -
	fi
) &

noudp=1
wait
cat ipmaps_new_tmp > ipmaps_new
echo -ne "\r                                                        "
echo -ne "\r"
script2_topogen_new fileboth ipmaps ${fh} 0
echo -ne "\r"
COLUMNS=85
for((colst=0;colst<=COLUMNS; colst++))
do
	echo -ne " "
done
echo -ne "\r"
makespecs.all ports ${fh}
echo "Extraction of user/application/network parameters complete for ${fh}"
