#pragma once

#include <gmi/iface/Controller.h>
#include <misc/gen/ElapseTimer.h>
#include <misc/gen/Sustain.h>

namespace sf
{

using gmi::Controller;
using gmi::EAxisLocation;
using gmi::EAxisMode;
using gmi::EMoveConCmd;
using gmi::EMovePosCmd;
using gmi::EMoveVelCmd;
using gmi::EParam;
using gmi::EResult;
using gmi::IdList;
using gmi::ParamInfo;

class GmiEmulator : public Controller
{
	public:
		typedef Controller TInherited;
		/**
		 * @brief  Axis type of this implementation.
		 */
		class TAxis : public Controller::Axis
		{
			public:
				typedef Controller::Axis TInherited;
				// Constructor.
				TAxis(GmiEmulator* me, EAxisLocation al);
				// Destructor.
				~TAxis();
				//
				void DoMovePos(EMovePosCmd mpc);
				void DoMoveVel(EMoveVelCmd mvc);
				void DoMoveCon(EMoveConCmd mcc);
				//
				bool IsMovePosComplete(timespec clk);
				bool IsMoveVelComplete(timespec clk);
				bool IsMoveConComplete(timespec clk);
				// Position move timer
				sf::ElapseTimer MovePosTimer;
				// Velocity move timer.
				sf::ElapseTimer MoveVelTimer;
				// Continuous move timer.
				sf::ElapseTimer MoveConTimer;
				// Target values.
				double TrgPos;
				double TrgVel;
				double TrgAcc;
				double TrgDist;
				double TrgDist2;
				// Holds the velocity in velocity mode from which is accelerated.
				double StartVel;
				// Current values.
				double CurPos;
				double CurVel;
				double OfsPos;
				// Maximum values.
				double MaxPos;
				double MaxVel;
				double MaxAcc;
				// Minimum values.
				double MinPos;
				// Holds the accuracy of this axis.
				double Accuracy;
				// Holds the fake resolution. (Does nothing at all)
				double Resolution;
				// Holds the rounding for the motion parameters of this axis.
				double Round;
				// Holds the mode set.
				EAxisMode Mode;
				//
				GmiEmulator* _Controller;

				friend GmiEmulator;
		};
		// Constructor.
		GmiEmulator(const Parameters&);
		// Destructor.
		~GmiEmulator() override;
		// Overloaded from base class.
		bool doInitialize(bool) override;
		// Overloaded from base class.
		bool doHomeAxes(bool skip) override;
		// Overloaded from base class.
		void addPropertyPages(PropertySheetDialog* sheet) override;
		// Overloaded from base class.
		bool enumParamIds(IdList& ids) const override;
		// Overloaded from base class.
		gmi::IdType getParamId(EParam param, int axis = std::numeric_limits<unsigned>::max()) const override;
		// Overloaded from base class.
		bool handleParam(gmi::IdType id, ParamInfo* info, const Value* setval, Value* getval) override;
		// Overloaded from base class.
		gmi::IdType getResultId(EResult result, int axis = std::numeric_limits<int>::max()) const override;
		// Overloaded from base class.
		bool enumResultIds(IdList& ids) const override;
		// Overloaded from base class.
		bool handleResult(gmi::IdType id, gmi::ResultInfo* info, gmi::BufferInfo* buf_info) override;
		//
		void doMovePos(EMovePosCmd mpc);
		//
		void doMoveVel(EMoveVelCmd mvc);
		//
		void doMoveCon(EMoveConCmd mcc);

		//
		TVector<TAxis*> AxisList;
		// Axis
		TAxis _axisX;// Lineair and moves in [m].
		TAxis _axisY;// Lineair and moves in [m].
		TAxis _axisZ;// Lineair and moves in [m].
		TAxis _axisA;// Angular Gimble and moves [rad].
		TAxis _axisB;// Angular Swiffel and moves [rad].
		TAxis _axisC;// Angular Turntable and moves [rad and/or Hz].
		//TAxis _axisD;  // Tool Angular and moves [rad].
		TAxis _axisE;// Tool Lineair and moves in [m].
		// Holds the axis initialization status.
		bool _flagInitialized;
		// Holds the joystick mode flag.
		int _joystickState;
		// Holds the axis responsible for triggering the measurement.
		int _triggerAxis;
		// Holds the axis trigger mode. (intern/extern)
		int _triggerMode;
		// Holds the select pop axis.
		int _popAxis;
		//
		double _triggerDensity;
		//
		double _triggerFrequency;
		//
		bool _triggerEnable;
		// Holds the current position state.
		EMovePosCmd _curMovePos;
		// Holds the current velocity state.
		EMoveVelCmd _curMoveVel;
		// Holds the current continuous state.
		EMoveConCmd _curMoveCon;
		// Hook to the sustain interface.
		TSustain<GmiEmulator> _sustainEntry;
		//
		bool sustain(const timespec& t);
		// Hold the reported firmware revision.
		std::string _firmwareRevision;
		// Artificial homing delay.
		ElapseTimer _homingDelay;
		// Holds the error status.
		int _error;
		//
		int _chuckJaw;
		// Holds the last pop event count.
		int _popEventCount;
		//
		int _debug;

		friend GmiEmulator::TAxis;
};

}// namespace sf
