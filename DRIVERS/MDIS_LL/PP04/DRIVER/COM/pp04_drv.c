/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  pp04_drv.c
 *
 *      \author  thomas.schnuerer@men.de
 *        $Date: 2013/05/16 17:46:43 $
 *    $Revision: 1.9 $
 *
 *      \brief   Low-level driver for PP04 modules, supporting Slave Operation
 *
 *     Required: OSS, DESC, DBG libraries
 *
 *     \switches 
 */
 /*
 *---------------------------------------------------------------------------
 * (c) Copyright 2004 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
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

#define _NO_LL_HANDLE		/* ll_defs.h: don't define LL_HANDLE struct */
#include <MEN/men_typs.h>   /* system dependent definitions  		*/
#include <MEN/mdis_err.h>   /* MDIS error codes               		*/
#include <MEN/maccess.h>    /* hw access macros and types     		*/
#include <MEN/mdis_api.h>   /* MDIS global defs               		*/
#ifdef PP04_MDIS_UNITTEST
# include <stdarg.h>		
# include <stdio.h>
# include <stdlib.h>
#else 
# include <MEN/dbg.h>       /* debug functions                		*/
# include <MEN/oss.h>       /* oss functions                  		*/
# include <MEN/desc.h>      /* descriptor functions           		*/
#endif
#include "pp04_drv_int.h"   /* intern driver stuff,not for Apps		*/
#include <MEN/ll_defs.h>    /* low-level driver definitions   		*/
#include <MEN/ll_entry.h>  	/* low-level driver branch table */
#include <MEN/mvb_drv.h>	/* external header, NSDB ports dekl.	*/

/*---------------------------+
 |  DEFINES                  |
 +---------------------------*/

/* for testing only! no full busmaster functionality */
#define MVB_MASTER_FRAME_COUNT		7
#define MVB_SMF_IMMEDIATE			1
#define MVB_SMF_TIMER1_CONTROLLED	2
#define MAS_MR_INITVAL ((MVB_SMF_TIMER1_CONTROLLED<<6)|MVB_MASTER_FRAME_COUNT)

/* care for Masterframe sending Timer1 to be 0x0000 every 1 ms */
#define MVB_MAS_TIMEBASE 	(99)  /* formula: (n+1)*10 us */

/* Bus admin Master Frame generation stuff */
#define MVB_FCODE_0			0	/* 2byte LA port */
#define MVB_FCODE_1			1 	/* 4byte LA port */
#define MVB_FCODE_2			2	/* 8byte LA port */
#define MVB_FCODE_3			3	/* 16byte LA port */
#define MVB_FCODE_4			4	/* 32byte LA port */

/* location of MF Table in TM. Remember this value in DPR is << by 2 ! also
 * check MO[1..0]. The Data Sheet states "The MF Tables may be aligned at
 * any location in TM, but care must be taken not to overwrite any used
 * section, e.g. LA PCS".
 */
#define MVB_MFT_START	(0x7000) 

#ifdef PP04_MDIS_UNITTEST
# define OSS_DBG_DEFAULT    0xc0008000
# undef DBGINIT
# define DBGINIT(_x_)
# undef DBGEXIT
# define DBGEXIT(_x_)
# undef DBH
# define DBH			stdout
# define OSH			(OSS_HANDLE*)MVB_NUM_1	/* just a dummy here */
#else
# define DBG_MYLEVEL	llHdl->dbgLevel
# define DBH			llHdl->dbgHdl
# define OSH			llHdl->osHdl
#endif

/* byte ordering check */
#if defined(_BIG_ENDIAN_) && defined(_LITTLE_ENDIAN_)
# error "Byteorder collide, dont define _BIG_ENDIAN_ and _LITTLE_ENDIAN_ both"
#endif
#if !defined(_BIG_ENDIAN_) && !defined(_LITTLE_ENDIAN_)
# error "Byte ordering unknown, please define _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif

/* check passed Parameters in all Functions */
#define CHKPARM(expression)\
 if(!(expression)) {\
	  DBGWRT_ERR((DBH, "\n*** wrong Parameter: %s\nfile %s\nline %d\n",\
                         #expression, __FILE__ , __LINE__  ));\
      goto ABORT;\
 }

/* calculate true TM data address of LA portdata from Port index PI */
#define PI2TMADDR(p_idx) 	(((p_idx & 0xfffc)*16)+((p_idx & 0x3)*8))
#define ADDRESS_IS_ODD(adr) (adr & 0x1)

/*-------------------------------------+
 |  TYPEDEFS                           |
 +-------------------------------------*/

/* none */

/*-------------------------------------+
 |  PROTOTYPES                         |
 +-------------------------------------*/

static int32 PP04_Init( DESC_SPEC *descSpec, OSS_HANDLE *osHdl,
						MACCESS *ma, OSS_SEM_HANDLE *devSemHdl,
						OSS_IRQ_HANDLE *irqHdl, LL_HANDLE **llHdlP);

static int32 PP04_Exit(	LL_HANDLE **llHdlP );
static int32 PP04_Read(	LL_HANDLE *llHdl, int32 ch, int32 *value);
static int32 PP04_Write( LL_HANDLE *llHdl, int32 ch, int32 value);
static int32 PP04_SetStat( LL_HANDLE *llHdl, int32 code, int32 ch, INT32_OR_64 value32_or_64);
static int32 PP04_GetStat( LL_HANDLE *llHdl, int32 code, int32 ch, INT32_OR_64 *value32_or_64);
static int32 PP04_BlockRead( LL_HANDLE *llHdl, int32 ch, void *buf, int32 size, int32 *nbrRdBytesP);
static int32 PP04_BlockWrite(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size, int32 *nbrWrBytesP);
static int32 PP04_Irq( LL_HANDLE *llHdl );
static int32 PP04_Info(int32 infoType, ... );
static char* Ident( void );
static int32 Cleanup( LL_HANDLE *llHdl, int32 retCode );

/* local helpers */
static int32 MVB_EnableIrq(LL_HANDLE *llHdl, u_int16 irqFlag, u_int32 imrReg,
						   u_int8 en );
static int32 MVB_GetFrameErrorCount(LL_HANDLE *llHdl);
static int32 MVB_SetInitializationLevel( LL_HANDLE *llHdl , u_int8 lvl );
static int32 MVB_SAoffsetFromMCM(u_int8 mcm);
static int32 MVB_ShutdownLineAB(LL_HANDLE *llHdl, u_int8 en, u_int8 line);
static int32 MVB_FormatTM(LL_HANDLE *llHdl);
static int32 MVB_SetupNSDB( LL_HANDLE *llHdl, MVB_LA_PORT *nsdbP,
							u_int16 nrPorts);
static int32 MVB_PortInit( LL_HANDLE *llHdl, MVB_LA_PORT_NODE *portP );
static int32 MVB_PerformDiagnose(LL_HANDLE *llHdl );
static int32 MVB_SetupSinktimeSupervision(LL_HANDLE *llHdl ,
										  u_int16 interval);
static void  MVB_ResetErrorCount(LL_HANDLE *llHdl );
static int32 MVB_GetSinktime(LL_HANDLE *llHdl, int32 ch);
static int32 MVB_SetDeviceStatusword(LL_HANDLE *llHdl, int32 value );

#if 0
#ifdef _LL_DRV_
#ifndef _ONE_NAMESPACE_PER_DRIVER_
#	ifdef ID_SW
#		define  PP04_GetEntry  PP04_SW_GetEntry
#	endif
	void PP04_GetEntry(LL_ENTRY* drvP);
#else
	void LL_GetEntry(LL_ENTRY* drvP);
#endif
#endif /* _LL_DRV_ */

#endif

/************************************************************************/
/** enable/disable one IRQ in IMR0 or IMR1
 *
 * 	Enable IRQ given as the only set bit in a 32bit value. see pp04_drv.h
 *
 *  \param llHdl	\IN Pointer to low-level driver handle
 *  \param irqFlag	\IN IRQ bit to set/clear in IMR0 / IMR1
 *  \param imrReg	\IN IMR0 or IMR1 (IRQ flags are spread over 2 Registers)
 *  \param en		\IN 1 to enable the IRQ, 0 to disable it
 *
 *  \return         \c 0 On success or error code
 */
static int32 MVB_EnableIrq(LL_HANDLE *llHdl, 
						   u_int16 irqFlag,
						   u_int32 imrReg,
						   u_int8 en )
{

    DBGWRT_1((DBH, "MVB_EnableIrq: irqFlag 0x%04x imrReg=0x%08x en=%d\n",
			  irqFlag, (u_int32)imrReg, en));

	CHKPARM((imrReg==MVB_REG_IMR0) || (imrReg==MVB_REG_IMR1));
	CHKPARM( irqFlag );

	if (en) {
		MWRITE_D16( llHdl->ma, llHdl->regOff + imrReg, irqFlag);
	}
	else 
	{
		MWRITE_D16( llHdl->ma, llHdl->regOff + imrReg, MVB_NUM_0 );
	}
	
	return ERR_SUCCESS;

 ABORT:
	return ERR_LL_ILL_PARAM;
}




/****************************************************************************/
/** Format the port index tables, depending on MCM mode. Must be
 *  called after TM area offsets have been initialized.
 *
 *  \param llHdl	\IN Pointer to low-level driver handle
 *
 *  \return         \c 0 On success or error code
 */
static int32 MVB_FormatTM(LL_HANDLE *llHdl)
{

	DBGWRT_1((DBH, "MVB_FormatTM\n"));
	CHKPARM(llHdl->tmMap.laPitLen );

	if ( llHdl->mvbMcm > MVB_MCM_MODE_16K ){
		/* no DA ports in 16k mode MCM0 available */
		CHKPARM( llHdl->tmMap.daPitLen	);
		CHKPARM( llHdl->tmMap.daPitStart );

 		/* clear DA PIT */
		MBLOCK_SET_D16(llHdl->ma, llHdl->tmMap.daPitStart,
					   llHdl->tmMap.daPitLen, MVB_NUM_0 );
		/* clear DA PCS Area */
		MBLOCK_SET_D16(llHdl->ma, llHdl->tmMap.daPcsStart,
					   llHdl->tmMap.daPcsLen, MVB_NUM_0 );
	}

 	/* clear LA PIT */
	MBLOCK_SET_D16( llHdl->ma, llHdl->tmMap.laPitStart,
				    llHdl->tmMap.laPitLen, MVB_NUM_0 );
	/* clear LA PCS Area */
	MBLOCK_SET_D16( llHdl->ma, llHdl->tmMap.laPcsStart,
				    llHdl->tmMap.laPcsLen, MVB_NUM_0 );
	/* set LA DAT Area to an initial Value */
	MBLOCK_SET_D16( llHdl->ma, llHdl->tmMap.laDatStart,
				    llHdl->tmMap.laDatLen, MVB_NUM_0 );
	/* clear service Area with physical ports*/
	MBLOCK_SET_D16( llHdl->ma, llHdl->tmMap.saStart,
				    MVB_SA_LENGTH, MVB_NUM_0 );

	return ERR_SUCCESS;

 ABORT:
	return ERR_LL_ILL_PARAM;

}


/****************************************************************************/
/** return offset between llHdl->ma and the internal Registers (Service Area)
 *
 *
 *  \param mcm 	   \IN Memory Configuration Mode
 *
 *  \return        \c Register offset or errorcode if wrong Parameter
 *
 *  \brief			The offset of the intern Registers depends on the 
 *					Memory Configuration Mode MCM. Once MCM Register is set, 
 *					its value shouldnt be changed.
 */
static int32 MVB_SAoffsetFromMCM(u_int8 mcm)
{

	switch(mcm){
	case MVB_MCM_MODE_16K:	
		return MVB_SA_OFFS_3000;
		break;		/* satisfy codewizard */
	case MVB_MCM_MODE_32K:	
		return MVB_SA_OFFS_7000;
		break;
	case MVB_MCM_MODE_64K:
	case MVB_MCM_MODE_256K:
	case MVB_MCM_MODE_1M:
		return MVB_SA_OFFS_F000;
		break;

	default:
		return ERR_LL_ILL_PARAM;
	}
}


/****************************************************************************/
/** set the User servicable bits in the device status word
 *
 *  \param llHdl	\IN Pointer to low-level driver handle
 *  \param value	\IN Bit Mask from which the 3 bits are evaluated
 *
 *  \return         \c Register offset , or 0 if wrong Parameter
 *	
 *	\brief        The function sets the bits in the Device Status Word which
 *                shall be set by the user.
 *                These are:        Device Not Ready  DNR
 *                             Some Device Disturbed  SDD
 *                             Some System Disturbed  SSD
 */
static int32 MVB_SetDeviceStatusword(LL_HANDLE *llHdl, int32 value )
{

	u_int16 dsw;
	/* only modify the mentioned 3 bits */
	u_int16	internMask = (u_int16)((LC_DSW_DNR|LC_DSW_SDD|LC_DSW_SSD) & value);

	CHKPARM(llHdl);

    DBGWRT_1((DBH, "MVB_SetDeviceStatusword: dsw=0x%04x\n", value));
	dsw = MREAD_D16(llHdl->ma, llHdl->tmMap.saStart + MVB_PP_DAT_FC15 );
    DBGWRT_3((DBH, " read FC15 phys. port: 0x%04x\n", dsw));

	dsw &=~(LC_DSW_DNR|LC_DSW_SDD|LC_DSW_SSD);
	dsw |=internMask;	

    DBGWRT_3((DBH, " write FC15 phys. port: 0x%04x\n", dsw));
	MWRITE_D16(llHdl->ma, llHdl->tmMap.saStart + MVB_PP_DAT_FC15,dsw  );

	return ERR_SUCCESS;
 ABORT:
	return ERR_LL_ILL_PARAM;
}


/****************************************************************************/
/** set the Memory configuration mode
 *
 *  \param llHdl	\IN Pointer to low-level driver handle
 *
 *  \return         \c Register offset or 0 if wrong Parameter
 *
 *  \brief  The offset of the intern Registers depends on the Memory 
 *			Configuration Mode MCM. Once MCM is set, its value shouldnt be 
 *			changed anymore. 
 */
static u_int32 MVB_SetupMemConfiguration(LL_HANDLE *llHdl )
{

	u_int16	scr;

	CHKPARM(llHdl);
	CHKPARM(llHdl->mvbMcm <= MVB_MCM_MODE_1M);

    DBGWRT_1((DBH, "MVB_SetupMemConfiguration\n"));

	/*!
	 *	After powerup the MCMode is unknown. So set the 
	 *	Memory Configuration Mode in all 3 possible Register offsets first.
	 *	This is also done with initial Initialization level IL[2..0]
	 */

	/* Q0 and MO not used (yet) */
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000 + MVB_REG_MCR, llHdl->mvbMcm );
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_7000 + MVB_REG_MCR, llHdl->mvbMcm );
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_F000 + MVB_REG_MCR, llHdl->mvbMcm );

	/* set Arbitration Strategy in SCR */
	scr = MREAD_D16(llHdl->ma,llHdl->regOff+ MVB_REG_SCR) & MVB_CLEAR_ARB_MASK;
	/* give MVBCS1 access priority over CPU */
	scr |= MVB_ARB_ASIC_PRIO;

	MWRITE_D16( llHdl->ma, llHdl->regOff + MVB_REG_SCR, scr);

	return ERR_SUCCESS;
 ABORT:
	return ERR_LL_ILL_PARAM;

}




/*****************************************************************************/
/** Clear all error count variables / flags of this LL Handle
 *  
 *  \param llHdl  \IN  Pointer to low-level driver handle
 *
 *  \return        - 
 *
 *	
 */
static void MVB_ResetErrorCount(LL_HANDLE *llHdl )
{

	llHdl->mvbErrFlags			= MVB_NUM_0;
	llHdl->mvbErrFramesSum		= MVB_NUM_0;
	llHdl->mvbErrMasterFrames	= MVB_NUM_0;
	llHdl->mvbErrSlaveFrames	= MVB_NUM_0;
	llHdl->mvbErrDupMaster		= MVB_NUM_0;
	llHdl->mvbErrDupSlave		= MVB_NUM_0;
	llHdl->mvbErrFramesLost		= MVB_NUM_0;
	llHdl->mvbErrReplyTimeout	= MVB_NUM_0;
	llHdl->mvbErrBusTimeout		= MVB_NUM_0;
	llHdl->mvbErrAsicHang		= MVB_NUM_0;

}





/*****************************************************************************/
/** return F code depending on size
 *  
 *  \param size    \IN  size of this port in byte (either 2,4,8,16,32 byte)
 *
 *  \return        \c 	F-code on success or error code\
 *	
 *	\brief To program a LA ports PCS Block with the correct F code
 *		   its size needs to be known so the F code in the Master frame 
 *		   retrieves the correct number of bits
 */
static u_int8 MVB_FcodeFromSize(u_int16 size)
{

	u_int8 retval = MVB_NUM_FF;

	switch(size){
 	case MVB_LA_SIZE_2:
		retval = MVB_NUM_0;
		break;
	case MVB_LA_SIZE_4:
		retval = MVB_NUM_1;
		break;
	case MVB_LA_SIZE_8:
		retval = MVB_NUM_2;
		break;
	case MVB_LA_SIZE_16:
		retval = MVB_NUM_3;
		break;
	case MVB_LA_SIZE_32:
		retval = MVB_NUM_4;
		break;
	default:
		retval = MVB_NUM_FF;
	}

	return retval;

}



/*****************************************************************************/
/** init Memory Maps on MCM mode
 *
 *  \param llHdl  \IN  Pointer to low-level driver handle
 *
 *  \return        \c 	0 on success or error code
 *
 *  \brief 	The memory start and lengths of each area in the memory map
 *			are initialized according to MCM0-4 as in datasheet MVBCS1 p. 27
 */
static u_int32 MVB_InitTMoffsets( LL_HANDLE *llHdl )
{

	CHKPARM(llHdl);
	CHKPARM(llHdl->mvbMcm <= MVB_MCM_MODE_1M);

	switch( llHdl->mvbMcm ){
	case MVB_MCM_MODE_16K:
		llHdl->tmMap.laPitStart		=	MVB_MC0_LPS;
		llHdl->tmMap.laPitLen		=	MVB_MC0_LPL;
		llHdl->tmMap.laPcsStart		=	MVB_MC0_LCS;
		llHdl->tmMap.laPcsLen		=	MVB_MC0_LCL;
		llHdl->tmMap.laDatStart		=	MVB_MC0_LDS;
		llHdl->tmMap.laDatLen		=	MVB_MC0_LDL;
												   
		llHdl->tmMap.daPitStart		=	MVB_MC0_DPS;
		llHdl->tmMap.daPitLen		=	MVB_MC0_DPL;
		llHdl->tmMap.daPcsStart		=	MVB_MC0_DCS;
		llHdl->tmMap.daPcsLen		=	MVB_MC0_DCL;
		llHdl->tmMap.daDatStart		=	MVB_MC0_DDS;
		llHdl->tmMap.daDatLen		=	MVB_MC0_DDL;
		llHdl->tmMap.saStart		=	MVB_MC0_SAS;
		break;

	case MVB_MCM_MODE_32K:	/* MCM1 */
		llHdl->tmMap.laPitStart		=	MVB_MC1_LPS;
		llHdl->tmMap.laPitLen		=	MVB_MC1_LPL;
		llHdl->tmMap.laPcsStart		=	MVB_MC1_LCS;
		llHdl->tmMap.laPcsLen		=	MVB_MC1_LCL;
		llHdl->tmMap.laDatStart		=	MVB_MC1_LDS;
		llHdl->tmMap.laDatLen		=	MVB_MC1_LDL;
												   
		llHdl->tmMap.daPitStart		=	MVB_MC1_DPS;
		llHdl->tmMap.daPitLen		=	MVB_MC1_DPL;
		llHdl->tmMap.daPcsStart		=	MVB_MC1_DCS;
		llHdl->tmMap.daPcsLen		=	MVB_MC1_DCL;
		llHdl->tmMap.daDatStart		=	MVB_MC1_DDS;
		llHdl->tmMap.daDatLen		=	MVB_MC1_DDL;
		llHdl->tmMap.saStart		=	MVB_MC1_SAS;
		break;

	case MVB_MCM_MODE_64K:	/* MCM2 */
		llHdl->tmMap.laPitStart		=	MVB_MC2_LPS;
		llHdl->tmMap.laPitLen		=	MVB_MC2_LPL;
		llHdl->tmMap.laPcsStart		=	MVB_MC2_LCS;
		llHdl->tmMap.laPcsLen		=	MVB_MC2_LCL;
		llHdl->tmMap.laDatStart		=	MVB_MC2_LDS;
		llHdl->tmMap.laDatLen		=	MVB_MC2_LDL;
												   
		llHdl->tmMap.daPitStart		=	MVB_MC2_DPS;
		llHdl->tmMap.daPitLen		=	MVB_MC2_DPL;
		llHdl->tmMap.daPcsStart		=	MVB_MC2_DCS;
		llHdl->tmMap.daPcsLen		=	MVB_MC2_DCL;
		llHdl->tmMap.daDatStart		=	MVB_MC2_DDS;
		llHdl->tmMap.daDatLen		=	MVB_MC2_DDL;
		llHdl->tmMap.saStart		=	MVB_MC2_SAS;
		break;

	case MVB_MCM_MODE_256K:	/* MCM3 */
		llHdl->tmMap.laPitStart		=	MVB_MC3_LPS;
		llHdl->tmMap.laPitLen		=	MVB_MC3_LPL;
		llHdl->tmMap.laPcsStart		=	MVB_MC3_LCS;
		llHdl->tmMap.laPcsLen		=	MVB_MC3_LCL;
		llHdl->tmMap.laDatStart		=	MVB_MC3_LDS;
		llHdl->tmMap.laDatLen		=	MVB_MC3_LDL;
												   
		llHdl->tmMap.daPitStart		=	MVB_MC3_DPS;
		llHdl->tmMap.daPitLen		=	MVB_MC3_DPL;
		llHdl->tmMap.daPcsStart		=	MVB_MC3_DCS;
		llHdl->tmMap.daPcsLen		=	MVB_MC3_DCL;
		llHdl->tmMap.daDatStart		=	MVB_MC3_DDS;
		llHdl->tmMap.daDatLen		=	MVB_MC3_DDL;
		llHdl->tmMap.saStart		=	MVB_MC3_SAS;
		break;

	case MVB_MCM_MODE_1M: /* MCM4 */
		llHdl->tmMap.laPitStart		=	MVB_MC4_LPS;
		llHdl->tmMap.laPitLen		=	MVB_MC4_LPL;
		llHdl->tmMap.laPcsStart		=	MVB_MC4_LCS;
		llHdl->tmMap.laPcsLen		=	MVB_MC4_LCL;
		llHdl->tmMap.laDatStart		=	MVB_MC4_LDS;
		llHdl->tmMap.laDatLen		=	MVB_MC4_LDL;
		
		llHdl->tmMap.daPitStart		=	MVB_MC4_DPS;
		llHdl->tmMap.daPitLen		=	MVB_MC4_DPL;
		llHdl->tmMap.daPcsStart		=	MVB_MC4_DCS;
		llHdl->tmMap.daPcsLen		=	MVB_MC4_DCL;
		llHdl->tmMap.daDatStart		=	MVB_MC4_DDS;
		llHdl->tmMap.daDatLen		=	MVB_MC4_DDL;
		llHdl->tmMap.saStart		=	MVB_MC4_SAS;
		break;

	default:
		return ERR_LL_ILL_PARAM;
	}
	
	DBGWRT_3((DBH, "assigned Mem Mapping (MCM%ld):\n",llHdl->mvbMcm ));
	DBGWRT_3((DBH, "laPitStart 0x%08lx\n", llHdl->tmMap.laPitStart	));
	DBGWRT_3((DBH, "laPitLen   0x%08lx\n", llHdl->tmMap.laPitLen	));
	DBGWRT_3((DBH, "laPcsStart 0x%08lx\n", llHdl->tmMap.laPcsStart	));
	DBGWRT_3((DBH, "laPcsLen   0x%08lx\n", llHdl->tmMap.laPcsLen	));
	DBGWRT_3((DBH, "laDatStart 0x%08lx\n", llHdl->tmMap.laDatStart	));
	DBGWRT_3((DBH, "laDatLen   0x%08lx\n", llHdl->tmMap.laDatLen	));
	DBGWRT_3((DBH, "daPitStart 0x%08lx\n", llHdl->tmMap.daPitStart	));
	DBGWRT_3((DBH, "daPitLen   0x%08lx\n", llHdl->tmMap.daPitLen	));
	DBGWRT_3((DBH, "daPcsStart 0x%08lx\n", llHdl->tmMap.daPcsStart	));
	DBGWRT_3((DBH, "daPcsLen   0x%08lx\n", llHdl->tmMap.daPcsLen	));
	DBGWRT_3((DBH, "daDatStart 0x%08lx\n", llHdl->tmMap.daDatStart	));
	DBGWRT_3((DBH, "daDatLen   0x%08lx\n", llHdl->tmMap.daDatLen	));

	return(ERR_SUCCESS);

 ABORT:
	return ERR_LL_ILL_PARAM;
}


/*****************************************************************************/
/** Do the complete initialization of PCS and PIT for this Port so it can
 *	be used when ASIC is put in full functional mode (IL=3)
 *
 *  \param llHdl   \IN  Pointer to low-level driver handle
 *  \param portP   \IN  pointer to definition struct for this LA Port
 *
 *  \return        \c 	0 on success or error code
 */
static int32 MVB_PortInit( LL_HANDLE *llHdl, MVB_LA_PORT_NODE *portP )
{

/*!
 * \code
 *	Mapping of Portindex PI to TM data Area looks like this:
 *
 * 	bit	|1|1| | | | | | | | |V| | |       Port Index PI
 * 		|1|0|9|8|7|6|5|4|3|2|P|1|0|       (VP = valid page pointer bit)
 * 
 * 	bit	|1|1|1|1|1|1| | | | | | | | | | | TM data Area LA DAT (max. 64k)
 * 		|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0| 
 *
 * => resulting algorithm: TMad=(( PI & 0xfffc ) << 4) + (PI & 0x3) << 3
 * Missing TM bit 5 is the VP page pointer toggled by MVBCS1. 
 * \endcode
 */

	u_int16 pitWord=MVB_NUM_0, pcs0val=MVB_NUM_0;
	u_int16	paddr, pindex;
	u_int8	fcode, srcsnk;
	void* pitstartP  = NULL;

	CHKPARM(portP);
	CHKPARM(llHdl);

	DBGWRT_1((DBH, "MVB_PortInit: port addr %d port index 0x%04x\n", 
			  portP->addr, portP->index ));

	/*--------------------------+
	 | set ports PIT entry 		|
	 +-------------------------*/

	pitstartP 	= (u_int16*)( ((INT32_OR_64)(llHdl->ma)) + llHdl->tmMap.laPitStart);
	paddr 		= portP->addr;
	pindex 		= portP->index;
	switch (llHdl->mvbMcm){
		/* in MCM0, MCM1 PIT contains 2 ports/word. Attention: mind swapping */
	case MVB_MCM_MODE_16K: 
	case MVB_MCM_MODE_32K:
        if (ADDRESS_IS_ODD(paddr)){
            pitWord = MREAD_D16( (int16*)pitstartP , (paddr & MVB_ADDR_ODD_MASK ));
			DBGWRT_2((DBH, "odd addr: read  pitWord 0x%04x\n", pitWord ));
            pitWord &= MVB_HIBYTE_WORD_MASK /* 0x00FF */;
            pitWord |= pindex/*  << MVB_NUM_8  */;
			DBGWRT_2((DBH, "odd addr: write pitWord 0x%04x\n", pitWord ));

            MWRITE_D16( pitstartP, (paddr & MVB_ADDR_ODD_MASK), pitWord);
		} else {
			pitWord = MREAD_D16( (int16*)pitstartP,  paddr  );
			DBGWRT_2((DBH, "even addr: read  pitWord 0x%04x\n", pitWord ));
			pitWord &= MVB_LOBYTE_WORD_MASK/* 0xFF00 */;
			pitWord |= pindex << MVB_NUM_8;
			DBGWRT_2((DBH, "even addr: write pitWord 0x%04x\n", pitWord ));
			MWRITE_D16( (int16*)pitstartP, paddr , pitWord );
		}

		break;
	case MVB_MCM_MODE_64K:
	case MVB_MCM_MODE_256K:
	case MVB_MCM_MODE_1M:
		/* all other modes maintain 1 Port per LA PIT address */
		pitWord = portP->index;
		MWRITE_D16(pitstartP, portP->addr<< MVB_NUM_1, pitWord );
		break;
	default:
		return ERR_LL_ILL_PARAM;
	}

	/*--------------------------+
	 | PCS info for this Port	|
	 +--------------------------*/

	/* F-code according to size */
	fcode = (u_int8)(MVB_FcodeFromSize(portP->size));
	if (fcode > MVB_NUM_4 ) {
		DBGWRT_ERR((DBH, "*** error: Port size mismatch! portP->size=%d\n", 
					portP->size ));
		return (ERR_LL_ILL_PARAM);
	}

	/* source/sink 0b10 = active source, 0b01 = active sink*/
	srcsnk = (u_int8)(portP->srcsink);
	pcs0val = (u_int16)((fcode << MVB_NUM_12) | (srcsnk << MVB_NUM_10));

	/* if its a sink, init TACK in PCS[2] and set compare mode if selected*/
	if (portP->srcsink == MVB_LA_PORT_SINK){
		MWRITE_D16( portP->pcs, MVB_NUM_4, MVB_NUM_FFFF);
		/* if compare mode was set for this Sink then set IEn=7 in PCS0 */
		if (portP->compare){
			pcs0val |= MVB_PCS0_COMPARE_MODE;
		}
	}

	DBGWRT_3((DBH, "set PCS[0] = 0x%04x\n", pcs0val ));
	MWRITE_D16( portP->pcs, MVB_NUM_0, pcs0val);

	return ERR_SUCCESS;

 ABORT:

	return ERR_LL_ILL_PARAM;

}


/*****************************************************************************/
/** setup Sinktime Supervision for given Range of ports
 *
 *  \param llHdl    \IN  Pointer to low-level driver handle
 *  \param interval \IN  sinktime interval as described in STSR.
 *
 *  \brief  the values passed to the Function go directly to the Sinktime
 *			Supervision Register. This LL driver per default monitors all
 *			(sink-)docks available in its MCM. The interval is corrected if
 *			too small for this MCM.
 *
 *  \return        \c 	0 on success or error code
 */
static int32 MVB_SetupSinktimeSupervision(LL_HANDLE *llHdl ,
										  u_int16 interval)
{

	u_int16	docks=0;
	u_int16 ssval=0;

	CHKPARM(llHdl);
	CHKPARM( interval<=MVB_STS_TIME_256MS );

	/* 
	 * monitor all available docks in this MCM per default. Care must be taken
	 * not to use smaller intervals than recommended by MVBCS1 data sheet.
	 * Formula for Sinktime intervals on p.91 results in the following minima:
	 * 
	 * MCM0: 2(2ms)	MCM1: 2(2ms)  MCM2: 4(8ms)  MCM3: 6(32ms) MCM4: 6(32ms)
	 */	

	if ((llHdl->mvbMcm==MVB_MCM_MODE_16K)||(llHdl->mvbMcm==MVB_MCM_MODE_32K)){
		if (interval && (interval < MVB_NUM_2))
			interval = MVB_NUM_2;
		docks = MVB_LA_DOCKS_256;
	}

	if (llHdl->mvbMcm==MVB_MCM_MODE_64K){
		if (interval && (interval < MVB_NUM_4))
			interval = MVB_NUM_4;
		docks = MVB_LA_DOCKS_1024;
	}

	if ((llHdl->mvbMcm==MVB_MCM_MODE_256K)||(llHdl->mvbMcm==MVB_MCM_MODE_1M)){
		if (interval && (interval < MVB_NUM_6))
			interval = MVB_NUM_6;
		docks = MVB_LA_DOCKS_4096;
	}
	/*
	 * Sinktime Supervision Count of sinks runs only if the Interval specified
	 * in MVB_REG_STSR	is not zero.
	 */
	llHdl->mvbDoSinktimeSupervision = (interval != MVB_STS_TIME_OFF);

	DBGWRT_2((DBH, "MVB_SetupSinktimeSupervision: interval=%d\n", interval ));
	ssval = (u_int16)((interval & MVB_NUM_F) << MVB_STSR_IVAL) | docks;

	DBGWRT_2((DBH, "MVB_SetupSinktimeSupervision: set STSR=0x%04x\n", ssval));
	MWRITE_D16( llHdl->ma, llHdl->regOff + MVB_REG_STSR, ssval );

	return ERR_SUCCESS;
 ABORT:
	return ERR_LL_ILL_PARAM;

}



/*****************************************************************************/
/** set the MVB dev address this PP04 shall use
 *
 *  \param llHdl  \IN  Pointer to low-level driver handle
 *  \param addr    \IN  address to set the PP04 to
 *
 *  \return        \c 	0 on success or error code
 */
static u_int32 MVB_ConfigDeviceAddress( LL_HANDLE *llHdl , u_int16 addr )
{

	DBGWRT_1((DBH, "MVB_ConfigDeviceAddress: addr = %d\n", addr ));

	CHKPARM(llHdl);
	CHKPARM((addr<MVB_MAX_ADDR) && (addr));

	/*
	 * setting the device address includes writing it to DAOR, and then
	 * enabling it by writing a specific key to DAOK.
	 */

	MWRITE_D16( llHdl->ma, llHdl->regOff + MVB_REG_DAOR, addr);
	MWRITE_D16( llHdl->ma, llHdl->regOff + MVB_REG_DAOK, MVB_DA_KEY_ENABLE );
	return ERR_SUCCESS;

 ABORT:

	return ERR_LL_ILL_PARAM;

}



/*****************************************************************************/
/** Bring MVBCS1 into one of 4 specific operational levels
 *
 *  \param llHdl  \IN  Pointer to low-level driver handle
 *  \param lvl     \IN  initialization level [0..3]
 *
 *  \return        \c 	0 on success or error code
 *
 *	\brief	the MVBCS1 can be in one of these 4 initialization levels:\n
 *       		MVB_INITLEVEL_SW_RESET              warm reset\n
 *       		MVB_INITLEVEL_CONFIG_MODE           needed to setup ports\n
 *		 		MVB_INITLEVEL_SELFTEST_MODE         for loopback tests\n
 *		 		MVB_INITLEVEL_FULL_OP               to start MVB communication
 */
static int32 MVB_SetInitializationLevel( LL_HANDLE *llHdl , u_int8 lvl )
{

	u_int16	scr;	
	int32 err;

	DBGWRT_1((DBH, "MVB_SetInitializationLevel: lvl = %d\n", lvl ));

	CHKPARM(llHdl);
	CHKPARM(lvl <= MVB_INITLEVEL_FULL_OP);
	 
	scr = MREAD_D16( llHdl->ma, llHdl->regOff + MVB_REG_SCR );
	DBGWRT_3((DBH, "   read SCR: 0x%04x\n", scr ));
	scr &= MVB_REG_MCR_MASK;

	switch (lvl){
		
	case MVB_INITLEVEL_SW_RESET:
		scr |=MVB_INITLEVEL_SW_RESET;

		if (llHdl->isActiveAlarmTI2){
			llHdl->isActiveAlarmTI2 = 0;
			OSS_AlarmClear(OSH, llHdl->ossAlarmTI2Hdl );
		}

		MWRITE_D16( llHdl->ma, llHdl->regOff + MVB_REG_SCR, scr);
		break;

	case MVB_INITLEVEL_CONFIG_MODE:
		/* Assume that the MCM isnt set up yet. Therefore write IL[1:0] to 
		 * all 3 possible locations. Value of scr from above is invalid then.
		 * Also, set initial Waitstates and timeout here. 
		 */
		scr = (u_int16)(MVB_INITLEVEL_CONFIG_MODE | \
			llHdl->mvbDefaultWaitstate << MVB_SCR_WS | \
			llHdl->mvbDefaultTimeout << MVB_SCR_TMO);

		DBGWRT_3((DBH, "   write SCR: 0x%04x\n", scr ));
		MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000 + MVB_REG_SCR, scr);
		MWRITE_D16( llHdl->ma, MVB_SA_OFFS_7000 + MVB_REG_SCR, scr);
		MWRITE_D16( llHdl->ma, MVB_SA_OFFS_F000 + MVB_REG_SCR, scr);

		/* stop running Alarms (if going back to Config Mode) */
		if (llHdl->isActiveAlarmTI2) {
			OSS_AlarmClear(OSH, llHdl->ossAlarmTI2Hdl );
			llHdl->isActiveAlarmTI2 = 0;
		}
		break;

	case MVB_INITLEVEL_SELFTEST_MODE:
		break;

	case MVB_INITLEVEL_FULL_OP:
		if (!llHdl->hasNsdb)
			return ERR_LL_ILL_PARAM;

		scr |=MVB_INITLEVEL_FULL_OP;

		/* LL driver monitors Asic by checking its Timer2 */
		if ((err = OSS_AlarmSet(OSH, 
							   llHdl->ossAlarmTI2Hdl,
							   MVB_TI2_CHK_INTERVAL,
							   MVB_OSS_CYCLIC_ALARM,
							   (u_int32*)&llHdl->realMsec2 )))
			return(err);
		llHdl->isActiveAlarmTI2 = 1;

		/* Init Device Status word FC15 with startup values */
	   	MWRITE_D16(llHdl->ma, llHdl->regOff + MVB_PP_PCS_FC15,
				   MVB_FC15|TM_PCS0_SRC|TM_PCS0_NUM);

		/* Init data Area (page 0) of FC15 phys. port */
	   	MWRITE_D16(llHdl->ma, llHdl->regOff + MVB_PP_DAT_FC15,
				   LC_DSW_LAA | LC_DSW_DNR );
		
		if (llHdl->noWdog == 0)
			MWRITE_D16( llHdl->ma, 
						llHdl->regOff + MVB_REG_WDC, 
						MVB_WATCHDOG_TIME);

		/* start Timer 2 to run free */
		MWRITE_D16(llHdl->ma,llHdl->regOff+MVB_REG_TR2, MVB_TR2_RELOAD_VALUE );
		MWRITE_D16(llHdl->ma,llHdl->regOff+MVB_REG_TCR, MVB_TCR_TA2 );

		/* and go... */
		MWRITE_D16( llHdl->ma, llHdl->regOff + MVB_REG_SCR, scr);
		
		break;
	default:
		return ERR_LL_ILL_PARAM;
	}

	return ERR_SUCCESS;
 ABORT:
	return ERR_LL_ILL_PARAM;
}



/*****************************************************************************/
/** retrieve the Sinktime Value of a certain (sink)port
 *
 *  \param llHdl	\IN  Pointer to low-level driver handle
 *  \param ch		\IN  port number previously selected with M_MK_CH_CURRENT
 *
 *  \return         \c 	 Value of word PCS[2] of the Port
 */
static int32 MVB_GetSinktime(LL_HANDLE *llHdl, int32 ch)
{

	u_int16	sinktime;

	MVB_LA_PORT_NODE *prtP = NULL; 

	CHKPARM( llHdl );
	CHKPARM( llHdl->la_prt_nodeP[ch]);

	prtP = llHdl->la_prt_nodeP[ch];

	if (prtP->srcsink == MVB_LA_PORT_SOURCE ){
		DBGWRT_ERR((DBH, "*** error: Port %ld is a source\n", ch));
		return (ERR_LL_ILL_DIR);
	}

	/* PCS[2] is 2 words / 4 bytes offset from pcs[0] */
	sinktime = MREAD_D16( prtP->pcs, MVB_NUM_4 );
	DBGWRT_2((DBH, "Sink Port %d TACK Value = 0x%04x\n",ch, sinktime));
	return sinktime;

 ABORT:
	DBGWRT_ERR((DBH, "*** error: MVB_GetSinktime: wrong Parameters\n"));
	return ERR_LL_ILL_PARAM;

}


/*****************************************************************************/
/** Shutdown or enable  Line A or B of the redundant MVBus connection
 *
 *  \param llHdl	\IN  Pointer to low-level driver handle
 *  \param en		\IN  0 to operate MVB line A, 1 to shut it down
 *  \param line		\IN  0: select MVB line A, 1: select line B
 *
 *  \return        \c 	0 on success or error code
 */
static int32 MVB_ShutdownLineAB(LL_HANDLE *llHdl, u_int8 en, u_int8 line)
{

	DBGWRT_1((DBH, "MVB_ShutdownLineAB: en=%d line=%s\n",en, line ? "B":"A"));

	CHKPARM(llHdl);
	CHKPARM( en  <= MVB_NUM_1);
	CHKPARM( line < MVB_LINE_B);

	switch (line){
	case MVB_LINE_A:
		if (en) {
			MSETMASK_D16(llHdl->ma, llHdl->regOff+MVB_REG_CONF, MVB_CONF_SLA);
		} else {
			MCLRMASK_D16(llHdl->ma, llHdl->regOff+MVB_REG_CONF, MVB_CONF_SLA);
		}
		break;
	case MVB_LINE_B:
		if (en) {
			MSETMASK_D16(llHdl->ma, llHdl->regOff+MVB_REG_CONF, MVB_CONF_SLB);
		}
		else {
			MCLRMASK_D16(llHdl->ma, llHdl->regOff+MVB_REG_CONF, MVB_CONF_SLB);
		}
		break;
	default:
		return ERR_LL_ILL_PARAM;
	}	
	
	return ERR_SUCCESS;

 ABORT:

	return ERR_LL_ILL_PARAM;
}


/*****************************************************************************/
/** Alarm function to check if MVBCS1 ASIC is operating
 *
 *  \param arg   \OUT void *arg pointer, here used for 'our' LL handle
 *
 *	\brief	the LL driver monitors that the MVB Asic is alive by checking if
 *			Timer2 decrements regulary, and by checking that the init
 *			level is still 3 (Full Operation)
 */
static void MVB_Timer2Func(void* arg)
{

	LL_HANDLE *llHdl = (LL_HANDLE*)arg;
	u_int16	ti2, scr;

	if ( llHdl->noWdog == 0 ) {
		/* 1. set TI2 to a known Value*/
		MWRITE_D16(llHdl->ma, llHdl->regOff+ MVB_REG_TC2, MVB_TI2_INITVAL );

		/* 2. Wait 10 us, TI2 decrements by about 80 (T_cnt=125ns) */
		OSS_MikroDelay(OSH, MVB_DELAY_10_US);

		/*
		 * 3. check if Timer2 decremented to some lower value. 
		 *	  Also test SCR Register for IL being set to full operation.
		 */
		ti2 = MREAD_D16( llHdl->ma, llHdl->regOff + MVB_REG_TC2 );
		scr = MREAD_D16( llHdl->ma, llHdl->regOff + MVB_REG_SCR ) & MVB_NUM_3;

		if ( (ti2>=MVB_TI2_INITVAL) || scr != MVB_INITLEVEL_FULL_OP ) {
			DBGWRT_2((DBH, "*** MVB_Timer2Func: MVB_TI2_INITVAL = %d "
					  "Timer2 now %d scr=0x%04x \n", 
					  MVB_TI2_INITVAL, ti2, scr ));
			/* TI2/MVBCS1 hang or reseted somehow -> alert Application */
			llHdl->mvbErrAsicHang++;
			if (llHdl->sigAlertHdl)
				OSS_SigSend(OSH, llHdl->sigAlertHdl);  
		}
	}

}

/*****************************************************************************/
/** retrigger the MVBCS1 Watchdog so the MVBCS1 'knows' the LL driver is alive
 *
 *  \param llHdl	\IN  Pointer to low-level driver handle
 *
 *	\brief	The MVBCS1 Asic monitors the LL driver with his Watchdog. 
 * 			The Watchdog is retriggered by the user application, this should
 *			happen about every 500 ms.\n
 * 			This should give enough overhead time to the CPU in case of e.g.
 * 			time consuming blocking I/O Operations.
 *
 */
static void MVB_WDtriggerFunc(LL_HANDLE* llHdl )
{
	
	DBGWRT_1((DBH, "MVB_WDtriggerFunc\n"));	
	if ( llHdl->noWdog == 0) 
		MWRITE_D16(llHdl->ma, 
				   llHdl->regOff+MVB_REG_WDC, 
				   MVB_WATCHDOG_TIME);
	return;
}


/****************************************************************************/
/** Store passed NSDB Setup in DL_NODE elements.
 *
 *  \param llHdl	\IN  Pointer to low-level driver handle
 *  \param nsdbP 	\IN  pointer to begin of MVB_LA_PORT array (User NSDB data)
 *  \param nrPorts 	\IN  number of ports to set up
 *
 *  \brief to store internal information for the LA Ports, a MVB_LA_PORT_NODE
 *		   is dynamically created. To access each LA port directly later, a LUT
 *         in the llHdl is used to get the ports struct over his port number.
 *
 */
static int32 MVB_SetupNSDB( LL_HANDLE *llHdl, MVB_LA_PORT *nsdbP,
							u_int16 nrPorts)
{

	MVB_LA_PORT_NODE	*prtP 	= 	NULL;
	u_int32				gotsize;
	u_int16				cnt 	= 	nrPorts;
	u_int16				idx;
	INT32_OR_64			mabase;
	u_int16				pi2adr;
	MVB_LA_PORT	*laP 	= nsdbP;

	DBGWRT_1((DBH, "MVB_SetupNSDB: nrPorts=%d\n", nrPorts ));
	CHKPARM( llHdl );
	CHKPARM( nsdbP );
	CHKPARM((nrPorts > 0) && (nrPorts < MVB_LA_PORTS_MAX));

	/* we either have no NSDB yet or setup a new one */
	llHdl->hasNsdb	=	FALSE;

	while ( cnt-- ){
		prtP = (MVB_LA_PORT_NODE *)OSS_MemGet( OSH, sizeof(MVB_LA_PORT_NODE),
											   &gotsize );
   		if (!prtP){
			DBGWRT_ERR((DBH, "*** error: OSS_MemGet failed\n"));
			return(ERR_OSS_MEM_ALLOC);
		}

		OSS_MemFill(OSH, sizeof(MVB_LA_PORT_NODE), (char*)prtP, MVB_NUM_0);
		prtP->addr			=	laP->addr;
		prtP->index			=	laP->index;
		prtP->size			=	laP->size;
		prtP->memAlloc		=	gotsize;
		prtP->srcsink		=	laP->srcsink;
		prtP->tack			=	laP->tack;
		prtP->compare		=	laP->compare;
		OSS_MemCopy(OSH,
					MVB_PORTNAME_LEN-MVB_NUM_1,	
					laP->portname, 
					(char*)prtP->name);

		/* calculate this ports data and port control status Area addresses */ 
		idx 		= prtP->index;
		mabase 		= (INT32_OR_64)(llHdl->ma);
		pi2adr		= (u_int16)(PI2TMADDR(idx));

		prtP->dat 	= mabase + llHdl->tmMap.laDatStart + pi2adr;
	    prtP->pcs 	= mabase + llHdl->tmMap.laPcsStart + (idx*8);

		DBGWRT_3((DBH, "allocated LA port %d:\n", 	prtP->addr 			));
		DBGWRT_3((DBH, "dat:%llx\n", prtP->dat-((INT32_OR_64)llHdl->ma)));
		DBGWRT_3((DBH, "pcs:        %llx\n", prtP->pcs-((INT32_OR_64)llHdl->ma)));
		DBGWRT_3((DBH, "index:      %d\n", 	prtP->index 		));
		DBGWRT_3((DBH, "size:       %d\n", 	prtP->size  		));
		DBGWRT_3((DBH, "identifier: %s\n", 	prtP->name 	 		));
		DBGWRT_3((DBH, "src/sink:   %s\n",
				  (prtP->srcsink == MVB_LA_PORT_SOURCE) ? "SOURCE":"SINK" 	));
		DBGWRT_3((DBH, "tack value: 0x%04x\n", 		prtP->tack 					));
		
		OSS_DL_AddTail( &llHdl->la_port_list , &prtP->node );

		/* store Nodeaddress in LUT for quick access (MDIS channel reads)*/
		llHdl->la_prt_nodeP[prtP->addr] = prtP; 

		/* Ports data copied, set the port up physically in TM */
		MVB_PortInit( llHdl, prtP );
		laP++;
	}

	/* ok, the NSDB has been set up */
	llHdl->hasNsdb	=  TRUE;
	return ERR_SUCCESS;

 ABORT:
	return Cleanup(llHdl, ERR_LL_ILL_PARAM);
}


/*****************************************************************************/
/** Do a Diagnose of the MVBCS1 by internal loopback test
 *
 *  \param llHdl	\IN  Pointer to low-level driver handle
 *
 *  \return        \c 	0 on success or error code
 *
 *	\brief			the physical test port is used in loopback mode and a 
 *					manual Master Frame is triggered. The received Test Sink 
 *					Port Value is tested for equality. \n
 *					Before, the access to a specific MVBCS1	Register is tested.
 */
static int32 MVB_PerformDiagnose(LL_HANDLE *llHdl )
{
 
    u_int16 scr, timeout=0;
	u_int16 adr, dat, dprtest;

	DBGWRT_1((DBH, "MVB_PerformDiagnose\n"));
    /* 1. Reset MVBC completely */
	scr = (u_int16)(MVB_INITLEVEL_SW_RESET | \
		llHdl->mvbDefaultWaitstate << MVB_SCR_WS | \
		llHdl->mvbDefaultTimeout << MVB_SCR_TMO);

	/* the reset has to be issued to all 3 possible SCR Register offsets */
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_F000 + MVB_REG_SCR, scr);
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_7000 + MVB_REG_SCR, scr);
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000 + MVB_REG_SCR, scr);

	/* Now we are definitely in MCM 0 */
	OSS_MikroDelay(OSH, MVB_DELAY_500_US);

	/* 2. set config Mode */
	scr = (u_int16)(MVB_INITLEVEL_CONFIG_MODE | \
		llHdl->mvbDefaultWaitstate << MVB_SCR_WS | \
		llHdl->mvbDefaultTimeout << MVB_SCR_TMO);

	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000 + MVB_REG_SCR, scr);

	OSS_MikroDelay(OSH, MVB_DELAY_10_US);

    /* 3. Test a MVBCS1 characteristic Register: DPR */
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000 + MVB_REG_DPR, MVB_NUM_FFFF);

	OSS_MikroDelay(OSH, MVB_DELAY_10_US);
	/* check that MVBCS1 and not some memory is accessed. 
	   Use 14-bit reg. DPR whose 2 LSBs are tied to 0 */
	dprtest = MREAD_D16(llHdl->ma, MVB_SA_OFFS_3000 + MVB_REG_DPR);
	DBGWRT_1((DBH, "dprtest = 0x%04x \n", dprtest));

	if (  dprtest!= MVB_ADDR_MASK_FFFC ){
		DBGWRT_ERR((DBH, "*** error: MVBCS1 Asic not found "\
					"(0x%04x instead 0xfffc)\n", dprtest));
		return(MVB_ERR_NO_ASIC);
    }

	/*------------------------------+
	 | MVBC loopback test		 	|
	 +------------------------------*/

	/* configure Test source and sink port */
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000 + MVB_PP_PCS_TSRC, 
				MVB_FC1|TM_PCS0_SRC|TM_PCS0_NUM);

	MWRITE_D16( llHdl->ma,MVB_SA_OFFS_3000 + MVB_PP_PCS_TSNK, 
				MVB_FC1|TM_PCS0_SINK|TM_PCS0_NUM );

	/* let test source data be read from TSRC data area page0 */
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000 + MVB_PP_PCS_TSRC + MVB_NUM_2, 
				TM_PCS1_VP0 );

	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000 + MVB_PP_PCS_TSNK + MVB_NUM_2, 
				TM_PCS1_VP0	);

	/* write test word on  PP TSRC page 0*/
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000 + MVB_PP_DAT_TSRC,
				MVB_LOOPBACK_VALUE );

	/* clear both sinkport pages to ensure we really receive our test Value */
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000+MVB_PP_DAT_TSNK+MVB_PP_DAT_PCS1,
				MVB_NUM_0);
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000+MVB_PP_DAT_TSNK, MVB_NUM_0);

	/*------------------------------+
	 | loopback test mode on	 	|
	 +------------------------------*/
	scr = MVB_INITLEVEL_SELFTEST_MODE |\
		MVB_NUM_3 << MVB_SCR_WS | MVB_NUM_1<<MVB_SCR_TMO | TM_SCR_RCEV |\
		TM_SCR_MAS | TM_SCR_UTQ | TM_SCR_UTS;
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000 + MVB_REG_SCR, scr);	

	OSS_MikroDelay(OSH, MVB_DELAY_10_US);
		
	/* set test master frame: poll FC=1, port 1 (testports)=0x1001 */	
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000 + MVB_REG_SMF, MVB_LOOPBACK_MF );

	/* and kick off manually */
	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_3000 + MVB_REG_MR, TM_MR_SMFM);

	/*------------------------------+
	 | wait for response or timeout	|
	 +------------------------------*/

	/*!
	 * poll MR[BUSY] after Masterframe start for max. 50x10us=500us.
	 * BUSY is deasserted after t(MF)+t(answer)=ca 40 us normally.
	 */
	do {
		timeout++;
		OSS_MikroDelay(OSH, MVB_DELAY_10_US);
	} while ((MREAD_D16(llHdl->ma, MVB_SA_OFFS_3000+MVB_REG_MR)&TM_MR_BUSY)&&\
			 timeout<MVB_LOOPBACK_TIMEOUT );

	if (timeout >= MVB_LOOPBACK_TIMEOUT){
		DBGWRT_ERR((DBH, "*** error: Testport wait timeout!\n"));
        return( MVB_ERR_LOOPBACK_TIMEOUT );  
	}

	/* Asic toggles VP bit - read received word from TSNK data page 1 */
	adr = MVB_SA_OFFS_3000 + MVB_PP_DAT_TSNK + MVB_PP_DAT_PCS1; 
	DBGWRT_2((DBH,"reading test sink: adr= 0x%04x ..", adr));
	dat = MREAD_D16(llHdl->ma, adr);
	DBGWRT_2((DBH,"TST port = 0x%04x\n", dat));

	if ( dat != MVB_LOOPBACK_VALUE ){ 
		DBGWRT_ERR((DBH, "*** error: MVBCS1 Asic loopback test failed\n"));
        return( MVB_ERR_LOOPBACK_FAIL );  /* MVBC not found */
	} else
		DBGWRT_2((DBH," Loopback test ok.\n"));
 
	scr = (u_int16)(MVB_INITLEVEL_CONFIG_MODE | \
		llHdl->mvbDefaultWaitstate << MVB_SCR_WS | \
		llHdl->mvbDefaultTimeout << MVB_SCR_TMO);

	MWRITE_D16( llHdl->ma, MVB_SA_OFFS_F000 + MVB_REG_SCR, scr);

	return ERR_SUCCESS;

}


/*****************************************************************************/
/** return accumulated number of errornous frames
 *
 *  \param llHdl	\IN  Pointer to low-level driver handle
 *
 */
static int32 MVB_GetFrameErrorCount(LL_HANDLE *llHdl)
{
	return llHdl->mvbErrFramesSum;
}



/*****************************************************************************/
/** Initialize driver's jump table
 *
 *  \param drvP     \IN Pointer to the initialized jump table structure
 */
#ifdef _ONE_NAMESPACE_PER_DRIVER_
void LL_GetEntry( LL_ENTRY* drvP )
#else
# ifdef ID_SW
 void PP04_GetEntry(LL_ENTRY* drvP)
# else
 void PP04_SW_GetEntry(LL_ENTRY* drvP)
# endif
#endif
{

    drvP->init        = PP04_Init;
    drvP->exit        = PP04_Exit;
    drvP->read        = PP04_Read;
    drvP->write       = PP04_Write;
    drvP->blockRead   = PP04_BlockRead;
    drvP->blockWrite  = PP04_BlockWrite;
    drvP->setStat     = PP04_SetStat;
    drvP->getStat     = PP04_GetStat;
    drvP->irq         = PP04_Irq;
    drvP->info        = PP04_Info;

}


/*****************************************************************************/
/** Allocate and return low-level handle, initialize hardware
 *
 * The function initializes all channels with the definitions made
 * in the descriptor. The interrupt is disabled.
 *
 * The function decodes \ref descriptor_entries "these descriptor entries"
 * in addition to the general descriptor keys.
 *
 *  \param descP      \IN  Pointer to descriptor data
 *  \param osHdl      \IN  OSS handle
 *  \param ma         \IN  HW access handle
 *  \param devSemHdl  \IN  Device semaphore handle
 *  \param irqHdl     \IN  IRQ handle
 *  \param llHdlP     \OUT Pointer to low-level driver handle
 *
 *  \return           \c 0 On success or error code
 */
static int32 PP04_Init(
    DESC_SPEC       *descP,
    OSS_HANDLE      *osHdl,
    MACCESS         *ma,
    OSS_SEM_HANDLE  *devSemHdl,
    OSS_IRQ_HANDLE  *irqHdl,
    LL_HANDLE       **llHdlP
	)
{

    LL_HANDLE *llHdl = NULL;
    u_int32 gotsize;
    int32 error;
	int32 saoffs = 0;
    u_int32 value = 0;

	/*-----------------------------+
	 |  prepare the handle         |
	 +-----------------------------*/
	*llHdlP = NULL;	

	/* alloc & clear */
    if ((llHdl = (LL_HANDLE*)OSS_MemGet(osHdl,
										sizeof(LL_HANDLE),
										&gotsize))==NULL)
		return(ERR_OSS_MEM_ALLOC);
    OSS_MemFill(osHdl, gotsize, (char*)llHdl, MVB_NUM_0);

	/* init base values */
    llHdl->ma		  = *ma;
    llHdl->memAlloc   = gotsize;
    llHdl->osHdl      = osHdl;
    llHdl->irqHdl     = irqHdl;

	/*------------------------------+
	 |  init Port list			 	|
	 +------------------------------*/
	OSS_DL_NewList( &llHdl->la_port_list );

	/*------------------------------+
	 |  init id function table      |
	 +------------------------------*/
	/* driver's ident function */
	llHdl->idFuncTbl.idCall[MVB_NUM_0].identCall = Ident;
	/* library's ident functions */
	llHdl->idFuncTbl.idCall[MVB_NUM_1].identCall = DESC_Ident;
	llHdl->idFuncTbl.idCall[MVB_NUM_2].identCall = OSS_Ident;
	/* terminator */
	llHdl->idFuncTbl.idCall[MVB_NUM_3].identCall = NULL;

	/*------------------------------+
	 |  prepare debugging           |
	 +------------------------------*/
	DBGINIT((NULL,&DBH));

	/*------------------------------+
	 |  scan descriptor             |
	 +------------------------------*/
	/* prepare access */
    if ((error = DESC_Init(descP, osHdl, &llHdl->descHdl)))
		return( Cleanup(llHdl, error) );

	DESC_DbgLevelSet(llHdl->descHdl, value);	/* set selected level */

    error = DESC_GetUInt32(llHdl->descHdl,	OSS_DBG_DEFAULT, 
						   &llHdl->dbgLevel, "DEBUG_LEVEL");
	if ( error && (error != ERR_DESC_KEY_NOTFOUND))
		return( Cleanup(llHdl, error));

    error = DESC_GetUInt32(llHdl->descHdl, OSS_DBG_DEFAULT, 
						   &value, "DEBUG_LEVEL_DESC");
	if ( error && (error != ERR_DESC_KEY_NOTFOUND))
		return( Cleanup(llHdl, error));
	
#ifdef PP04_MDIS_UNITTEST
	llHdl->mvbMcm = MVB_DEFAULT_MCM;
	DBGWRT_1((DBH, "Unittest: use llHdl->mvbMcm = %ld\n",llHdl->mvbMcm));
#else
	error = DESC_GetUInt32(llHdl->descHdl, MVB_DEFAULT_MCM, 
						   &llHdl->mvbMcm, "MVB_MCM_MODE");
	if ( error && (error != ERR_DESC_KEY_NOTFOUND))
		return( Cleanup(llHdl, error));
#endif

    error = DESC_GetUInt32(llHdl->descHdl, MVB_DEFAULT_FILTER,
						   &llHdl->mvbUseFilter, "MVB_CONFIG_FILTER");
	if ( error && (error != ERR_DESC_KEY_NOTFOUND))
		return( Cleanup(llHdl, error));


    error = DESC_GetUInt32(llHdl->descHdl,	MVB_DEFAULT_TIMEOUT, 
						   &llHdl->mvbDefaultTimeout, "MVB_TIMEOUT");
	if ( error && (error != ERR_DESC_KEY_NOTFOUND))
		return( Cleanup(llHdl, error));


    error = DESC_GetUInt32(llHdl->descHdl,	MVB_DEFAULT_WAITSTATES, 
						   &llHdl->mvbDefaultWaitstate, "MVB_TM_WAITSTATES");
	if ( error && (error != ERR_DESC_KEY_NOTFOUND))
		return( Cleanup(llHdl, error));


	if (llHdl->mvbMcm > MVB_MCM_MODE_1M)
		return( Cleanup(llHdl, ERR_LL_ILL_PARAM));

	saoffs = MVB_SAoffsetFromMCM((u_int8)(llHdl->mvbMcm));
	if ( saoffs == ERR_LL_ILL_PARAM)
		return( Cleanup(llHdl, ERR_LL_ILL_PARAM));
	llHdl->regOff = (u_int16)saoffs;

	/*------------------------------+
	 | watchdog/timer/IMR masks 	|
	 +------------------------------*/
	llHdl->mvbWDRetriggerTime	= MVB_WD_RETRIGGER_TIME;

	/* by default all error reporting IRQs are used */
	llHdl->mvbIMR0DefaultMask =  
		IF_EMF | IF_ESF | IF_DMF | IF_DSF | IF_BTI | IF_DTI7 | IF_AMFX;
	llHdl->mvbIMR1DefaultMask =	 IF_WD | IF_FL|IF_FEV;

    /* set offsets of MCM specific Areas (PIT, PCS ...) */
	if (MVB_InitTMoffsets(llHdl))
		return( Cleanup(llHdl, ERR_LL_ILL_PARAM));

	/*-----------------------------+
	 | create alarm functions	   |
	 +-----------------------------*/

	if ( OSS_AlarmCreate( OSH, MVB_Timer2Func, (void*)( llHdl ),
						  &llHdl->ossAlarmTI2Hdl ))
		return( Cleanup(llHdl, ERR_OSS_ALARM_CREATE ));
	llHdl->isActiveAlarmTI2 = 0;


	DBGWRT_3((DBH, " llHdl->ma 		= 	0x%08x\n", llHdl->ma));
	DBGWRT_3((DBH, " llHdl->mvbMcm  = %d\n", llHdl->mvbMcm));
	DBGWRT_3((DBH, " llHdl->llHdl->mvbIMR0DefaultMask = 0x%04x\n", 
			  llHdl->mvbIMR0DefaultMask ));
	DBGWRT_3((DBH, "   llHdl->llHdl->mvbIMR1DefaultMask = 0x%04x\n", 
			  llHdl->mvbIMR1DefaultMask ));

	/*-----------------------------+
	 |Perform Asic Initialization  |
	 +-----------------------------*/

	/* first, check if the device is working */
	if ((error = MVB_PerformDiagnose(llHdl )))
				return( Cleanup(llHdl, error ));

	/* go in config Mode (IL=1)*/
	if ((error = MVB_SetInitializationLevel(llHdl,
											MVB_INITLEVEL_CONFIG_MODE )))
		return( Cleanup(llHdl, error ));

	/* setup Traffic Memory according to MCM */
	if ((error = MVB_SetupMemConfiguration( llHdl )))
		return( Cleanup(llHdl, error ));

	/*  format PITs, PCS Area (clear SDRAM) */
	if ((error = MVB_FormatTM( llHdl )))
		return( Cleanup(llHdl, error ));

	*llHdlP = llHdl;	
	return(ERR_SUCCESS);

}	


/*****************************************************************************/
/** De-initialize hardware and clean up memory
 *
 *  The function deinitializes all channels.
 *  The interrupt is disabled.
 *
 *  \param llHdlP      \IN  Pointer to low-level driver handle
 *
 *  \return           \c 0 On success or error code
 */
static int32 PP04_Exit( LL_HANDLE    **llHdlP )
{
    LL_HANDLE *llHdl = *llHdlP;
	int32 error;

    DBGWRT_1((DBH, "LL - PP04_Exit\n"));

	/*------------------------------+
	 |  de-init hardware            |
	 +------------------------------*/

	MVB_SetInitializationLevel(llHdl, MVB_INITLEVEL_SW_RESET);
	/*------------------------------+
	 |  clean up memory             |
	 +------------------------------*/
	*llHdlP = NULL;		/* set low-level driver handle to NULL */ 
	error = Cleanup(llHdl, ERR_SUCCESS);

	return(error);
}



/*****************************************************************************/
/** Read a value from the device
 *
 *  \param llHdl      \IN  Low-level handle
 *  \param ch         \IN  Current channel
 *  \param valueP     \OUT Read value
 *
 *  \return           \c ERR_LL_READ always
 *
 *	\brief			  this Function is a no op, LA Ports are read and written
 *					  using the BlockRead / BlockWrite Functions
 */
static int32 PP04_Read(
    LL_HANDLE *llHdl,
    int32 ch,
    int32 *valueP
	)
{
    DBGWRT_ERR((DBH, "*** error: LL - PP04_Read: No op\n"));
	return(ERR_LL_READ);
}


/*****************************************************************************/
/** Write a value to the device
 *
 *  \param llHdl      \IN  Low-level handle
 *  \param ch         \IN  Current channel
 *  \param value      \IN  Read value
 *
 *  \return           always error ERR_LL_ILL_FUNC
 *	\brief			  this Function is a no op, LA Ports are read and written
 *					  using the BlockRead / BlockWrite Functions
 */
static int32 PP04_Write(
    LL_HANDLE *llHdl,
    int32 ch,
    int32 value
	)
{

    DBGWRT_ERR((DBH, "*** error: LL - PP04_Write: No op\n"));
	return( ERR_LL_ILL_FUNC );
}


/*****************************************************************************/
/** Set the driver status
 *
 *  The driver supports \ref getstat_setstat_codes "these status codes"
 *  in addition to the standard codes (see mdis_api.h).
 *
 *  \param llHdl  	  		\IN  Low-level handle
 *  \param ch         		\IN  Current channel
 *  \param code       		\IN  \ref getstat_setstat_codes "status code"
 *  \param value32_or_64   	\IN  Data or pointer to block data structure (M_SG_BLOCK) for
 *                         	block status codes
 *  \return           		\c 0 On success or error code
 */
static int32 PP04_SetStat( LL_HANDLE *llHdl, int32 code, int32 ch, INT32_OR_64 value32_or_64)
{
	
	int32 error 			= ERR_SUCCESS;
	u_int32	nrPorts 		= MVB_NUM_0;
	u_int32	i		 		= MVB_NUM_0;
	u_int16	scr		 		= MVB_NUM_0;
    int32  value     		= (int32)value32_or_64; 		/* pointer to 32 bit value */
    INT32_OR_64 valueP    	= value32_or_64;         		/* stores 32/64bit pointer */
	M_SG_BLOCK *blk 		= (M_SG_BLOCK*)valueP;
	MVB_LA_PORT *theNsdbP 	= NULL;

	/* 
	 * Master Frame Table to be emitted automatically. This Table polls the ports
	 * given in the mvb_simp example. A Master Frame is 16 bit long, least 
	 * 12bit=port number. This must match the G_ports[] setting given in mvb_simp. 
	 */
	u_int16 MFtable[]={
		(MVB_FCODE_4 << 12) |  14, /* Slave B */  /* 0x400e */
		(MVB_FCODE_4 << 12) |  15, /* Slave B */
		(MVB_FCODE_4 << 12) |  16, /* Slave B */
		(MVB_FCODE_2 << 12) |  17, /* Slave B */
		(MVB_FCODE_4 << 12) |  18, /* Slave B */
		(MVB_FCODE_2 << 12) | 890, /* Slave A */
		(MVB_FCODE_4 << 12) | 891, /* Slave A */
		0
	};


    DBGWRT_1((DBH, "LL - PP04_SetStat: ch=0x%08lx code=0x%04lx value=0x%lx\n",
			  ch, code, value));

    switch(code) {
		/*--------------------------+
		 |   debug level            |
		 +-------------------------*/
    case M_LL_DEBUG_LEVEL:
        llHdl->dbgLevel = value;
        break;
		/*--------------------------+
		 |  enable interrupts       |
		 +-------------------------*/
    case M_MK_IRQ_ENABLE:
        if (!IN_RANGE(value, MVB_NUM_0, MVB_NUM_1))
            return(ERR_LL_ILL_PARAM);

		/* the MVBCS1 contains 2 IRQ enable Registers.. */
		if ((error = MVB_EnableIrq(llHdl, llHdl->mvbIMR0DefaultMask, 
								   MVB_REG_IMR0, (u_int8)value )))
			return(error);
		
		if ((error = MVB_EnableIrq(llHdl, llHdl->mvbIMR1DefaultMask, 
								   MVB_REG_IMR1, (u_int8)value )))
			return(error);
        break;
		/*--------------------------+
		 |   set irq counter        |
		 +-------------------------*/
    case M_LL_IRQ_COUNT:
        llHdl->irqCount = value;
        break;
		/*--------------------------+
		 |  channel direction       |
		 +-------------------------*/
    case M_LL_CH_DIR:		
		/* MVB Ports cant change their direction during operation */
		error = ERR_LL_ILL_FUNC;
        break;

		/*--------------------------+
		 |  register signal (data)  |
		 +-------------------------*/
	case MVB_SET_SIGNAL_DATA:
		error = OSS_SigCreate( OSH, value, &llHdl->sigDataHdl );
		break;
		/*--------------------------+
		 |  register signal (alerts)|
		 +-------------------------*/
	case MVB_SET_SIGNAL_ALERT:
		error = OSS_SigCreate( OSH, value, &llHdl->sigAlertHdl );
		break;
		/*--------------------------+
		 |  unregister alert signal |
		 +-------------------------*/
	case MVB_CLR_SIGNAL_DATA:
		error = OSS_SigRemove( OSH, &llHdl->sigDataHdl );
		break;			
		/*--------------------------+
		 | unreg. data changed sig  |
		 +-------------------------*/
	case MVB_CLR_SIGNAL_ALERT:
		error = OSS_SigRemove( OSH, &llHdl->sigAlertHdl );
		break;			

		/*--------------------------+
		 | set initialization level |
		 +-------------------------*/
    case MVB_INITLEVEL:
        if (!IN_RANGE(value, MVB_INITLEVEL_SW_RESET, MVB_INITLEVEL_FULL_OP))
            return(ERR_LL_ILL_PARAM);

		error = MVB_SetInitializationLevel( llHdl, (u_int8)value );
        break;
		/*--------------------------+
		 | disable transmit 		|
		 +-------------------------*/
    case MVB_DISABLE_IMMEDIATE:
		DBGWRT_3((DBH, "MVB_DISABLE_IMMEDIATE:\n"));
		if (!IN_RANGE(value, MVB_NUM_0, MVB_NUM_1))
			return(ERR_LL_ILL_PARAM);

		if (value){
            MCLRMASK_D16(llHdl->ma, llHdl->regOff+MVB_REG_SCR, MVB_SCR_QUIET );
            MSETMASK_D16(llHdl->ma, llHdl->regOff+MVB_REG_SCR, MVB_SCR_QUIET );
		} else {
            MCLRMASK_D16(llHdl->ma, llHdl->regOff+MVB_REG_SCR, MVB_SCR_QUIET );
		}
        break;

		/*--------------------------+
		 | Set Device Statusword    |
		 +-------------------------*/
	case MVB_PP_FC15:
		/* exactly, only 3 bits in DSW are 'user servicable' */
		error = MVB_SetDeviceStatusword(llHdl, value );
		break;

		/*--------------------------+
		 |  shutdown line B         |
		 +-------------------------*/
    case MVB_SHUTDOWN_LINE_B:
		DBGWRT_3((DBH, "MVB_SHUTDOWN_LINE_B:\n"));
		if (!IN_RANGE(value, MVB_NUM_0, MVB_NUM_1))
			return(ERR_LL_ILL_PARAM);		

		error = MVB_ShutdownLineAB(llHdl, (u_int8)value, MVB_LINE_B);
		break;

		/*--------------------------+
		 |  shutdown line A         |
		 +-------------------------*/
    case MVB_SHUTDOWN_LINE_A:
		DBGWRT_3((DBH, "MVB_SHUTDOWN_LINE_A:\n"));
		if (!IN_RANGE(value, MVB_NUM_0, MVB_NUM_1))
			return(ERR_LL_ILL_PARAM);		
		error = MVB_ShutdownLineAB(llHdl, (u_int8)value, MVB_LINE_A);
        break;

		/*--------------------------+
		 | Config MVB device address|
		 +--------------------------*/
    case MVB_DEVICE_ADDRESS: 
		DBGWRT_3((DBH, "MVB_DEVICE_ADDRESS:\n"));
		/* DIN IEC-61375 states: "Device Address 0 shall not be used" */
		if (!IN_RANGE(value, MVB_NUM_1, MVB_MAX_ADDR))
			return(ERR_LL_ILL_PARAM);
		error = MVB_ConfigDeviceAddress(llHdl, (u_int16)value);
		break;

		/*--------------------------+
		 | SetStats for testing only|
		 +--------------------------*/
    case MVB_BUSADMIN: 
		/* 
		 * This is solely for test purposes! if some basic Bus Administrator
		 * function is desired, adjust the MFtable[] array with its
		 * port numbers to the NSDB setup of the given MVBus!
		 */
		DBGWRT_3((DBH, "MVB_BUSADMIN:\n"));
		llHdl->doBusmaster = (u_int16)value;

		/* set MAS bit in SCR and TMO[1..0] */	
		scr = MREAD_D16(llHdl->ma,llHdl->regOff + MVB_REG_SCR);
		scr |=MVB_SCR_MAS;
		scr &= ~(3<<10);  /* clear TMO[1..0] first	*/
		scr |=  (1<<10);  /* then set wanted value	*/
		MWRITE_D16( llHdl->ma,llHdl->regOff + MVB_REG_SCR, scr );

		/* setup Timer 1 with BA Timebase 1 ms = 1000us (k+1)*10us = 99 */
		MWRITE_D16( llHdl->ma,llHdl->regOff + MVB_REG_TR1, MVB_MAS_TIMEBASE );
		/* and activate timer1 */
		MWRITE_D16( llHdl->ma,llHdl->regOff + MVB_REG_TCR, 1 );
		
		/* copy Masterframe table (MF) to TM, set DPP  */
		for (i = 0; MFtable[i]; i++ ) {
		   MWRITE_D16(llHdl->ma,llHdl->regOff+MVB_MFT_START+(i*2),MFtable[i]);
		}

		/* remember: DPR contains address of MF table << by 2! */
		MWRITE_D16(llHdl->ma,llHdl->regOff+ MVB_REG_DPR,  MVB_MFT_START >> 2);
		MWRITE_D16(llHdl->ma,llHdl->regOff+ MVB_REG_DPR2, MVB_MFT_START >> 2);
		
		/* and set MF table size. MVBCS1 ASIC is set to full op in mvb_simp */
		MWRITE_D16(llHdl->ma,llHdl->regOff+ MVB_REG_MR,  MAS_MR_INITVAL );
		MWRITE_D16(llHdl->ma,llHdl->regOff+ MVB_REG_MR2, MAS_MR_INITVAL );
		break;

		/*
		 * 1: disable intern Wdog / ASIC hang check for test purposes, 
		 * 0: use wdog regular(default)  
		 */
	case MVB_NOWDOG:
		DBGWRT_3((DBH, "MVB_NOWDOG:\n"));
		if (value !=0)
			llHdl->noWdog = 1;
		else
			llHdl->noWdog = 0;
		break;

		/*--------------------------+
		 | NSDB config block stat	|
		 +-------------------------*/
    case MVB_BLK_NSDB:
		DBGWRT_3((DBH, "MVB_BLK_NSDB:\n"));
		theNsdbP = (MVB_LA_PORT *)blk->data; 
		if(!theNsdbP){
			error = ERR_LL_ILL_PARAM;
			break;
		}
		nrPorts = blk->size / sizeof(MVB_LA_PORT);
		DBGWRT_2((DBH, "blk->size = %ld\n", 	blk->size ));
		DBGWRT_2((DBH, "nrPorts = %d\n", 		nrPorts ));
		error = MVB_SetupNSDB( llHdl, theNsdbP, (u_int16)nrPorts );
		break;
		/*--------------------------+
		 | reset error counter   	|
		 +-------------------------*/
	case MVB_RESET_ERROR_COUNT:	
		DBGWRT_3((DBH, "MVB_RESET_ERROR_COUNT\n"));
		MVB_ResetErrorCount( llHdl );
		break;

		/*--------------------------+
		 | set Sinktime supervision |
		 +-------------------------*/
	case MVB_SINKTIME_SUPERVISION:
		DBGWRT_3((DBH, "MVB_SINKTIME_SUPERVISION\n"));
		error = MVB_SetupSinktimeSupervision( llHdl, (u_int16)value );
		break;
		/*--------------------------+
		 | trigger WD 				|
		 +-------------------------*/
	case MVB_TRIGGER_WATCHDOG:
		DBGWRT_3((DBH, " MVB_TRIGGER_WATCHDOG\n"));
		MVB_WDtriggerFunc( llHdl );
		break;
		/*--------------------------+
		 |  unknown           		|
		 +-------------------------*/
    default:
		error = ERR_LL_UNK_CODE;
    }
	return(error);
}



/*****************************************************************************/
/** Get a driver status
 *
 *  The driver supports \ref getstat_setstat_codes these status codes
 *  in addition to the standard codes (see mdis_api.h).
 *
 *  \param llHdl      		\IN  Low-level handle
 *  \param code       		\IN  \ref getstat_setstat_codes "status code"
 *  \param ch         		\IN  Current channel
 *  \param value32_or_64P	\IN  Pointer to block data structure (M_SG_BLOCK) for
 *                        		 block status codes
 *  \param value32_or_64P   \OUT Data pointer or pointer to block data structure
 *                         		 (M_SG_BLOCK) for block status codes
 *
 *  \return           		\c 0 On success or error code
 */
static int32 PP04_GetStat(LL_HANDLE *llHdl, int32 code, int32 ch, INT32_OR_64 *value32_or_64P)
{
	u_int16 	*datP 		= NULL;
	int32 		error 		= ERR_SUCCESS;
    int32       *valueP     = (int32*)value32_or_64P;
    INT32_OR_64 *value64P   = value32_or_64P; 
	M_SG_BLOCK 	*blk 		= (M_SG_BLOCK*)valueP;
	u_int32		words		= 0;
	INT32_OR_64 adr			= 0;

    DBGWRT_1((DBH, "LL - PP04_GetStat: ch=0x%08lx code=0x%04lx\n", ch,code));

    switch(code)
    {
		/*--------------------------+
		 |  debug level             |
		 +--------------------------*/
    case M_LL_DEBUG_LEVEL:
        *valueP = llHdl->dbgLevel;
        break;
		/*--------------------------+
		 |  number of channels      |
		 +-------------------------*/
    case M_LL_CH_NUMBER:
        *valueP = CH_NUMBER;
        break;
		/*--------------------------+
		 |  channel direction       |
		 +-------------------------*/
    case M_LL_CH_DIR:
		/* on PP04 this reflects the ports srcsink property */
		*valueP = llHdl->la_prt_nodeP[ch]->srcsink;
		break;
		/*--------------------------+
		 |  channel length [bits]   |
		 +--------------------------*/
    case M_LL_CH_LEN:
		*valueP = llHdl->la_prt_nodeP[ch]->size;
        break;
		/*--------------------------+
		 |  channel type info       |
		 +--------------------------*/
    case M_LL_CH_TYP:
        *valueP = M_CH_BINARY;
        break;
		/*--------------------------+
		 |  irq counter             |
		 +--------------------------*/
    case M_LL_IRQ_COUNT:
        *valueP = llHdl->irqCount;
        break;
		/*--------------------------+
		 |  ident table             |
		 +--------------------------*/
    case M_MK_BLK_REV_ID:
		*value64P = (INT32_OR_64)&llHdl->idFuncTbl;
        break;
		/*--------------------------+
		 |  error Frame counter     |
		 +--------------------------*/
    case MVB_ERR_COUNT:
		DBGWRT_3((DBH, "MVB_ERR_COUNT:\n"));		
		*valueP = MVB_GetFrameErrorCount(llHdl);
        break;
		/* --- PP04 specifics --- */
		/*--------------------------+
		 |  error Flags 			|
		 +-------------------------*/
    case MVB_ERR_FLAGS:
		DBGWRT_3((DBH, "MVB_ERR_FLAGS:\n"));		
		*valueP = llHdl->mvbErrFlags;
        break;
		/*--------------------------+
		 |  errornous Masterframes	|
		 +-------------------------*/
    case MVB_ERR_MASTER_FRAMES:
		DBGWRT_3((DBH, "MVB_ERR_MASTER_FRAMES:\n"));		
		*valueP = llHdl->mvbErrMasterFrames;
        break;
		/*--------------------------+
		 |  errornous Slave frames	|
		 +-------------------------*/
    case MVB_ERR_SLAVE_FRAMES:
		DBGWRT_3((DBH, "MVB_ERR_SLAVE_FRAMES:\n"));		
		*valueP = llHdl->mvbErrSlaveFrames;
        break;
		/*--------------------------+
		 |  duplicate Master frames	|
		 +-------------------------*/
    case MVB_ERR_DUPLICATE_MASTER_FRAMES:
		DBGWRT_3((DBH, "MVB_ERR_DUPLICATE_MASTER_FRAMES:\n"));		
		*valueP = llHdl->mvbErrDupMaster;
        break;
		/*--------------------------+
		 |  duplicate slave frames	|
		 +-------------------------*/
    case MVB_ERR_DUPLICATE_SLAVE_FRAMES:
		DBGWRT_3((DBH, "MVB_ERR_DUPLICATE_SLAVE_FRAMES:\n"));		
		*valueP = llHdl->mvbErrDupSlave;
        break;
		/*--------------------------+
		 |  Frames Lost				|
		 +-------------------------*/
    case MVB_ERR_FRAMES_LOST:
		DBGWRT_3((DBH, "MVB_ERR_FRAMES_LOST:\n"));		
		*valueP = llHdl->mvbErrFramesLost;
        break;
		/*--------------------------+
		 |  Reply timeout			|
		 +-------------------------*/
    case MVB_ERR_REPLY_TIMEOUT:
		DBGWRT_3((DBH, "MVB_ERR_REPLY_TIMEOUT:\n"));
		*valueP = llHdl->mvbErrReplyTimeout;
        break;
		/*--------------------------+
		 |  Bus timeout				|
		 +-------------------------*/
    case MVB_ERR_BUS_TIMEOUT:
		DBGWRT_3((DBH, "MVB_ERR_BUS_TIMEOUT:\n"));		
		*valueP = llHdl->mvbErrBusTimeout;
        break;
		/*--------------------------+
		 |  Timer2/Asic Hangs		|
		 +-------------------------*/
	case MVB_ERR_ASIC_HANG:
		DBGWRT_3((DBH, "MVB_ERR_ASIC_HANG:\n"));		
		*valueP = llHdl->mvbErrAsicHang;
        break;
		/*--------------------------+
		 |  Device Address			|
		 +-------------------------*/
	case MVB_DEVICE_ADDRESS:
		DBGWRT_3((DBH, "MVB_DEVICE_ADDRESS:\n"));		
		*valueP = MREAD_D16(llHdl->ma,llHdl->regOff+ MVB_REG_DAOR);
		break;
		/*--------------------------+
		 |  get sinktime Value		|
		 +-------------------------*/
	case MVB_SINKTIME_SUPERVISION:
		DBGWRT_3((DBH, "MVB_SINKTIME_SUPERVISION:\n"));		
		*valueP = MVB_GetSinktime(llHdl, ch);
		break;
		/*--------------------------+
		 | Get Device Statusword    |
		 +-------------------------*/
	case MVB_PP_FC15:
		DBGWRT_3((DBH, "MVB_PP_FC15:\n"));
		*valueP = MREAD_D16(llHdl->ma,llHdl->tmMap.saStart + MVB_PP_DAT_FC15);
		break;

		/*--------------------------+
		 | dump whole traffic memory|
		 +-------------------------*/
    case MVB_BLK_DIRECT:
		DBGWRT_1((DBH, "MVB_BLK_DIRECT:\n"));
		datP = (u_int16*)blk->data; 

		if(!datP){
			DBGWRT_3((DBH, "*** error: no valid buffer:\n"));
			error = ERR_LL_ILL_PARAM;
			break;
		}

		/* mind byte order: big endian (in MVBCS1)*/
		for ( words = MVB_NUM_0; words < llHdl->tmMap.laDatLen;words+=MVB_NUM_2 )	
		{
			adr = (INT32_OR_64)(llHdl->ma) + (INT32_OR_64)(llHdl->tmMap.laDatStart);
			MWRITE_D16( adr, words, MREAD_D16( adr, words));
		}
		break;

		/*--------------------------+
		 |         unknown          |
		 +--------------------------*/
    default:
		error = ERR_LL_UNK_CODE;
    }
	return(error);
}


/*****************************************************************************/
/** Read a data block from the device, here the complete LA port content
 *
 *  \param llHdl       \IN  Low-level handle
 *  \param ch          \IN  Current channel, is equal to the port address
 *  \param buf         \IN  Data buffer to copy to
 *  \param size        \IN  Data buffer size
 *  \param nbrRdBytesP \OUT Number of read bytes (always equal to port size)
 *
 *  \return            \c 0 On success or error code
 *
 *  \brief 				the MVB Port data with address ch is read from TM
 *						Byte order of stored data: Bytes are always stored
 *						in big endian Format. That means that for example the
 *						sink data from an 8 byte port , received as:
 *						|b7|b6|b5|b4|b3|b2|b1|b0| (b7 sent first)
 *	                                       _____
 *						are stored as     |b7|b6|  *buf,     *(buf+1)
 *				                           -----
 *										  |b5|b4|  *(buf+2), *(buf+3)  etc.
 *                                         -----
 */
static int32 PP04_BlockRead(
	LL_HANDLE *llHdl,
	int32     ch,		/* LA port address as in Masterframe 		*/
	void      *buf,		/* where to copy data to 					*/
	int32     size,		/* must be max. this ports size				*/
	int32     *nbrRdBytesP
	)
{

	MVB_LA_PORT_NODE  	*prtP = NULL;
	u_int16				pcs1, vp, sinktime, bytes;
	u_int16				*dataP;
	u_int32				adr=0;
	CHKPARM( llHdl );
	CHKPARM( IN_RANGE( ch, MVB_NUM_1, MVB_LA_PORTS_MAX ));
	CHKPARM( buf );

	DBGWRT_1((DBH, "PP04_BlockRead: ch=%ld, size=%ld\n", ch, size ));

	/* lookup this ports MVB_LA_PORT_NODE */
	if (!(prtP = llHdl->la_prt_nodeP[ch] )){
		DBGWRT_ERR((DBH, "*** No Port %ld found in List\n", ch));
		return(ERR_LL_ILL_CHAN);
	}

	/* Do not write behind Ports data area */
	if (size > prtP->size)
		size = prtP->size;

	if (prtP->size < size){
		DBGWRT_ERR((DBH,"*** error: size read: %ld, port%ld size:%d\n",
					size, ch, prtP->size));
		return(ERR_LL_READ);
	}

	/* dont allow read from SOURCE ports (Asic & CPU would use same page)*/
	if (prtP->srcsink == MVB_LA_PORT_SOURCE ){
		DBGWRT_ERR((DBH, "*** error: Port %ld is a source\n", ch));
		return (ERR_LL_ILL_DIR);
	}

	/*
	 * if Sinktime Supervision is on, check if Sinktime of this port (=TACK in
	 * PCS[2]) is higher then the treshold Value passed in the NSDB struct.
	 */
	if (llHdl->mvbDoSinktimeSupervision){
		sinktime = (u_int16)MVB_GetSinktime(llHdl, ch);
		if ( sinktime < prtP->tack ){
			DBGWRT_ERR((DBH,"*** error: Sinktime of port%ld expired\n", ch ));
			/* no data shall be read then */
			*nbrRdBytesP = MVB_NUM_0;
			return( ERR_LL_READ );
		}
	}

	/* copy Port data to buf. Sinkport data must be read from page VP */
	pcs1 	= MREAD_D16(prtP->pcs, MVB_NUM_2 );
	vp 	 	= ( pcs1 & MVB_PCS1_VP ) ? MVB_PCS_VP_OFFS : MVB_NUM_0;

	DBGWRT_3((DBH, "PP04_BlockRead: vp=%d\n", vp ));
	dataP = (u_int16*)buf;
	for ( bytes = MVB_NUM_0; bytes < size; bytes += MVB_NUM_2) {
		adr = (u_int32)(prtP->dat+vp);
		*dataP = MREAD_D16(adr, bytes); 
		dataP++;
	}

	*nbrRdBytesP = size; 

	DBGWRT_3((DBH, " *nbrRdBytesP = %d\n", *nbrRdBytesP));
	return(ERR_SUCCESS);

 ABORT:
	return(ERR_LL_ILL_PARAM);

}


/*****************************************************************************/
/** Write a data block to the device
 *
 *  The function is not supported and always returns an ERR_LL_ILL_FUNC error.
 *
 *  \param llHdl  	   \IN  Low-level handle
 *  \param ch          \IN  channel, is equal to Port address
 *  \param buf         \IN  Data buffer to otain data from
 *  \param size        \IN  Data buffer size
 *  \param nbrWrBytesP \OUT Number of written bytes
 *
 *  \return            \c 	ERR_LL_ILL_FUNC
 */
static int32 PP04_BlockWrite(
	LL_HANDLE *llHdl,
	int32     ch,
	void      *buf,
	int32     size,
	int32     *nbrWrBytesP
	)
{

	u_int16 pcs1, notVP;
	MVB_LA_PORT_NODE  *prtP = NULL;
	u_int16 *dat16P = NULL;
	int32 bytes;
	u_int32 adr;
	
	CHKPARM( llHdl );
	CHKPARM( IN_RANGE(ch, MVB_NUM_1, MVB_LA_PORTS_MAX) );
	CHKPARM( buf );

    DBGWRT_1((DBH, "LL - PP04_BlockWrite: ch=%ld, size=%ld\n", ch, size));

	/* 'No Port found' will also occur if NSDB setup wasnt done before */
	if (!(prtP = llHdl->la_prt_nodeP[ch] )){
		DBGWRT_ERR((DBH, "*** No Port %ld found in List\n", ch));
		return(ERR_LL_ILL_CHAN);
	}	

	DBGWRT_2((DBH, "found Port '%s' \n", prtP->name  ));
	*nbrWrBytesP = MVB_NUM_0;

	/* writes to SINK ports are not allowed */
	if (prtP->srcsink == MVB_LA_PORT_SINK){
		DBGWRT_ERR((DBH, "*** error: Port %ld is a sink\n", ch));
		return (ERR_LL_ILL_DIR);
	}

	/* Do not write behind Ports data area */
	if (size > prtP->size)
		size = prtP->size;
	
	/* source data goes to page /VP ("VP not") */
	pcs1  = MREAD_D16( prtP->pcs, MVB_NUM_2 );
	notVP = ( pcs1 & MVB_PCS1_VP) ? MVB_NUM_0 : MVB_PCS_VP_OFFS;
	DBGWRT_3((DBH, "pcs1: 0x%04x notVP=0x%02x\n", pcs1, notVP ));

	for ( bytes = MVB_NUM_0; bytes < size; bytes += MVB_NUM_2 )	{
		dat16P = (u_int16*)(((u_int8*)buf)+bytes);
		adr = (u_int32)(prtP->dat+notVP);
		MWRITE_D16( adr, bytes, *dat16P );
	}

	/* toggle VP bit (bit6) after writing, as advised in data sheet */
	pcs1 ^=MVB_PCS1_VP;
	DBGWRT_3((DBH, "write pcs1: 0x%04x\n", pcs1 ));
	MWRITE_D16( prtP->pcs, MVB_NUM_2, pcs1 );

	*nbrWrBytesP = size;

	return(ERR_SUCCESS);

 ABORT:
	return(ERR_LL_ILL_PARAM);

}


/*****************************************************************************/
/** Interrupt service routine
 *
 *  The interrupt is triggered when one of the enabled Interrupt reasons 
 *  occured, see MVBCS1 data sheet 
 *
 *  If the driver can detect the interrupt's cause it returns
 *  LL_IRQ_DEVICE or LL_IRQ_DEV_NOT, otherwise LL_IRQ_UNKNOWN.
 *
 *  \param llHdl  	   \IN  Low-level handle
 *  \return LL_IRQ_DEVICE	IRQ caused by device
 *          LL_IRQ_DEV_NOT  IRQ not caused by device
 *          LL_IRQ_UNKNOWN  Unknown
 */
static int32 PP04_Irq(
	LL_HANDLE *llHdl
	)
{
	u_int32 doSignalAlert = FALSE;
	u_int32 doSignalData  = FALSE;
	u_int16 isr0, isr1;

	/*--------------------------+
	 | check interrupt pending  |
	 +--------------------------*/
	isr0 =  MREAD_D16(  llHdl->ma, llHdl->regOff + MVB_REG_ISR0);
	isr1 =  MREAD_D16(  llHdl->ma, llHdl->regOff + MVB_REG_ISR1);

	if (!(isr0 || isr1))
        return(LL_IRQ_DEV_NOT);

    IDBGWRT_1((DBH, ">>> PP04_Irq\n"));

	if (isr0){
		if (isr0 & IF_EMF){		/* Errornous Master Frame   */
			IDBGWRT_1((DBH, "IF_EMF\n"));
			llHdl->mvbErrFramesSum++;
			llHdl->mvbErrMasterFrames++;			
			llHdl->mvbErrFlags |=MVB_COMM_ERR_EMF;
			doSignalAlert = TRUE;
		}

		if (isr0 & IF_ESF){ 	/* Errornous Slave Frame    */
			IDBGWRT_1((DBH, "IF_ESF\n"));
			llHdl->mvbErrFramesSum++;
			llHdl->mvbErrSlaveFrames++;
			llHdl->mvbErrFlags |=MVB_COMM_ERR_ESF;
			doSignalAlert = TRUE;
		} 

		if (isr0 & IF_DMF){		/* Duplicate Master Frame   */
			IDBGWRT_1((DBH, "IF_DMF\n"));
			llHdl->mvbErrFramesSum++;
			llHdl->mvbErrDupMaster++;
			llHdl->mvbErrFlags |=MVB_COMM_ERR_DMF;
			doSignalAlert = TRUE;
		}

		if (isr0 & IF_DSF){		/* Duplicate Slave Frame    */
			IDBGWRT_1((DBH, "IF_DSF\n"));
			llHdl->mvbErrFramesSum++;
			llHdl->mvbErrDupSlave++;
			llHdl->mvbErrFlags |=MVB_COMM_ERR_DSF;
			doSignalAlert = TRUE;	
		}

		if (isr0 & IF_AMFX){ /* All Master Frames Sent */
			IDBGWRT_1((DBH, "IF_AMFX\n"));
			/*	data sheet p.70 states: when AMFX occurs MR2 is copied to 
			 *	MR and MR2 becomes zero */
			MWRITE_D16(llHdl->ma,llHdl->regOff + MVB_REG_MR2, MAS_MR_INITVAL);
			MWRITE_D16(llHdl->ma,llHdl->regOff + MVB_REG_MR,  MAS_MR_INITVAL);
		} 
		
		if (isr0 & IF_RTI){ 	/* Reply Timeout Interrupt  */
			IDBGWRT_1((DBH, "IF_RTI\n"));
			llHdl->mvbErrReplyTimeout++;
			llHdl->mvbErrFlags |=MVB_COMM_ERR_RTI;
			doSignalAlert = TRUE;
		}

		if (isr0 & IF_BTI){ 	/* Bus Timeout Interrupt    */
			IDBGWRT_1((DBH, "IF_BTI\n"));
			llHdl->mvbErrBusTimeout++;
			llHdl->mvbErrFlags |=MVB_COMM_ERR_BTI;
			doSignalAlert = TRUE;
		}	

		/* IF_DT7: compare IRQ when incoming Sink data DIFFERs from previous*/
		if (isr0 & IF_DTI7 ){ 	
			IDBGWRT_1((DBH, "IF_DTI7\n"));
			doSignalData = TRUE;
		}		
	}
	if ( isr1 ){
		if (isr1 & IF_FL){ 		/* Frame Lost */
			IDBGWRT_1((DBH, "IF_FL\n"));
			llHdl->mvbErrFramesLost++;
			llHdl->mvbErrFlags |=MVB_COMM_ERR_FL;
			doSignalAlert = TRUE;
		}

		if ( isr1 & IF_WD ){
			IDBGWRT_1((DBH, "IF_WD\n"));
			/* when WD IRQ occured, the Asic is in Config Mode Again */
			llHdl->mvbErrAsicHang++;
			llHdl->mvbErrFlags |=MVB_COMM_ERR_HANG;
			doSignalAlert = TRUE;
			/* MWRITE_D16(llHdl->ma,llHdl->regOff+MVB_REG_STSR,MVB_NUM_0); */
		}

	}


	/*----------------------+
	 | send signal          |
	 +----------------------*/
	
	if ( doSignalAlert && llHdl->sigAlertHdl)
		OSS_SigSend(OSH, llHdl->sigAlertHdl);
	
	if ( doSignalData && llHdl->sigDataHdl)
		OSS_SigSend(OSH, llHdl->sigDataHdl);


	/* clear the pending & ISR bits */
	MWRITE_D16(llHdl->ma,llHdl->regOff + MVB_REG_IPR0, MVB_NUM_0 );
	MWRITE_D16(llHdl->ma,llHdl->regOff + MVB_REG_IPR1, MVB_NUM_0 );

	MWRITE_D16(llHdl->ma,llHdl->regOff + MVB_REG_ISR0, MVB_NUM_0 );
	MWRITE_D16(llHdl->ma,llHdl->regOff + MVB_REG_ISR1, MVB_NUM_0 );

	/* release Interrupt controller by writing IVRs */
	MWRITE_D16(llHdl->ma,llHdl->regOff + MVB_REG_IVR0, MVB_NUM_0 );
	MWRITE_D16(llHdl->ma,llHdl->regOff + MVB_REG_IVR1, MVB_NUM_0 );

	llHdl->irqCount++;

	return(LL_IRQ_DEVICE);

}



/*****************************************************************************/
/** Get information about hardware and driver requirements
 *
 *  The following info codes are supported:
 *
 * \code
 *  Code                      Description
 *  ------------------------  -----------------------------
 *  LL_INFO_HW_CHARACTER      Hardware characteristics
 *  LL_INFO_ADDRSPACE_COUNT   Number of required address spaces
 *  LL_INFO_ADDRSPACE         Address space information
 *  LL_INFO_IRQ               Interrupt required
 *  LL_INFO_LOCKMODE          Process lock mode required
 * \endcode
 *
 *  The LL_INFO_HW_CHARACTER code returns all address and
 *  data modes (ORed) which are supported by the hardware
 *  (MDIS_MAxx, MDIS_MDxx).
 *
 *  The LL_INFO_ADDRSPACE_COUNT code returns the number
 *  of address spaces used by the driver.
 *
 *  The LL_INFO_ADDRSPACE code returns information about one
 *  specific address space (MDIS_MAxx, MDIS_MDxx). The returned
 *  data mode represents the widest hardware access used by
 *  the driver.
 *
 *  The LL_INFO_IRQ code returns whether the driver supports an
 *  interrupt routine (TRUE or FALSE).
 *
 *  The LL_INFO_LOCKMODE code returns which process locking
 *  mode the driver needs (LL_LOCK_xxx).
 *
 *  \param infoType	   \IN  Info code
 *  \param ...         \IN  Argument(s)
 *
 *  \return            \c 0 On success or error code
 */
static int32 PP04_Info(
	int32  infoType,
	...
	)
{
    int32   error = ERR_SUCCESS;
    va_list argptr;

    va_start(argptr, infoType );

    switch(infoType) {
	/*-------------------------------+
	 | hardware characteristics      |
	 | (all addr/data modes ORed)    |
	 +-------------------------------*/
    case LL_INFO_HW_CHARACTER:
	{
		u_int32 *addrModeP = va_arg(argptr, u_int32*);
		u_int32 *dataModeP = va_arg(argptr, u_int32*);

		*addrModeP = MDIS_MA24;	/* we support up to 16 MB TM RAM 	 */
		*dataModeP = MDIS_MD16;	/* MVB Asic uses only 16bit accesses */
		break;
	}
	/*-------------------------------+
	 | nr of required address spaces |
	 | (total spaces used)           |
	 +-------------------------------*/
    case LL_INFO_ADDRSPACE_COUNT:
	{
		u_int32 *nbrOfAddrSpaceP = va_arg(argptr, u_int32*);
		*nbrOfAddrSpaceP = MVB_NUM_1/* ADDRSPACE_COUNT */;
		break;
	}
	/*------------------------------+
	 |  address space type          |
	 |  (widest used data mode)     |
	 +------------------------------*/
    case LL_INFO_ADDRSPACE:
	{
		u_int32 addrSpaceIndex 	= va_arg(argptr, u_int32 );
		u_int32 *addrModeP 		= va_arg(argptr, u_int32*);
		u_int32 *dataModeP 		= va_arg(argptr, u_int32*);
		u_int32 *addrSizeP 		= va_arg(argptr, u_int32*);

		if (addrSpaceIndex >= ADDRSPACE_COUNT)
			error = ERR_LL_ILL_PARAM;
		else {
			*addrModeP = MDIS_MA24;
			*dataModeP = MDIS_MD16;			/* 16bit access only 	*/
			*addrSizeP = ADDRSPACE_SIZE; 	/* 1 MB TM	*/	
		}

		break;
	}
	/*-------------------------------+
	 |   interrupt required ?        |
	 +-------------------------------*/
    case LL_INFO_IRQ:
	{
		u_int32 *useIrqP = va_arg(argptr, u_int32*);
		*useIrqP = USE_IRQ;
		break;
	}
	/*-------------------------------+
	 |   process lock mode           |
	 +-------------------------------*/
    case LL_INFO_LOCKMODE:
	{
		u_int32 *lockModeP = va_arg(argptr, u_int32*);
		*lockModeP = LL_LOCK_NONE;
		break;
	}
	/*-------------------------------+
	 |   (unknown)                   |
	 +-------------------------------*/
    default:
        error = ERR_LL_ILL_PARAM;
    }

    va_end(argptr);

    return(error);
}


/*****************************************************************************/
/** Return ident string
 *
 *  \return            Pointer to ident string
 */
static char* Ident( void )	/* nodoc */
{
    return( "PP04 - PP04 low level driver: $Id: pp04_drv.c,v 1.9 2013/05/16 17:46:43 ts Exp $" );
}


/*****************************************************************************/
/** Close all handles, free memory and return error code
 *
 *	\warning The low-level handle is invalid after this function is called.
 *
 *  \param llHdl      \IN  Low-level handle
 *  \param retCode    \IN  Return value
 *
 *  \return           \IN   retCode
 */
static int32 Cleanup( LL_HANDLE    *llHdl,
					  int32        retCode		/* nodoc */
	)
{

	int32 sizetofree;
	OSS_DL_NODE *nP = NULL;
    DBGWRT_1((DBH, "PP04 LL Cleanup\n"));

	/*------------------------------+
	 |  close handles               |
	 +------------------------------*/

	/* clean up desc */
	if (llHdl->descHdl)
		DESC_Exit(&llHdl->descHdl);

	/* clean up signal */
	if (llHdl->sigAlertHdl)
		OSS_SigRemove(OSH, &llHdl->sigAlertHdl);

	/* clean up Alarms */	
	if (llHdl->isActiveAlarmTI2) {
		OSS_AlarmClear(OSH, llHdl->ossAlarmTI2Hdl );
		llHdl->isActiveAlarmTI2 = 0;
	}
	OSS_AlarmRemove(OSH, &llHdl->ossAlarmTI2Hdl );

	/* clean up debug */
	DBGEXIT((&DBH));

	/*------------------------------+
	 |  free allocated ports        |
	 +------------------------------*/
	while ((nP = OSS_DL_RemHead( &llHdl->la_port_list ))){
		sizetofree = ((MVB_LA_PORT_NODE*)nP)->memAlloc;
		OSS_MemFree(OSH, (void*)nP, sizetofree );
	}

    /* free my handle at last */
    OSS_MemFree(OSH, (int8*)llHdl, llHdl->memAlloc);

	return(retCode);
}
