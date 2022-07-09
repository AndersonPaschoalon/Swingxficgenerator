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


#             Takes in a one way trace (in pcap format) and makes a two way trace.
#             input has to be in tcpdump format obviously


function Usage(){
    echo "	This takes in an input file in pcap format and
	produces two files <file0> and <file1> based on
	link-level headers in the original pcap trace"

}

if [ "$#" -lt "1" ]
then
    echo "Usage $0 <pcapfile> "
	Usage;
    exit
fi

helpcmdline=`echo $* | grep -w -e "-h" | wc -l `
if [ "${helpcmdline}" -ge "1" ]
then
  	Usage;
	exit
fi

tcpdumpfile=$1

cat < /dev/null > dir0file
cat < /dev/null > dir1file
echo -ne "making dir0file and dir1file ..."
tcpdump -nn -e -r ${tcpdumpfile} 2>/dev/null| grep IP| convert_date_precision | awk '{
    if(NR==1) { dir[$2]="0" ; dir[$4]="1" }
    thisdir=dir[$2];

    if(thisdir=="0") print >>"dir0file"
    else             print >>"dir1file"
}' 

echo  "done"

echo -ne "making file0 and file 1 ..."
cat dir0file | sed 's/ .* length [0-9]*://'  | sed 's/ IP / /'| grep "ack\|win"| grep -iv "nodomain" | grep -iv icmp |grep -iv BGP | grep -v ":.*:.*:.*:.*:.*:"> file0
cat dir1file | sed 's/ .* length [0-9]*://'  | sed 's/ IP / /'| grep "ack\|win"| grep -iv "nodomain" | grep -iv icmp |grep -iv BGP | grep -v ":.*:.*:.*:.*:.*:"> file1
echo done
echo -ne "merging to fileboth ..."
sort -mk1,1n file0 file1 > fileboth
echo "done"

