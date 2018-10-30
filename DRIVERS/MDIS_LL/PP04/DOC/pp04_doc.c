/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  pp04_doc.c
 *
 *      \author  $
 *        $Date: 2005/06/28 16:51:17 $
 *    $Revision: 1.1 $
 *
 *      \brief   User documentation for PP04 module driver
 *
 *     Required: -
 *
 *     \switches -
 */
 /*
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN Mikro Elektronik GmbH, Nuernberg, Germany
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

/*! \mainpage
    This is the documentation of the MDIS low-level driver for the PP04 module.
    The PP04 PCI104 Card is a MVB (Multifunction Vehicle Bus) Interface Card 
	that can participate on the IEC 61375 Bus as a slave device. The Card uses 
	the PCI104 form factor, it uses the MVBCS1 MVB Asic which performs 
	autonomous communication on the MVB side. The port data is exchanged 
	between MVBus and CPU by a memory used by both the Asic and the CPU, the 
	Traffic Memory.
    Its furthermore assumed that the reader of this documentation is familiar
    with the basic Concepts of MVB communication, the reference for it is the
    european standard IEC 61375. The terms like source port, sink port, logical
	port etc. are not explained in detail.
    
    \n
    \section Variants Variants
    The PP04 is a PCI104 Card, therefore its used together with 
    either CPUs with a direct PCI104 connector (e.g. ESMs) or the F207 cPCI
    to PCI104 adapter.
    
    \code
    Driver              Variant Description
    --------            --------------------------------
    Standard            D16 register access, non-swapped
    _sw D16             D16 register access, swapped
    \endcode

    Here are some combinations of MEN CPU and carrier boards together with the
    required variants:

    \code
    CPU                    Carrier Board          Driver Variant
    ----------------       -------------          --------------
    MEN PP1  (MPC5200)          -                 	swapped
	MEN EM4  (MPC8245)      	-					swapped
	MEN D3	 (MPC8245)     		F207               	swapped
    \endcode

    \n \section FuncDesc Functional Description

    The driver deals with the MVBCS1 Asic which is connected to traffic memory 
	and	the CPU with a 16bit wide data bus. Therefore, in the driver only the 
	16 bit access Macros are used. The driver takes care of initializing and 
	operating the MVB Asic properly	so the application can participate on MVB 
	traffic with few Get/SetStat commands.

	some main features of the driver are:
		- support for up to 4096 process data ports
		- error reporting capabilities for different problems on MVBus
		- possibility to automatically signal the application when new data is 
		  available
		- support for Sinktime supervision to prevent the application from 
		  reading of old sink port data from another device that may hang
		- automatic selftest of ASIC via loopback mode during every M_open.

    \n \subsection General General

	General description of device independent MVB Operation

	The main principles of MVB Communication is that a number of slaves is
	sending data on the bus after being polled from a Master device, the
	bus Administrator.\n
	A simple Bus administrator software for linux is provided with this driver
    on a "as-is" basis for own implementations.

    \n \subsection channels Logical channels

	The concept of MDIS logical channels was applied to this driver in the way
	that each logical channel that is e.g. selected with M_MK_CURR_CHANNEL 
	corresponds to the process datas port number. For example, the process
	port 42 is read by setting the current channel to 42 and then perform a
	M_getblock call. Furthermore only the term "Ports" is used, but means
	also a logical MDIS channel.
		
	In the concept of MVB, logical ports appear in 5 different sizes on
	the Bus, these are 2, 4, 8, 16 and 32 byte. Because of the limitation of
	standard MDIS M_read / M_write commands to transfer a maximum 32bit, all
	accesses to the Ports are done with M_getblock/M_setblock commands.
	
	Accesses to Ports can transfer a maximum of the number of bytes that is
	equal to the ports predefined size. Any smaller amount is possible.

    \n \subsection measurement communication principles
    
    An example program, mvb_simp, is delivered that demonstrates the basic
    setup for the PP04 to participate in MVB communication. 
    
    The basic steps to establish MVB communication are as follows:

    - open the MVB device with M_open. the tool m_open from the MDIS package
      can be used to ensure that the device was properly set up.
      
    - set the MVB device into initialization level with SetStat MVB_INIT_LEVEL

    - set the MVB device Adress with the MVB_DEVICE_ADDRESS SetStat

    - pass a NSDB database to the driver with all information about the
      ports to set up. 
      
      The port data is passed with M_setblock MVB_SETUP_NSDB. 
      
    - kick off communication by setting the MVB to full operation with the
      SetStat MVB_INIT_LEVEL.
      
    \n \subsection sinktime_supervision Sinktime Supervision
    
    The sinktime supervision controls the age of data delivered by a 
    sink port. When sinktime supervision is enabled via the 
	MVB_SINKTIME_INTERVAL SetStat, the TACK counter of each sinkport is 
	decremented and when the treshold value given in the ports NSDB definition
	is reached, any Read to this sink port fails.

    \n \subsection nsdb Passing process port info with MVB_BLK_NSDB
	
	The user application has to pass the NSDB to the LL driver with the
	MVB_BLK_NSDB Block SetStat. The M_SG_BLOCK data block has to consist of n 
	continuous MVB_LA_PORT structs in memory. For example, the user App may 
	define 10 ports with 
	\code
	MVB_LA_PORT myMVBports[10];
	\endcode
	and pass these as shown in the mvb_simp program.
	The struct MVB_LA_PORT is defined as follows:\n
	\code 
	struct G_MVBPORTS{
	u_int16		addr;		port Number, as in Master Frame
	u_int16		index;		port index, used as port index to traffic mem
	u_int16		size;		2, 4, 8, 16 or 32 byte
	char		portname[];	ident string, zero terminated  
	u_int16		srcsink;	0 for source port, or non zero for sink	
	u_int16		tack;		sinktime supervision treshold  			
	u_int16		compare;	if nonzero: driver signals if this sinkport
	                        data have changed, differing from before
	u_int16		pad[];		aligning struct to 64 byte size 			
	} MVB_LA_PORT;
	\endcode

    \n \subsection reading Reading the Data
    
    \n \subsubsection getblock Using M_getblock()

	MVB sinkport Data is fetched from the device with a block read done with
	M_getblock. Because the size of ports can be up to 256 bit, the standard
	M_read/M_write functions are not appropriate. The typical call looks
	like
	\code
	M_setstat( path, M_MK_CH_CURRENT, portAddress);
	bytesRead = M_getblock(path, buf, portSize);
	\endcode
	The current channel is set to the sink ports address (=channel number),
	then the block read is done.
	if everything went ok, the read number of bytes is equal to the sinkports
	size. Several errors may occur upon which the M_getblock returns 0 or -1:

	- the port is a source port, not a sink
	  This is not allowed since the MVB Asic and the CPU would access the same
	  page in TM.
	- the sinktime expired
	  when sinktime Supervision is on, the ports TACK value is checked and
	  compared to the given treshold (see MVB_LA_PORT definition).

	Its not possible to read more then port size bytes. If a larger
	value is passed for size, then port size bytes are returned.

    \n \subsection writing Writing the Data
    
    \n \subsubsection setblock Using M_setblock()

	MVB sourceport Data is written to the device with a block write done with
	M_setblock. Because the size of ports can be up to 256 bit, the standard
	M_read/M_write functions are not appropriate. The typical call looks
	like
	\code
	M_setstat( path, M_MK_CH_CURRENT, portAddress);
	bytesWritten = M_setblock(path, buf, portSize);
	\endcode
	The current channel is set to the sink ports address (=channel number),
	then the block read is done.
	if everything went ok, the read number of bytes is equal to the sinkports
	size. Several errors may occur upon which the M_setblock returns 0 or -1:

	- the port is a sink port, not a source
	  This is not allowed since the MVB Asic and the CPU would access the same
	  page in TM.
	- the port doesnt exist

	Its not possible to write more then \c port \c size bytes. If a larger
	value is passed for size, then \c port \c size bytes are written.

	
    \n \section interrupts Interrupts
    The driver supports interrupts from the PP4. The Modules interrupt
    can be enabled/disabled through the M_MK_IRQ_ENABLE SetStat code.

    Each MVB interrupt can trigger the following 2 actions:

    - send a definable user signal to the application to alert it about
	  unnormal or errornous MVB behaviour. The error conditions can then
	  be retrieved with the appropreate GetStat codes.

    - send a definable user signal to the application to tell it that the
	  data in a Sinkport has changed. This is useful if the application wants
	  to be informed about changes in data from an important sink port.

    
    \n \section signals Signals
    The driver can send signals to notify the application of the end of
    measurement. The signal must be activated via the PP04_SIG_SET SetStat
    code and can be cleared through SetStat PP04_SIG_CLR.


    \n \section api_functions Supported API Functions
    <table border="0">
    <tr>
        <td><b>API function</b></td>
        <td><b>Functionality</b></td>
        <td><b>Corresponding low level function</b></td></tr>

    <tr><td>M_open()</td><td>Open device</td><td>PP04_Init()</td></tr>
    <tr><td>M_close()     </td><td>Close device             </td>
    <td>PP04_Exit())</td></tr>
    <tr><td>M_read()      </td><td>Read from device         </td>
    <td>PP04_Read()</td></tr>
    <tr><td>M_write()     </td><td>Write to device          </td>
    <td>PP04_Write()</td></tr>
    <tr><td>M_setstat()   </td><td>Set device parameter     </td>
    <td>PP04_SetStat()</td></tr>
    <tr><td>M_getstat()   </td><td>Get device parameter     </td>
    <td>PP04_GetStat()</td></tr>
    <tr><td>M_getblock()  </td><td>Block read from device   </td>
    <td>PP04_BlockRead()</td></tr>
    <tr><td>M_setblock()  </td><td>Block write from device  </td>
    <td>PP04_BlockWrite()</td></tr>
    <tr><td>M_errstringTs() </td><td>Generate error message </td>
    <td>-</td></tr>
    </table>

    \n \section descriptor_entries Descriptor Entries
    
    The low-level driver initialization routine decodes the following entries
    ("keys") in addition to the general descriptor keys:
    
    <table border="0">
    <tr><td><b>Descriptor entry</b></td>
        <td><b>Description</b></td>
        <td><b>Values</b></td>
    </tr>
    <tr><td>MVB_MCM_MODE</td>
        <td>Memory Configuration Mode</td>
        <td>0..4, default: 0</td>
    </tr>
    <tr><td>MVB_CONFIG_FILTER</td>
        <td>If spikes of up to 40ns shall be filtered from Inputs</td>
        <td>0..1, default: 0</td>
    </tr>
    <tr><td>MVB_TIMEOUT</td>
        <td>Reply timeout value: 21,3 us, 42,7 us 64,0 us or 85,4 us</td>
        <td>0..3, default: 1</td>
    </tr>
    </table>    

	\n \section getstat_setstat_codes
    
	an Overview of all MVB Get/Setstats can be found in file mvb_drv.h

    \n \section Documents Overview of all Documents

    \subsection mvb_simp  Tool for demonstrating MVB Slave Communication
    %mxx_xyz.c

*/

/** \example mvb_simp.c
Simple example for driver usage
*/

/*! \page dummy
  \menimages
*/

