PP04_SW_1 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING PP04_SW

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING F205_1
    DEVICE_SLOT = U_INT32 0x0

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------

    # Memory Configuration Mode
    # 0 := 16kB TM Configuration
    # 1 := 32kB TM Configuration
    # 2 := 32kB TM Configuration
    # 3 := 256kB TM Configuration 
    # 4 := 1 MB TM Configuration
	MVB_MCM_MODE = U_INT32 2

    # Filter spikes up to 40 ns from Line A/B in harsh Environments
    # 0 := No filtering of spikes
    # 1 := Filter spikes up to 40 ns from Line A/B
	MVB_CONFIG_FILTER = U_INT32 0

    # Timout Coefficient for Telegram Analysis Unit
    # 0 := 21,3 us
    # 1 := 42,7 us
    # 2 := 64,0 us
    # 3 := 85,4 us
	MVB_TIMEOUT = U_INT32 1


	# minimum Waitstates inserted by Traffic Memory Controller for all TM accesses
    # 0 := 0 Waitstates inserted
    # 1 := 1 Waitstates inserted
    # 2 := 2 Waitstates inserted
    # 3 := 3 Waitstates inserted
	MVB_TM_WAITSTATES = U_INT32 3

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        this keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_MK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}
# EOF
 
