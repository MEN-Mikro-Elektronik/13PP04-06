/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  mvb_drv.h
 *
 *      \author  $
 *
 *      \brief  declarations needed for PP04 MDIS driver usage 
 *
 *     Required: -
 *
 *     \switches -
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

#ifndef _PP04_DRV_H
#define _PP04_DRV_H

#ifdef __cplusplus
      extern "C" {
#endif


/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
/* none */

/* LA Port identifier string, includes 0 terminator */
#define MVB_PORTNAME_LEN		32		


/*!
 * the user application has to pass the NSDB to the LL driver with the
 * MVB_BLK_NSDB Block SetStat. The M_SG_BLOCK data block has to consist of n 
 * continuous MVB_LA_PORT structs in memory. 
 * For example, the User App may define 10 ports with
 * MVB_LA_PORT myMVBports[10];
 * and pass these as shown in the pp04_simp program
 */ 
typedef struct G_MVBPORTS{
	u_int16		addr;			/* port address (as in Master Frame)		*/
	u_int16		index;			/* port index, used as index to PIT 		*/
	u_int16		size;			/* 2, 4, 8, 16 or 32 byte 					*/
	char		portname[MVB_PORTNAME_LEN];	/* ident string, \0 terminate   */
	u_int16		srcsink;		/* 0 for source port, or non zero for sink	*/
	u_int16		tack;			/* sinktime supervision treshold  			*/
	u_int16		compare;		/* if nonzero: driver signals if this sinkport
								   data have _changed_,differing from before*/
/* space needed until now = 6+32+6 = 44 byte */
	u_int16		pad[10];		/* align struct to 64 byte size 			*/
} MVB_LA_PORT;


/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/

/*! MVB status codes (BLK)	
 *  \section mvbcodes 
 *
 *	This header file contains the device independent MVB Codes.
 *
 */	/* S,G: S=setstat, G=getstat */

/*!
 * set one of 4 initialization levels of the driver 
 * 
 */
#define	MVB_INITLEVEL					M_DEV_OF + 0x00	/* S/G */	

/*!
 * immediately stop sending on the bus
 * 
 */
#define MVB_DISABLE_IMMEDIATE			M_DEV_OF + 0x01	/* S   */	

/*!
 * stops Frame sending and receiving from Line B in the redundant MVB Bus
 * Connection
 */
#define MVB_SHUTDOWN_LINE_B				M_DEV_OF + 0x02 /* S   */	

/*!
 * stops Frame sending and receiving from Line A in the redundant MVB Bus
 * Connection
 */
#define MVB_SHUTDOWN_LINE_A				M_DEV_OF + 0x03 /* S   */	

/*!	sets the device address of the MVB device to a value as specified in
 *  DIN IEC 61375
 */
#define MVB_DEVICE_ADDRESS				M_DEV_OF + 0x04 /* S/G */	

/*!	
 * The MVB device Watchdog has to be triggered regulary by the Application
 * otherwise the MVB device times out and goes back to configuration Mode.
 */
#define MVB_TRIGGER_WATCHDOG			M_DEV_OF + 0x05 /* S   */

/*!	
 * GetStat to return accumulated sum of errornous Frames, can be used to
 * detect bad line quality
 */
#define MVB_ERR_COUNT					M_DEV_OF + 0x06 /*   G */	

/*!	
 * GetStat to read value or ORed Flags to decide which Errorcounter to read out
 */
#define MVB_ERR_FLAGS					M_DEV_OF + 0x07 /*   G */	

/*!	
 * GetStat to read accumulated number of errornous Masterframes
 */
#define MVB_ERR_MASTER_FRAMES			M_DEV_OF + 0x08 /*   G */	

/*!	
 * GetStat to read accumulated number of errornous Slave frames
 */
#define MVB_ERR_SLAVE_FRAMES			M_DEV_OF + 0x09 /*   G */	

/*!	
 * GetStat to read accumulated number of duplicate Masterframes
 */
#define MVB_ERR_DUPLICATE_MASTER_FRAMES	M_DEV_OF + 0x0a /*   G */	

/*!	
 * GetStat to read accumulated number of duplicate Slave frames
 */
#define MVB_ERR_DUPLICATE_SLAVE_FRAMES	M_DEV_OF + 0x0b /*   G */	

/*!	
 * GetStat to read accumulated number of lost Frames
 */
#define MVB_ERR_FRAMES_LOST				M_DEV_OF + 0x0c /*   G */	


/*!	
 * GetStat to read accumulated number of reply timeout Interrupts
 */
#define MVB_ERR_REPLY_TIMEOUT			M_DEV_OF + 0x0d /*   G */	

/*!	
 * GetStat to read accumulated number of bus timeout Interrupts. The MVBCS1
 * for example asserts this IRQ only once.
 */
#define MVB_ERR_BUS_TIMEOUT				M_DEV_OF + 0x0e /*   G */	

/*!	
 * GetStat to read accumulated number of Asic hangs, normally just one
 */
#define MVB_ERR_ASIC_HANG				M_DEV_OF + 0x0f /*   G */	

/*!	
 * SetStat to reset the number of all accumulated errors.
 */
#define MVB_RESET_ERROR_COUNT		    M_DEV_OF + 0x11 /* S   */	

/*!	
 * SetStat to turn on Sinktime supervision for sink ports. Valid Values are
 * given in section sinktime
 */
#define MVB_SINKTIME_SUPERVISION	    M_DEV_OF + 0x12 /* S/G */	

/*!	
 * Get/SetStat for the 3 bits in Device Statusword that are user definable.
 * changed sinkport data arrived.
 */
#define MVB_PP_FC15			    		M_DEV_OF + 0x13 /* S/G */	

/*!	
 * SetStat to set user definable signal that is sent to the application when
 * changed sinkport data arrived.
 * 
 */
#define MVB_SET_SIGNAL_DATA				M_DEV_OF + 0x16 /* S   */

/*!	
 * SetStat to remove changed data signal
 */
#define MVB_CLR_SIGNAL_DATA				M_DEV_OF + 0x17 /* S   */

/*!	
 * SetStat to set user definable signal that is sent to the application when
 * a problem or error IRQ is received by the driver 
 */
#define MVB_SET_SIGNAL_ALERT			M_DEV_OF + 0x18 /* S   */

/*!	
 * SetStat to remove error alert signal
 */
#define MVB_CLR_SIGNAL_ALERT			M_DEV_OF + 0x19 /* S   */

/*!	
 * Test SetStats for internal HW test purposes only!
 */
#define MVB_BUSADMIN					M_DEV_OF + 0x1a /* S   */
#define MVB_NOWDOG						M_DEV_OF + 0x1b /* S   */

/* Block Get/Sets */

/*!	
 * Pass the NSBD to the LL driver
 */
#define MVB_BLK_NSDB					M_DEV_BLK_OF + 0x00	/* S   */

/*!	
 * dump the content of LA DAT area directly
 */
#define MVB_BLK_DIRECT					M_DEV_BLK_OF + 0x01	/*   G */

/* Device specific error codes */
/*!
 * The MVB ASIC/FPGA could not be found
 */
#define MVB_ERR_NO_ASIC					ERR_DEV + 0x00
/*!
 * The MVB ASIC/FPGA loopback test failed upon self diagnose
 */
#define MVB_ERR_LOOPBACK_FAIL			ERR_DEV + 0x01
/*!
 * The MVB ASIC/FPGA loopback test timed out - initialization problem
 */
#define MVB_ERR_LOOPBACK_TIMEOUT		ERR_DEV + 0x02

/* Defaults for Descriptors */
#define MVB_DEFAULT_TIMEOUT 			1		/* 42,1 us					*/
#define MVB_DEFAULT_WAITSTATES 			3		
#define MVB_DEFAULT_MCM					0x04	/* 1 Mbyte mode 			*/
#define MVB_DEFAULT_FILTER				0x00	/* no filtering of spikes 	*/

/*! \section sinktime 
 *  Sinktime Supervision Intervalls  
 *	The following values can be passed to setstat MVB_SINKTIME_INTERVAL
 */

/*!
 * sinktime supervision turned off
 */
#define	MVB_STS_TIME_OFF				0

/*!	
 * decrement sinktime counter every 1 ms
 */
#define	MVB_STS_TIME_1MS				1

/*!	
 * decrement sinktime counter every 2 ms
 */
#define	MVB_STS_TIME_2MS				2

/*!	
 * decrement sinktime counter every 4 ms
 */
#define	MVB_STS_TIME_4MS				3

/*!	
 * decrement sinktime counter every 8 ms
 */
#define	MVB_STS_TIME_8MS				4

/*!	
 * decrement sinktime counter every 16 ms
 */
#define	MVB_STS_TIME_16MS				5

/*!	
 * decrement sinktime counter every 32 ms
 */
#define	MVB_STS_TIME_32MS				6

/*!	
 * decrement sinktime counter every 64 ms
 */
#define	MVB_STS_TIME_64MS				7

/*!	
 * decrement sinktime counter every 128 ms
 */
#define	MVB_STS_TIME_128MS				8

/*!	
 * decrement sinktime counter every 256 ms
 */
#define	MVB_STS_TIME_256MS				9


/*!
 * mark logical ports if theyre source or sink, in the publisher/subscriber 
 * MVB model every logical port has exactly one source, any other MVB Slave or
 * Bus Admin can set  it up to listen to it as a sink.
 */
#define MVB_LA_PORT_SOURCE				0x2
#define MVB_LA_PORT_SINK				0x1

/* possible port sizes */

/*!	
 * 16bit - 2 byte logical port size
 */
#define	MVB_LA_SIZE_2 					2 
/*!	
 * 32bit - 4 byte logical port size
 */
#define	MVB_LA_SIZE_4 					4 
/*!	
 * 64bit - 8 byte logical port size
 */
#define	MVB_LA_SIZE_8 					8 
/*!	
 * 128bit - 16 byte logical port size
 */
#define	MVB_LA_SIZE_16					16

/*!	
 * 256bit - 32 byte logical port size
 */
#define	MVB_LA_SIZE_32					32

/* Init Levels of MVBCS1 Asic */
/*!	
 * Software reset of MVB Asic/FPGA
 */
#define MVB_INITLEVEL_SW_RESET			0
/*!	
 * Config mode, must be set prior to all initialization
 */
#define MVB_INITLEVEL_CONFIG_MODE		1
/*!	
 * selftest mode, must be set prior to loopback tests
 */
#define MVB_INITLEVEL_SELFTEST_MODE		2
/*!	
 * normal operational mode, must be set after all initialization took place.
 */
#define MVB_INITLEVEL_FULL_OP			3

/* Error Flags for Error reporting GetStats */
/*!	
 * Flag for errornous master frame
 */
#define MVB_COMM_ERR_EMF				(1<<0)
/*!	
 * Flag for errornous slave frame
 */
#define	MVB_COMM_ERR_ESF				(1<<1)
/*!	
 * Flag for duplicate master frame
 */
#define	MVB_COMM_ERR_DMF				(1<<2)
/*!	
 * Flag for duplicate slave frame
 */
#define	MVB_COMM_ERR_DSF				(1<<3)
/*!	
 * Flag for reply timeout interrupt
 */
#define	MVB_COMM_ERR_RTI				(1<<4)
/*!	
 * Flag for bus timeout interrupt, comes after 1,3ms without master frame
 */
#define	MVB_COMM_ERR_BTI				(1<<5)
/*!	
 * Flag for frames lost interrupt
 */
#define	MVB_COMM_ERR_FL					(1<<6)
/*!	
 * Flag for Asic/FPGA hang detection
 */
#define	MVB_COMM_ERR_HANG				(1<<7)

/*!
 *  memory configuration modes 
 */

#define MVB_MCM_MODE_16K				0
#define MVB_MCM_MODE_32K				1
#define MVB_MCM_MODE_64K				2	
#define MVB_MCM_MODE_256K				3
#define MVB_MCM_MODE_1M					4


/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
#ifdef _LL_DRV_
#ifdef _ONE_NAMESPACE_PER_DRIVER_
 extern void LL_GetEntry(LL_ENTRY* drvP);
#else
# ifdef ID_SW
 extern void PP04_GetEntry(LL_ENTRY* drvP);
# else
 extern void PP04_SW_GetEntry(LL_ENTRY* drvP);
# endif
#endif /* _ONE_NAMESPACE_PER_DRIVER_ */
#endif /* _LL_DRV_ */

/*-----------------------------------------+
|  BACKWARD COMPATIBILITY TO MDIS4         |
+-----------------------------------------*/
#ifndef U_INT32_OR_64
    /* we have an MDIS4 men_types.h and mdis_api.h included */
    /* only 32bit compatibility needed!                     */
    #define INT32_OR_64  int32
        #define U_INT32_OR_64 u_int32
    typedef INT32_OR_64  MDIS_PATH;
#endif /* U_INT32_OR_64 */


#ifdef __cplusplus
      }
#endif

#endif /* _PP04_DRV_H */


 
