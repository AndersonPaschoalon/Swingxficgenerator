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
if [ "$#" -lt "5" ]
then
	echo "Usage: $0 <origtracedir> <filehandle> <swinghome> <outputdir> <outputdirpath>"
	exit
fi

#Stage 0: 2way to 1way traces
#Stage 1: user/nw/app params extract from trace
#Stage 2: configure the xpt
#Stage 3: run the xpt and collect swing trace

origtracedir=$1 # should be full dir
#origpcapfile=$2 # not needed anymore
fh=$2 # imp.
SWINGHOME=$3 # imp
OUTPUTDIR=$4 # needed
outputdirpath=$5 # needed
#filetag=$7 # can also be ${OUTPUTDIR}
filetag=${OUTPUTDIR}

TOPOHOME=${outputdirpath}/${OUTPUTDIR}

skipstage0=0
skipstage1=0
skipstage2=0

#stage 0
if [ "${skipstage0}" != "1" ]
then
	cd ${origtracedir}
	if ! test -f file0
	then
	    origpcapfile=fileboth.dump
		swing_trace_convert_2way_2_1way.sh ${origpcapfile}
	fi
	cd -
fi

if [ "${skipstage1}" != "1" ]
then
	cd ${origtracedir}
	#stage 1
	swing_extractuserappnwparams.sh ${fh} Swing 
	swing_create_paramsforscript_entry.sh `pwd` ${filetag} ${fh}
	rm -rf ${SWINGHOME}/specs.all
	ln -s ${outputdirpath}/${OUTPUTDIR}/specs.all ${SWINGHOME}/
	cd -
fi

if [ "${skipstage2}" != "1" ]
then
	cd ${SWINGHOME}
	#stage 2
	swing_exptpreprocess.sh ${SWINGHOME} ${OUTPUTDIR}  ${TOPOHOME} ${origtracedir} ${outputdirpath}
fi
