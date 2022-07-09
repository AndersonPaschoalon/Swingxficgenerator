/**********************************************************************************
* 
*  Copyright (C) 2008 Kashi Venkatesh Vishwanath <swingxficgenerator@gmail.com>
* 
*  Permission is hereby granted, free of charge, to any person obtaining a copy of
*  this software and associated documentation files (the "Software"), to  deal  in
*  the Software without restriction, including without limitation  the  rights  to
*  use, copy, modify, merge, publish, distribute, sublicense, and/or  sell  copies
*  of the Software, and to permit persons to whom the Software is furnished to  do
*  so, subject to the following conditions:
* 
*  The above copyright notice and this permission notice shall be included in  all
*  copies of the Software and its documentation and acknowledgment shall be  given
*  in the documentation  and  software  packages  that  this  Software  was  used.
*  
*  You should have received a copy of the GNU Library General Public License along
*  with this library; if not, write to the  Free  Software  Foundation,  Inc.,  59
*  Temple Place, Suite 330, Boston, MA 02111-1307, USA.
* 
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY  OF  ANY  KIND,  EXPRESS  OR
*  IMPLIED, INCLUDING BUT  NOT  LIMITED  TO  THE  WARRANTIES  OF  MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO  EVENT  SHALL  THE
*  AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR  OTHER  LIABILITY,  WHETHER  IN  AN
*  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF  OR  IN  CONNECTION
*  WITH  THE  SOFTWARE  OR  THE  USE  OR   OTHER   DEALINGS  IN   THE   SOFTWARE.
*
*
*
*  Last Modified : Thu Sep 18 22:51:59 PDT 2008
*
**********************************************************************************/
#ifndef __SWING_GETCONNINFO_H__
#define __SWING_GETCONNINFO_H__
#define LINESIZE 1024
# define NUMFIELDS 15
# define FIELDSIZE 1024
# define TCPFLG_URG 0x01
# define TCPFLG_ACK 0x02
# define TCPFLG_FIN 0x04
# define TCPFLG_SYN 0x08
# define TCPFLG_PSH 0x10
# define TCPFLG_RST 0x20
# define TCPFLG_NON 0x40

# define CONNSTATE_BAD 0x0
# define CONNSTATE_SYN 0x01
# define CONNSTATE_SYNACK 0x02
# define CONNSTATE_ACKSYN 0x04
# define CONNSTATE_REQ 0x08
# define CONNSTATE_REQACK 0x10
# define CONNSTATE_RSP 0x20
# define CONNSTATE_RSPACK 0x40 
# define CONNSTATE_FIN 0x80
# define CONNSTATE_FINACK 0x100
# define CONNSTATE_ACKFIN 0x200

typedef struct
{
	long long int seqstart;
	long long int seqend;
	int pkt_data;
} seqinfo;

#define TOLOG 1
//#define PRINTFS 1
//#define PRINTTCPDUMPLINE 1
#define min(a,b) ((a<b)?(a):(b))
#define max(a,b) ((a>b)?(a):(b))
#define WANT_PRINTF_LONG_LONG
#define SYNTIMEOUT 3
#define MAXNUMREQRSP 50000
typedef struct {
	long long int req;
	long long int rsp;
	double reqstart;
	double reqend;
	double rspstart;
	double rspend;
} req_rsp_struct;
#endif /* __SWING_GETCONNINFO_H__*/
