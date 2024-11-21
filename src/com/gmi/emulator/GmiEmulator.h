#pragma once

#include "Controller.h"
#include <misc/gen/ElapseTimer.h>
#include <misc/gen/Sustain.h>

namespace sf
{

using gmi::EAxisLocation;
using gmi::EAxisMode;
using gmi::EMoveConCmd;
using gmi::EMovePosCmd;
using gmi::EMoveVelCmd;
using gmi::EParam;
using gmi::EResult;
using gmi::TBufferInfo;
using gmi::TController;
using gmi::TIdList;
using gmi::TParamInfo;
using gmi::TResultInfo;

class MotionEmulator : public TController
{
	public:
		typedef TController TInherited;
		/**
		 * @brief  Axis type of this implementation.
		 */
		class TAxis : public TController::TAxis
		{
			public:
				typedef TController::TAxis TInherited;
				// Constructor.
				TAxis(MotionEmulator* me, EAxisLocation al);
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
				MotionEmulator* Controller;

				friend MotionEmulator;
		};
		// Constructor.
		MotionEmulator(const Parameters&);
		// Destructor.
		~MotionEmulator() override;
		// Overloaded from base class.
		bool DoInitialize(bool) override;
		// Overloaded from base class.
		virtual bool DoHomeAxes(bool skip) override;
		// Overloaded from base class.
		virtual void AddPropertyPages(PropertySheetDialog* sheet) override;
		// Overloaded from base class.
		virtual bool EnumParamIds(TIdList& ids) const override;
		// Overloaded from base class.
		virtual int GetParamId(EParam param, unsigned axis = std::numeric_limits<unsigned>::max()) const override;
		// Overloaded from base class.
		virtual bool HandleParam(int id, TParamInfo* info, const Value* setval, Value* getval) override;
		// Overloaded from base class.
		virtual int GetResultId(EResult result, unsigned axis = UINT_MAX) const override;
		// Overloaded from base class.
		virtual bool EnumResultIds(TIdList& ids) const override;
		// Overloaded from base class.
		virtual bool HandleResult(int id, TResultInfo* info, TBufferInfo* bufinfo) override;
		//
		TVector<TAxis*> AxisList;
		// Axis
		TAxis X_Axis;// Lineair and moves in [m].
		TAxis Y_Axis;// Lineair and moves in [m].
		TAxis Z_Axis;// Lineair and moves in [m].
		TAxis A_Axis;// Angular Gimble and moves [rad].
		TAxis B_Axis;// Angular Swiffel and moves [rad].
		TAxis C_Axis;// Angular Turntable and moves [rad and/or Hz].
		//TAxis D_Axis;  // Tool Angular and moves [rad].
		TAxis E_Axis;// Tool Lineair and moves in [m].
		// Holds the axis initialization status.
		bool FlagInitialized;
		// Holds the joystick mode flag.
		int FJoystickState;
		// Holds the axis responsible for triggering the measurement.
		int FTriggerAxis;
		// Holds the axis trigger mode. (intern/extern)
		int FTriggerMode;
		// Holds the select pop axis.
		int FPopAxis;
		//
		double FTriggerDensity;
		//
		double FTriggerFreq;
		//
		bool FTriggerEnable;
		// Holds the current position state.
		EMovePosCmd FCurMovePos;
		// Holds the current velocity state.
		EMoveVelCmd FCurMoveVel;
		// Holds the current continuous state.
		EMoveConCmd FCurMoveCon;
		//
		void DoMovePos(EMovePosCmd mpc);
		//
		void DoMoveVel(EMoveVelCmd mvc);
		//
		void DoMoveCon(EMoveConCmd mcc);

		// Hook to the sustain interface.
		TSustain<MotionEmulator> FSustainEntry;
		//
		bool Sustain(const timespec& t);

		// Hold the reported firmware revision.
		std::string FFirmwareRevision;
		// Arteficial homing delay.
		ElapseTimer FHomingDelay;
		// Holds the error status.
		int FError;
		//
		int FChuckJaw;
		// Holds the last pop event count.
		int FPopEventCount;
		//
		int FDebug;

		friend MotionEmulator::TAxis;
};

}// namespace sf
