#***************************  M a k e f i l e  *******************************
#
#         Author: ts
#          $Date: 2005/06/28 16:51:19 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for the PP04 example program
#
#-----------------------------------------------------------------------------
#   Copyright (c) 2005-2019, MEN Mikro Elektronik GmbH
#*****************************************************************************

MAK_NAME=mvb_simp

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/mdis_api$(LIB_SUFFIX)	\
	        $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_utl$(LIB_SUFFIX)\
			$(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)

MAK_INCL=$(MEN_INC_DIR)/mvb_drv.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/mdis_api.h	\
         $(MEN_INC_DIR)/usr_oss.h   \
         $(MEN_INC_DIR)/usr_utl.h

MAK_INP1=mvb_simp$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)
