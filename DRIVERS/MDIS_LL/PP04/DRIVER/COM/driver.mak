#***************************  M a k e f i l e  *******************************
#
#         Author: ts
#          $Date: 2005/06/28 16:51:14 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for the PP04 driver- SWAPPED access
#
#-----------------------------------------------------------------------------
#   Copyright (c) 2000-2019, MEN Mikro Elektronik GmbH
#*****************************************************************************

MAK_NAME=pp04

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)	


MAK_INCL=$(MEN_INC_DIR)/mvb_drv.h	\
	     $(MEN_MOD_DIR)/pp04_drv_int.h \
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/oss.h		\
         $(MEN_INC_DIR)/mdis_err.h	\
         $(MEN_INC_DIR)/mbuf.h		\
         $(MEN_INC_DIR)/maccess.h	\
         $(MEN_INC_DIR)/desc.h		\
         $(MEN_INC_DIR)/mdis_api.h	\
         $(MEN_INC_DIR)/mdis_com.h	\
         $(MEN_INC_DIR)/ll_defs.h	\
         $(MEN_INC_DIR)/ll_entry.h	\
         $(MEN_INC_DIR)/dbg.h		

MAK_INP1=pp04_drv$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)
