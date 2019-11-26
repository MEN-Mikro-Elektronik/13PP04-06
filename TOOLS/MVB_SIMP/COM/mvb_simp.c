/****************************************************************************/
/*!
 *         \file mvb_simp.c
 *       \author thomas.schnuerer@men.de
 *
 *       \brief  Simple example program for the MVB LL driver
 *
 *               sets up some LA Ports and performs tests, works together
 *				 with the Siemens Bus Administrator Port setup. For
 *				 customer use, the Port setup must be adapted of course.
 *
 *     Required: libraries: mdis_api, usr_oss
 */
 /*
 *---------------------------------------------------------------------------
 * Copyright 2003-2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/
/*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/mdis_api.h>
#include <MEN/mvb_drv.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

#define CHK(expression) \
 if((expression)) {\
	 printf("\n*** Error during: %s\nfile %s\nline %d\n", \
      #expression,__FILE__,__LINE__);\
     goto ABORT;\
 }

#define MS_PER_SEC				1000

#define MVB_DEVICE_ADDRESS_SLAVE_B 		0x0001
#define MVB_DEVICE_ADDRESS_SLAVE_A		0x004d

/* MVB_LA_PORT struct is 64 byte aligned */
#define LP_PAD	{0,0,0,0,0,0,0,0,0,0}

/* indices of G_port[] array below which are used as source/sink examples */
#define PORT_SIZE_MAX			32

#define WD_TIME					200		/* milliseconds */
#define MVB_TX_TIME				250	/* milliseconds */

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

static volatile u_int32 G_getnewdata 	= FALSE;
static volatile u_int32 G_geterrors  	= FALSE;
static volatile u_int32 G_quit			= FALSE;
static volatile u_int32 G_timeSec 		= 0;

/*!
 * Example Ports matching the Test Busadministrator setup.
 * Mind here the sinkports 890/891 TACK tresholds of 0xfe00:
 * when Sinktime Supervision is switched then reading from these ports will 
 * fail after 4095 ms when Sinktime Interval=8 ms, 8190 ms when interval = 16ms
 * and so on (0xffff - 0xfe00 = 512 cycles before sink timeout).
 * 
 */

/* This is SlaveB setup, if SlaveA is used source and sink is switched later */
static MVB_LA_PORT G_ports[]={ 
	{  14,  4, 32,	"ATO_ATS_S",  MVB_LA_PORT_SOURCE, 0xffdf, 0, LP_PAD },
	{  15,  8, 32,	"ATO_DGP_S",  MVB_LA_PORT_SOURCE, 0xff7f, 0, LP_PAD },
	{  16, 12, 32,	"ATO_MMI_M",  MVB_LA_PORT_SOURCE, 0xffef, 0, LP_PAD },
	{  17, 24,  8,	"ATO_TMS_F",  MVB_LA_PORT_SOURCE, 0xfffb, 0, LP_PAD },
	{  18, 16, 32,	"ATO_TMS_S",  MVB_LA_PORT_SOURCE, 0xff7f, 0, LP_PAD },
	{ 890, 25,  8,	"VCU_ATO_F",  MVB_LA_PORT_SINK,   0xfe00, 0, LP_PAD },
	{ 891, 20, 32,	"VCU_ATO_M",  MVB_LA_PORT_SINK,   0xfe00, 0, LP_PAD }
};


/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void __MAPILIB SignalHandler( u_int32 sig );
static void DumpMvbErrors(MDIS_PATH mdispath);
static void Usage(void);
static void DumpData(u_int8 *dat, u_int32 size, int32 nbrBytesRead );
static void SetupData(u_int8 *dat, u_int32 size, int32 show);

/***************************************************************************/
/** Print MDIS example usage message
 *
 *  \param exitcode  \IN  exitcode return value to exit with
 *
 */
static void Usage()
{
    printf("mvb_simp <device> [options]\n" );
    printf("Example for MVB slave device driver.\n");
    printf("MVB port setup is based on busmaster shown below \n");
    printf("(the Master is a 3rd party device, Slave A/B are both PP4)\n");
    printf("  +---------+   +---------+   +---------+\n");
    printf("  | Master  |   | Slave A |   | Slave B |\n");
    printf("  | Adr:58H |   | Adr:4DH |   | Adr:01H |\n");
    printf("  +---------+   +---------+   +---------+\n");
    printf("       |             |             |\n");
    printf("     -[_]-----------[_]-----------[_]-\n\n");
    printf("|idx |port| size |  cycle | name      | source | tack\n");
    printf("|    |    |[byte]|   [ms] |           |        |\n");
    printf("|----+----+------+--------+-----------+--------+-------\n");
    printf("|  0 | 14 |   32 |    256 |'ATO_ATS_S'| SlaveB | 0FFDFH\n");
    printf("|  1 | 15 |   32 |   1024 |'ATO_DGP_S'| SlaveB | 0FF7FH\n");
    printf("|  2 | 16 |   32 |    128 |'ATO_MMI_M'| SlaveB | 0FFEFH\n");
    printf("|  3 | 17 |    8 |     32 |'ATO_TMS_F'| SlaveB | 0FFFBH\n");
    printf("|  4 | 18 |   32 |   1024 |'ATO_TMS_S'| SlaveB | 0FF7FH\n");
    printf("|  5 |890 |    8 |     16 |'VCU_ATO_F'| SlaveA | 0FFFBH\n");
    printf("|  6 |891 |   32 |    128 |'VCU_ATO_M'| SlaveA | 0FFDFH\n\n");
    printf(" -r=1:      dump chosen sink port\n");
    printf(" -w=1:      write to port specified by -p=port\n");
    printf(" -p=idx:    index of port to access             -p=3: port 17\n");
    printf(" -d=1:      dump whole TM after execution\n");
    printf(" -s=1:      dump sinktime of example sinkport\n");
    printf(" -a=<slave> -a=0: be Slave B,  -a=1: be Slave A. Port setup:\n");
    printf("            Slave A: 14,15,16,17,18: sink    890,891: source\n");
    printf("            Slave B: 14,15,16,17,18: source  890,891: sink\n");
    printf(" -t=1:      trigger the Watchdog every 500ms.\n");   
    printf("            must be set, omitting causes Watchdog timeout.\n");
    printf(" -i=[1..9]: sinktime supervision Interval:\n");
    printf("            -i=0  No Sinktime Supervision\n");
    printf("            -i=1: 1 ms -i=2: 2 ms -i=3: 4 ms .. -i=9: 256 ms\n");
}



/***************************************************************************/
/** Set the example source buffer with some Values
 *
 *  \param dat        \IN  where to store the data
 *  \param size       \IN  how many bytes to write
 *  \param show       \IN  if nonzero: dump data that is sent
 *
 *  \return	          success (0) or error (1)
 *
 *	\brief		  the function initializes the source port with some random
 *				  like values
 */
static void SetupData(u_int8 *dat, u_int32 size, int32 show)
{
	u_int32  i;
	/* some pseudo random seed */
	u_int32  seed = UOS_MsecTimerGet() & 0xff;

	for (i = 0; i < size; i++){
		*dat = (u_int8)(i + seed);
		if (show)
			printf( "%02x", *dat );

		dat++;
	}
	if (!!show)
		printf( "\n");
}


/***************************************************************************/
/** print out data from a sink port
 *
 *  \param dat        	\IN  where to store the data
 *  \param size       	\IN  how many bytes to read
 *  \param nbrBytesRead \IN  actual read bytes from M_getblock call
 *
 *  \return	          success (0) or error (1)
 */
static void DumpData(u_int8 *dat, u_int32 size, int32 nbrBytesRead )
{
	u_int8  i;
	if(size > PORT_SIZE_MAX) 
		return;

	printf("Data:\n");
	for (i=0; i < size; i++ )
		printf("%02x", *dat++);
	printf("\n");

}


/********************************* main ************************************/
/** Program main function
 *
 *  \param argc       \IN  argument counter
 *  \param argv       \IN  argument vector
 *
 *  \return	          success (0) or error (1)
 */
int main(int argc, char *argv[])
{
	MDIS_PATH path;
	int32 sinktime, retval;
	u_int32 errnumber = 0;

	M_SG_BLOCK blk;
	int32 *blkstartP=NULL;

	u_int32 optReadSink, optWriteSrc, optDumpSinktime, optSinktimeInterval;
	u_int32 optWdTrigger, optBA, optNoWdog, optDumpTm, i;
	u_int32 optSlave, optPort;
	u_int32 msWriteTime=0, timeElapsed=0, startTime=0, timeCurrent=0;

	/* dummy buffer to read data into */
	u_int16 dataBuf[65536];

	char *device, *str, *errstr;
	unsigned char buf[40];


	/* init globals to 0 for OSes which keep mvb_simp persistent (vxWorks) */
	G_getnewdata 	= FALSE;
	G_geterrors  	= FALSE;
	G_quit			= FALSE;
	G_timeSec 		= 0;

	if (argc < 2 || strcmp(argv[1],"-?") == 0){
		Usage();
		return 1;
	}
	
	/*--------------------+
	 | check arguments    |
	 +--------------------*/
	device = argv[1];

	if ((errstr = UTL_ILLIOPT("nba=d=p=l=t=w=r=s=i=?", (char*)buf) )) {
		printf("*** %s\n", errstr);
		return(1);
	}

	optReadSink 		=   ((str = UTL_TSTOPT("r=")) ? atoi(str) : 0);
	optWriteSrc  		=   ((str = UTL_TSTOPT("w=")) ? atoi(str) : 0);
	optDumpSinktime		=   ((str = UTL_TSTOPT("s=")) ? atoi(str) : 0);
	optSinktimeInterval	=   ((str = UTL_TSTOPT("i=")) ? atoi(str) : 0);
	optWdTrigger		=   ((str = UTL_TSTOPT("t=")) ? atoi(str) : 0);
	optDumpTm			=   ((str = UTL_TSTOPT("d=")) ? atoi(str) : 0);
	optSlave			=   ((str = UTL_TSTOPT("a=")) ? atoi(str) : 0);
	optPort				=   ((str = UTL_TSTOPT("p=")) ? atoi(str) : 0);

	/* unofficial test options: */
	optBA				=   ((str = UTL_TSTOPT("b")) ? 1 : 0);
	optNoWdog			=   ((str = UTL_TSTOPT("n")) ? 1 : 0);

	/*--------------------+
	 | open path          |
	 +--------------------*/
	CHK((path = M_open(device)) < 0);

	/*--------------------+
	 |  config signals    |
	 +--------------------*/

    /* install 2 signals: for problem report and new data arrival */
    CHK(UOS_SigInit( SignalHandler ));
    CHK(UOS_SigInstall( UOS_SIG_USR1 ));
    CHK(UOS_SigInstall( UOS_SIG_USR2 ));

	/* here UOS_SIG_USR1 is received upon all problem issues encountered */
    CHK(M_setstat( path, MVB_SET_SIGNAL_ALERT, UOS_SIG_USR1 ));

	/* UOS_SIG_USR2 is sent when a sink port was set up with compare mode on */
    CHK(M_setstat( path, MVB_SET_SIGNAL_DATA,  UOS_SIG_USR2 ));

	/* 
	 * before passing the NSDB set source and sink port property according	
	 * to passed slave instance B or A. The global G_ports is initialized as 
	 * SlaveB, so turn the sink/source property for SlaveA.
	 */
	printf("Running mvb_simp as slave ");
	if (optSlave) { /* if -a=1 we are slave A, 14-18 are sink */
		printf("A\n");
		G_ports[0].srcsink = MVB_LA_PORT_SINK;
		G_ports[1].srcsink = MVB_LA_PORT_SINK;
		G_ports[2].srcsink = MVB_LA_PORT_SINK;
		G_ports[3].srcsink = MVB_LA_PORT_SINK;
		G_ports[4].srcsink = MVB_LA_PORT_SINK;
		G_ports[5].srcsink = MVB_LA_PORT_SOURCE;
		G_ports[6].srcsink = MVB_LA_PORT_SOURCE;		
	} else {
		printf("B\n"); /* -a=0: leave G_ports as it is, 14-18 are source */
	}

	/* if port specified is a sink, set compare mode to get changed data */
	if (G_ports[optPort].srcsink == MVB_LA_PORT_SINK )
		G_ports[optPort].compare = 1;	

	/*--------------------+
	 | set Device address |
	 +--------------------*/
	CHK(M_setstat( path, 
		MVB_DEVICE_ADDRESS, 
		optSlave ? MVB_DEVICE_ADDRESS_SLAVE_A : MVB_DEVICE_ADDRESS_SLAVE_B ));

	/*--------------------+
	 | enable MVBCS1 IRQs |
	 +--------------------*/
	CHK(M_setstat(path, M_MK_IRQ_ENABLE, 1));

	/*------------------------------+
	 | start sinktime supervision   |
	 +------------------------------*/
	CHK(M_setstat(path, MVB_SINKTIME_SUPERVISION, optSinktimeInterval));

	/* disable intern Wdog? */
	if (optNoWdog) {
		printf("- disable intern Watchdog/hang detection\n");
		CHK(M_setstat(path, MVB_NOWDOG, 1));
	}

	/*--------------------+
	  | pass the NSDB	  |
	 +--------------------*/
	blk.size 	= sizeof(G_ports);
	blk.data 	= (void*)G_ports; 	
	CHK(M_setstat( path, MVB_BLK_NSDB, (INT32_OR_64)&blk ));

	/*--------------------+
	 | start communication|
	 +--------------------*/
	CHK(M_setstat( path, MVB_INITLEVEL, MVB_INITLEVEL_FULL_OP ));	

	/* generate MF tables, intern use only! */
	if (optBA) {
		printf("- set BA mode\n");
		CHK(M_setstat(path, MVB_BUSADMIN, 1));
	}

	timeElapsed = UOS_MsecTimerGet();
	msWriteTime = UOS_MsecTimerGet();
	startTime 	= UOS_MsecTimerGet();


	/* init send buffer */
	SetupData(buf, G_ports[optPort].size , 0);

	printf("Start Time (seconds): %ld", startTime / MS_PER_SEC);

	/* perform selected actions... */
	while( 1 ) {

		/* update time */	
		if (UOS_MsecTimerGet() > (timeCurrent + MS_PER_SEC)) {
			timeCurrent = UOS_MsecTimerGet();
			G_timeSec++;
		}

		/*--------------------+
		 | write a source port|
		 +--------------------*/
		if (optWriteSrc && (UOS_MsecTimerGet() >(msWriteTime + MVB_TX_TIME))) {
			/* a sourceport is updated every second with changing content*/
			SetupData(buf, G_ports[optPort].size , 1);
			CHK(M_setstat( path, M_MK_CH_CURRENT, G_ports[optPort].addr ));
			M_setblock(path, buf, G_ports[optPort].size);
			msWriteTime = UOS_MsecTimerGet();
		}

		/*--------------------+
		 | read from sink port|
		 +--------------------*/
		if (optReadSink ) {
			CHK(M_setstat( path, M_MK_CH_CURRENT, G_ports[optPort].addr));
			retval = M_getblock(path, buf, G_ports[optPort].size);
			CHK( retval <= 0 );
			DumpData(buf, G_ports[optPort].size, retval);
		}

		/*------------------------+
		 | dump sinkports sinktime|
		 +------------------------*/
		if (optDumpSinktime) {
			CHK(M_setstat( path, M_MK_CH_CURRENT, G_ports[optPort].addr));
			CHK(M_getstat( path, MVB_SINKTIME_SUPERVISION, &sinktime));
			printf("Port%d Sinktime: 0x%04x\n",G_ports[optPort].addr, sinktime);
		}

		/*--------------------+
		 | trigger Watchdog   |
		 +--------------------*/ 				
		if (optWdTrigger && (UOS_MsecTimerGet() > timeElapsed + WD_TIME)) { 
			timeElapsed = UOS_MsecTimerGet();
			CHK(M_setstat( path, MVB_TRIGGER_WATCHDOG, 0/* unused */));
		}	

		/*--------------------+
		 | dump errors        |
		 +--------------------*/ 		
		if (G_geterrors) {
			DumpMvbErrors(path);
			/* break; */
		}

		/*--------------------+
		 | dump changed data  |
		 +--------------------*/
		/* always dump changed data */
		if (G_getnewdata) {
			CHK(M_setstat( path, M_MK_CH_CURRENT, G_ports[optPort].addr ));
			CHK(( retval = M_getblock(path, buf, G_ports[optPort].size) < 0 ));
			DumpData(buf, G_ports[optPort].size, retval);

			/* dump current system date for logging purposes */
			printf("time %ld s\n", G_timeSec );
			G_getnewdata = FALSE;
		}
		
		if (G_quit || (UOS_KeyPressed() != -1) )
			break;

	}

	printf("mvb_simp stopped.\n");

	if (optDumpTm){
	/*--------------------+
	 | read whole data TM |
	 +--------------------*/
		blk.size 	= sizeof(dataBuf);
		blk.data 	= (void*)dataBuf; 
		blkstartP 	= (int32*)&blk;
		CHK(M_getstat( path, MVB_BLK_DIRECT, blkstartP));
		printf("read back start of TM with getstat MVB_BLK_DIRECT:\n");
		for (i = 0; i < 128; i++){
			if (!(i % 16))
				printf("\n");
			printf("0x%04x ", dataBuf[i]);
		}
		printf("\n");
	}

	/*--------------------+
     |  cleanup           |
	 +--------------------*/
	printf("cleaning up.");
    CHK(UOS_SigRemove( UOS_SIG_USR1 ));
    CHK(UOS_SigRemove( UOS_SIG_USR2 ));
    CHK(UOS_SigExit());

	if (M_close( path ) < 0)
		printf("error on close\n");

	return(0);

 ABORT:
	errnumber = UOS_ErrnoGet();
	printf("*** Error 0x%08x (%s)\n", errnumber, M_errstring(errnumber));
	return(0);
}



/***************************************************************************/
/** Get and dump MVB error Counters/Flags
 *
 *  \param mdispath	\IN				path returned by M_open
 */
static void DumpMvbErrors(MDIS_PATH mdispath)
{

	int32 retval;
	G_geterrors = FALSE;

	/*
	 * to minimize accesses the user can make Error retrieving
	 * getstats based on the error flags. Here all Error getstats are
	 * done for completeness
	 */

	printf("Alert Signal received.\n");
	CHK(M_getstat( mdispath, MVB_ERR_FLAGS, &retval ));

	printf("   Error flags: 0x%04x\n",(u_int16)retval);

	CHK(M_getstat( mdispath, MVB_ERR_MASTER_FRAMES, &retval ));
	printf("   errornous Masterframes:        %d\n", retval );

	CHK(M_getstat( mdispath, MVB_ERR_SLAVE_FRAMES, &retval ));
	printf("   errornous Slaveframes:         %d\n", retval );		

	CHK(M_getstat( mdispath, MVB_ERR_DUPLICATE_MASTER_FRAMES, &retval ));
	printf("   Duplicate Masterframes:        %d\n", retval );		

	CHK(M_getstat( mdispath, MVB_ERR_DUPLICATE_SLAVE_FRAMES, &retval ));
	printf("   Duplicate Slaveframes:         %d\n", retval );			

	CHK(M_getstat( mdispath, MVB_ERR_COUNT, &retval ));
	printf("   # of errornous Frames:         %d\n", retval );			

	CHK(M_getstat( mdispath, MVB_ERR_FRAMES_LOST, &retval ));
	printf("   Lost Frames:                   %d\n", retval );

	CHK(M_getstat( mdispath, MVB_ERR_REPLY_TIMEOUT, &retval ));
	printf("   reply timeouts:                %d\n", retval );

	CHK(M_getstat( mdispath, MVB_ERR_BUS_TIMEOUT, &retval ));
	printf("   Bus timeouts:                  %d\n", retval );			

	CHK(M_getstat( mdispath, MVB_ERR_ASIC_HANG, &retval ));
	printf("   MVB ASIC hang:                 %d\n", retval );			

	/* dump current system date for logging purposes */
	printf("time %ld s\n", G_timeSec );

	/* The user may let the errorcount accumulate or reset it after each read*/
	CHK(M_setstat( mdispath, MVB_RESET_ERROR_COUNT, 0 ));
	
 ABORT:
	return;
}

/***************************************************************************/
/** Signal handler for reporting error conditions
 *
 *  \param  sig    \IN   received signal
 */
static void __MAPILIB SignalHandler( u_int32 sig )
{

	if( sig == UOS_SIG_USR1 ) {
		G_geterrors = TRUE;
	}
    
    if( sig == UOS_SIG_USR2 ) {
		G_getnewdata = TRUE;
	}
}

