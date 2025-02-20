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
185=0x57001,Project|Binder|Medium Velocity,m/s,ASE,Sound velocity of the medium.,FLOAT,,1,3020,100,10000
186=0x50005,Project|Project,N,RAE,Reference name of the loaded project.,STRING,,256,,,
187=0x50006,Project|Inspection,N,RAE,Reference name of the current inspection.,STRING,,256,,,
188=0x50007,Project|Medium,F,APE,Name of the medium used for the part.,STRING,,40,,,
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
; 'Project|Project' in 'Normal'
0x50005=ET Aft Side@CF6-50 9253M66
; 'Project|Inspection' in 'Normal'
0x50006=CF6-50 9253M66>MPOV8496A<20050113_095530
; 'Project|Medium' in 'File'
0x50007=Water
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

)";

}// namespace sf
