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

if [ "$#" -lt "6" ]
then
	echo "Usage $0 <tracedir> <port> <fh> <class> <totaltime> <starttime>"
	exit

fi

tracedir=$1
port=$2
fh=$3
class=$4
totaltime=$5
starttime=$6;
echo -n class $class 1 1 1" "
t1=`wc -l specs_param_sessstartcdf_${port}_${fh} | awk '{ printf "%d", $1}'`
toprint_t1=$((t1-2))
echo "${toprint_t1} ${totaltime} ${starttime}"

echo -n "cdf "
t2=`wc -l specs_param_sessstartcdf_${port}_${fh} | awk '{ printf "%d", $1}'`
echo "${t2}"
cat specs_param_sessstartcdf_${port}_${fh}
