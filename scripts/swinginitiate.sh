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
#  Last Modified : Thu Sep 18 22:48:34 PDT 2008
#
###################################################################################
# in this specify what all directories will be made etc.
# and also generate the files etc.
#SWINGHOME=`pwd`
#export SWINGHOME=${SWINGHOME}
SWINGHOME=$1
export SWINGHOME=${SWINGHOME}
user=`whoami`
outputdir=$SWINGHOME/output
psizevalstrace=`pwd`/psizevalstrace
listenerdir=$SWINGHOME/listener
modelfile=$SWINGHOME/bin/latest/example.model
hostsfile=$SWINGHOME/bin/latest/hosts
hostxmlfile=$SWINGHOME/bin/latest/hosts.xml
indifilesdir=`pwd`/indifiles
listener=${SWINGHOME}/bin/listener
generator_daemon=${SWINGHOME}/bin/generator_daemon
startlistener=${SWINGHOME}/bin/startlistener
specsfile=${SWINGHOME}/specs.all
timecommonfile=`pwd`/timecommonfile
gexec_script=${SWINGHOME}/bin/gexec_script
gather_script=${SWINGHOME}/bin/gather_script
tmpindifilesdir=/tmp/`whoami`/indifiles
tmpoutputdir=/tmp/`whoami`/output
tmplistenerdir=/tmp/`whoami`/listener
listen_script=${SWINGHOME}/bin/listen_script
cp_listener_script=${SWINGHOME}/bin/cp_listener_script
suspend_queues=$SWINGHOME/bin/latest/suspendqueues.bash
unsuspend_queues=$SWINGHOME/bin/latest/unsuspendqueues.bash
common_file=`pwd`/common_file
common_list_file=$SWINGHOME/common_list_file
ofile=/usr/tmp/tlatest
ofilemntcpdump=/usr/tmp/tlatestmntcpdump
getdump=$SWINGHOME/bin/getdump
mgetmntcpdump=$SWINGHOME/bin/getmntcpdump
cpdump=$SWINGHOME/bin/cpdump
stopdump=$SWINGHOME/bin/stopdump
stopmntcpdump=$SWINGHOME/bin/stopmntcpdump
tmpfwderfile=/tmp/`whoami`/tmpfwderfile
startlistenerfile=`pwd`/startlistenerfile


cat </dev/null > params.file
mkdir -p bin
mkdir -p $outputdir
mkdir -p $listenerdir
mkdir -p $indifilesdir
rm -f $outputdir/*

echo "SWINGHOME=${SWINGHOME}">>params.file
echo "user=$user" >> params.file
echo "outputdir=$outputdir" >> params.file
echo "modelfile=$modelfile">> params.file
echo "hostsfile=$hostsfile">> params.file
echo "indifilesdir=$indifilesdir">>params.file
echo "startlistener=$startlistener">>params.file
echo "listener=$listener">>params.file 
echo "generator_daemon=$generator_daemon">>params.file
echo "timecommonfile=$timecommonfile">>params.file
echo "specsfile=$specsfile">>params.file
echo "gexec_script=$gexec_script">>params.file
echo "tmpoutputdir=$tmpoutputdir">>params.file
echo "tmpindifilesdir=$tmpindifilesdir">>params.file
echo "gather_script=$gather_script">>params.file
echo "listenerdir=$listenerdir">>params.file
echo "tmplistenerdir=$tmplistenerdir">>params.file
echo "listen_script=$listen_script">>params.file
echo "cp_listener_script=$cp_listener_script">>params.file
#echo "suspendqueues=$suspend_queues" >> params.file
#echo "unsuspendqueues=$unsuspend_queues" >> params.file
echo "common_file=$common_file" >> params.file
echo "common_list_file=$common_list_file" >> params.file
echo "hostxmlfile=$hostxmlfile" >> params.file
echo "getdump=$getdump" >> params.file
echo "getmntcpdump=$getmntcpdump" >> params.file
echo "stopdump=$stopdump" >> params.file
echo "stopmntcpdump=$stopmntcpdump" >> params.file
echo "ofile=$ofile" >> params.file
echo "ofilemntcpdump=$ofilemntcpdump" >> params.file
echo "cpdump=$cpdump" >> params.file
echo "tmpfwderfile=$tmpfwderfile" >> params.file
echo "startlistenerfile=$startlistenerfile" >> params.file
echo "psizevalstrace=${psizevalstrace}" >> params.file
