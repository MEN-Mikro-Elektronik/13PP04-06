/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: pp04_drv_int.h
 *      Project: PP04 module driver (MDIS4)
 *
 *       Author: ts
 *        $Date: 2013/05/16 17:48:07 $
 *    $Revision: 1.5 $
 *
 *  Description: PP04 driver internal declarations and defines
 *
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 2001-2019, MEN Mikro Elektronik GmbH
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

#ifndef _PP04_DRV_INT_H_
#define _PP04_DRV_INT_H_

#ifdef PP04_MDIS_UNITTEST
# include <MEN/maccess.h>
# include "user_oss.h"
#else
# include <MEN/oss.h>
# include <MEN/desc.h>
# include <MEN/maccess.h>
# include <MEN/mdis_com.h>
# include <MEN/dbg.h>
# include <MEN/mbuf.h>
#endif


/* general macros */
#define CH_NUMBER_LOGICAL_PORTS		4096
#define CH_NUMBER_PHYSICAL_PORTS	8

#define CH_NUMBER CH_NUMBER_PHYSICAL_PORTS + CH_NUMBER_LOGICAL_PORTS

#define USE_IRQ					TRUE
#define ADDRSPACE_COUNT			1	
#define ADDRSPACE_SIZE			1*1024*1024 /* max. used size of TM	*/

/* maximum possible number of LA ports */
#define MVB_LA_PORTS_MAX		4096
#define MVB_MAX_ADDR			4095	/* max. Device address (12bit) 		*/

/* some number replacements to satisfy CodeWizard*/
#define MVB_NUM_0				0
#define MVB_NUM_1				1
#define MVB_NUM_2				2
#define MVB_NUM_3				3
#define MVB_NUM_4				4
#define MVB_NUM_6				6
#define MVB_NUM_8				8
#define MVB_NUM_12				12
#define MVB_NUM_10				10
#define MVB_NUM_F				0xf
#define MVB_NUM_FF				0xff
#define MVB_NUM_FFFF			0xffff

/* poll MR[BUSY] after Masterframe kickoff for max. 50x10us = 500us so system 
   load remains low */
#define MVB_LOOPBACK_VALUE		0xa55a	/* unique value for loopback test 	*/ 
#define MVB_LOOPBACK_MF			0x1001	/* Masterframe used for selftest 	*/
#define MVB_LOOPBACK_TIMEOUT	50		/* cycles to poll BUSY bit in MR 	*/
#define MVB_PP_DAT_PCS1			0x20	/* page distance (bytes) in phy.port*/
#define MVB_DELAY_500_US		500		/* time to do SW reset 				*/

/* Function Codes (F-Codes) , in PCS Word 0 and Master Frames */
#define MVB_FC0 				(0x0<<12)    /* F-Code 0  */
#define MVB_FC1 				(0x1<<12)    /* F-Code 1  */
#define MVB_FC2 				(0x2<<12)    /* F-Code 2  */
#define MVB_FC3 				(0x3<<12)    /* F-Code 3  */
#define MVB_FC4 				(0x4<<12)    /* F-Code 4  */
#define MVB_FC5 				(0x5<<12)    /* F-Code 5  */
#define MVB_FC6 				(0x6<<12)	 /* F-Code 6  */
#define MVB_FC7 				(0x7<<12)	 /* F-Code 7  */
#define MVB_FC8 				(0x8<<12)    /* F-Code 8  */
#define MVB_FC9                 (0x9<<12)    /* F-Code 9  */
#define MVB_FC10   	   	   	   	(0xa<<12)    /* F-Code 10 */
#define MVB_FC11                (0xb<<12)    /* F-Code 11 */
#define MVB_FC12                (0xc<<12)    /* F-Code 12 */
#define MVB_FC13                (0xd<<12)    /* F-Code 13 */
#define MVB_FC14                (0xe<<12)    /* F-Code 14 */
#define MVB_FC15                (0xf<<12)    /* F-Code 15 */

/* The Timer2 test for Asic being alive is done every 1000ms */
#define MVB_OSS_CYCLIC_ALARM	1
#define MVB_TI2_CHK_INTERVAL	1000

#define MVB_TI2_INITVAL			0xf000
#define MVB_DELAY_10_US			10


/*---------------------------+
 | internal Registers + bits |
 +---------------------------*/
/* 
 * The MCM dependent Service Area offset must be added to all these addresses.
 * (is either 0x3000, 0x7000 or 0xf000.	
 */

#define MVB_REG_MCR				0x0F84	/* 	memory config. reg 			*/
#define MVB_REG_DR				0x0F88	/* 	Decoder reg 				*/
#define MVB_REG_STSR			0x0F8C	/* 	Sinktime Supervision Reg	*/
#define MVB_STSR_IVAL			(12)	

/* # of LA data docks in the different MCMs */
#define MVB_LA_DOCKS_256		256
#define MVB_LA_DOCKS_1024		1024
#define MVB_LA_DOCKS_4096		4096

/* mask 3 LSBs from MCR */
#define	MVB_REG_MCR_MASK		0xFFF8

/* Telegram analysis unit */
#define MVB_REG_FC				0x0F90	
#define MVB_REG_EC				0x0F94	
#define MVB_REG_MFR				0x0F98	
#define MVB_REG_MFRE			0x0F9C	

/* Master Frame Slot for selftests */
#define MVB_REG_MFS				0x0E00	

/* SAOFFSin control Unit */
#define MVB_REG_MR				0x0FA0	
#define MVB_REG_MR2				0x0FA4	
#define MVB_REG_DPR				0x0FA8	
#define MVB_REG_DPR2			0x0FAC

/* Interrupt logic */
#define MVB_REG_IPR0			0x0FB0
#define MVB_REG_IPR1			0x0FB4
#define MVB_REG_IMR0			0x0FB8
#define MVB_REG_IMR1			0x0FBC
#define MVB_REG_ISR0			0x0FC0
#define MVB_REG_ISR1			0x0FC4
#define MVB_REG_IVR0			0x0FC8
#define MVB_REG_IVR1			0x0FCC

#define MVB_REG_CONF			0x0FD0
#define MVB_CONF_SLA			(1<<4)
#define MVB_CONF_SLB			(1<<5)
#define MVB_CONF_LED2			(1<<1)	

#define MVB_REG_WDC				0x0FD4
/* MVBCS1 WD is retriggered double as often as the WD Reload Value itself */
#define MVB_WD_RETRIGGER_TIME 	500		/* ms */
#define MVB_WATCHDOG_TIME		1000	/* ms */
#define MVB_WD_TIMEBASE_1MS		(0<<16)
#define MVB_WD_TIMEBASE_32MS	(1<<16)

/* Device Address */
#define MVB_REG_DAOR			0x0FD8
#define MVB_REG_DAOK			0x0FDC
#define MVB_DA_KEY_ENABLE		0x94	/* makes the new DA effective 		*/
#define MVB_DA_KEY_DISABLE		0x49	/* disabl DA in DAOR, use pin value */

/* Status and Control Register */
#define MVB_REG_SCR				0x0F80
#define MVB_SCR_MAS				(1<<3)
#define MVB_SCR_QUIET			(1<<14)
#define MVB_SCR_WS				8
#define MVB_SCR_TMO				10

/* universal timer / MVBCS1 Register */
#define MVB_REG_TCR				0x0FE0
#define MVB_TCR_TA2 			(1<<4)
#define MVB_TCR_TA1 			(1<<4)

#define MVB_REG_EC_A			0x0FE4
#define MVB_REG_EC_B			0x0FE8
#define MVB_REG_LIV				0x0FEC
#define MVB_REG_TR1				0x0FF0

#define MVB_REG_TR2				0x0FF4
#define MVB_TR2_RELOAD_VALUE	0xffff

#define MVB_REG_TC1				0x0FF8
#define MVB_REG_TC2				0x0FFC

/* line selection for enabling/disabling */
#define MVB_LINE_A				0
#define MVB_LINE_B				1

/* the PCS is a Register in TM belonging to each port */
#define MVB_PCS1_VP				(1<<6)
#define MVB_PCS_VP_OFFS			0x20

#define MVB_PCS0_COMPARE_MODE	(7<<5)	/* comparison Mode (IEn=7 in PCS0) */


/* several bit masks to set / clear bits */
#define MVB_CLEAR_ARB_MASK		0xff3f	
#define MVB_LOBYTE_WORD_MASK	0x00ff
#define MVB_HIBYTE_WORD_MASK	0xff00
#define	MVB_ADDR_ODD_MASK 		0xfffe	
#define	MVB_ADDR_MASK_FFFC		0xfffc

#define MVB_ARB_CPU_PRIO		(0<<6)
#define MVB_ARB_ASIC_PRIO		(1<<6)
#define MVB_ARB_CPU_ONLY		(2<<6)
#define MVB_ARB_CPU_XFER		(3<<6)


/* interupt flags - 16 sources of Reg. Set 0 */
#define IF_EMF		(1<<15)		/* Errornous Master Frame 			*/
#define IF_ESF		(1<<14)		/* Errornous Slave Frame 			*/
#define IF_DMF		(1<<13)		/* Duplicate Master Frame 			*/
#define IF_DSF		(1<<12)		/* Duplicate Slave Frame 			*/
#define IF_AMFX		(1<<11)		/* All Master Frames Transmitted 	*/
#define IF_MFC		(1<<10)		/* Master Frame Checked 			*/
#define IF_SFC		(1<<9)		/* Slave Frame Checked 				*/
#define IF_RTI		(1<<8)		/* Reply Timeout Interrupt 			*/
#define IF_BTI		(1<<7)		/* Bus Timeout Interrupt 			*/
#define IF_DTI7		(1<<6)		/* Data Transfer Int ( at Port PCS)	*/
#define IF_DTI6		(1<<5)		/* 			" 						*/
#define IF_DTI5		(1<<4)		/* 			" 						*/
#define IF_DTI4		(1<<3)		/* 			" 						*/
#define IF_DTI3		(1<<2)		/* 			" 						*/
#define IF_DTI2		(1<<1)		/* 			" 						*/
#define IF_DTI1		(1<<0)		/* 			" 						*/

/* interupt flags: 16 sources of Reg. Set 1 */

#define IF_TI2		(1<<15)		/* Universal Timer 2 Interrupt		*/
#define IF_XI3		(1<<14)		/* external Interrupt 3 			*/
#define IF_XI2		(1<<13)		/* external Interrupt 2 			*/
#define IF_XQE		(1<<12)		/* transmit queue exception 		*/
#define IF_RQE		(1<<11)		/* receive queue exception 			*/
#define IF_XQ1C		(1<<10)		/* transmit queue 1 complete 		*/
#define IF_XQ0C		(1<<9 )		/* transmit queue 0 complete 		*/
#define IF_RQC		(1<<8 )		/* receive queue complete 			*/
#define IF_FEV		(1<<7 )		/* Frames evaluated interrupt 		*/
#define IF_FL		(1<<6 )		/* Frame Lost	 					*/
#define IF_WD		(1<<5 )		/* Watchdog Interrupt 				*/
/* gap: bits 4,3 are not used! */
#define IF_TI1		(1<<2 )		/* Universal Timer 1 Interrupt 		*/
#define IF_XI1		(1<<1 )		/* external Interrupt 1 			*/
#define IF_XI0		(1<<0 )		/* external Interrupt 0 			*/




/*---------------------------+
 | physical ports (all page0)|
 +---------------------------*/

/* port control and status addresses */
#define MVB_PP_PCS_FC8			0x0E00	/* Mastership offer src		*/
#define MVB_PP_PCS_EFS			0x0E08	/* Eventframe sink			*/
#define MVB_PP_PCS_EF0			0x0E20	/* Eventframe 0 source		*/
#define MVB_PP_PCS_EF1			0x0E28	/* Eventframe 1 source		*/
#define MVB_PP_PCS_MOS			0x0E30	/* Mastership offer sink	*/
#define MVB_PP_PCS_FC15			0x0E38	/* Device status port 		*/
#define MVB_PP_PCS_TSRC			0x0E40	/* Message/Test source		*/
#define MVB_PP_PCS_TSNK			0x0E60	/* Message/Test sink  		*/

#define	MVB_REG_SMF				0x0F00	/* Manual Masterframe Slot 	*/
/* port data areas */
#define MVB_PP_DAT_FC8			0x0C00	
#define MVB_PP_DAT_EFS			0x0C08	
#define MVB_PP_DAT_EF0			0x0C40	
#define MVB_PP_DAT_EF1			0x0C48	
#define MVB_PP_DAT_MOS			0x0C50	
#define MVB_PP_DAT_FC15			0x0C58	
#define MVB_PP_DAT_TSRC			0x0C80	
#define MVB_PP_DAT_TSNK			0x0CC0	

/* bits in physical device status word */
#define LC_DSW_DNR				0x0002   /* Device Not Ready 			*/
#define LC_DSW_ERD				0x0008   /* Extended Reply Delay 		*/
#define LC_DSW_SDD				0x0010   /* Some Device Disturbance 	*/
#define LC_DSW_SSD				0x0020   /* Some System Disturbance 	*/
#define LC_DSW_LAA				0x0080   /* Line A Active (same as LAT) */
#define LC_DSW_RLD				0x0040   /* Redundant Line Distrbed 	*/

/* Physical Port bit specification for PCS */
#define TM_PCS0_SRC     		0x0800
#define TM_PCS0_SINK    		0x0400
#define TM_PCS0_TWCS    		0x0200
#define TM_PCS0_WA      		0x0100
#define TM_PCS0_QA      		0x0004
#define TM_PCS0_NUM     		0x0002
#define TM_PCS0_FE      		0x0001

#define TM_PCS1_VP0     		0x0000
#define TM_SCR_UTS      		0x0020
#define TM_SCR_UTQ      		0x0010
#define TM_SCR_MAS      		0x0008
#define TM_SCR_RCEV     		0x0004

#define TM_MR_SMFM   			0x0020
#define TM_MR_BUSY  	 		0x0200



/*---------------------------+
 | MVBCS1 address maps		 |
 +---------------------------*/
#define MVB_SA_OFFS_3000		0x3000
#define MVB_SA_OFFS_7000		0x7000
#define MVB_SA_OFFS_F000		0xF000

#define MVB_PP_OFFS_3C00		0x3c00
#define MVB_PP_OFFS_7C00		0x7c00
#define MVB_PP_OFFS_FC00		0xfc00

/* MCM0  (16 kByte Traffic Memory used) */
#define MVB_MC0_LPS 			0x0000	/* LA PIT start always at offs. 0x0 */
#define MVB_MC0_LPL				0x1000	/* LA PIT length 			*/
#define MVB_MC0_LCS				0x3000	/* LA PCS Start 			*/
#define MVB_MC0_LCL				0x800	/* LA PCS length 			*/
#define MVB_MC0_LDS				0x1000	/* LA DAT Start 			*/
#define MVB_MC0_LDL				0x1000	/* LA DAT length 			*/
																
#define MVB_MC0_DPS 			0x0000	/* DA PIT start 			*/
#define MVB_MC0_DPL				0x0000	/* DA PIT length 			*/
#define MVB_MC0_DCS				0x0000	/* DA PCS Start 			*/
#define MVB_MC0_DCL				0x0000	/* DA PCS length 			*/
#define MVB_MC0_DDS				0x0000	/* DA DAT Start 			*/
#define MVB_MC0_DDL				0x0000	/* DA DAT length 			*/
#define	MVB_MC0_SAS				0x3c00	/* Service Area Start 		*/
																	
/* MCM1 (32 kByte Traffic Memory used)	*/
#define MVB_MC1_LPS 			0x0000	   
#define MVB_MC1_LPL				0x1000
#define MVB_MC1_LCS				0x3000
#define MVB_MC1_LCL				0x0800 
#define MVB_MC1_LDS				0x1000
#define MVB_MC1_LDL				0x1000
								   
#define MVB_MC1_DPS 			0x4000
#define MVB_MC1_DPL				0x1000
#define MVB_MC1_DCS				0x7000
#define MVB_MC1_DCL				0x800 
#define MVB_MC1_DDS				0x5000
#define MVB_MC1_DDL				0x1000
#define	MVB_MC1_SAS				0x7c00

/* MCM2 (64 kByte Traffic Memory used)	*/
#define MVB_MC2_LPS  			0	   
#define MVB_MC2_LPL	 			0x2000
#define MVB_MC2_LCS	 			0xc000
#define MVB_MC2_LCL	 			0x2000
#define MVB_MC2_LDS	 			0x4000
#define MVB_MC2_LDL	 			0x4000
					 	   	   
#define MVB_MC2_DPS  			0x2000
#define MVB_MC2_DPL	 			0x2000
#define MVB_MC2_DCS	 			0xf000
#define MVB_MC2_DCL	 			0x800 
#define MVB_MC2_DDS	 			0xe000
#define MVB_MC2_DDL	 			0x1000
#define	MVB_MC2_SAS				0xfc00

/* MCM3 (256 kByte Traffic Memory used)	*/
#define MVB_MC3_LPS  			0		   
#define MVB_MC3_LPL	 			0x2000	   
#define MVB_MC3_LCS	 			0x30000   
#define MVB_MC3_LCL	 			0x8000	   
#define MVB_MC3_LDS	 			0x10000   
#define MVB_MC3_LDL	 			0x10000   
					 													   
#define MVB_MC3_DPS  			0x2000	   
#define MVB_MC3_DPL	 			0x2000	   
#define MVB_MC3_DCS	 			0x4000	   
#define MVB_MC3_DCL	 			0x4000	   
#define MVB_MC3_DDS	 			0x38000   
#define MVB_MC3_DDL	 			0x8000    
#define	MVB_MC3_SAS	 			0xfc00

/* MCM4 ( 1 MByte Traffic Memory used)	*/
#define MVB_MC4_LPS  			0x0	
#define MVB_MC4_LPL				0x2000	
#define MVB_MC4_LCS	 			0x30000
#define MVB_MC4_LCL	 			0x8000	
#define MVB_MC4_LDS	 			0x10000
#define MVB_MC4_LDL	 			0x10000
					   		
#define MVB_MC4_DPS  			0x2000	
#define MVB_MC4_DPL	 			0x2000	
#define MVB_MC4_DCS	 			0x38000
#define MVB_MC4_DCL	 			0x8000	
#define MVB_MC4_DDS	 			0x40000
#define MVB_MC4_DDL	 			0x10000
#define	MVB_MC4_SAS				0xfc00

/* Service Areas length is currently 0x300 till start of internal Regs*/
#define MVB_SA_LENGTH			0x300

/* LA Port identifier string, includes 0 terminator */
#define MVB_PORTNAME_LEN		32		


/* internal representation of one Port */
typedef struct _MVB_LA_PORT_NODE {
	OSS_DL_NODE	node;					/* Node to hook port in DL_LIST		*/
	u_int16		addr;					/* port address as in Masterframe	*/
	u_int16		index;					/* Index # in LA PIT 				*/
	u_int16		size;					/* port size (2/4/8/16/32 byte) 	*/
	u_int16 	tack;					/* sinktime supervision value 		*/
	u_int32		srcsink;				/* 0 :source port, other: sink		*/
	INT32_OR_64	dat;					/* complete pointer to ports data	*/
	INT32_OR_64	pcs;					/* complete pointer to PCS[0] word 	*/
	u_int16		num;					/* 1: num. data 0: nonnumeric data 	*/
	u_int16		compare;				/* use IRQ DT7 if data  	
										   are DIFFERENT from previous recv.*/
	u_int8		name[MVB_PORTNAME_LEN];	/* ident string for Port 			*/
    int32       memAlloc;				/* true size allocated for  struct	*/
	OSS_SEM_HANDLE	*portSem;			/* channel lock for used ports only	*/
} MVB_LA_PORT_NODE;


/* keep memory mapping */
typedef struct _MVB_TM_MAP {
	/* logical addressed areas */
	u_int32		laPitStart;				/* LA port index table 				*/
	u_int32		laPitLen;
	u_int32		laPcsStart;				/* LA port control&status			*/
	u_int32		laPcsLen;
	u_int32		laDatStart;				/* LA data area			  			*/
	u_int32		laDatLen;
	/* device adressed areas */
	u_int32		daPitStart;
	u_int32		daPitLen;
	u_int32		daPcsStart;
	u_int32		daPcsLen;
	u_int32		daDatStart;
	u_int32		daDatLen;
	/* physical ports inside Service Area */
	u_int32		saStart;
} MVB_TM_MAP;


/* low-level handle */
typedef struct {
	/* general */
    MACCESS         ma;             	/* hw handle (overall base) 		*/
    int32           memAlloc;			/* size allocated for the handle 	*/
    OSS_HANDLE      *osHdl;         	/* oss handle 						*/
    OSS_IRQ_HANDLE  *irqHdl;        	/* irq handle 						*/
    DESC_HANDLE     *descHdl;       	/* desc handle 						*/
	MDIS_IDENT_FUNCT_TBL idFuncTbl;		/* id function table 				*/
    OSS_SIG_HANDLE  *sigAlertHdl;       /* signalhandle f. Errorconditions  */
    OSS_SIG_HANDLE  *sigDataHdl;        /* signalhandle f. Data availability*/

	OSS_SEM_HANDLE	*devSemHdl;			/* device semaphore 				*/

 	/* debug */
    u_int32    	 	dbgLevel;			/* debug level 						*/
	DBG_HANDLE  	*dbgHdl;   			/* debug handle 					*/

	u_int32			realMsec1;			/* true Alarm Timer Periods 		*/
	u_int32			realMsec2;
	u_int32			doBusmaster;		/* emit Master frames */
	u_int32			noWdog;				/* 0: use intern Wdog, 1: DONT use  */
	u_int16			mvbIMR0DefaultMask;	/* default IRQ enable mask,IMR0		*/
	u_int16			mvbIMR1DefaultMask;	/* default IRQ enable mask,IMR0		*/

	OSS_ALARM_HANDLE *ossAlarmTI2Hdl;	/* for alive check (Timer2) 		*/
	u_int32			isActiveAlarmTI2;	/* helper to remember TI2 state		*/


	/* misc, device specific */
    u_int32     	irqCount; 			/* interrupt counter 				*/
    u_int32     	mvbMcm;				/* MCM we run in					*/
	u_int16			regOff;				/* offset betw. ma and Service Area */
	u_int32			mvbUseFilter;		/* bit CONF[FILTER] , 0 or 1 		*/
	u_int32			mvbDefaultTimeout;	/* Timeout for Slaveframe occurance	*/
	u_int32			mvbDefaultWaitstate;/* TM Waitstates for MVBCS1 		*/
	u_int16			mvbWDRetriggerTime;	/* Watchdog retrigger interval		*/

	MVB_TM_MAP		tmMap;				/* Traffic Memory mapping			*/
	OSS_DL_LIST 	la_port_list;		/* anchor for LA Port DL list		*/
	u_int32			hasNsdb;			/* Nonzero if a NSDB was passed		*/

	/* LUT for port node entries */
	MVB_LA_PORT_NODE  *la_prt_nodeP[MVB_LA_PORTS_MAX];

	/* --- error management --- */
	u_int32			mvbErrFlags;		/* collected Error Bits 			*/
    u_int32     	mvbErrFramesA;		/* # of errornous Frames, line A	*/
    u_int32     	mvbErrFramesB;		/* # of errornous Frames, line B	*/
	u_int32     	mvbErrFramesSum;	/* sum of errornous Frames 			*/
	u_int32			mvbErrMasterFrames;	/* # of Err. Master Frames recvd	*/
	u_int32			mvbErrSlaveFrames;	/* # of err. slave Frames recvd 	*/
	u_int32			mvbErrDupMaster;	/* # of duplicate Master Frames 	*/
	u_int32			mvbErrDupSlave;		/* # of duplicate slave Frames		*/
	u_int32			mvbErrFramesLost;	/* # of lost Frames					*/
	u_int32			mvbErrReplyTimeout;	/* # of reply timeouts occured		*/
	u_int32			mvbErrBusTimeout;	/* # of MVBus timeouts occured		*/
	u_int32			mvbErrAsicHang;		/* MVBCS1 hang detected 			*/
	u_int32			mvbDoSinktimeSupervision;/*!=0 if Sinktime Superv. is on*/

} LL_HANDLE;


#endif /* _PP04_DRV_INT_H_ */
