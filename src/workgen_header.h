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
*  Last Modified : Thu Sep 18 22:52:00 PDT 2008
*
**********************************************************************************/
#ifndef __SWING_WORKGEN_HEADER_H__
#define __SWING_WORKGEN_HEADER_H__
/*************************** REPEATTIMECOMMONFILE USES THE SAME HEADER FILE SO BE CAREFUL *************************************************/
// for listener.c
# define TERM_STRING "#"
#define MY_OWN_PORT 22222
#define LINESIZE (1480*8)
//# define SWINGPROJECT_REQTHINK_TIMES 0
//# define SWINGPROJECT_INTERRRE_TIMES 1
//# define SWINGPROJECT_INTERCONN_TIMES 1
#define MAGICLINESIZE (500)  /* has to be smaller than LINESIZE*/
#define LINESIZE_UDP 1024
#define SEED 17687
#define TIMECOMMONLINESIZE (2048*100)

//#define DEBUG 1
//#define UDP_PKT_DURATION_THRESHOLD 10000
#define UDP_PKT_DURATION_THRESHOLD 200
#define UDP_DURATION_THRESHOLD 120 /* i.e. 2 mins */
#define UDP_PKT_THRESHOLD 100
#define UDP_NUMPAIRS_THRESHOLD 10
//#define SPECSFILE "/home/kvishwanath/workgen/specs.all" 
//#define HOSTSFILE "/home/kvishwanath/bin/latest/hosts"

/* JUST changing random lines  */
//#define STARTLISTENERFILENAME "/home/kvishwanath/Swing/swing_code/startlistener"
#define HACK_CONN1 1
// for generator_daemon.c

//#define LINESIZE 8*1024
//# define TERM_STRING "22over22"
//#define SEED 34521
#define TIMEOUTSECS 10
#define TIMEOUTSECS_DURINGSEND 2
#define NODELAYHACK 1

#define MSSHACK 1

#define NOPRINTFS 1
//#define NEWPRINTFS
//#define MACRODEBUGS 1
//#define DEBUG_SLOWNESS
//#define MINIMALPRINTOUT 1
//#define DEBUG_REQRSPSIZE 1
//#define REQRSPSIZE100

/*
# Explanation
# MINIMALPRINTOUT =0 for abso no output
# MINIMALPRINTOUT =1 + NOPRINTFS =1
# MACRODEBUGS=1 for  little extra macro information
# NEWPRINTFS also on => server is also printing lots of stuff
# and client is printing what it sent etc. also.,
# only NEWPRINTFS=1 but NOPRINTFS also =1 =>
# slightly less output. in particulare the Inline is
# not printed but only the sizes etc.
# DEBUG_SLOWNESS => timeing information
# DEBUG_REQRSPSIZE => only prints expected and actual
# REQ/RSP size differences
# REQRSPSIZE100 just makes all req and rsp of size 100 
# for debugging
*/

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define UDPPKTSIZEMAX 1470
#define UDPPKTSIZEMIN 20
#define TCPMAXSIZE LINESIZE-1
#define NOLOGGING 1
#define udpportnum 9918
#define MAXTOLERATESIZEDIFF 2000
//#define PKTSIZE (1480)
#define PKTSIZE_DEFAULT 1448
//#define PKTSIZE (500)
//#define DEBUG_GENERATOR_DAEMON 1
#define TIMESCALEFACTOR 1000
//#define DEBUG_LISTENER 1

enum kinds {file=0, fixed, randval, function};

enum parametersnames {param_REQ=0, param_RSP, param_numpairs, param_serverthink, param_userthink, param_brate, param_protocol, param_pktsize,param_numconn, param_interconn, param_numrre, param_interRRE, param_serverdist, param_clientdist, param_port, param_transport};
#define numofparams 17
typedef struct {
	int bytes;
	int pkts;
	int duration;
}udp_req;
#endif /* __SWING_WORKGEN_HEADER_H__*/
