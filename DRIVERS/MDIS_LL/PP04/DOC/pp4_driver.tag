<?xml version='1.0' encoding='ISO-8859-1' standalone='yes'?>
<tagfile>
  <compound kind="page">
    <filename>index</filename>
    <title></title>
    <name>index</name>
    <docanchor>getblock</docanchor>
    <docanchor>nsdb</docanchor>
    <docanchor>Variants</docanchor>
    <docanchor>setblock</docanchor>
    <docanchor>Documents</docanchor>
    <docanchor>measurement</docanchor>
    <docanchor>interrupts</docanchor>
    <docanchor>mvb_simp</docanchor>
    <docanchor>General</docanchor>
    <docanchor>getstat_setstat_codes</docanchor>
    <docanchor>signals</docanchor>
    <docanchor>sinktime_supervision</docanchor>
    <docanchor>api_functions</docanchor>
    <docanchor>FuncDesc</docanchor>
    <docanchor>reading</docanchor>
    <docanchor>channels</docanchor>
    <docanchor>descriptor_entries</docanchor>
    <docanchor>writing</docanchor>
  </compound>
  <compound kind="file">
    <name>mvb_drv.h</name>
    <path>/opt/menlinux/INCLUDE/COM/MEN/</path>
    <filename>mvb__drv_8h</filename>
    <class kind="struct">G_MVBPORTS</class>
    <member kind="define">
      <type>#define</type>
      <name>MVB_PORTNAME_LEN</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_INITLEVEL</name>
      <anchor>a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_DISABLE_IMMEDIATE</name>
      <anchor>a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_SHUTDOWN_LINE_B</name>
      <anchor>a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_SHUTDOWN_LINE_A</name>
      <anchor>a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_DEVICE_ADDRESS</name>
      <anchor>a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_TRIGGER_WATCHDOG</name>
      <anchor>a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_COUNT</name>
      <anchor>a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_FLAGS</name>
      <anchor>a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_MASTER_FRAMES</name>
      <anchor>a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_SLAVE_FRAMES</name>
      <anchor>a10</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_DUPLICATE_MASTER_FRAMES</name>
      <anchor>a11</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_DUPLICATE_SLAVE_FRAMES</name>
      <anchor>a12</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_FRAMES_LOST</name>
      <anchor>a13</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_REPLY_TIMEOUT</name>
      <anchor>a14</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_BUS_TIMEOUT</name>
      <anchor>a15</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_ASIC_HANG</name>
      <anchor>a16</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_RESET_ERROR_COUNT</name>
      <anchor>a17</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_SINKTIME_SUPERVISION</name>
      <anchor>a18</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_PP_FC15</name>
      <anchor>a19</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_SET_SIGNAL_DATA</name>
      <anchor>a20</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_CLR_SIGNAL_DATA</name>
      <anchor>a21</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_SET_SIGNAL_ALERT</name>
      <anchor>a22</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_CLR_SIGNAL_ALERT</name>
      <anchor>a23</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_BUSADMIN</name>
      <anchor>a24</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_NOWDOG</name>
      <anchor>a25</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_BLK_NSDB</name>
      <anchor>a26</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_BLK_DIRECT</name>
      <anchor>a27</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_NO_ASIC</name>
      <anchor>a28</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_LOOPBACK_FAIL</name>
      <anchor>a29</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_ERR_LOOPBACK_TIMEOUT</name>
      <anchor>a30</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_DEFAULT_TIMEOUT</name>
      <anchor>a31</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_DEFAULT_WAITSTATES</name>
      <anchor>a32</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_DEFAULT_MCM</name>
      <anchor>a33</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_DEFAULT_FILTER</name>
      <anchor>a34</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_STS_TIME_OFF</name>
      <anchor>a35</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_STS_TIME_1MS</name>
      <anchor>a36</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_STS_TIME_2MS</name>
      <anchor>a37</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_STS_TIME_4MS</name>
      <anchor>a38</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_STS_TIME_8MS</name>
      <anchor>a39</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_STS_TIME_16MS</name>
      <anchor>a40</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_STS_TIME_32MS</name>
      <anchor>a41</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_STS_TIME_64MS</name>
      <anchor>a42</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_STS_TIME_128MS</name>
      <anchor>a43</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_STS_TIME_256MS</name>
      <anchor>a44</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_LA_PORT_SOURCE</name>
      <anchor>a45</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_LA_PORT_SINK</name>
      <anchor>a46</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_LA_SIZE_2</name>
      <anchor>a47</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_LA_SIZE_4</name>
      <anchor>a48</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_LA_SIZE_8</name>
      <anchor>a49</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_LA_SIZE_16</name>
      <anchor>a50</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_LA_SIZE_32</name>
      <anchor>a51</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_INITLEVEL_SW_RESET</name>
      <anchor>a52</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_INITLEVEL_CONFIG_MODE</name>
      <anchor>a53</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_INITLEVEL_SELFTEST_MODE</name>
      <anchor>a54</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_INITLEVEL_FULL_OP</name>
      <anchor>a55</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_COMM_ERR_EMF</name>
      <anchor>a56</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_COMM_ERR_ESF</name>
      <anchor>a57</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_COMM_ERR_DMF</name>
      <anchor>a58</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_COMM_ERR_DSF</name>
      <anchor>a59</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_COMM_ERR_RTI</name>
      <anchor>a60</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_COMM_ERR_BTI</name>
      <anchor>a61</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_COMM_ERR_FL</name>
      <anchor>a62</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_COMM_ERR_HANG</name>
      <anchor>a63</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_MCM_MODE_16K</name>
      <anchor>a64</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_MCM_MODE_32K</name>
      <anchor>a65</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_MCM_MODE_64K</name>
      <anchor>a66</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_MCM_MODE_256K</name>
      <anchor>a67</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_MCM_MODE_1M</name>
      <anchor>a68</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>INT32_OR_64</name>
      <anchor>a69</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>U_INT32_OR_64</name>
      <anchor>a70</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>G_MVBPORTS</type>
      <name>MVB_LA_PORT</name>
      <anchor>a71</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>INT32_OR_64</type>
      <name>MDIS_PATH</name>
      <anchor>a72</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>mvb_simp.c</name>
    <path>/opt/menlinux/TOOLS/MVB_SIMP/COM/</path>
    <filename>mvb__simp_8c</filename>
    <member kind="define">
      <type>#define</type>
      <name>CHK</name>
      <anchor>a0</anchor>
      <arglist>(expression)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MS_PER_SEC</name>
      <anchor>a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_DEVICE_ADDRESS_SLAVE_B</name>
      <anchor>a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_DEVICE_ADDRESS_SLAVE_A</name>
      <anchor>a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LP_PAD</name>
      <anchor>a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PORT_SIZE_MAX</name>
      <anchor>a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>WD_TIME</name>
      <anchor>a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_TX_TIME</name>
      <anchor>a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>void __MAPILIB</type>
      <name>SignalHandler</name>
      <anchor>a13</anchor>
      <arglist>(u_int32 sig)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>DumpMvbErrors</name>
      <anchor>a14</anchor>
      <arglist>(MDIS_PATH mdispath)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>Usage</name>
      <anchor>a15</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>DumpData</name>
      <anchor>a16</anchor>
      <arglist>(u_int8 *dat, u_int32 size, int32 nbrBytesRead)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>SetupData</name>
      <anchor>a17</anchor>
      <arglist>(u_int8 *dat, u_int32 size, int32 show)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchor>a18</anchor>
      <arglist>(int argc, char *argv[])</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>volatile u_int32</type>
      <name>G_getnewdata</name>
      <anchor>a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>volatile u_int32</type>
      <name>G_geterrors</name>
      <anchor>a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>volatile u_int32</type>
      <name>G_quit</name>
      <anchor>a10</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>volatile u_int32</type>
      <name>G_timeSec</name>
      <anchor>a11</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>MVB_LA_PORT</type>
      <name>G_ports</name>
      <anchor>a12</anchor>
      <arglist>[]</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>pp04_doc.c</name>
    <path>/opt/menlinux/DRIVERS/MDIS_LL/PP04/DOC/</path>
    <filename>pp04__doc_8c</filename>
  </compound>
  <compound kind="file">
    <name>pp04_drv.c</name>
    <path>/opt/menlinux/DRIVERS/MDIS_LL/PP04/DRIVER/COM/</path>
    <filename>pp04__drv_8c</filename>
    <member kind="define">
      <type>#define</type>
      <name>_NO_LL_HANDLE</name>
      <anchor>a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_MASTER_FRAME_COUNT</name>
      <anchor>a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_SMF_IMMEDIATE</name>
      <anchor>a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_SMF_TIMER1_CONTROLLED</name>
      <anchor>a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MAS_MR_INITVAL</name>
      <anchor>a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_MAS_TIMEBASE</name>
      <anchor>a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_FCODE_0</name>
      <anchor>a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_FCODE_1</name>
      <anchor>a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_FCODE_2</name>
      <anchor>a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_FCODE_3</name>
      <anchor>a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_FCODE_4</name>
      <anchor>a10</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MVB_MFT_START</name>
      <anchor>a11</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DBG_MYLEVEL</name>
      <anchor>a12</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DBH</name>
      <anchor>a13</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSH</name>
      <anchor>a14</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CHKPARM</name>
      <anchor>a15</anchor>
      <arglist>(expression)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PI2TMADDR</name>
      <anchor>a16</anchor>
      <arglist>(p_idx)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ADDRESS_IS_ODD</name>
      <anchor>a17</anchor>
      <arglist>(adr)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>PP04_Init</name>
      <anchor>a18</anchor>
      <arglist>(DESC_SPEC *descSpec, OSS_HANDLE *osHdl, MACCESS *ma, OSS_SEM_HANDLE *devSemHdl, OSS_IRQ_HANDLE *irqHdl, LL_HANDLE **llHdlP)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>PP04_Exit</name>
      <anchor>a19</anchor>
      <arglist>(LL_HANDLE **llHdlP)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>PP04_Read</name>
      <anchor>a20</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 ch, int32 *value)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>PP04_Write</name>
      <anchor>a21</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 ch, int32 value)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>PP04_SetStat</name>
      <anchor>a22</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 code, int32 ch, INT32_OR_64 value32_or_64)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>PP04_GetStat</name>
      <anchor>a23</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 code, int32 ch, INT32_OR_64 *value32_or_64)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>PP04_BlockRead</name>
      <anchor>a24</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size, int32 *nbrRdBytesP)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>PP04_BlockWrite</name>
      <anchor>a25</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size, int32 *nbrWrBytesP)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>PP04_Irq</name>
      <anchor>a26</anchor>
      <arglist>(LL_HANDLE *llHdl)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>PP04_Info</name>
      <anchor>a27</anchor>
      <arglist>(int32 infoType,...)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>char *</type>
      <name>Ident</name>
      <anchor>a28</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>Cleanup</name>
      <anchor>a29</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 retCode)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>MVB_EnableIrq</name>
      <anchor>a30</anchor>
      <arglist>(LL_HANDLE *llHdl, u_int16 irqFlag, u_int32 imrReg, u_int8 en)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>MVB_GetFrameErrorCount</name>
      <anchor>a31</anchor>
      <arglist>(LL_HANDLE *llHdl)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>MVB_SetInitializationLevel</name>
      <anchor>a32</anchor>
      <arglist>(LL_HANDLE *llHdl, u_int8 lvl)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>MVB_SAoffsetFromMCM</name>
      <anchor>a33</anchor>
      <arglist>(u_int8 mcm)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>MVB_ShutdownLineAB</name>
      <anchor>a34</anchor>
      <arglist>(LL_HANDLE *llHdl, u_int8 en, u_int8 line)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>MVB_FormatTM</name>
      <anchor>a35</anchor>
      <arglist>(LL_HANDLE *llHdl)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>MVB_SetupNSDB</name>
      <anchor>a36</anchor>
      <arglist>(LL_HANDLE *llHdl, MVB_LA_PORT *nsdbP, u_int16 nrPorts)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>MVB_PortInit</name>
      <anchor>a37</anchor>
      <arglist>(LL_HANDLE *llHdl, MVB_LA_PORT_NODE *portP)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>MVB_PerformDiagnose</name>
      <anchor>a38</anchor>
      <arglist>(LL_HANDLE *llHdl)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>MVB_SetupSinktimeSupervision</name>
      <anchor>a39</anchor>
      <arglist>(LL_HANDLE *llHdl, u_int16 interval)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>MVB_ResetErrorCount</name>
      <anchor>a40</anchor>
      <arglist>(LL_HANDLE *llHdl)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>MVB_GetSinktime</name>
      <anchor>a41</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 ch)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>int32</type>
      <name>MVB_SetDeviceStatusword</name>
      <anchor>a42</anchor>
      <arglist>(LL_HANDLE *llHdl, int32 value)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>u_int32</type>
      <name>MVB_SetupMemConfiguration</name>
      <anchor>a43</anchor>
      <arglist>(LL_HANDLE *llHdl)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>u_int8</type>
      <name>MVB_FcodeFromSize</name>
      <anchor>a44</anchor>
      <arglist>(u_int16 size)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>u_int32</type>
      <name>MVB_InitTMoffsets</name>
      <anchor>a45</anchor>
      <arglist>(LL_HANDLE *llHdl)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>u_int32</type>
      <name>MVB_ConfigDeviceAddress</name>
      <anchor>a46</anchor>
      <arglist>(LL_HANDLE *llHdl, u_int16 addr)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>MVB_Timer2Func</name>
      <anchor>a47</anchor>
      <arglist>(void *arg)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>void</type>
      <name>MVB_WDtriggerFunc</name>
      <anchor>a48</anchor>
      <arglist>(LL_HANDLE *llHdl)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>PP04_SW_GetEntry</name>
      <anchor>a49</anchor>
      <arglist>(LL_ENTRY *drvP)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>G_MVBPORTS</name>
    <filename>structG__MVBPORTS.html</filename>
    <member kind="variable">
      <type>u_int16</type>
      <name>addr</name>
      <anchor>m0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>u_int16</type>
      <name>index</name>
      <anchor>m1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>u_int16</type>
      <name>size</name>
      <anchor>m2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>portname</name>
      <anchor>m3</anchor>
      <arglist>[MVB_PORTNAME_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>u_int16</type>
      <name>srcsink</name>
      <anchor>m4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>u_int16</type>
      <name>tack</name>
      <anchor>m5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>u_int16</type>
      <name>compare</name>
      <anchor>m6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>u_int16</type>
      <name>pad</name>
      <anchor>m7</anchor>
      <arglist>[10]</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>pp04dummy</name>
    <title>MEN logo</title>
    <filename>pp04dummy</filename>
  </compound>
</tagfile>
