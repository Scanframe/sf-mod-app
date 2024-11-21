#pragma once

#include "AxesCoord.h"
#include "GmiTypes.h"
#include "global.h"
#include <misc/gen/TVector.h>
#include <misc/gen/Value.h>

namespace sf::gmi
{

/**
 * @brief Forward declarations of TController.
 */
class _GMI_CLASS Controller;

/**
 * @brief Structure to hold a single state
 */
class _GMI_CLASS ParamState
{
	public:
		/**
		 * @brief Copy constructor.
		 */
		ParamState(const ParamState& ps);

		/**
		* @brief Copy constructor.
		*/
		ParamState(const std::string& name, Value value);

		/**
		 * @brief Compare equal operator.
		 */
		bool operator==(const ParamState& ps) const;

		/**
		 * @brief Holds the name of the state.
		 */
		std::string _name;
		/**
		 * @brief Holds the value of the state.
		 */
		Value _value;

		/**
		 * @brief Vector holding param-state instances.
		 */
		typedef TVector<ParamState> Vector;
};

/**
 * @brief Parameter flags for defining parameter behaviour.
 */
enum EParamFlag
{
	/** @brief Parameter cannot be changed at all times. */
	pfREADONLY = 1 << 0,
	/** @brief Parameter effects other parameters. */
	pfEFFECTPARAM = 1 << 1,
	/** @brief Parameter effects results. */
	pfEFFECTRESULT = 1 << 2,
	/** @brief Parameter is axis specific. */
	pfAXIS = 1 << 3,
	/** @brief Parameter cannot be changed when moving in any mode. */
	pfMOVE = 1 << 4,
	/** @brief Parameter cannot be changed when moving in position mode.*/
	pfMOVEPOS = 1 << 5,
	/** @brief Parameter cannot be changed when moving in velocity mode. */
	pfMOVEVEL = 1 << 6,
	/** @brief Parameter is exported globally. */
	pfEXPORT = 1 << 7,
	/** @brief Parameter is a system setting. */
	pfSYSTEM = 1 << 8,
	/** @brief Parameter is always writeable in any mode. */
	pfWRITEABLE = 1 << 9,
	/** @brief Alias of another parameter in a different form. */
	pfALIAS = 1 << 10,
	//
	// Parameters like these must not be used to restore.
	//
	/** @brief Parameter can be stored for restoring settings. */
	pfARCHIVE = 1 << 11,
	/** @brief Parameter cannot be changed when moving in continuous mode.*/
	pfMOVECON = 1 << 12,
};

/**
 * @brief  Result flags for defining result behaviour.
 */
enum EResultFlag
{
	/** @brief Result contains synced i/o input. */
	rfINDEX = 1 << 0,
	/** @brief Result data is not sync synchrone. */
	rfASYNC = 1 << 1,
	/** @brief Result keeps track of sync counter values at the time async data was generated. */
	rfASYNCINDEX = 1 << 2,
	/** @brief Result that generates a huge amount of data. */
	rfHUGE = 1 << 3,
};

/**
 * @brief Structure to hold parameter information.
 */
struct _GMI_CLASS ParamInfo
{
		/**
	 * @brief Default constructor.
	 */
		ParamInfo() = default;

		/**
	 * @brief Copy constructor.
	 * @param pi
	 */
		ParamInfo(const ParamInfo& pi)
		{
			copyFrom(pi);
		}

		/**
	 * @brief Assignment operator.
	 * @param pi
	 * @return
	 */
		ParamInfo& operator=(const ParamInfo& pi)
		{
			return copyFrom(pi);
		}

		/**
	 * @brief initialize/reset the instance as when it was constructed.
	 */
		void Init()
		{
			Id = 0;
			Flags = 0;
			Axis = 0;
			Index = 0;
			Name.clear();
			Unit = Name;
			Description = Name;
			Round.setType(Value::vitUndefined);
			Default.set(Round);
			Minimum.set(Round);
			Maximum.set(Round);
			States.flush();
		}

		/**
	 * @brief Sets the e instance as when it was constructed.
	 */
		ParamInfo& copyFrom(const ParamInfo& pi)
		{
			Id = pi.Id;
			Flags = pi.Flags;
			Controller = pi.Controller;
			Axis = pi.Axis;
			Index = pi.Index;
			Name = pi.Name;
			Unit = pi.Unit;
			Description = pi.Description;
			Round = pi.Round;
			Default = pi.Default;
			Maximum = pi.Maximum;
			Minimum = pi.Minimum;
			States = pi.States;
			return *this;
		}

		/** @brief Holds a unique id for the current situation. */
		IdType Id{0};
		/** @brief  Holds the controller instance number. */
		int Controller{0};
		/** @brief  Holds the gate number this gate belongs to. When Axis is UINT_MAX the param is not an axis related param. */
		unsigned Axis{0};
		/** @brief Param index in the hierarchy of the gate or non gate params. */
		unsigned Index{0};
		/** @brief  Name of the parameter. */
		std::string Name;
		/** @brief  Unit of the parameter. */
		std::string Unit;
		/** @brief  Usage off the parameter of the parameter. */
		std::string Description;
		/** @brief  Round or step value of the parameter. */
		Value Round{Value::vitUndefined};
		/** @brief  Default or start value. */
		Value Default{Value::vitUndefined};
		/** @brief  Minimum allowed value. */
		Value Minimum{Value::vitUndefined};
		/** @brief  Maximum allowed value. */
		Value Maximum{Value::vitUndefined};
		/** @brief  Number of specified states for this. */
		ParamState::Vector States;
		/** @brief  Parameter is readonly. */
		int Flags{0};
};

/**
 * @brief  Structure to hold parameter information
 */
struct _GMI_CLASS ResultInfo
{
		ResultInfo() = default;

		ResultInfo(const ResultInfo& pi) { Copy(pi); }

		ResultInfo& operator=(const ResultInfo& pi) { return Copy(pi); }

		void Init()
		{
			Id = 0;
			Flags = 0;
			Axis = 0;
			Index = 0;
			Name = "";
			Description = Name;
			WordSize = 0;
			ArraySize = 0;
			Bits = 0;
		}

		ResultInfo& Copy(const ResultInfo& pi)
		{
			Id = pi.Id;
			Flags = pi.Flags;
			Name = pi.Name;
			Axis = pi.Axis;
			Index = pi.Index;
			WordSize = pi.WordSize;
			ArraySize = pi.ArraySize;
			Bits = pi.Bits;
			return *this;
		}

		/**
	 * @brief Holds a unique id for the current situation.
	 */
		IdType Id{0};
		/**
	 * @brief Flags
	 */
		int Flags{0};
		/**
	 * @brief  Holds the gate number this gate belongs to.
	 * When gate is UINT_MAX the param is not a gate related param.
	 */
		unsigned Axis{0};
		/**
	 * @brief Param index in the hierarchy of the gate or non gate params.
	 */
		unsigned Index{0};
		/**
	 * @brief  Name of the parameter.
	 */
		std::string Name;
		/**
	 * @brief Usage off the parameter of the parameter.
	 */
		std::string Description;
		/**
	 * @brief Amount of Bits used to store the actual value.
	 */
		unsigned Bits{0};
		/**
	 * @brief  Amount of bytes 1, 2 or 4 (later 8) used for storing a single value.
	 */
		unsigned WordSize{0};
		/**
	 * @brief  Amount of values in an array for an arrayed value.
	 */
		unsigned ArraySize{0};
};

/**
 * @brief Structure to hold data update information.
 */
struct BufferInfo
{
		/**
	 * @brief Initializing constructor.
	 */
		BufferInfo() = default;

		/**
	 * @brief Clears all members.
	 */
		void Clear();

		/**
	 * @brief Holds the ID of the result which is responsible for this data.
	 */
		IdType Id{0};
		/**
	 * @brief Holds the buffer pointer.
	 */
		void* Buffer{nullptr};
		/**
	 * @brief Holds the size of the buffer.
	 */
		unsigned Size{0};
		/**
	 * @brief Holds the remaining size to store.
	 */
		unsigned Remain{0};
		/**
	 * @brief Holds the counter value of the amount of measurements since the last reset of the implementation.
	 */
		unsigned Counter{0};
};

inline void BufferInfo::Clear()
{
	Id = 0;
	Buffer = nullptr;
	Size = 0;
	Remain = 0;
	Counter = 0;
}

/**
 * @brief Function type for the function type which is called when a parameter
 * changes as a result of changes in the interface itself.
 */
typedef void (*NotifyProc)(void* data, IdType id);

/**
 * @brief Motion default parameters.
 * List of ids which must always be implemented for each motion implementation.
 */
enum EParam
{
	/**
	 * @brief Used to indicate no parameter. Interface parameters from here.
	 */
	mpNONE = 0x0000,
	/**
	 * @brief Version information of software and hardware drivers and implementation.
	 */
	mpVERSION = 0x0001,
	/**
	 * @brief Amount of implemented axes. The states carry the axes names.
	 */
	mpAXISCOUNT = 0x0002,
	/**
	 * Starts a movement using the current target velocity position or loaded curve. See enum EMovePos.
	 */
	mpMOVEPOS = 0x0003,
	/**
	 * @brief Starts a movement using the target velocity for axis in the velocity mode group
	 */
	mpMOVEVEL = 0x0004,
	/**
	 * @brief Controls the access to the joystick. Inhibit=-1 (Only when Off), Off=0, Stop=1 or On=2.
	 */
	mpJOYSTICK = 0x0005,
	/**
	 * @brief Sets the axis which pulses are used to derive the trigger pulses. Values are as EAxisLoc.
	 */
	mpTRIGGER_AXIS = 0x0006,
	/**
	 * @brief Density of measurements in radians or meters depending on selected axis.
	 */
	mpTRIGGER_DENSITY = 0x0007,
	/**
	 * @brief Trigger mode 'true' is intern 'false' is extern.
	 */
	mpTRIGGER_MODE = 0x0008,

	/**
	 * @brief Joystick axis selection. Values are as EAxisLoc.
	 */
	mpJOYSTICK_AXIS = 0x0009,
	/**
	 * @brief Position orientation event source selection. Values are as EAxisLoc.
	 */
	mpPOP_AXIS = 0x000A,
	/**
	 * @brief Enable output of trigger, true is enable.
	 */
	mpTRIGGER_ENABLE = 0x000B,
	/**
	 * @brief Starts a movement using the target velocity for axis in the continuous mode group.
	 */
	mpMOVECON = 0x000C,
	/**
	 * @brief Controls chuck-jaw Off=0, Open=1 and Close=2.
	 */
	mpCHUCKJAW = 0x000D,
	/**
	 * @brief Reports error and is used to clear it if possible.
	 * No error=0, Warning=1, Soft Error=2, Hard Error=3.
	 * Hard errors need restart (of system/application)
	 */
	mpERROR = 0x000E,
	/**
	 * @brief Error message from above error code.
	 */
	mpERROR_MESSAGE = 0x000F,
	/**
	 * @brief Trigger frequency in Hz.
	 */
	mpTRIGGER_FREQ = 0x0010,
	/**
	 * @brief Time unit in Seconds.
	 */
	mpTIMEUNIT = 0x0011,
	/**
	 * @brief Free number from here. Axis parameters from here.
	 */
	mpLAST = 0x0200,
	/**
	 * @brief Mask to detect gated parameter.
	 */
	mpAXIS_MASK = 0x8000,
	/**
	 * @brief Name of this axis.
	 */
	mpAXIS_NAME = 0x8001,
	/**
	 * @brief Multiplication factor for the index positions.
	 */
	mpAXIS_IDX_POS = 0x8002,
	/**
	 * @brief Mode of the axis (Disabled, Position, Velocity, Home, Contineous)
	 */
	mpAXIS_MODE = 0x8003,
	/**
	 * @brief Accuracy of the axis for velocity and position. (readonly)
	 */
	mpAXIS_ACCURACY = 0x8004,
	/**
	 * @brief Resolution of the axis for velocity and position. (readonly)
	 */
	mpAXIS_RESOLUTION = 0x8005,
	//
	// Axis minimum and maximum positions.
	//
	/**
	 * @brief Minimum position of the axis.
	 */
	mpAXIS_MIN_POS = 0x8010,
	/**
	 * @brief Maximum position of the axis.
	 */
	mpAXIS_MAX_POS = 0x8011,
	/**
	 * @brief Maximum velocity of the axis.
	 */
	mpAXIS_MAX_VEL = 0x8012,
	/**
	 * @brief Maximum acceleration of the axis.
	 */
	mpAXIS_MAX_ACC = 0x8013,
	/**
	 * @brief offset to position of Axis.
	 */
	mpAXIS_OFS_POS = 0x8014,
	//
	// Volatile data. It changes when moving.
	//
	/**
	 * @brief Current position of the axis.
	 */
	mpAXIS_CUR_POS = 0x8020,
	/**
	 * @brief Current velocity of the axis.
	 */
	mpAXIS_CUR_VEL = 0x8021,
	//
	// Targeting data before move.
	//
	/**
	 * @brief Target position of the axis.
	 */
	mpAXIS_TRG_POS = 0x8030,
	/**
	 * @brief Target velocity of the axis.
	 */
	mpAXIS_TRG_VEL = 0x8031,
	/**
	 * @brief Target acceleration of the axis.
	 */
	mpAXIS_TRG_ACC = 0x8032,
	//
	// Effects all position fields.
	//
	/**
	 * @brief Defines position after homing of the axis.
	 */
	mpAXIS_HOME_OFS = 0x8033,
	/**
	 * @brief Defines Movement of axis ( linear=0, radial=1, radial_limited=2) .
	 */
	mpAXIS_MOVEMENT = 0x8034,
	/**
	 * @brief Free axis parameters numbers from here.
	 */
	mpAXIS_LAST = 0x8200,
};

/**
 * @brief Motion default results.
 */
enum EResult
{
	/**
	 * @brief Free number from here.
	 */
	mrLAST = 0x0200,
	// Axis results from here.
	/**
	 * @brief Positions of the axis at each index or
	 */
	mrAXIS_INDEX_POS = 0x8000,
	// measurement trigger. Position is relative from
	/**
	 * @brief Free axis parameters numbers from here.
	 */
	mrAXIS_LAST = 0x8200,
};

/**
 * @brief Enumerate for controller position movement command.
 */
enum EMovePosCmd
{
	/**
	 * @brief Stop current movement smooth.
	 */
	mpcCOMPLETE = 0,
	/**
	 * @brief Abort current movement abrupt.
	 */
	mpcABORT = 1,
	/**
	 * @brief Stop current movement smooth.
	 */
	mpcSTOP = 2,
	/**
	 * @brief Move in position mode using targeting position,
	 */
	mpcPOSITION = 3,
	/**
	 * @brief Move along the downloaded curve.
	 */
	mpcCURVE = 4,
};

/**
 * @brief Enumerate for controller velocity movement command.
 */
enum EMoveVelCmd
{
	/**
	 * @brief No movement.
	 */
	mvcCOMPLETE = 0,
	/**
	 * @brief Abort current movement abrupt.
	 */
	mvcABORT = 1,
	/**
	 * @brief Stop with target deceleration value.
	 */
	mvcSTOP = 2,
	/**
	 * @brief Moving with target velocity.
	 */
	mvcON = 3,
};

/**
 * @brief Enumerate for controller continuous movement command.
 */
enum EMoveConCmd
{
	/**
	 * @brief No movement.
	 */
	mccCOMPLETE = 0,
	/**
	 * @brief Abort current movement abrupt.
	 */
	mccABORT = 1,
	/**
	 * @brief Stop with target deceleration value.
	 */
	mccSTOP = 2,
	/**
	 * @brief Moving with target velocity.
	 */
	mccON = 3,
};

/**
 * @brief Enumerate for controller Joy movement command.
 */
enum EJoystickCmd
{
	/**
	 * @brief  Inhibits the joystick control.
	 */
	jscINHIBIT = -1,
	/**
	 * @brief Status report of movement stop.
	 */
	jscOFF = 0,

	/**
	 * @brief Stops movement, and waits for off.
	 */
	jscSTOP = 1,
	/**
	 * @brief Joystick active.
	 */
	jscON = 2,
};

/**
 * @brief Enumerate for controller mode of operation.
 */
enum EAxisMode
{
	/**
	 * @brief Axis is cannot be moved at all.
	 */
	amDISABLED = 0,
	/**
	 * @brief Axis is moved only to specified position.
	 */
	amPOSITION = 1,

	/**
	 * @brief Axis is moved with a specified velocity.
	 */
	amVELOCITY = 2,
	/**
	 * @brief Axis executes home sequence on position command.(is optional)
	 */
	amHOME = 3,
	/**
	 * @brief Axis is moved with a specified velocity.
	 */
	amCONTINUE = 4
};

/**
 * @brief Enumerate for retrieving axis extremes.
 */
enum EAxisMinMax : unsigned int
{
	/**
	 * @brief Axis minimum position.
	 */
	ammMIN_POS = 0,
	/**
	 * @brief Axis maximum position.
	 */
	ammMAX_POS = 1,
	/**
	 * @brief Axis maximum velocity.
	 */
	ammMAX_VEL = 2,
	/**
	 * @brief Axis maximum acceleration.
	 */
	ammMAX_ACC = 3
};

/**
 * Enumerate for ChuckJaw states.
 */
enum EChuckJaw
{
	/**
	 * @brief ChuckJaw is off.
	 */
	cjOFF = 0,
	/**
	 * @brief ChuckJaw is open.
	 */
	cjOPEN = 1,
	/**
	 * @brief ChuckJaw is closed.
	 */
	cjCLOSE = 2,
};
/**
 * @brief Enumerate for controller events.
 */
enum EControllerEvent
{
	/**
	 * @brief Controller status changed.
	 */
	ceSTATUS = 0x0000,
	/**
	 * @brief Handler was hooked to the controller.
	 */
	ceHOOKED = 0x1001,
	/**
	 * @brief Handler was unhooked to the controller.
	 */
	ceUNHOOKED = 0x1002,
	/**
	 * @brief Complete event on an axis group. Not implemented yet.
	 */
	ceCOMPLETE = 0x2001,
	/**
	 * @brief POP axis has made a rotation.
	 */
	cePOPEVENT = 0x3001,
};

/**
 * @brief Handler type for motion event handling.
 */
typedef void (*ControllerEvent)(Controller* ctrl, EControllerEvent event);

}// namespace sf::gmi
