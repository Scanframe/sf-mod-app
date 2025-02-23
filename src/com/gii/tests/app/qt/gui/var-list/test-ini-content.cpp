namespace sf
{

auto UnitConversionIniContent = R"(

[Followers]
0x99003='1e-6 * x',m,0xb0030,0xb0031,
0x99004='1e-6 * x',m,0xb0038,0xb0039,0xb003e,0xb003f

; <from-unit>,<from-precision>=<to-unit>,<multiplier>,<offset>,<to-precision>

[Disabled]


[Metric]
m,-3=m,1,0,-3
m,-2=m,1,0,-2
m,-1=m,1,0,-1
m,0=mm,1000,0,-3
m,1=mm,1000,0,-2
m,2=mm,1000,0,-1
m,3=mm,1000,0,0
m,4=mm,1000,0,1
m,5=mm,1000,0,2
m,6=mm,1000,0,3
m,7=mm,1000,0,4
m,8=mm,1000,0,5

m/s,-1=m/s,1,0,0
m/s,0=m/s,1,0,0
m/s,1=mm/s,1000,0,-2
m/s,2=mm/s,1000,0,-1
m/s,3=mm/s,1000,0,0
m/s,4=mm/s,1000,0,1
m/s,5=mm/s,1000,0,2
m/s,6=mm/s,1000,0,3

us,1=µs,1,0,1
us,2=µs,1,0,2

s,0=s,1,0,0
s,1=s,1,0,1
s,2=s,1,0,2
s,3=ms,1e3,0,0
s,4=ms,1e3,0,1
s,5=ms,1e3,0,2
s,6=µs,1e6,0,0
s,7=µs,1e6,0,1
s,8=µs,1e6,0,2
s,9=µs,1e6,0,3

rad,0=deg,57.2958,0,0
rad,1=deg,57.2958,0,0
rad,2=deg,57.2958,0,1
rad,3=deg,57.2958,0,2
rad,4=deg,57.2958,0,3
rad,5=deg,57.2958,0,4
rad,6=deg,57.2958,0,5

rad/s,0=deg/s,57.2958,0,0
rad/s,1=deg/s,57.2958,0,0
rad/s,2=deg/s,57.2958,0,1
rad/s,3=deg/s,57.2958,0,2
rad/s,4=deg/s,57.2958,0,3
rad/s,5=deg/s,57.2958,0,4
rad/s,6=deg/s,57.2958,0,5

°C,1=°C,1
V,2=mV,1000,0,-1

kB,0=
%,0=
dB,1=
%,0=



[Imperial]
m/s,0="/s,39.3700787401,0,0
m/s,1="/s,39.3700787401,0,1
m/s,2="/s,39.3700787401,0,2
m/s,3="/s,39.3700787401,0,3
m/s,4="/s,39.3700787401,0,4
m/s,5="/s,39.3700787401,0,5

rad,0=deg,57.2958,0,0
rad,1=deg,57.2958,0,0
rad,2=deg,57.2958,0,1
rad,3=deg,57.2958,0,2
rad,4=deg,57.2958,0,3
rad,5=deg,57.2958,0,4
rad,6=deg,57.2958,0,5

°C,1=°F,1.8,32,1

)";

const char* VariableIniContent = R"(

[GenericParamInfo]
Entries=258
0=0x5,Unit Conversion|Followers Enabled,,ASP,,INTEGER,,1,1,0,1,Off=0,On=1
;1=0x10,Unit Conversion|Sound Velocity Water,m/s,ASP,Sound velocity of water.,FLOAT,,1,1481,0,10000
;2=0x11,Unit Conversion|Sound Velocity Medium,m/s,ASP,Sound velocity of the medium.,FLOAT,,1,3125,0,10000
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
45=0xB0000,Uspc2100|A-scan|Start Mode,,ASEW,Source of start trigger for A-scan display,INTEGER,,1,0,0,2,Initial Pulse=0,Artificial=1,Interface=2
46=0xB0001,Uspc2100|A-scan|Delay,us,ASEW,Delay in us,FLOAT,FLOAT,0.01,202.1,-10,310
47=0xB0002,Uspc2100|A-scan|Range,us,ASEW,Range in us,FLOAT,FLOAT,0.01,20.8,1,321
48=0xB0003,Uspc2100|A-scan|Display Mode,,ASEW,Rectification for A-scan display,INTEGER,,1,1,0,3,RF=0,Full=1,Pos=2,Neg=3
49=0xB0004,Uspc2100|A-scan|Reject,%,ASEW,Ascan Reject,FLOAT,FLOAT,1,0,0,100
50=0xB0005,Uspc2100|A-scan|Stream,,ASEW,Ascan Streaming Modes,INTEGER,,1,0,0,2,Disabled=0,Enabled=1,Gates=2
51=0xB0006,Uspc2100|A-scan|Step Gate Display,,ASEW,Ascan Step gate display mode,INTEGER,,1,1,0,1,Off=0,On=1
53=0xB0030,Uspc2100|IF Gate|Delay,us,ASEW,Interface Gate Delay,FLOAT,,0.02,212.12,0,320
54=0xB0031,Uspc2100|IF Gate|Range,us,ASEW,Interface Gate Range,FLOAT,,0.02,8.35,0,320
55=0xB0032,Uspc2100|IF Gate|Threshold,%,ASEW,Interface Gate Flank Threshold,INTEGER,,1,80,0,100
56=0xB004A,Uspc2100|IF Gate|Trigger Mode,,ASEW,Source of trigger for Interface Gate,INTEGER,,1,0,0,1,Peak=0,Flank=1
57=0xB004C,Uspc2100|IF Gate|Rectify,,ASEW,Interface Gate rectify mode,INTEGER,,1,1,0,3,RF=0,Full=1,Pos=2,Neg=3
67=0xB0037,Uspc2100|Gates|Mode,,ASEW,Type of measurement for Gate(s),INTEGER,,1,1,1,2,Flaw=1,Thick=2
68=0xB0034,Uspc2100|Gates|Start Mode,,ASEW,Source of start trigger for Gate(s),INTEGER,,1,2,0,2,Initial Pulse=0,Artificial=1,Interface=2
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
0xB0051=0.2
; 'Uspc2100|Gates|Mode' in ''
0xB0037=1
; 'Uspc2100|Gates|Start Mode' in ''
0xB0034=2
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
; 'Project|Binder|Medium Velocity' in 'm/s'
0x57001=1491

)";

}// namespace sf
