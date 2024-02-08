namespace sf
{

const char* IniContent = R"(

[GenericParamInfo]
Entries=258
0=0x43C01,Motion|Galil 21x2 - NIP|Version,,RAS,Version of hardware and driver,STRING,,60,,,
1=0x43C11,Motion|Galil 21x2 - NIP|Trigger|Time Unit,s,RAS,Time in seconds that is used for the time dependant results.,FLOAT,,1e-06,0.01,1e-06,100000000
2=0x43E02,Motion|Galil 21x2 - NIP|System|Firmware Revision,,RAS,Identification of revision of embedded application,STRING,,60,R1.2 2005/01/04 18:45:51,,
3=0xE0001,Storage|Filename,F,ASP,Name for the file to be stored. This is without extension.,STRING,,256,Unknown,,
4=0xE0009,Storage|Sub Directory,S,ASP,Sub directory name for the file to be stored.,STRING,,256,,,
5=0xE0007,Storage|Directory,D,ASPE,Directory to store the data in.,STRING,,256,C:\DOCUME~1\bcbdebug\LOCALS~1\Temp\,,
6=0xE000A,Storage|File Number,#,RASE,Next file number.,STRING,,0,1,0,999,Unknown=0
7=0xE0003,Storage|Compression,!,ASPE,File format compatibility/ compression mode.,INTEGER,,1,0,0,2,None=0,ZLIB_FAST=1,ZLIB_BEST=2
8=0xE0006,Storage|Disk Space,kB,RASE,Free diskspace of current file drive,FLOAT,,1,0,0,0
9=0xE0008,Storage|Logfile Path,P,ASPE,File path to the log file which determines the next file number.,STRING,,256,C:\DOCUME~1\bcbdebug\LOCALS~1\Temp\stogii32,,
10=0xBF201,Uspc2100|Error,!,RASE,Errorcode,INTEGER,,1,0,0,1,No errors=0,Failure=1
11=0xBF096,Uspc2100|Pulser|Rep.Rate,Hz,ASEW,Pulser Repetition Rate,INTEGER,,1,500,1,10000
12=0xBF221,Uspc2100|Time Unit,s,RASE,Time unit of the digitiser sample rate.,FLOAT,,1e-08,1e-08,1e-08,1e-08
13=0xB0110,Uspc2100|Receiver|TCG|Gain|00,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
14=0xB0100,Uspc2100|Receiver|TCG|Time|00,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,0,0,320
15=0xB0111,Uspc2100|Receiver|TCG|Gain|01,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
16=0xB0101,Uspc2100|Receiver|TCG|Time|01,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,5,0,320
17=0xB0112,Uspc2100|Receiver|TCG|Gain|02,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
18=0xB0102,Uspc2100|Receiver|TCG|Time|02,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,10,0,320
19=0xB0113,Uspc2100|Receiver|TCG|Gain|03,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
20=0xB0103,Uspc2100|Receiver|TCG|Time|03,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,15,0,320
21=0xB0114,Uspc2100|Receiver|TCG|Gain|04,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
22=0xB0104,Uspc2100|Receiver|TCG|Time|04,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,20,0,320
23=0xB0115,Uspc2100|Receiver|TCG|Gain|05,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
24=0xB0105,Uspc2100|Receiver|TCG|Time|05,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,25,0,320
25=0xB0116,Uspc2100|Receiver|TCG|Gain|06,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
26=0xB0106,Uspc2100|Receiver|TCG|Time|06,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,30,0,320
27=0xB0117,Uspc2100|Receiver|TCG|Gain|07,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
28=0xB0107,Uspc2100|Receiver|TCG|Time|07,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,35,0,320
29=0xB0118,Uspc2100|Receiver|TCG|Gain|08,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
30=0xB0108,Uspc2100|Receiver|TCG|Time|08,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,40,0,320
31=0xB0119,Uspc2100|Receiver|TCG|Gain|09,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
32=0xB0109,Uspc2100|Receiver|TCG|Time|09,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,45,0,320
33=0xB011A,Uspc2100|Receiver|TCG|Gain|10,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
34=0xB010A,Uspc2100|Receiver|TCG|Time|10,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,50,0,320
35=0xB011B,Uspc2100|Receiver|TCG|Gain|11,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
36=0xB010B,Uspc2100|Receiver|TCG|Time|11,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,55,0,320
37=0xB011C,Uspc2100|Receiver|TCG|Gain|12,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
38=0xB010C,Uspc2100|Receiver|TCG|Time|12,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,60,0,320
39=0xB011D,Uspc2100|Receiver|TCG|Gain|13,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
40=0xB010D,Uspc2100|Receiver|TCG|Time|13,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,65,0,320
41=0xB011E,Uspc2100|Receiver|TCG|Gain|14,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
42=0xB010E,Uspc2100|Receiver|TCG|Time|14,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,70,0,320
43=0xB011F,Uspc2100|Receiver|TCG|Gain|15,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
44=0xB010F,Uspc2100|Receiver|TCG|Time|15,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,75,0,320
45=0xB0000,Uspc2100|A-scan|Start Mode,,ASEW,Source of start trigger for A-scan display,INTEGER,,1,0,0,2,Initial Pulse=0,Artificial=1,Interface=2
46=0xB0001,Uspc2100|A-scan|Delay,us,ASEW,Delay in us,FLOAT,FLOAT,0.01,202.1,-10,310
47=0xB0002,Uspc2100|A-scan|Range,us,ASEW,Range in us,FLOAT,FLOAT,0.01,20.8,1,321
48=0xB0003,Uspc2100|A-scan|Display Mode,,ASEW,Rectification for A-scan display,INTEGER,,1,1,0,3,RF=0,Full=1,Pos=2,Neg=3
49=0xB0004,Uspc2100|A-scan|Reject,%,ASEW,Ascan Reject,FLOAT,FLOAT,1,0,0,100
50=0xB0005,Uspc2100|A-scan|Stream,,ASEW,Ascan Streaming Modes,INTEGER,,1,0,0,2,Disabled=0,Enabled=1,Gates=2
51=0xB0006,Uspc2100|A-scan|Step Gate Display,,ASEW,Ascan Step gate display mode,INTEGER,,1,1,0,1,Off=0,On=1
52=0xB0212,Uspc2100|System|One SHot,,ASEW,Generates one shot of data,INTEGER,,1,1,0,1,Off=0,On=1
53=0xB0030,Uspc2100|IF Gate|Delay,us,ASEW,Interface Gate Delay,FLOAT,,0.02,212.12,0,320
54=0xB0031,Uspc2100|IF Gate|Range,us,ASEW,Interface Gate Range,FLOAT,,0.02,8.35,0,320
55=0xB0032,Uspc2100|IF Gate|Threshold,%,ASEW,Interface Gate Flank Threshold,INTEGER,,1,80,0,100
56=0xB004A,Uspc2100|IF Gate|Trigger Mode,,ASEW,Source of trigger for Interface Gate,INTEGER,,1,0,0,1,Peak=0,Flank=1
57=0xB004C,Uspc2100|IF Gate|Rectify,,ASEW,Interface Gate rectify mode,INTEGER,,1,1,0,3,RF=0,Full=1,Pos=2,Neg=3
58=0xB0033,Uspc2100|WallThickness|Average Window,?,ASEW,WallThickness|Average Window,INTEGER,,1,0,0,100
59=0xB0035,Uspc2100|WallThickness|Start,,ASEW,Source of Wallthickness measurement START position,INTEGER,,1,0,0,3,IF=0,G1=1,G2(not used)=2,IP=3
60=0xB0036,Uspc2100|WallThickness|Stop,,ASEW,Source of Wallthickness measurement STOP position,INTEGER,,1,0,0,3,IF(not used)=0,G1=1,G2=2,IP=3
61=0xB004B,Uspc2100|WallThickness|Accumulate Mode,,ASEW,WallThickness Accumulate mode,INTEGER,,1,0,0,2,External=0,Sample=1,Instantaneous=2
62=0xB004D,Uspc2100|WallThickness|Noise Immun Count,,ASEW,WallThickness Noise Immun count,INTEGER,,1,0,0,15
63=0xB004E,Uspc2100|WallThickness|Min Limit,S,ASEW,WallThickness Min Limit,FLOAT,,0.2,0.2,0,5000
64=0xB004F,Uspc2100|WallThickness|Max Limit,S,ASEW,WallThickness Max Limit,FLOAT,,0.2,0.2,0,5000
65=0xB0050,Uspc2100|WallThickness|Num Echos,,ASEW,WallThickness Number of echos used,INTEGER,,1,2,1,4
66=0xB0051,Uspc2100|WallThickness|Zero Offset,S,ASEW,WallThickness Zero Offset,FLOAT,,0.2,0.2,0,5000
67=0xB0037,Uspc2100|Gates|Mode,,ASEW,Type of measurement for Gate(s),INTEGER,,1,1,1,2,Flaw=1,Thick=2
68=0xB0034,Uspc2100|Gates|Start Mode,,ASEW,Source of start trigger for Gate(s),INTEGER,,1,2,0,2,Initial Pulse=0,Artificial=1,Interface=2
69=0xB0060,Uspc2100|System|TDR,,ASEW,Function of external pin for measurement control,INTEGER,,1,0,0,3,Ignore_Enabled_output=0,Ignore_Disabled_output=1,Enable_High_Active=2,Enable_Low_Active=3
70=0xB0063,Uspc2100|Analog Out 1|Source out,,ASEW,Analog Output 1 signal source,INTEGER,,1,0,0,1,AMP_G1=0,AMP_G2=1
71=0xB0064,Uspc2100|Analog Out 1|Offset,%,ASEW,Analog Output 1 signal offset ,FLOAT,,0.1,0,0,127
72=0xB0065,Uspc2100|Analog Out 1|Range,%,ASEW,Analog Output 1 signal range,FLOAT,,0.1,5,0,127
73=0xB0067,Uspc2100|Analog Out 1|Timing,,ASEW,Analog Output 1 signal Timing,INTEGER,,1,1,0,2,Instant=0,Timed Max=1,Timed Min=2
74=0xB0066,Uspc2100|Analog Out 1|Muting,%,ASEW,Analog Output 1 muting,FLOAT,FLOAT,0.1,0,0,127
75=0xB0068,Uspc2100|Analog Out 2|Source out,,ASEW,Analog Output 2 signal source,INTEGER,,1,0,0,1,AMP_G1=0,AMP_G2=1
76=0xB0069,Uspc2100|Analog Out 2|Offset,%,ASEW,Analog Output 2 signal offset ,FLOAT,,0.1,0,0,127
77=0xB006A,Uspc2100|Analog Out 2|Range,%,ASEW,Analog Output 2 signal range,FLOAT,,0.1,5,0,127
78=0xB006C,Uspc2100|Analog Out 2|Timing,,ASEW,Analog Output 2 signal Timing,INTEGER,,1,1,0,2,Instant=0,Timed Max=1,Timed Min=2
79=0xB006B,Uspc2100|Analog Out 2|Muting,%,ASEW,Analog Output 2 muting,FLOAT,,0.1,0,0,127
80=0xB0222,Uspc2100|System|Version,Normal,RASE,All combined versions.,STRING,,256,<n/a>,<n/a>,
81=0xB0097,Uspc2100|Pulser|Sync Mode,,ASEW,Pulser sync mode,INTEGER,,1,1,0,1,Internal=0,External=1
82=0xB009C,Uspc2100|Pulser|Test Mode,,ASEW,Pulser probe test mode,INTEGER,,1,0,0,1,Single=0,Dual=1
83=0xB009E,Uspc2100|Pulser|Energy,,ASEW,Pulser Energy Level,INTEGER,,1,1,0,1,Low=0,High=1
84=0xB009F,Uspc2100|Pulser|Voltage,,ASEW,Pulser voltage level,INTEGER,,1,1,0,1,Low=0,High=1
85=0xB0098,Uspc2100|Receiver|Gain,dB,ASEW,Receiver gain,FLOAT,FLOAT,0.1,70,0,110
86=0xB009A,Uspc2100|Receiver|TCG|Enable,,ASEW,Receiver TCG Enable,INTEGER,,1,0,0,1,Off=0,On=1
87=0xB0223,Uspc2100|Receiver|TCG|Delay,us,ASEW,Receiver TCG delay time.,FLOAT,,0.05,0,0,320
88=0xB00A0,Uspc2100|Multiplexer|Table|Count,,ASEW,Number of Cyle Table Entries,INTEGER,,1,1,0,15
89=0xB0093,Uspc2100|Multiplexer|TransmitCycleTable,,ASEW,TransmitCycleTable,INTEGER,,1,65535,1,65535
90=0xB0094,Uspc2100|Multiplexer|ReceiveCycleTable,,ASEW,ReceiveCycleTable,INTEGER,,1,65535,1,65535
91=0xB0099,Uspc2100|Receiver|RF Filter,,ASEW,Receiver RF Filter selected,INTEGER,,1,4,0,4,1MHz=0,2.25MHz=1,5MHz=2,10MHz=3,BB=4
92=0xB009D,Uspc2100|Pulser|Damping,,ASEW,Pulser Damping,INTEGER,,1,2,0,2,500 Ohm=0,75 Ohm=1,50 Ohm=2
93=0xB0038,Uspc2100|Gate 1|Delay,us,ASEW,Gate 1 Delay,FLOAT,,0.02,1.45,0.18,320
94=0xB0039,Uspc2100|Gate 1|Range,us,ASEW,Gate 1 Range,FLOAT,,0.02,16.7,0.18,320
95=0xB003A,Uspc2100|Gate 1|Threshold,%,ASEW,Gate 1 Threshold,INTEGER,,1,30,0,100
96=0xB003B,Uspc2100|Gate 1|Noise Immun count,,ASEW,Gate 1 Noise Immun count,INTEGER,,1,1,0,15
97=0xB003C,Uspc2100|Gate 1|Average Window,,ASEW,Gate 1 Average Window,INTEGER,,1,1,0,65534
98=0xB003D,Uspc2100|Gate 1|Trigger Mode,,ASEW,Source of trigger for Gate 1,INTEGER,,1,0,0,2,Peak=0,Flank=1,Zero Crossing=2
99=0xB0044,Uspc2100|Gate 1|Accumulate Reset,,ASEW,Gate 1 Accumulate reset,INTEGER,,1,0,0,1,Off=0,Reset=1
100=0xB0046,Uspc2100|Gate 1|Rectify,,ASEW,Gate 1 rectify mode,INTEGER,,1,1,0,3,RF=0,Full=1,Pos=2,Neg=3
101=0xB0048,Uspc2100|Gate 1|Accumulate Mode,,ASEW,Gate 1 Accumulate mode,INTEGER,,1,2,0,2,External=0,Sample=1,Instantaneous=2
102=0xB003E,Uspc2100|Gate 2|Delay,us,ASEW,Gate 2 Delay,FLOAT,,0.02,1.45,0.18,320
103=0xB003F,Uspc2100|Gate 2|Range,us,ASEW,Gate 2 Range,FLOAT,,0.02,16.7,0.18,320
104=0xB0040,Uspc2100|Gate 2|Threshold,%,ASEW,Gate 2 Threshold,INTEGER,,1,30,0,100
105=0xB0041,Uspc2100|Gate 2|Noise Immun count,,ASEW,Gate 2 Noise Immun count,INTEGER,,1,1,0,15
106=0xB0042,Uspc2100|Gate 2|Average Window,,ASEW,Gate 2 Average Window,INTEGER,,1,1,0,65534
107=0xB0043,Uspc2100|Gate 2|Trigger Mode,,ASEW,Source of trigger for Gate 2,INTEGER,,1,0,0,2,Peak=0,Flank=1,Zero Crossing=2
108=0xB0045,Uspc2100|Gate 2|Accumulate Reset,,ASEW,Gate 2 Accumulate reset,INTEGER,,1,0,0,1,Off=0,Reset=1
109=0xB0047,Uspc2100|Gate 2|Rectify,,ASEW,Gate 2 rectify mode,INTEGER,,1,1,0,3,RF=0,Full=1,Pos=2,Neg=3
110=0xB0049,Uspc2100|Gate 2|Accumulate Mode,,ASEW,Gate 2 Accumulate mode,INTEGER,,1,2,0,2,External=0,Sample=1,Instantaneous=2
111=0x30002,Eddy Current|Channels,!,ASP,Eddy Current Number of available channels.,INTEGER,,1,1,1,8
112=0x30005,Eddy Current|Amplitude|Unit,V,RAS,Eddy Current Translates one unit of amplitude to a value.,FLOAT,,1e-99,1,1e-99,1000000000000
113=0x30401,Eddy Current|Channel|Trigger|Rep.Rate,Hz,ASP,Eddy Current Repetition rate for the internal trigger.,INTEGER,,1,1000,1,20000
114=0x30402,Eddy Current|Channel|Trigger|Sync Mode,!,ASP,Eddy Current Sync signal internal/external selection.,INTEGER,,1,1,0,1,Internal=0,External=1
115=0x30405,Eddy Current|Channel|Trigger|Time Unit,s,RAS,Eddy Current Time in seconds that is used for the time dependant results.,FLOAT,,1e-06,0.01,1e-06,100000000
116=0x30403,Eddy Current|Channel|Gates,!,RAS,Eddy Current Number of available gates.,INTEGER,,1,4,4,4
117=0x30522,Eddy Current|Channel|X|Attenuator,,AS,Eddy Current X Input sensitivity ±,INTEGER,,1,1,0,4,±10.0V=0,±5.0V=1,±2.5mV=2,±1.25V=3
118=0x30526,Eddy Current|Channel|X|Rectifier,!,AS,Eddy Current X Rectifier,INTEGER,,1,0,0,3,RF=0,FULL=1,1/2 POS=2,1/2 NEG=3
119=0x30520,Eddy Current|Channel|X|Calibration|Invert,!,AS,Eddy Current X Calibration inverter,INTEGER,,1,0,0,1,Non invert=0,Invert=1
120=0x30524,Eddy Current|Channel|X|Calibration|Offset,,AS,Eddy Current X Calibration offset.,FLOAT,,1,0,-2.5,2.5
121=0x30525,Eddy Current|Channel|X|Calibration|Scale,,AS,Eddy Current X Calibration scale or digital input zoom.,FLOAT,,0.001,1,1,100
122=0x3052C,Eddy Current|Channel|X|Amplitude|Unit,,RAS,Eddy Current X Translates one unit of amplitude to a value.,FLOAT,,1e-99,0.00122100122100122,1e-99,1000000000000
123=0x3052D,Eddy Current|Channel|X|Amplitude|Unit Select,,ASP,Eddy Current X Selectes Amplitude Unit.,INTEGER,,1,0,0,3,Default=0,Percentage=1,Volts=2,Meter=3
124=0x30505,Eddy Current|Channel|X|Enable,!,ASP,Eddy Current X Result Enabler.,INTEGER,,1,1,0,1,Off=0,On=1
125=0x30622,Eddy Current|Channel|Y|Attenuator,,AS,Eddy Current Y Input sensitivity ╠,INTEGER,,1,1,0,4,╠10.0V=0,╠5.0V=1,╠2.5mV=2,╠1.25V=3
126=0x30626,Eddy Current|Channel|Y|Rectifier,!,AS,Eddy Current Y Rectifier,INTEGER,,1,0,0,3,RF=0,FULL=1,1/2 POS=2,1/2 NEG=3
127=0x30620,Eddy Current|Channel|Y|Calibration|Invert,!,AS,Eddy Current Y Calibration inverter,INTEGER,,1,0,0,1,Non invert=0,Invert=1
128=0x30624,Eddy Current|Channel|Y|Calibration|Offset,,AS,Eddy Current Y Calibration offset.,FLOAT,,1,0,-2.5,2.5
129=0x30625,Eddy Current|Channel|Y|Calibration|Scale,,AS,Eddy Current Y Calibration scale or digital input zoom.,FLOAT,,0.001,1,1,100
130=0x3062C,Eddy Current|Channel|Y|Amplitude|Unit,,RAS,Eddy Current Y Translates one unit of amplitude to a value.,FLOAT,,1e-99,0.00122100122100122,1e-99,1000000000000
131=0x3062D,Eddy Current|Channel|Y|Amplitude|Unit Select,,ASP,Eddy Current Y Selectes Amplitude Unit.,INTEGER,,1,0,0,3,Default=0,Percentage=1,Volts=2,Meter=3
132=0x30605,Eddy Current|Channel|Y|Enable,!,ASP,Eddy Current Y Result Enabler.,INTEGER,,1,1,0,1,Off=0,On=1
133=0x30210,Eddy Current|Instrument|Identification,,RAS,Eddy Current Name parameter. [SV ,STRING,,80,,,
134=0x30212,Eddy Current|Instrument|Frequency,Hz,ASP,Eddy Current Driver frequency. [B] ,FLOAT,,1,2000000,10,10000000
135=0x30213,Eddy Current|Instrument|Pre-Gain,,ASP,Eddy Current Preamplifier gain. [B] ,INTEGER,,1,2,0,6,-20 dB=0,-10 dB=1,0 dB=2,10 dB=3,20 dB=4,30 dB=5,40 dB=6
136=0x30214,Eddy Current|Instrument|Gain,dB,ASP,Eddy Current Main Amplifier gain. [B] ,INTEGER,,1,20,0,60
137=0x30215,Eddy Current|Instrument|Vertical Spread,dB,ASP,Eddy Current Vertical spread. [B] ,INTEGER,,1,0,0,20
138=0x30216,Eddy Current|Instrument|Horizontal Spread,dB,ASP,Eddy Current Horizontal spread. [B] ,INTEGER,,1,0,0,20
139=0x30217,Eddy Current|Instrument|Phase,Deg,ASP,Eddy Current Phase. [B] ,INTEGER,,1,0,0,359
140=0x30218,Eddy Current|Instrument|Bandwidth Limit,,ASP,Eddy Current Bandwidth Limit. [B] ,INTEGER,,1,0,0,2,On=0,Off=1
141=0x30219,Eddy Current|Instrument|HP Filter,,ASP,Eddy Current High pass filter. [B] ,INTEGER,,1,0,0,40,0.5 Hz=0,1.5 Hz=1,2.1 Hz=2,2.8 Hz=3,3.5 Hz=4,4 Hz=5,5 Hz=6,6.5 Hz=7,8 Hz=8,10 Hz=9,12.5 Hz=10,16 Hz=11,20 Hz=12,25 Hz=13,32 Hz=14,40 Hz=15,50 Hz=16,65 Hz=17,80 Hz=18,100 Hz=19,125 Hz=20,160 Hz=21,200 Hz=22,250 Hz=23,300 Hz=24,400 Hz=25,500 Hz=26,650 Hz=27,800 Hz=28,1 kHz=29,1.25 kHz=30,1.6 kHz=31,2 kHz=32,2.5 kHz=33,3.2 kHz=34
142=0x3021A,Eddy Current|Instrument|LP Filter,,ASP,Eddy Current Low pass filter. [B] ,INTEGER,,1,0,0,39,0.5 Hz=0,1.5 Hz=1,2.1 Hz=2,2.8 Hz=3,3.5 Hz=4,4 Hz=5,5 Hz=6,6.5 Hz=7,8 Hz=8,10 Hz=9,12.5 Hz=10,16 Hz=11,20 Hz=12,25 Hz=13,32 Hz=14,40 Hz=15,50 Hz=16,65 Hz=17,80 Hz=18,100 Hz=19,125 Hz=20,160 Hz=21,200 Hz=22,250 Hz=23,300 Hz=24,400 Hz=25,500 Hz=26,650 Hz=27,800 Hz=28,1 kHz=29,1.25 kHz=30,1.6 kHz=31,2 kHz=32,2.5 kHz=33,3.2 kHz=34
143=0x3021C,Eddy Current|Instrument|?,units,RAS,Eddy Current ? [?] ,UNDEF,,,,,
144=0x99003,Project|Sound Velocity|Medium,m/s,AS,Sound velocity of the medium,FLOAT,,1,1480,100,10000
145=0x99004,Project|Sound Velocity|Material,m/s,AS,Sound velocity of the inspected material,FLOAT,,1,5820,100,10000
146=0x99005,Project|System|Time,s,AS,Current time,STRING,,100,0,0,0
147=0x99006,Project|System|Date,s,AS,Current Date,STRING,,100,0,0,0
148=0x99201,Project|Ultrasonic|Threshold,%,ASP,UT C-scan display threshold.,FLOAT,,1,30,0,100
149=0x99214,Project|Eddy Current|Peak-to-Peak|Threshold,V,ASP,ET Peak-Peak C-scan display threshold.,FLOAT,,0.01,2,0,4
150=0x9E705,Ultrasonic|Receiver|Gain|Scan|Active,,AS,Ultrasonic receiver TCG Receiver Scanning gain active.,INTEGER,,1,0,0,1,Off=0,On=1
151=0x9E706,Ultrasonic|Receiver|Gain|Scan|Gain,dB,ASP,Ultrasonic receiver TCG Receiver Scanning Gain Correction value.,FLOAT,,0.1,0,-110,110
152=0x9E707,Ultrasonic|Receiver|Gain|TCG Corr|Active,,AS,Ultrasonic receiver TCG Receiver TCG Gain Correction Scanning gain active.,INTEGER,,1,0,0,1,Off=0,On=1
153=0x9E708,Ultrasonic|Receiver|Gain|Main,dB,ASP,Ultrasonic receiver TCG Receiver Gain value before correction with scanning gain.,FLOAT,,0.1,70,0,110
154=0x9E701,Ultrasonic|Receiver|Gain|Target Amplitude,%,ASP,Ultrasonic receiver TCG target amplitude.,INTEGER,,1,80,0,128
155=0x9E711,Ultrasonic|Receiver|TCG|Point  1|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 1 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
156=0x9E721,Ultrasonic|Receiver|TCG|Point  2|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 2 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
157=0x9E731,Ultrasonic|Receiver|TCG|Point  3|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 3 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
158=0x9E741,Ultrasonic|Receiver|TCG|Point  4|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 4 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
159=0x9E751,Ultrasonic|Receiver|TCG|Point  5|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 5 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
160=0x9E761,Ultrasonic|Receiver|TCG|Point  6|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 6 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
161=0x9E771,Ultrasonic|Receiver|TCG|Point  7|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 7 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
162=0x9E781,Ultrasonic|Receiver|TCG|Point  8|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 8 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
163=0x9E791,Ultrasonic|Receiver|TCG|Point  9|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 9 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
164=0x9E7A1,Ultrasonic|Receiver|TCG|Point 10|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 10 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
165=0x9E7B1,Ultrasonic|Receiver|TCG|Point 11|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 11 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
166=0x9E7C1,Ultrasonic|Receiver|TCG|Point 12|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 12 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
167=0x9E7D1,Ultrasonic|Receiver|TCG|Point 13|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 13 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
168=0x9E7E1,Ultrasonic|Receiver|TCG|Point 14|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 14 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
169=0x9E7F1,Ultrasonic|Receiver|TCG|Point 15|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 15 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
170=0x9E712,Ultrasonic|Receiver|TCG|Point  1|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 1 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
171=0x9E722,Ultrasonic|Receiver|TCG|Point  2|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 2 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
172=0x9E732,Ultrasonic|Receiver|TCG|Point  3|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 3 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
173=0x9E742,Ultrasonic|Receiver|TCG|Point  4|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 4 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
174=0x9E752,Ultrasonic|Receiver|TCG|Point  5|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 5 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
175=0x9E762,Ultrasonic|Receiver|TCG|Point  6|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 6 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
176=0x9E772,Ultrasonic|Receiver|TCG|Point  7|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 7 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
177=0x9E782,Ultrasonic|Receiver|TCG|Point  8|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 8 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
178=0x9E792,Ultrasonic|Receiver|TCG|Point  9|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 9 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
179=0x9E7A2,Ultrasonic|Receiver|TCG|Point 10|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 10 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
180=0x9E7B2,Ultrasonic|Receiver|TCG|Point 11|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 11 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
181=0x9E7C2,Ultrasonic|Receiver|TCG|Point 12|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 12 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
182=0x9E7D2,Ultrasonic|Receiver|TCG|Point 13|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 13 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
183=0x9E7E2,Ultrasonic|Receiver|TCG|Point 14|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 14 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
184=0x9E7F2,Ultrasonic|Receiver|TCG|Point 15|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 15 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
185=0x57001,Project|Binder|Medium Velocity,m/s,ASE,Sound velocity of the medium.,FLOAT,,1,3020,100,10000
186=0x50005,Project|Project,N,RAE,Reference name of the loaded project.,STRING,,256,,,
187=0x50006,Project|Inspection,N,RAE,Reference name of the current inspection.,STRING,,256,,,
188=0x50007,Project|Medium,F,APE,Name of the medium used for the part.,STRING,,40,,,
189=0x53100,Project|Calibration|PreCal|Eddy Current|Instrument|Pre-Gain,,RAE,PreCal copy of; Eddy Current Preamplifier gain. [B] ,INTEGER,,1,2,0,0
190=0x53101,Project|Calibration|PreCal|Eddy Current|Instrument|Gain,dB,RAE,PreCal copy of; Eddy Current Main Amplifier gain. [B] ,INTEGER,,1,20,0,0
191=0x53102,Project|Calibration|PreCal|Eddy Current|Instrument|Phase,Deg,RAE,PreCal copy of; Eddy Current Phase. [B] ,INTEGER,,1,0,0,0
192=0x53103,Project|Calibration|PreCal|Eddy Current|Instrument|Frequency,Hz,RAE,PreCal copy of; Eddy Current Driver frequency. [B] ,FLOAT,,1,2000000,10,10
193=0x53104,Project|Calibration|PreCal|Eddy Current|Instrument|HP Filter,,RAE,PreCal copy of; Eddy Current High pass filter. [B] ,INTEGER,,1,0,0,0
194=0x53105,Project|Calibration|PreCal|Eddy Current|Instrument|LP Filter,,RAE,PreCal copy of; Eddy Current Low pass filter. [B] ,INTEGER,,1,0,0,0
195=0x53200,Project|Calibration|PostCal|Eddy Current|Instrument|Pre-Gain,,RAE,PostCal copy of; Eddy Current Preamplifier gain. [B] ,INTEGER,,1,2,0,0
196=0x53201,Project|Calibration|PostCal|Eddy Current|Instrument|Gain,dB,RAE,PostCal copy of; Eddy Current Main Amplifier gain. [B] ,INTEGER,,1,20,0,0
197=0x53202,Project|Calibration|PostCal|Eddy Current|Instrument|Phase,Deg,RAE,PostCal copy of; Eddy Current Phase. [B] ,INTEGER,,1,0,0,0
198=0x53203,Project|Calibration|PostCal|Eddy Current|Instrument|Frequency,Hz,RAE,PostCal copy of; Eddy Current Driver frequency. [B] ,FLOAT,,1,2000000,10,10
199=0x53204,Project|Calibration|PostCal|Eddy Current|Instrument|HP Filter,,RAE,PostCal copy of; Eddy Current High pass filter. [B] ,INTEGER,,1,0,0,0
200=0x53205,Project|Calibration|PostCal|Eddy Current|Instrument|LP Filter,,RAE,PostCal copy of; Eddy Current Low pass filter. [B] ,INTEGER,,1,0,0,0
201=0x52000,Project|Report|Custom|UT|Standard|P/N,F,ASEW,Calibration part number.,STRING,,40,?,,
202=0x52001,Project|Report|Custom|UT|Standard|S/N,F,ASEW,Calibration serial number.,STRING,,40,?,,
203=0x52002,Project|Report|Custom|UT|Probe|P/N,F,ASEW,Probe part number.,STRING,,40,?,,
204=0x52003,Project|Report|Custom|UT|Probe|S/N,F,ASEW,Probe serial number.,STRING,,40,?,,
205=0x52004,Project|Report|Custom|UT|Instrument|P/N,F,ASEW,Instrument part number.,STRING,,40,?,,
206=0x52005,Project|Report|Custom|UT|Instrument|S/N,F,ASEW,Instrument serial number.,STRING,,40,?,,
207=0x52006,Project|Report|Custom|UT|Recorder|P/N,F,ASEW,Recorder part number.,STRING,,40,?,,
208=0x52007,Project|Report|Custom|UT|Recorder|S/N,F,ASEW,Recorder serial number.,STRING,,40,?,,
209=0x52008,Project|Report|Custom|ET|Standard|P/N,F,ASEW,Calibration part number.,STRING,,40,?,,
210=0x52009,Project|Report|Custom|ET|Standard|S/N,F,ASEW,Calibration serial number.,STRING,,40,?,,
211=0x5200A,Project|Report|Custom|ET|Probe|P/N,F,ASEW,Probe part number.,STRING,,40,?,,
212=0x5200B,Project|Report|Custom|ET|Probe|S/N,F,ASEW,Probe serial number.,STRING,,40,?,,
213=0x5200C,Project|Report|Custom|ET|Instrument|P/N,F,ASEW,Instrument part number.,STRING,,40,?,,
214=0x5200D,Project|Report|Custom|ET|Instrument|S/N,F,ASEW,Instrument serial number.,STRING,,40,?,,
215=0x5200E,Project|Report|Custom|ET|Recorder|P/N,F,ASEW,Recorder part number.,STRING,,40,?,,
216=0x5200F,Project|Report|Custom|ET|Recorder|S/N,F,ASEW,Recorder serial number.,STRING,,40,?,,
217=0x52010,Project|Report|Custom|ESN,F,ASEW,ESN number.,STRING,,40,?,,
218=0x52011,Project|Report|Custom|SAP-AUN,F,ASEW,SAP-AUN number.,STRING,,40,?,,
219=0x52012,Project|Report|Custom|ARF,F,ASEW,ARF number.,STRING,,40,?,,
220=0x52013,Project|Report|Custom|Comment,F,ASEW,Project comment.,STRING,,256,?,,
221=0x5F001,Project|Method|Circle|Part,F,RAPE,Part Name.,STRING,,40,,,
222=0x5F002,Project|Method|Circle|Tool,F,APE,Name of the used tool.,STRING,,40,,,
223=0x5F014,Project|Method|Circle|Tool|Serial,N,RAE,Tool Serial number of the used tool.,STRING,,40,,,
224=0x5F003,Project|Method|Circle|Element,F,APE,Name of the used element.,STRING,,40,,,
225=0x5F015,Project|Method|Circle|Element|Serial,N,RAE,Serial number of the used element.,STRING,,40,,,
226=0x5F016,Project|Method|Circle|Material,F,APE,Name of the inspected material.,STRING,,40,,,
227=0x5F017,Project|Method|Circle|Material|Velocity,m/s,RAE,Sound velocity of the material.,FLOAT,,1,3020,100,10000
228=0x5F018,Project|Method|Circle|Velocity Mode,,APE,Determines material sound velocity mode in shear or longitudinal.,INTEGER,,1,0,0,1,Longitudinal=0,Shear=1
229=0x5F004,Project|Method|Circle|Settings,F,APE,Settings name.,STRING,,40,,,
230=0x5F005,Project|Method|Circle|Area,F,RAPE,Scan area name.,STRING,,40,,,
231=0x5F006,Project|Method|Circle|Method,F,RAPE,Scan method name.,STRING,,40,,,
232=0x5F007,Project|Method|Circle|State,,RAPE,Scan method state.,INTEGER,,1,0,-2,5,CFGERR=-2,ERROR=-1,NOINIT=0,READY=1,ACTIVE=2,CHECKING=3,SCANNING=4,EVALUATING=5
233=0x5F008,Project|Method|Circle|Calibration Type,y/n,APE,Determines the type of calibration and the behaviour in the project.,INTEGER,,1,0,0,2,None=0,PreCal=1,PostCal=2
234=0x5F009,Project|Method|Circle|Calibration Set,F,APE,Calibration parameter set name.,STRING,,40,,,
235=0x5F011,Project|Method|Circle|Index Count,x,RAE,Amount indices in the scan.,INTEGER,,1,0,0,0
236=0x5F012,Project|Method|Circle|Analysis|Start,x,APE,Restricted analysis range start fraction of the scan area.,FLOAT,,0.001,0,0,1
237=0x5F013,Project|Method|Circle|Analysis|Stop,x,APE,Restricted analysis range start fraction of the scan area.,FLOAT,,0.001,1,0,1
238=0x5F010,Project|Method|Circle|Entry,x,RAE,Entry in the project scan list.,INTEGER,,1,0,0,0
239=0x5F10A,Project|Method|Circle|Inspection|Gate Mode,,APE,Determines the way gates are positioned automatically.,INTEGER,,1,0,0,3,Manual=0,Surface=1,Material=2,Backwall=3
240=0x5F1FF,Project|Method|Circle|Angle Calculation,,APE,The way the angles are used in calculations.,INTEGER,,1,0,0,2,None=0,Material=1,Surface=2
241=0x5F107,Project|Method|Circle|Surface|Angle B,rad,APE,Surface angle of inspection as a rotation around the surface local Y-axis.,FLOAT,,0.001,0,-1.570796,1.570796
242=0x5F108,Project|Method|Circle|Surface|Angle A,rad,APE,Surface angle of inspection as a rotation around the surface local X-axis.,FLOAT,,0.001,0,-1.570796,1.570796
243=0x5F105,Project|Method|Circle|Material|Angle B,rad,APE,Angle of inspection as a rotation around the surface local Y-axis.,FLOAT,,0.001,0,-1.570796,1.570796
244=0x5F106,Project|Method|Circle|Material|Angle A,rad,APE,Angle of inspection as a rotation around the surface local X-axis.,FLOAT,,0.001,0,-1.570796,1.570796
245=0x5F118,Project|Method|Circle|Depth|Factor,,RAE,A ratio factor between depth from the surface and depth in the material.,FLOAT,,1e-06,1,0,1
246=0x5F1FE,Project|Method|Circle|Depth|Max,m,RAPE,Maximum allowed depth for any depth mode.,FLOAT,,1e-05,1,1e-05,1
247=0x5F1FD,Project|Method|Circle|Depth|Offset,m,RAPE,Offset to take in account from the scan specified depth.,FLOAT,,1e-05,0,0,1
248=0x5F117,Project|Method|Circle|Depth,m,APE,Uncorrected calculated depth value.,FLOAT,,1e-05,0,0,1
249=0x5F104,Project|Method|Circle|Density|Mode,!,APE,Switches between radial density mode in static or dynamic angles.,INTEGER,,1,0,0,1,Static=0,Dynamic=1
250=0x5F202,Project|Method|Circle|Scan|Velocity,rad/s,APE,Velocity during the scan of the line pattern.,FLOAT,,0.0001,0,0,7
251=0x5F102,Project|Method|Circle|Scan|Density,m,APE,Distance between measurements on the line.,FLOAT,,1e-06,1e-06,1e-06,1
252=0x5F103,Project|Method|Circle|Scan|Length,rad,RAE,Width of a scan line.,FLOAT,,0.01,6.283185,6.283185,0,6.283185=0
253=0x5F101,Project|Method|Circle|Index|Density,m,APE,Distance between measurements.,FLOAT,,1e-05,0.001,1e-05,1
254=0x5F100,Project|Method|Circle|Index|Steps,x,RAE,Steps to reach end of the contour.,INTEGER,,1,0,0,0
255=0x5F201,Project|Method|Circle|Surface|Speed,m/s,APE,Speed at the surface for calculating the velocity of an axis.,FLOAT,,1e-06,0.3,0,1
256=0x5F20A,Project|Method|Circle|Surface|Back Side,,APE,Use backside of the defined surface area.,INTEGER,,1,0,0,1,Top Side=0,Back Side=1
257=0x5F20B,Project|Method|Circle|Surface|Rotate,,APE,Rotate the defined surface around the surface Z-axis.,INTEGER,,1,0,0,3,0 Deg=0,90 Deg=1,180 Deg=2,270 Deg=3

[GenericParamValue]
; 'Motion|Galil 21x2 - NIP|Version' in 'Normal'
0x43C01=DMC2162 Rev 1.0l
; 'Motion|Galil 21x2 - NIP|Trigger|Time Unit' in 's'
0x43C11=0.005
; 'Motion|Galil 21x2 - NIP|System|Firmware Revision' in 'Normal'
0x43E02=R1.2 2005/01/04 18:45:51
; 'Storage|Filename' in 'File'
0xE0001=20050113_095530@#ET Cal Pre
; 'Storage|Sub Directory' in 'Subdir'
0xE0009=
; 'Storage|Directory' in 'Dir'
0xE0007=D:\\Data\\CF6-50 9253M66\\MPOV8496A\\20050113_095530\\
; 'Storage|File Number' in 'Normal'
0xE000A=5
; 'Storage|Compression' in '!'
0xE0003=0
; 'Storage|Disk Space' in 'kB'
0xE0006=64391936
; 'Storage|Logfile Path' in 'Path'
0xE0008=D:\\Data\\files
; 'Uspc2100|Error' in '!'
0xBF201=0
; 'Uspc2100|Pulser|Rep.Rate' in 'Hz'
0xBF096=1
; 'Uspc2100|Time Unit' in 's'
0xBF221=1e-08
; 'Uspc2100|Receiver|TCG|Gain|00' in 'dB'
0xB0110=0
; 'Uspc2100|Receiver|TCG|Time|00' in 'us'
0xB0100=2.35
; 'Uspc2100|Receiver|TCG|Gain|01' in 'dB'
0xB0111=0
; 'Uspc2100|Receiver|TCG|Time|01' in 'us'
0xB0101=17.7
; 'Uspc2100|Receiver|TCG|Gain|02' in 'dB'
0xB0112=0
; 'Uspc2100|Receiver|TCG|Time|02' in 'us'
0xB0102=0
; 'Uspc2100|Receiver|TCG|Gain|03' in 'dB'
0xB0113=0
; 'Uspc2100|Receiver|TCG|Time|03' in 'us'
0xB0103=4
; 'Uspc2100|Receiver|TCG|Gain|04' in 'dB'
0xB0114=0
; 'Uspc2100|Receiver|TCG|Time|04' in 'us'
0xB0104=0
; 'Uspc2100|Receiver|TCG|Gain|05' in 'dB'
0xB0115=0
; 'Uspc2100|Receiver|TCG|Time|05' in 'us'
0xB0105=0
; 'Uspc2100|Receiver|TCG|Gain|06' in 'dB'
0xB0116=0
; 'Uspc2100|Receiver|TCG|Time|06' in 'us'
0xB0106=0
; 'Uspc2100|Receiver|TCG|Gain|07' in 'dB'
0xB0117=0
; 'Uspc2100|Receiver|TCG|Time|07' in 'us'
0xB0107=8
; 'Uspc2100|Receiver|TCG|Gain|08' in 'dB'
0xB0118=0
; 'Uspc2100|Receiver|TCG|Time|08' in 'us'
0xB0108=0
; 'Uspc2100|Receiver|TCG|Gain|09' in 'dB'
0xB0119=0
; 'Uspc2100|Receiver|TCG|Time|09' in 'us'
0xB0109=10
; 'Uspc2100|Receiver|TCG|Gain|10' in 'dB'
0xB011A=0
; 'Uspc2100|Receiver|TCG|Time|10' in 'us'
0xB010A=0
; 'Uspc2100|Receiver|TCG|Gain|11' in 'dB'
0xB011B=0
; 'Uspc2100|Receiver|TCG|Time|11' in 'us'
0xB010B=0
; 'Uspc2100|Receiver|TCG|Gain|12' in 'dB'
0xB011C=0
; 'Uspc2100|Receiver|TCG|Time|12' in 'us'
0xB010C=0
; 'Uspc2100|Receiver|TCG|Gain|13' in 'dB'
0xB011D=0
; 'Uspc2100|Receiver|TCG|Time|13' in 'us'
0xB010D=0
; 'Uspc2100|Receiver|TCG|Gain|14' in 'dB'
0xB011E=0
; 'Uspc2100|Receiver|TCG|Time|14' in 'us'
0xB010E=0
; 'Uspc2100|Receiver|TCG|Gain|15' in 'dB'
0xB011F=0
; 'Uspc2100|Receiver|TCG|Time|15' in 'us'
0xB010F=0
; 'Uspc2100|A-scan|Start Mode' in ''
0xB0000=0
; 'Uspc2100|A-scan|Delay' in 'us'
0xB0001=200.2
; 'Uspc2100|A-scan|Range' in 'us'
0xB0002=25
; 'Uspc2100|A-scan|Display Mode' in ''
0xB0003=1
; 'Uspc2100|A-scan|Reject' in '%'
0xB0004=0
; 'Uspc2100|A-scan|Stream' in ''
0xB0005=0
; 'Uspc2100|A-scan|Step Gate Display' in ''
0xB0006=1
; 'Uspc2100|System|One SHot' in ''
0xB0212=1
; 'Uspc2100|IF Gate|Delay' in 'us'
0xB0030=196.72
; 'Uspc2100|IF Gate|Range' in 'us'
0xB0031=11.96
; 'Uspc2100|IF Gate|Threshold' in '%'
0xB0032=80
; 'Uspc2100|IF Gate|Trigger Mode' in ''
0xB004A=0
; 'Uspc2100|IF Gate|Rectify' in ''
0xB004C=1
; 'Uspc2100|WallThickness|Average Window' in '?'
0xB0033=0
; 'Uspc2100|WallThickness|Start' in ''
0xB0035=0
; 'Uspc2100|WallThickness|Stop' in ''
0xB0036=0
; 'Uspc2100|WallThickness|Accumulate Mode' in ''
0xB004B=0
; 'Uspc2100|WallThickness|Noise Immun Count' in ''
0xB004D=0
; 'Uspc2100|WallThickness|Min Limit' in 'S'
0xB004E=0.2
; 'Uspc2100|WallThickness|Max Limit' in 'S'
0xB004F=0.2
; 'Uspc2100|WallThickness|Num Echos' in ''
0xB0050=2
; 'Uspc2100|WallThickness|Zero Offset' in 'S'
0xB0051=0.2
; 'Uspc2100|Gates|Mode' in ''
0xB0037=1
; 'Uspc2100|Gates|Start Mode' in ''
0xB0034=2
; 'Uspc2100|System|TDR' in ''
0xB0060=0
; 'Uspc2100|Analog Out 1|Source out' in ''
0xB0063=0
; 'Uspc2100|Analog Out 1|Offset' in '%'
0xB0064=0
; 'Uspc2100|Analog Out 1|Range' in '%'
0xB0065=5
; 'Uspc2100|Analog Out 1|Timing' in ''
0xB0067=1
; 'Uspc2100|Analog Out 1|Muting' in '%'
0xB0066=0
; 'Uspc2100|Analog Out 2|Source out' in ''
0xB0068=0
; 'Uspc2100|Analog Out 2|Offset' in '%'
0xB0069=0
; 'Uspc2100|Analog Out 2|Range' in '%'
0xB006A=5
; 'Uspc2100|Analog Out 2|Timing' in ''
0xB006C=1
; 'Uspc2100|Analog Out 2|Muting' in '%'
0xB006B=0
; 'Uspc2100|System|Version' in 'Normal'
0xB0222=<n/a>
; 'Uspc2100|Pulser|Sync Mode' in ''
0xB0097=1
; 'Uspc2100|Pulser|Test Mode' in ''
0xB009C=0
; 'Uspc2100|Pulser|Energy' in ''
0xB009E=1
; 'Uspc2100|Pulser|Voltage' in ''
0xB009F=1
; 'Uspc2100|Receiver|Gain' in 'dB'
0xB0098=70
; 'Uspc2100|Receiver|TCG|Enable' in ''
0xB009A=1
; 'Uspc2100|Receiver|TCG|Delay' in 'us'
0xB0223=0
; 'Uspc2100|Multiplexer|Table|Count' in ''
0xB00A0=1
; 'Uspc2100|Multiplexer|TransmitCycleTable' in ''
0xB0093=65535
; 'Uspc2100|Multiplexer|ReceiveCycleTable' in ''
0xB0094=65535
; 'Uspc2100|Receiver|RF Filter' in ''
0xB0099=4
; 'Uspc2100|Pulser|Damping' in ''
0xB009D=0
; 'Uspc2100|Gate 1|Delay' in 'us'
0xB0038=1.88
; 'Uspc2100|Gate 1|Range' in 'us'
0xB0039=17.02
; 'Uspc2100|Gate 1|Threshold' in '%'
0xB003A=40
; 'Uspc2100|Gate 1|Noise Immun count' in ''
0xB003B=1
; 'Uspc2100|Gate 1|Average Window' in ''
0xB003C=1
; 'Uspc2100|Gate 1|Trigger Mode' in ''
0xB003D=0
; 'Uspc2100|Gate 1|Accumulate Reset' in ''
0xB0044=0
; 'Uspc2100|Gate 1|Rectify' in ''
0xB0046=1
; 'Uspc2100|Gate 1|Accumulate Mode' in ''
0xB0048=2
; 'Uspc2100|Gate 2|Delay' in 'us'
0xB003E=30
; 'Uspc2100|Gate 2|Range' in 'us'
0xB003F=5
; 'Uspc2100|Gate 2|Threshold' in '%'
0xB0040=30
; 'Uspc2100|Gate 2|Noise Immun count' in ''
0xB0041=1
; 'Uspc2100|Gate 2|Average Window' in ''
0xB0042=1
; 'Uspc2100|Gate 2|Trigger Mode' in ''
0xB0043=0
; 'Uspc2100|Gate 2|Accumulate Reset' in ''
0xB0045=0
; 'Uspc2100|Gate 2|Rectify' in ''
0xB0047=1
; 'Uspc2100|Gate 2|Accumulate Mode' in ''
0xB0049=2
; 'Eddy Current|Channels' in '!'
0x30002=1
; 'Eddy Current|Amplitude|Unit' in 'V'
0x30005=0.00122100122100122
; 'Eddy Current|Channel|Trigger|Rep.Rate' in 'Hz'
0x30401=5000
; 'Eddy Current|Channel|Trigger|Sync Mode' in '!'
0x30402=0
; 'Eddy Current|Channel|Trigger|Time Unit' in 's'
0x30405=0.0002
; 'Eddy Current|Channel|Gates' in '!'
0x30403=4
; 'Eddy Current|Channel|X|Attenuator' in ''
0x30522=2
; 'Eddy Current|Channel|X|Rectifier' in '!'
0x30526=0
; 'Eddy Current|Channel|X|Calibration|Invert' in '!'
0x30520=0
; 'Eddy Current|Channel|X|Calibration|Offset' in ''
0x30524=0
; 'Eddy Current|Channel|X|Calibration|Scale' in ''
0x30525=1
; 'Eddy Current|Channel|X|Amplitude|Unit' in ''
0x3052C=0.00122100122100122
; 'Eddy Current|Channel|X|Amplitude|Unit Select' in ''
0x3052D=0
; 'Eddy Current|Channel|X|Enable' in '!'
0x30505=1
; 'Eddy Current|Channel|Y|Attenuator' in ''
0x30622=2
; 'Eddy Current|Channel|Y|Rectifier' in '!'
0x30626=0
; 'Eddy Current|Channel|Y|Calibration|Invert' in '!'
0x30620=0
; 'Eddy Current|Channel|Y|Calibration|Offset' in ''
0x30624=0
; 'Eddy Current|Channel|Y|Calibration|Scale' in ''
0x30625=1
; 'Eddy Current|Channel|Y|Amplitude|Unit' in ''
0x3062C=0.00122100122100122
; 'Eddy Current|Channel|Y|Amplitude|Unit Select' in ''
0x3062D=0
; 'Eddy Current|Channel|Y|Enable' in '!'
0x30605=1
; 'Eddy Current|Instrument|Identification' in 'Normal'
0x30210=ELOTEST B1 V4 UM  Version: 6.04  SN: 02513
; 'Eddy Current|Instrument|Frequency' in 'Hz'
0x30212=2000000
; 'Eddy Current|Instrument|Pre-Gain' in ''
0x30213=3
; 'Eddy Current|Instrument|Gain' in 'dB'
0x30214=28
; 'Eddy Current|Instrument|Vertical Spread' in 'dB'
0x30215=0
; 'Eddy Current|Instrument|Horizontal Spread' in 'dB'
0x30216=0
; 'Eddy Current|Instrument|Phase' in 'Deg'
0x30217=146
; 'Eddy Current|Instrument|Bandwidth Limit' in ''
0x30218=1
; 'Eddy Current|Instrument|HP Filter' in ''
0x30219=15
; 'Eddy Current|Instrument|LP Filter' in ''
0x3021A=26
; 'Eddy Current|Instrument|?' in 'units'
0x3021C=
; 'Project|Sound Velocity|Medium' in 'm/s'
0x99003=1491
; 'Project|Sound Velocity|Material' in 'm/s'
0x99004=5820
; 'Project|System|Time' in 'Normal'
0x99005=12:04:05
; 'Project|System|Date' in 'Normal'
0x99006=2005-13-01
; 'Project|Ultrasonic|Threshold' in '%'
0x99201=30
; 'Project|Eddy Current|Peak-to-Peak|Threshold' in 'V'
0x99214=2
; 'Ultrasonic|Receiver|Gain|Scan|Active' in ''
0x9E705=1
; 'Ultrasonic|Receiver|Gain|Scan|Gain' in 'dB'
0x9E706=0
; 'Ultrasonic|Receiver|Gain|TCG Corr|Active' in ''
0x9E707=0
; 'Ultrasonic|Receiver|Gain|Main' in 'dB'
0x9E708=70
; 'Ultrasonic|Receiver|Gain|Target Amplitude' in '%'
0x9E701=80
; 'Ultrasonic|Receiver|TCG|Point  1|L-Gain' in 'dB'
0x9E711=0
; 'Ultrasonic|Receiver|TCG|Point  2|L-Gain' in 'dB'
0x9E721=0
; 'Ultrasonic|Receiver|TCG|Point  3|L-Gain' in 'dB'
0x9E731=0
; 'Ultrasonic|Receiver|TCG|Point  4|L-Gain' in 'dB'
0x9E741=0
; 'Ultrasonic|Receiver|TCG|Point  5|L-Gain' in 'dB'
0x9E751=0
; 'Ultrasonic|Receiver|TCG|Point  6|L-Gain' in 'dB'
0x9E761=0
; 'Ultrasonic|Receiver|TCG|Point  7|L-Gain' in 'dB'
0x9E771=0
; 'Ultrasonic|Receiver|TCG|Point  8|L-Gain' in 'dB'
0x9E781=0
; 'Ultrasonic|Receiver|TCG|Point  9|L-Gain' in 'dB'
0x9E791=0
; 'Ultrasonic|Receiver|TCG|Point 10|L-Gain' in 'dB'
0x9E7A1=0
; 'Ultrasonic|Receiver|TCG|Point 11|L-Gain' in 'dB'
0x9E7B1=0
; 'Ultrasonic|Receiver|TCG|Point 12|L-Gain' in 'dB'
0x9E7C1=0
; 'Ultrasonic|Receiver|TCG|Point 13|L-Gain' in 'dB'
0x9E7D1=0
; 'Ultrasonic|Receiver|TCG|Point 14|L-Gain' in 'dB'
0x9E7E1=0
; 'Ultrasonic|Receiver|TCG|Point 15|L-Gain' in 'dB'
0x9E7F1=0
; 'Ultrasonic|Receiver|TCG|Point  1|Corr' in 'dB'
0x9E712=0
; 'Ultrasonic|Receiver|TCG|Point  2|Corr' in 'dB'
0x9E722=0
; 'Ultrasonic|Receiver|TCG|Point  3|Corr' in 'dB'
0x9E732=0
; 'Ultrasonic|Receiver|TCG|Point  4|Corr' in 'dB'
0x9E742=0
; 'Ultrasonic|Receiver|TCG|Point  5|Corr' in 'dB'
0x9E752=0
; 'Ultrasonic|Receiver|TCG|Point  6|Corr' in 'dB'
0x9E762=0
; 'Ultrasonic|Receiver|TCG|Point  7|Corr' in 'dB'
0x9E772=0
; 'Ultrasonic|Receiver|TCG|Point  8|Corr' in 'dB'
0x9E782=0
; 'Ultrasonic|Receiver|TCG|Point  9|Corr' in 'dB'
0x9E792=0
; 'Ultrasonic|Receiver|TCG|Point 10|Corr' in 'dB'
0x9E7A2=0
; 'Ultrasonic|Receiver|TCG|Point 11|Corr' in 'dB'
0x9E7B2=0
; 'Ultrasonic|Receiver|TCG|Point 12|Corr' in 'dB'
0x9E7C2=0
; 'Ultrasonic|Receiver|TCG|Point 13|Corr' in 'dB'
0x9E7D2=0
; 'Ultrasonic|Receiver|TCG|Point 14|Corr' in 'dB'
0x9E7E2=0
; 'Ultrasonic|Receiver|TCG|Point 15|Corr' in 'dB'
0x9E7F2=0
; 'Project|Binder|Medium Velocity' in 'm/s'
0x57001=1491
; 'Project|Project' in 'Normal'
0x50005=ET Aft Side@CF6-50 9253M66
; 'Project|Inspection' in 'Normal'
0x50006=CF6-50 9253M66>MPOV8496A<20050113_095530
; 'Project|Medium' in 'File'
0x50007=Water
; 'Project|Calibration|PreCal|Eddy Current|Instrument|Pre-Gain' in ''
0x53100=3
; 'Project|Calibration|PreCal|Eddy Current|Instrument|Gain' in 'dB'
0x53101=23
; 'Project|Calibration|PreCal|Eddy Current|Instrument|Phase' in 'Deg'
0x53102=146
; 'Project|Calibration|PreCal|Eddy Current|Instrument|Frequency' in 'Hz'
0x53103=2000000
; 'Project|Calibration|PreCal|Eddy Current|Instrument|HP Filter' in ''
0x53104=15
; 'Project|Calibration|PreCal|Eddy Current|Instrument|LP Filter' in ''
0x53105=26
; 'Project|Calibration|PostCal|Eddy Current|Instrument|Pre-Gain' in ''
0x53200=2
; 'Project|Calibration|PostCal|Eddy Current|Instrument|Gain' in 'dB'
0x53201=20
; 'Project|Calibration|PostCal|Eddy Current|Instrument|Phase' in 'Deg'
0x53202=0
; 'Project|Calibration|PostCal|Eddy Current|Instrument|Frequency' in 'Hz'
0x53203=2000000
; 'Project|Calibration|PostCal|Eddy Current|Instrument|HP Filter' in ''
0x53204=0
; 'Project|Calibration|PostCal|Eddy Current|Instrument|LP Filter' in ''
0x53205=0
; 'Project|Report|Custom|UT|Standard|P/N' in 'File'
0x52000=
; 'Project|Report|Custom|UT|Standard|S/N' in 'File'
0x52001=
; 'Project|Report|Custom|UT|Probe|P/N' in 'File'
0x52002=
; 'Project|Report|Custom|UT|Probe|S/N' in 'File'
0x52003=
; 'Project|Report|Custom|UT|Instrument|P/N' in 'File'
0x52004=
; 'Project|Report|Custom|UT|Instrument|S/N' in 'File'
0x52005=
; 'Project|Report|Custom|UT|Recorder|P/N' in 'File'
0x52006=
; 'Project|Report|Custom|UT|Recorder|S/N' in 'File'
0x52007=
; 'Project|Report|Custom|ET|Standard|P/N' in 'File'
0x52008=ET Std PN
; 'Project|Report|Custom|ET|Standard|S/N' in 'File'
0x52009=ET Std SN
; 'Project|Report|Custom|ET|Probe|P/N' in 'File'
0x5200A=MDK1
; 'Project|Report|Custom|ET|Probe|S/N' in 'File'
0x5200B=1234
; 'Project|Report|Custom|ET|Instrument|P/N' in 'File'
0x5200C=ELOTEST B1 V4 UM  Version 6.04
; 'Project|Report|Custom|ET|Instrument|S/N' in 'File'
0x5200D=02513
; 'Project|Report|Custom|ET|Recorder|P/N' in 'File'
0x5200E=KPCI3100
; 'Project|Report|Custom|ET|Recorder|S/N' in 'File'
0x5200F=unknown
; 'Project|Report|Custom|ESN' in 'File'
0x52010=
; 'Project|Report|Custom|SAP-AUN' in 'File'
0x52011=
; 'Project|Report|Custom|ARF' in 'File'
0x52012=
; 'Project|Report|Custom|Comment' in 'File'
0x52013=
; 'Project|Method|Circle|Part' in 'File'
0x5F001=CF6-50 9253M66
; 'Project|Method|Circle|Tool' in 'File'
0x5F002=EddyLoth
; 'Project|Method|Circle|Tool|Serial' in 'Normal'
0x5F014=001
; 'Project|Method|Circle|Element' in 'File'
0x5F003=MDK1
; 'Project|Method|Circle|Element|Serial' in 'Normal'
0x5F015=1234
; 'Project|Method|Circle|Material' in 'File'
0x5F016=Titanium
; 'Project|Method|Circle|Material|Velocity' in 'm/s'
0x5F017=5820
; 'Project|Method|Circle|Velocity Mode' in ''
0x5F018=0
; 'Project|Method|Circle|Settings' in 'File'
0x5F004=ET
; 'Project|Method|Circle|Area' in 'File'
0x5F005=ET Cal Pre
; 'Project|Method|Circle|Method' in 'File'
0x5F006=Circle
; 'Project|Method|Circle|State' in ''
0x5F007=4
; 'Project|Method|Circle|Calibration Type' in 'y/n'
0x5F008=1
; 'Project|Method|Circle|Calibration Set' in 'File'
0x5F009=ET
; 'Project|Method|Circle|Index Count' in 'x'
0x5F011=25
; 'Project|Method|Circle|Analysis|Start' in 'x'
0x5F012=0
; 'Project|Method|Circle|Analysis|Stop' in 'x'
0x5F013=1
; 'Project|Method|Circle|Entry' in 'x'
0x5F010=0
; 'Project|Method|Circle|Inspection|Gate Mode' in ''
0x5F10A=0
; 'Project|Method|Circle|Angle Calculation' in ''
0x5F1FF=0
; 'Project|Method|Circle|Surface|Angle B' in 'rad'
0x5F107=0
; 'Project|Method|Circle|Surface|Angle A' in 'rad'
0x5F108=0
; 'Project|Method|Circle|Material|Angle B' in 'rad'
0x5F105=0
; 'Project|Method|Circle|Material|Angle A' in 'rad'
0x5F106=0
; 'Project|Method|Circle|Depth|Factor' in ''
0x5F118=1
; 'Project|Method|Circle|Depth|Max' in 'm'
0x5F1FE=1
; 'Project|Method|Circle|Depth|Offset' in 'm'
0x5F1FD=0
; 'Project|Method|Circle|Depth' in 'm'
0x5F117=0
; 'Project|Method|Circle|Density|Mode' in '!'
0x5F104=1
; 'Project|Method|Circle|Scan|Velocity' in 'rad/s'
0x5F202=3.1416
; 'Project|Method|Circle|Scan|Density' in 'm'
0x5F102=0.000127
; 'Project|Method|Circle|Scan|Length' in 'rad'
0x5F103=6.283185
; 'Project|Method|Circle|Index|Density' in 'm'
0x5F101=0.00013
; 'Project|Method|Circle|Index|Steps' in 'x'
0x5F100=25
; 'Project|Method|Circle|Surface|Speed' in 'm/s'
0x5F201=0.3048
; 'Project|Method|Circle|Surface|Back Side' in ''
0x5F20A=0
; 'Project|Method|Circle|Surface|Rotate' in ''
0x5F20B=2

[GenericResultInfo]
CompressionMethod=0
Entries=8
0=0x204e0,Ultrasonic|Gate 2|Peak|Amplitude,AS,Ultrasonic Gate 2 Amplitude of found peak.,INT8,1,1048576,7,0
1=0x204e1,Ultrasonic|Gate 2|Peak|Time Of Flight,AS,Ultrasonic Gate 2 Time Of Flight (TOF) of found peak.,INT32,1,1048576,24,0
2=0x30401,Eddy Current|Index|POP Primary,AS,Eddy Current Position orientation pulse index,INT32,1,1048576,24,0
3=0x30520,Eddy Current|Amplitude|X,AS,Eddy Current X Amplitude measurement,INT16,1,10485760,12,2047
4=0x30620,Eddy Current|Amplitude|Y,AS,Eddy Current Y Amplitude measurement,INT16,1,10485760,12,2047
5=0x30720,Eddy Current|Amplitude|P,AS,Eddy Current P Amplitude measurement,INT16,1,10485760,12,2047
6=0x30420,Eddy Current|Amplitude|R,AS,Eddy Current R Amplitude measurement,INT16,1,10485760,12,2047
7=0x5f001,Project|Method|Circle|Positions,AS,Index positions in comma separated form.,STRING,1,262144,8,0

[GenericResultPositions]
0x204E0=0,4108,0,0
0x204E1=0,4108,0,0
0x30401=25,4108,100,0
0x30520=86784,4208,173568,0
0x30620=86784,177776,173568,0
0x30720=86784,351344,173568,0
0x30420=86784,524912,173568,0
0x5F001=1644,698480,1644,0

)";

}
