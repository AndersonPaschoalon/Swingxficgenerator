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

# This is a meta-script that calls individual scripts to do setup of modelnet/bells-whistles etc.

# assumne ssh and gexec functionality

# Flow:
# 0.Start with list of nodes 
# 1. swinginitiate(./setup) and swingparams.file(params.file)
# 2.Cleanup
# 3.Call individual scripts
# 4.Cleanup(?)


if [ "$#" -lt "5" ]
then
  	echo "Usage: $0 <listofnodes> <username> a b c"
	echo "a = swing, b=modelnet c=parallelapp"
	echo "0=no, 1=yes"
	exit
fi

# Step 0. list of nodes
nodelist=$1
nodeuname=$2
swingrun=$3
modelnetrun=$4
parallelapp=$5



# Step 1. run the ./swinginitiate file and then do source swingparams.file

swinginitiate

exit
# Step 2. Cleanup
for i in `cat ${nodelist}`
do
	ssh ${nodeuname}@${i} cleanswingexptnodes.sh 
done

# Step 3. Call individual scripts
#
# Step 4. Cleanup(?)


