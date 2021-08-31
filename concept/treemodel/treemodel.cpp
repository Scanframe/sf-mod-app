#include <QStringList>
#include <misc/gen/gen_utils.h>
#include "treemodel.h"

TreeModel::TreeItem::TreeItem(TreeItem* parent, const QString& name)
	:_parentItem(parent)
{
	if (_parentItem)
	{
		_parentItem->_childItems.append(this);
	}
	_name = name;
}

TreeModel::TreeItem::~TreeItem()
{
	qDeleteAll(_childItems);
}

TreeModel::TreeModel(QObject* parent)
	:QAbstractItemModel(parent)
	,_rootItem(new TreeItem(nullptr, "Root"))
{
}

TreeModel::~TreeModel()
{
	delete _rootItem;
}

int TreeModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

QVariant TreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || role != Qt::DisplayRole)
	{
		return {};
	}
	auto item = static_cast<TreeItem*>(index.internalPointer());
	return item->_name;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return Qt::NoItemFlags;
	}

	return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		return QString("Column %1").arg(section);
	}

	return {};
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
	{
		return {};
	}
	auto parentItem = parent.isValid() ? static_cast<TreeItem*>(parent.internalPointer()) : _rootItem;
	auto childItem = parentItem->_childItems.at(row);
	if (childItem)
	{
		return createIndex(row, column, childItem);
	}
	return {};
}

QModelIndex TreeModel::parent(const QModelIndex& child) const
{
	if (!child.isValid())
	{
		return {};
	}

	auto* childItem = static_cast<TreeItem*>(child.internalPointer());
	auto* parentItem = childItem->_parentItem;

	if (parentItem == _rootItem)
	{
		return {};
	}

	auto row = childItem->_parentItem ? static_cast<int>(childItem->_parentItem->_childItems.indexOf(childItem)) : 0;
	return createIndex(row, 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex& parent) const
{
	TreeItem* parentItem;
	if (parent.column() > 0)
	{
		return 0;
	}
	if (!parent.isValid())
	{
		parentItem = _rootItem;
	}
	else
	{
		parentItem = static_cast<TreeItem*>(parent.internalPointer());
	}
	return static_cast<int>(parentItem->_childItems.count());
}

const char* VariableSpecs = R"(
0x43C01,Motion|Galil 21x2 - NIP|Version,,RAS,Version of hardware and driver,STRING,,60,,,
0x43C11,Motion|Galil 21x2 - NIP|Trigger|Time Unit,s,RAS,Time in seconds that is used for the time dependant results.,FLOAT,,1e-06,0.01,1e-06,100000000
0x43E02,Motion|Galil 21x2 - NIP|System|Firmware Revision,,RAS,Identification of revision of embedded application,STRING,,60,R1.2 2005/01/04 18:45:51,,
0xE0001,Storage|Filename,F,ASP,Name for the file to be stored. This is without extension.,STRING,,256,Unknown,,
0xE0009,Storage|Sub Directory,S,ASP,Sub directory name for the file to be stored.,STRING,,256,,,
0xE0007,Storage|Directory,D,ASPE,Directory to store the data in.,STRING,,256,C:\DOCUME~1\bcbdebug\LOCALS~1\Temp\,,
0xE000A,Storage|File Number,#,RASE,Next file number.,STRING,,0,1,0,999,Unknown=0
0xE0003,Storage|Compression,!,ASPE,File format compatibility/ compression mode.,INTEGER,,1,0,0,2,None=0,ZLIB_FAST=1,ZLIB_BEST=2
0xE0006,Storage|Disk Space,kB,RASE,Free diskspace of current file drive,FLOAT,,1,0,0,0
0xE0008,Storage|Logfile Path,P,ASPE,File path to the log file which determines the next file number.,STRING,,256,C:\DOCUME~1\bcbdebug\LOCALS~1\Temp\stogii32,,
0xBF201,Uspc2100|Error,!,RASE,Errorcode,INTEGER,,1,0,0,1,No errors=0,Failure=1
0xBF096,Uspc2100|Pulser|Rep.Rate,Hz,ASEW,Pulser Repetition Rate,INTEGER,,1,500,1,10000
0xBF221,Uspc2100|Time Unit,s,RASE,Time unit of the digitiser sample rate.,FLOAT,,1e-08,1e-08,1e-08,1e-08
0xB0110,Uspc2100|Receiver|TCG|Gain|00,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB0100,Uspc2100|Receiver|TCG|Time|00,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,0,0,320
0xB0111,Uspc2100|Receiver|TCG|Gain|01,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB0101,Uspc2100|Receiver|TCG|Time|01,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,5,0,320
0xB0112,Uspc2100|Receiver|TCG|Gain|02,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB0102,Uspc2100|Receiver|TCG|Time|02,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,10,0,320
0xB0113,Uspc2100|Receiver|TCG|Gain|03,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB0103,Uspc2100|Receiver|TCG|Time|03,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,15,0,320
0xB0114,Uspc2100|Receiver|TCG|Gain|04,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB0104,Uspc2100|Receiver|TCG|Time|04,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,20,0,320
0xB0115,Uspc2100|Receiver|TCG|Gain|05,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB0105,Uspc2100|Receiver|TCG|Time|05,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,25,0,320
0xB0116,Uspc2100|Receiver|TCG|Gain|06,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB0106,Uspc2100|Receiver|TCG|Time|06,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,30,0,320
0xB0117,Uspc2100|Receiver|TCG|Gain|07,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB0107,Uspc2100|Receiver|TCG|Time|07,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,35,0,320
0xB0118,Uspc2100|Receiver|TCG|Gain|08,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB0108,Uspc2100|Receiver|TCG|Time|08,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,40,0,320
0xB0119,Uspc2100|Receiver|TCG|Gain|09,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB0109,Uspc2100|Receiver|TCG|Time|09,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,45,0,320
0xB011A,Uspc2100|Receiver|TCG|Gain|10,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB010A,Uspc2100|Receiver|TCG|Time|10,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,50,0,320
0xB011B,Uspc2100|Receiver|TCG|Gain|11,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB010B,Uspc2100|Receiver|TCG|Time|11,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,55,0,320
0xB011C,Uspc2100|Receiver|TCG|Gain|12,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB010C,Uspc2100|Receiver|TCG|Time|12,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,60,0,320
0xB011D,Uspc2100|Receiver|TCG|Gain|13,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB010D,Uspc2100|Receiver|TCG|Time|13,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,65,0,320
0xB011E,Uspc2100|Receiver|TCG|Gain|14,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB010E,Uspc2100|Receiver|TCG|Time|14,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,70,0,320
0xB011F,Uspc2100|Receiver|TCG|Gain|15,dB,ASEW,Receiver TCG point gain.,FLOAT,,0.1,0,0,35
0xB010F,Uspc2100|Receiver|TCG|Time|15,us,ASEW,Receiver TCG point time.,FLOAT,,0.05,75,0,320
0xB0000,Uspc2100|A-scan|Start Mode,,ASEW,Source of start trigger for A-scan display,INTEGER,,1,0,0,2,Initial Pulse=0,Artificial=1,Interface=2
0xB0001,Uspc2100|A-scan|Delay,us,ASEW,Delay in us,FLOAT,FLOAT,0.01,202.1,-10,310
0xB0002,Uspc2100|A-scan|Range,us,ASEW,Range in us,FLOAT,FLOAT,0.01,20.8,1,321
0xB0003,Uspc2100|A-scan|Display Mode,,ASEW,Rectification for A-scan display,INTEGER,,1,1,0,3,RF=0,Full=1,Pos=2,Neg=3
0xB0004,Uspc2100|A-scan|Reject,%,ASEW,Ascan Reject,FLOAT,FLOAT,1,0,0,100
0xB0005,Uspc2100|A-scan|Stream,,ASEW,Ascan Streaming Modes,INTEGER,,1,0,0,2,Disabled=0,Enabled=1,Gates=2
0xB0006,Uspc2100|A-scan|Step Gate Display,,ASEW,Ascan Step gate display mode,INTEGER,,1,1,0,1,Off=0,On=1
0xB0212,Uspc2100|System|One SHot,,ASEW,Generates one shot of data,INTEGER,,1,1,0,1,Off=0,On=1
0xB0030,Uspc2100|IF Gate|Delay,us,ASEW,Interface Gate Delay,FLOAT,,0.02,212.12,0,320
0xB0031,Uspc2100|IF Gate|Range,us,ASEW,Interface Gate Range,FLOAT,,0.02,8.35,0,320
0xB0032,Uspc2100|IF Gate|Threshold,%,ASEW,Interface Gate Flank Threshold,INTEGER,,1,80,0,100
0xB004A,Uspc2100|IF Gate|Trigger Mode,,ASEW,Source of trigger for Interface Gate,INTEGER,,1,0,0,1,Peak=0,Flank=1
0xB004C,Uspc2100|IF Gate|Rectify,,ASEW,Interface Gate rectify mode,INTEGER,,1,1,0,3,RF=0,Full=1,Pos=2,Neg=3
0xB0033,Uspc2100|WallThickness|Average Window,?,ASEW,WallThickness|Average Window,INTEGER,,1,0,0,100
0xB0035,Uspc2100|WallThickness|Start,,ASEW,Source of Wallthickness measurement START position,INTEGER,,1,0,0,3,IF=0,G1=1,G2(not used)=2,IP=3
0xB0036,Uspc2100|WallThickness|Stop,,ASEW,Source of Wallthickness measurement STOP position,INTEGER,,1,0,0,3,IF(not used)=0,G1=1,G2=2,IP=3
0xB004B,Uspc2100|WallThickness|Accumulate Mode,,ASEW,WallThickness Accumulate mode,INTEGER,,1,0,0,2,External=0,Sample=1,Instantaneous=2
0xB004D,Uspc2100|WallThickness|Noise Immun Count,,ASEW,WallThickness Noise Immun count,INTEGER,,1,0,0,15
0xB004E,Uspc2100|WallThickness|Min Limit,S,ASEW,WallThickness Min Limit,FLOAT,,0.2,0.2,0,5000
0xB004F,Uspc2100|WallThickness|Max Limit,S,ASEW,WallThickness Max Limit,FLOAT,,0.2,0.2,0,5000
0xB0050,Uspc2100|WallThickness|Num Echos,,ASEW,WallThickness Number of echos used,INTEGER,,1,2,1,4
0xB0051,Uspc2100|WallThickness|Zero Offset,S,ASEW,WallThickness Zero Offset,FLOAT,,0.2,0.2,0,5000
0xB0037,Uspc2100|Gates|Mode,,ASEW,Type of measurement for Gate(s),INTEGER,,1,1,1,2,Flaw=1,Thick=2
0xB0034,Uspc2100|Gates|Start Mode,,ASEW,Source of start trigger for Gate(s),INTEGER,,1,2,0,2,Initial Pulse=0,Artificial=1,Interface=2
0xB0060,Uspc2100|System|TDR,,ASEW,Function of external pin for measurement control,INTEGER,,1,0,0,3,Ignore_Enabled_output=0,Ignore_Disabled_output=1,Enable_High_Active=2,Enable_Low_Active=3
0xB0063,Uspc2100|Analog Out 1|Source out,,ASEW,Analog Output 1 signal source,INTEGER,,1,0,0,1,AMP_G1=0,AMP_G2=1
0xB0064,Uspc2100|Analog Out 1|Offset,%,ASEW,Analog Output 1 signal offset ,FLOAT,,0.1,0,0,127
0xB0065,Uspc2100|Analog Out 1|Range,%,ASEW,Analog Output 1 signal range,FLOAT,,0.1,5,0,127
0xB0067,Uspc2100|Analog Out 1|Timing,,ASEW,Analog Output 1 signal Timing,INTEGER,,1,1,0,2,Instant=0,Timed Max=1,Timed Min=2
0xB0066,Uspc2100|Analog Out 1|Muting,%,ASEW,Analog Output 1 muting,FLOAT,FLOAT,0.1,0,0,127
0xB0068,Uspc2100|Analog Out 2|Source out,,ASEW,Analog Output 2 signal source,INTEGER,,1,0,0,1,AMP_G1=0,AMP_G2=1
0xB0069,Uspc2100|Analog Out 2|Offset,%,ASEW,Analog Output 2 signal offset ,FLOAT,,0.1,0,0,127
0xB006A,Uspc2100|Analog Out 2|Range,%,ASEW,Analog Output 2 signal range,FLOAT,,0.1,5,0,127
0xB006C,Uspc2100|Analog Out 2|Timing,,ASEW,Analog Output 2 signal Timing,INTEGER,,1,1,0,2,Instant=0,Timed Max=1,Timed Min=2
0xB006B,Uspc2100|Analog Out 2|Muting,%,ASEW,Analog Output 2 muting,FLOAT,,0.1,0,0,127
0xB0222,Uspc2100|System|Version,Normal,RASE,All combined versions.,STRING,,256,<n/a>,<n/a>,
0xB0097,Uspc2100|Pulser|Sync Mode,,ASEW,Pulser sync mode,INTEGER,,1,1,0,1,Internal=0,External=1
0xB009C,Uspc2100|Pulser|Test Mode,,ASEW,Pulser probe test mode,INTEGER,,1,0,0,1,Single=0,Dual=1
0xB009E,Uspc2100|Pulser|Energy,,ASEW,Pulser Energy Level,INTEGER,,1,1,0,1,Low=0,High=1
0xB009F,Uspc2100|Pulser|Voltage,,ASEW,Pulser voltage level,INTEGER,,1,1,0,1,Low=0,High=1
0xB0098,Uspc2100|Receiver|Gain,dB,ASEW,Receiver gain,FLOAT,FLOAT,0.1,70,0,110
0xB009A,Uspc2100|Receiver|TCG|Enable,,ASEW,Receiver TCG Enable,INTEGER,,1,0,0,1,Off=0,On=1
0xB0223,Uspc2100|Receiver|TCG|Delay,us,ASEW,Receiver TCG delay time.,FLOAT,,0.05,0,0,320
0xB00A0,Uspc2100|Multiplexer|Table|Count,,ASEW,Number of Cyle Table Entries,INTEGER,,1,1,0,15
0xB0093,Uspc2100|Multiplexer|TransmitCycleTable,,ASEW,TransmitCycleTable,INTEGER,,1,65535,1,65535
0xB0094,Uspc2100|Multiplexer|ReceiveCycleTable,,ASEW,ReceiveCycleTable,INTEGER,,1,65535,1,65535
0xB0099,Uspc2100|Receiver|RF Filter,,ASEW,Receiver RF Filter selected,INTEGER,,1,4,0,4,1MHz=0,2.25MHz=1,5MHz=2,10MHz=3,BB=4
0xB009D,Uspc2100|Pulser|Damping,,ASEW,Pulser Damping,INTEGER,,1,2,0,2,500 Ohm=0,75 Ohm=1,50 Ohm=2
0xB0038,Uspc2100|Gate 1|Delay,us,ASEW,Gate 1 Delay,FLOAT,,0.02,1.45,0.18,320
0xB0039,Uspc2100|Gate 1|Range,us,ASEW,Gate 1 Range,FLOAT,,0.02,16.7,0.18,320
0xB003A,Uspc2100|Gate 1|Threshold,%,ASEW,Gate 1 Threshold,INTEGER,,1,30,0,100
0xB003B,Uspc2100|Gate 1|Noise Immun count,,ASEW,Gate 1 Noise Immun count,INTEGER,,1,1,0,15
0xB003C,Uspc2100|Gate 1|Average Window,,ASEW,Gate 1 Average Window,INTEGER,,1,1,0,65534
0xB003D,Uspc2100|Gate 1|Trigger Mode,,ASEW,Source of trigger for Gate 1,INTEGER,,1,0,0,2,Peak=0,Flank=1,Zero Crossing=2
0xB0044,Uspc2100|Gate 1|Accumulate Reset,,ASEW,Gate 1 Accumulate reset,INTEGER,,1,0,0,1,Off=0,Reset=1
0xB0046,Uspc2100|Gate 1|Rectify,,ASEW,Gate 1 rectify mode,INTEGER,,1,1,0,3,RF=0,Full=1,Pos=2,Neg=3
0xB0048,Uspc2100|Gate 1|Accumulate Mode,,ASEW,Gate 1 Accumulate mode,INTEGER,,1,2,0,2,External=0,Sample=1,Instantaneous=2
0xB003E,Uspc2100|Gate 2|Delay,us,ASEW,Gate 2 Delay,FLOAT,,0.02,1.45,0.18,320
0xB003F,Uspc2100|Gate 2|Range,us,ASEW,Gate 2 Range,FLOAT,,0.02,16.7,0.18,320
0xB0040,Uspc2100|Gate 2|Threshold,%,ASEW,Gate 2 Threshold,INTEGER,,1,30,0,100
0xB0041,Uspc2100|Gate 2|Noise Immun count,,ASEW,Gate 2 Noise Immun count,INTEGER,,1,1,0,15
0xB0042,Uspc2100|Gate 2|Average Window,,ASEW,Gate 2 Average Window,INTEGER,,1,1,0,65534
0xB0043,Uspc2100|Gate 2|Trigger Mode,,ASEW,Source of trigger for Gate 2,INTEGER,,1,0,0,2,Peak=0,Flank=1,Zero Crossing=2
0xB0045,Uspc2100|Gate 2|Accumulate Reset,,ASEW,Gate 2 Accumulate reset,INTEGER,,1,0,0,1,Off=0,Reset=1
0xB0047,Uspc2100|Gate 2|Rectify,,ASEW,Gate 2 rectify mode,INTEGER,,1,1,0,3,RF=0,Full=1,Pos=2,Neg=3
0xB0049,Uspc2100|Gate 2|Accumulate Mode,,ASEW,Gate 2 Accumulate mode,INTEGER,,1,2,0,2,External=0,Sample=1,Instantaneous=2
0x30002,Eddy Current|Channels,!,ASP,Eddy Current Number of available channels.,INTEGER,,1,1,1,8
0x30005,Eddy Current|Amplitude|Unit,V,RAS,Eddy Current Translates one unit of amplitude to a value.,FLOAT,,1e-99,1,1e-99,1000000000000
0x30401,Eddy Current|Channel|Trigger|Rep.Rate,Hz,ASP,Eddy Current Repetition rate for the internal trigger.,INTEGER,,1,1000,1,20000
0x30402,Eddy Current|Channel|Trigger|Sync Mode,!,ASP,Eddy Current Sync signal internal/external selection.,INTEGER,,1,1,0,1,Internal=0,External=1
0x30405,Eddy Current|Channel|Trigger|Time Unit,s,RAS,Eddy Current Time in seconds that is used for the time dependant results.,FLOAT,,1e-06,0.01,1e-06,100000000
0x30403,Eddy Current|Channel|Gates,!,RAS,Eddy Current Number of available gates.,INTEGER,,1,4,4,4
0x30522,Eddy Current|Channel|X|Attenuator,,AS,Eddy Current X Input sensitivity ±,INTEGER,,1,1,0,4,±10.0V=0,±5.0V=1,±2.5mV=2,±1.25V=3
0x30526,Eddy Current|Channel|X|Rectifier,!,AS,Eddy Current X Rectifier,INTEGER,,1,0,0,3,RF=0,FULL=1,1/2 POS=2,1/2 NEG=3
0x30520,Eddy Current|Channel|X|Calibration|Invert,!,AS,Eddy Current X Calibration inverter,INTEGER,,1,0,0,1,Non invert=0,Invert=1
0x30524,Eddy Current|Channel|X|Calibration|Offset,,AS,Eddy Current X Calibration offset.,FLOAT,,1,0,-2.5,2.5
0x30525,Eddy Current|Channel|X|Calibration|Scale,,AS,Eddy Current X Calibration scale or digital input zoom.,FLOAT,,0.001,1,1,100
0x3052C,Eddy Current|Channel|X|Amplitude|Unit,,RAS,Eddy Current X Translates one unit of amplitude to a value.,FLOAT,,1e-99,0.00122100122100122,1e-99,1000000000000
0x3052D,Eddy Current|Channel|X|Amplitude|Unit Select,,ASP,Eddy Current X Selectes Amplitude Unit.,INTEGER,,1,0,0,3,Default=0,Percentage=1,Volts=2,Meter=3
0x30505,Eddy Current|Channel|X|Enable,!,ASP,Eddy Current X Result Enabler.,INTEGER,,1,1,0,1,Off=0,On=1
0x30622,Eddy Current|Channel|Y|Attenuator,,AS,Eddy Current Y Input sensitivity ╠,INTEGER,,1,1,0,4,╠10.0V=0,╠5.0V=1,╠2.5mV=2,╠1.25V=3
0x30626,Eddy Current|Channel|Y|Rectifier,!,AS,Eddy Current Y Rectifier,INTEGER,,1,0,0,3,RF=0,FULL=1,1/2 POS=2,1/2 NEG=3
0x30620,Eddy Current|Channel|Y|Calibration|Invert,!,AS,Eddy Current Y Calibration inverter,INTEGER,,1,0,0,1,Non invert=0,Invert=1
0x30624,Eddy Current|Channel|Y|Calibration|Offset,,AS,Eddy Current Y Calibration offset.,FLOAT,,1,0,-2.5,2.5
0x30625,Eddy Current|Channel|Y|Calibration|Scale,,AS,Eddy Current Y Calibration scale or digital input zoom.,FLOAT,,0.001,1,1,100
0x3062C,Eddy Current|Channel|Y|Amplitude|Unit,,RAS,Eddy Current Y Translates one unit of amplitude to a value.,FLOAT,,1e-99,0.00122100122100122,1e-99,1000000000000
0x3062D,Eddy Current|Channel|Y|Amplitude|Unit Select,,ASP,Eddy Current Y Selectes Amplitude Unit.,INTEGER,,1,0,0,3,Default=0,Percentage=1,Volts=2,Meter=3
0x30605,Eddy Current|Channel|Y|Enable,!,ASP,Eddy Current Y Result Enabler.,INTEGER,,1,1,0,1,Off=0,On=1
0x30210,Eddy Current|Instrument|Identification,,RAS,Eddy Current Name parameter. [SV ,STRING,,80,,,
0x30212,Eddy Current|Instrument|Frequency,Hz,ASP,Eddy Current Driver frequency. [B] ,FLOAT,,1,2000000,10,10000000
0x30213,Eddy Current|Instrument|Pre-Gain,,ASP,Eddy Current Preamplifier gain. [B] ,INTEGER,,1,2,0,6,-20 dB=0,-10 dB=1,0 dB=2,10 dB=3,20 dB=4,30 dB=5,40 dB=6
0x30214,Eddy Current|Instrument|Gain,dB,ASP,Eddy Current Main Amplifier gain. [B] ,INTEGER,,1,20,0,60
0x30215,Eddy Current|Instrument|Vertical Spread,dB,ASP,Eddy Current Vertical spread. [B] ,INTEGER,,1,0,0,20
0x30216,Eddy Current|Instrument|Horizontal Spread,dB,ASP,Eddy Current Horizontal spread. [B] ,INTEGER,,1,0,0,20
0x30217,Eddy Current|Instrument|Phase,Deg,ASP,Eddy Current Phase. [B] ,INTEGER,,1,0,0,359
0x30218,Eddy Current|Instrument|Bandwidth Limit,,ASP,Eddy Current Bandwidth Limit. [B] ,INTEGER,,1,0,0,2,On=0,Off=1
0x30219,Eddy Current|Instrument|HP Filter,,ASP,Eddy Current High pass filter. [B] ,INTEGER,,1,0,0,40,0.5 Hz=0,1.5 Hz=1,2.1 Hz=2,2.8 Hz=3,3.5 Hz=4,4 Hz=5,5 Hz=6,6.5 Hz=7,8 Hz=8,10 Hz=9,12.5 Hz=10,16 Hz=11,20 Hz=12,25 Hz=13,32 Hz=14,40 Hz=15,50 Hz=16,65 Hz=17,80 Hz=18,100 Hz=19,125 Hz=20,160 Hz=21,200 Hz=22,250 Hz=23,300 Hz=24,400 Hz=25,500 Hz=26,650 Hz=27,800 Hz=28,1 kHz=29,1.25 kHz=30,1.6 kHz=31,2 kHz=32,2.5 kHz=33,3.2 kHz=34
0x3021A,Eddy Current|Instrument|LP Filter,,ASP,Eddy Current Low pass filter. [B] ,INTEGER,,1,0,0,39,0.5 Hz=0,1.5 Hz=1,2.1 Hz=2,2.8 Hz=3,3.5 Hz=4,4 Hz=5,5 Hz=6,6.5 Hz=7,8 Hz=8,10 Hz=9,12.5 Hz=10,16 Hz=11,20 Hz=12,25 Hz=13,32 Hz=14,40 Hz=15,50 Hz=16,65 Hz=17,80 Hz=18,100 Hz=19,125 Hz=20,160 Hz=21,200 Hz=22,250 Hz=23,300 Hz=24,400 Hz=25,500 Hz=26,650 Hz=27,800 Hz=28,1 kHz=29,1.25 kHz=30,1.6 kHz=31,2 kHz=32,2.5 kHz=33,3.2 kHz=34
0x3021C,Eddy Current|Instrument|?,units,RAS,Eddy Current ? [?] ,UNDEF,,,,,
0x99003,Project|Sound Velocity|Medium,m/s,AS,Sound velocity of the medium,FLOAT,,1,1480,100,10000
0x99004,Project|Sound Velocity|Material,m/s,AS,Sound velocity of the inspected material,FLOAT,,1,5820,100,10000
0x99005,Project|System|Time,s,AS,Current time,STRING,,100,0,0,0
0x99006,Project|System|Date,s,AS,Current Date,STRING,,100,0,0,0
0x99201,Project|Ultrasonic|Threshold,%,ASP,UT C-scan display threshold.,FLOAT,,1,30,0,100
0x99214,Project|Eddy Current|Peak-to-Peak|Threshold,V,ASP,ET Peak-Peak C-scan display threshold.,FLOAT,,0.01,2,0,4
0x9E705,Ultrasonic|Receiver|Gain|Scan|Active,,AS,Ultrasonic receiver TCG Receiver Scanning gain active.,INTEGER,,1,0,0,1,Off=0,On=1
0x9E706,Ultrasonic|Receiver|Gain|Scan|Gain,dB,ASP,Ultrasonic receiver TCG Receiver Scanning Gain Correction value.,FLOAT,,0.1,0,-110,110
0x9E707,Ultrasonic|Receiver|Gain|TCG Corr|Active,,AS,Ultrasonic receiver TCG Receiver TCG Gain Correction Scanning gain active.,INTEGER,,1,0,0,1,Off=0,On=1
0x9E708,Ultrasonic|Receiver|Gain|Main,dB,ASP,Ultrasonic receiver TCG Receiver Gain value before correction with scanning gain.,FLOAT,,0.1,70,0,110
0x9E701,Ultrasonic|Receiver|Gain|Target Amplitude,%,ASP,Ultrasonic receiver TCG target amplitude.,INTEGER,,1,80,0,128
0x9E711,Ultrasonic|Receiver|TCG|Point  1|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 1 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E721,Ultrasonic|Receiver|TCG|Point  2|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 2 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E731,Ultrasonic|Receiver|TCG|Point  3|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 3 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E741,Ultrasonic|Receiver|TCG|Point  4|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 4 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E751,Ultrasonic|Receiver|TCG|Point  5|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 5 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E761,Ultrasonic|Receiver|TCG|Point  6|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 6 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E771,Ultrasonic|Receiver|TCG|Point  7|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 7 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E781,Ultrasonic|Receiver|TCG|Point  8|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 8 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E791,Ultrasonic|Receiver|TCG|Point  9|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 9 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E7A1,Ultrasonic|Receiver|TCG|Point 10|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 10 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E7B1,Ultrasonic|Receiver|TCG|Point 11|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 11 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E7C1,Ultrasonic|Receiver|TCG|Point 12|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 12 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E7D1,Ultrasonic|Receiver|TCG|Point 13|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 13 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E7E1,Ultrasonic|Receiver|TCG|Point 14|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 14 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E7F1,Ultrasonic|Receiver|TCG|Point 15|L-Gain,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 15 Local Gain value (before correction).,FLOAT,,0.1,0,0,35
0x9E712,Ultrasonic|Receiver|TCG|Point  1|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 1 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E722,Ultrasonic|Receiver|TCG|Point  2|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 2 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E732,Ultrasonic|Receiver|TCG|Point  3|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 3 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E742,Ultrasonic|Receiver|TCG|Point  4|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 4 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E752,Ultrasonic|Receiver|TCG|Point  5|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 5 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E762,Ultrasonic|Receiver|TCG|Point  6|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 6 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E772,Ultrasonic|Receiver|TCG|Point  7|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 7 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E782,Ultrasonic|Receiver|TCG|Point  8|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 8 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E792,Ultrasonic|Receiver|TCG|Point  9|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 9 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E7A2,Ultrasonic|Receiver|TCG|Point 10|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 10 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E7B2,Ultrasonic|Receiver|TCG|Point 11|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 11 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E7C2,Ultrasonic|Receiver|TCG|Point 12|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 12 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E7D2,Ultrasonic|Receiver|TCG|Point 13|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 13 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E7E2,Ultrasonic|Receiver|TCG|Point 14|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 14 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x9E7F2,Ultrasonic|Receiver|TCG|Point 15|Corr,dB,ASP,Ultrasonic receiver TCG Receiver Tcg Point 15 Local Gain Correction value.,FLOAT,,0.1,0,-12,35
0x57001,Project|Binder|Medium Velocity,m/s,ASE,Sound velocity of the medium.,FLOAT,,1,3020,100,10000
0x50005,Project|Project,N,RAE,Reference name of the loaded project.,STRING,,256,,,
0x50006,Project|Inspection,N,RAE,Reference name of the current inspection.,STRING,,256,,,
0x50007,Project|Medium,F,APE,Name of the medium used for the part.,STRING,,40,,,
0x53100,Project|Calibration|PreCal|Eddy Current|Instrument|Pre-Gain,,RAE,PreCal copy of; Eddy Current Preamplifier gain. [B] ,INTEGER,,1,2,0,0
0x53101,Project|Calibration|PreCal|Eddy Current|Instrument|Gain,dB,RAE,PreCal copy of; Eddy Current Main Amplifier gain. [B] ,INTEGER,,1,20,0,0
0x53102,Project|Calibration|PreCal|Eddy Current|Instrument|Phase,Deg,RAE,PreCal copy of; Eddy Current Phase. [B] ,INTEGER,,1,0,0,0
0x53103,Project|Calibration|PreCal|Eddy Current|Instrument|Frequency,Hz,RAE,PreCal copy of; Eddy Current Driver frequency. [B] ,FLOAT,,1,2000000,10,10
0x53104,Project|Calibration|PreCal|Eddy Current|Instrument|HP Filter,,RAE,PreCal copy of; Eddy Current High pass filter. [B] ,INTEGER,,1,0,0,0
0x53105,Project|Calibration|PreCal|Eddy Current|Instrument|LP Filter,,RAE,PreCal copy of; Eddy Current Low pass filter. [B] ,INTEGER,,1,0,0,0
0x53200,Project|Calibration|PostCal|Eddy Current|Instrument|Pre-Gain,,RAE,PostCal copy of; Eddy Current Preamplifier gain. [B] ,INTEGER,,1,2,0,0
0x53201,Project|Calibration|PostCal|Eddy Current|Instrument|Gain,dB,RAE,PostCal copy of; Eddy Current Main Amplifier gain. [B] ,INTEGER,,1,20,0,0
0x53202,Project|Calibration|PostCal|Eddy Current|Instrument|Phase,Deg,RAE,PostCal copy of; Eddy Current Phase. [B] ,INTEGER,,1,0,0,0
0x53203,Project|Calibration|PostCal|Eddy Current|Instrument|Frequency,Hz,RAE,PostCal copy of; Eddy Current Driver frequency. [B] ,FLOAT,,1,2000000,10,10
0x53204,Project|Calibration|PostCal|Eddy Current|Instrument|HP Filter,,RAE,PostCal copy of; Eddy Current High pass filter. [B] ,INTEGER,,1,0,0,0
0x53205,Project|Calibration|PostCal|Eddy Current|Instrument|LP Filter,,RAE,PostCal copy of; Eddy Current Low pass filter. [B] ,INTEGER,,1,0,0,0
0x52000,Project|Report|Custom|UT|Standard|P/N,F,ASEW,Calibration part number.,STRING,,40,?,,
0x52001,Project|Report|Custom|UT|Standard|S/N,F,ASEW,Calibration serial number.,STRING,,40,?,,
0x52002,Project|Report|Custom|UT|Probe|P/N,F,ASEW,Probe part number.,STRING,,40,?,,
0x52003,Project|Report|Custom|UT|Probe|S/N,F,ASEW,Probe serial number.,STRING,,40,?,,
0x52004,Project|Report|Custom|UT|Instrument|P/N,F,ASEW,Instrument part number.,STRING,,40,?,,
0x52005,Project|Report|Custom|UT|Instrument|S/N,F,ASEW,Instrument serial number.,STRING,,40,?,,
0x52006,Project|Report|Custom|UT|Recorder|P/N,F,ASEW,Recorder part number.,STRING,,40,?,,
0x52007,Project|Report|Custom|UT|Recorder|S/N,F,ASEW,Recorder serial number.,STRING,,40,?,,
0x52008,Project|Report|Custom|ET|Standard|P/N,F,ASEW,Calibration part number.,STRING,,40,?,,
0x52009,Project|Report|Custom|ET|Standard|S/N,F,ASEW,Calibration serial number.,STRING,,40,?,,
0x5200A,Project|Report|Custom|ET|Probe|P/N,F,ASEW,Probe part number.,STRING,,40,?,,
0x5200B,Project|Report|Custom|ET|Probe|S/N,F,ASEW,Probe serial number.,STRING,,40,?,,
0x5200C,Project|Report|Custom|ET|Instrument|P/N,F,ASEW,Instrument part number.,STRING,,40,?,,
0x5200D,Project|Report|Custom|ET|Instrument|S/N,F,ASEW,Instrument serial number.,STRING,,40,?,,
0x5200E,Project|Report|Custom|ET|Recorder|P/N,F,ASEW,Recorder part number.,STRING,,40,?,,
0x5200F,Project|Report|Custom|ET|Recorder|S/N,F,ASEW,Recorder serial number.,STRING,,40,?,,
0x52010,Project|Report|Custom|ESN,F,ASEW,ESN number.,STRING,,40,?,,
0x52011,Project|Report|Custom|SAP-AUN,F,ASEW,SAP-AUN number.,STRING,,40,?,,
0x52012,Project|Report|Custom|ARF,F,ASEW,ARF number.,STRING,,40,?,,
0x52013,Project|Report|Custom|Comment,F,ASEW,Project comment.,STRING,,256,?,,
0x5F001,Project|Method|Circle|Part,F,RAPE,Part Name.,STRING,,40,,,
0x5F002,Project|Method|Circle|Tool,F,APE,Name of the used tool.,STRING,,40,,,
0x5F014,Project|Method|Circle|Tool|Serial,N,RAE,Tool Serial number of the used tool.,STRING,,40,,,
0x5F003,Project|Method|Circle|Element,F,APE,Name of the used element.,STRING,,40,,,
0x5F015,Project|Method|Circle|Element|Serial,N,RAE,Serial number of the used element.,STRING,,40,,,
0x5F016,Project|Method|Circle|Material,F,APE,Name of the inspected material.,STRING,,40,,,
0x5F017,Project|Method|Circle|Material|Velocity,m/s,RAE,Sound velocity of the material.,FLOAT,,1,3020,100,10000
0x5F018,Project|Method|Circle|Velocity Mode,,APE,Determines material sound velocity mode in shear or longitudinal.,INTEGER,,1,0,0,1,Longitudinal=0,Shear=1
0x5F004,Project|Method|Circle|Settings,F,APE,Settings name.,STRING,,40,,,
0x5F005,Project|Method|Circle|Area,F,RAPE,Scan area name.,STRING,,40,,,
0x5F006,Project|Method|Circle|Method,F,RAPE,Scan method name.,STRING,,40,,,
0x5F007,Project|Method|Circle|State,,RAPE,Scan method state.,INTEGER,,1,0,-2,5,CFGERR=-2,ERROR=-1,NOINIT=0,READY=1,ACTIVE=2,CHECKING=3,SCANNING=4,EVALUATING=5
0x5F008,Project|Method|Circle|Calibration Type,y/n,APE,Determines the type of calibration and the behaviour in the project.,INTEGER,,1,0,0,2,None=0,PreCal=1,PostCal=2
0x5F009,Project|Method|Circle|Calibration Set,F,APE,Calibration parameter set name.,STRING,,40,,,
0x5F011,Project|Method|Circle|Index Count,x,RAE,Amount indices in the scan.,INTEGER,,1,0,0,0
0x5F012,Project|Method|Circle|Analysis|Start,x,APE,Restricted analysis range start fraction of the scan area.,FLOAT,,0.001,0,0,1
0x5F013,Project|Method|Circle|Analysis|Stop,x,APE,Restricted analysis range start fraction of the scan area.,FLOAT,,0.001,1,0,1
0x5F010,Project|Method|Circle|Entry,x,RAE,Entry in the project scan list.,INTEGER,,1,0,0,0
0x5F10A,Project|Method|Circle|Inspection|Gate Mode,,APE,Determines the way gates are positioned automatically.,INTEGER,,1,0,0,3,Manual=0,Surface=1,Material=2,Backwall=3
0x5F1FF,Project|Method|Circle|Angle Calculation,,APE,The way the angles are used in calculations.,INTEGER,,1,0,0,2,None=0,Material=1,Surface=2
0x5F107,Project|Method|Circle|Surface|Angle B,rad,APE,Surface angle of inspection as a rotation around the surface local Y-axis.,FLOAT,,0.001,0,-1.570796,1.570796
0x5F108,Project|Method|Circle|Surface|Angle A,rad,APE,Surface angle of inspection as a rotation around the surface local X-axis.,FLOAT,,0.001,0,-1.570796,1.570796
0x5F105,Project|Method|Circle|Material|Angle B,rad,APE,Angle of inspection as a rotation around the surface local Y-axis.,FLOAT,,0.001,0,-1.570796,1.570796
0x5F106,Project|Method|Circle|Material|Angle A,rad,APE,Angle of inspection as a rotation around the surface local X-axis.,FLOAT,,0.001,0,-1.570796,1.570796
0x5F118,Project|Method|Circle|Depth|Factor,,RAE,A ratio factor between depth from the surface and depth in the material.,FLOAT,,1e-06,1,0,1
0x5F1FE,Project|Method|Circle|Depth|Max,m,RAPE,Maximum allowed depth for any depth mode.,FLOAT,,1e-05,1,1e-05,1
0x5F1FD,Project|Method|Circle|Depth|Offset,m,RAPE,Offset to take in account from the scan specified depth.,FLOAT,,1e-05,0,0,1
0x5F117,Project|Method|Circle|Depth,m,APE,Uncorrected calculated depth value.,FLOAT,,1e-05,0,0,1
0x5F104,Project|Method|Circle|Density|Mode,!,APE,Switches between radial density mode in static or dynamic angles.,INTEGER,,1,0,0,1,Static=0,Dynamic=1
0x5F202,Project|Method|Circle|Scan|Velocity,rad/s,APE,Velocity during the scan of the line pattern.,FLOAT,,0.0001,0,0,7
0x5F102,Project|Method|Circle|Scan|Density,m,APE,Distance between measurements on the line.,FLOAT,,1e-06,1e-06,1e-06,1
0x5F103,Project|Method|Circle|Scan|Length,rad,RAE,Width of a scan line.,FLOAT,,0.01,6.283185,6.283185,0,6.283185=0
0x5F101,Project|Method|Circle|Index|Density,m,APE,Distance between measurements.,FLOAT,,1e-05,0.001,1e-05,1
0x5F100,Project|Method|Circle|Index|Steps,x,RAE,Steps to reach end of the contour.,INTEGER,,1,0,0,0
0x5F201,Project|Method|Circle|Surface|Speed,m/s,APE,Speed at the surface for calculating the velocity of an axis.,FLOAT,,1e-06,0.3,0,1
0x5F20A,Project|Method|Circle|Surface|Back Side,,APE,Use backside of the defined surface area.,INTEGER,,1,0,0,1,Top Side=0,Back Side=1
0x5F20B,Project|Method|Circle|Surface|Rotate,,APE,Rotate the defined surface around the surface Z-axis.,INTEGER,,1,0,0,3,0 Deg=0,90 Deg=1,180 Deg=2,270 Deg=3
)";


TreeModel::TreeItem* TreeModel::createPath(const QStringList& namePath)
{
	// When not empty.
	if (!namePath.empty())
	{
		auto cur = _rootItem;
		// Iterate through the name path names.
		for (auto it = namePath.begin(); it != namePath.end(); it++)
		{
			// Find the name in the child list.
			auto found = std::find_if(cur->_childItems.begin(), cur->_childItems.end(), [&](const TreeItem* i) -> bool
			{
				return *it == i->_name;
			});
			// When found.
			if (found != cur->_childItems.end())
			{
				// Restart the loop using the found item when not at the end.
				if (it + 1 != namePath.end())
				{
					cur = *found;
					continue;
				}
				else
				{
					// When at the end of the list return the found item.
					return *found;
				}
			}
			else
			{
				// Create new items for the last part of the name list.
				for (auto itn = it; itn != namePath.end(); itn++)
				{
					cur = new TreeItem(cur, *itn);
				}
				return cur;
			}
		}
	}
	return nullptr;
}

void TreeModel::update()
{
	sf::strings lines;
	lines.split(VariableSpecs, '\n');
	//
	for (auto& line: lines)
	{
		sf::strings fields;
		fields.split(line, ',', '"');
		if (fields.size() >= 2)
		{
			auto namePath = QString::fromStdString(fields.at(1)).split('|');
			createPath(namePath);
			//qDebug() << "" << namePath.join('/');
		}
	}
}
