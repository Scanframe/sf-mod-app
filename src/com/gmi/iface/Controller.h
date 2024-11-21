#pragma once

#include "GmiInterface.h"
#include "global.h"
#include <misc/gen/TClassRegistration.h>
#include <misc/qt/PropertySheetDialog.h>

namespace sf::gmi
{

/**
 * @brief Pure virtual class for implementation of motion controllers.
 */
class _GMI_CLASS TController
{
	public:
		/**
		 * @brief Arguments for when creating a controller implementation.
		 */
		struct Parameters
		{
				Parameters() = default;
				explicit Parameters(int mode)
					: _mode(mode)
				{}

				int _mode{0};
		};
		/**
		 * @brief Constructor for passing general structure for derived classes.
		 */
		TController(const Parameters&);
		/**
		 * @brief Must be called from a derived class in the destructor before anything.
		 */
		void Destroy();
		/**
		 * @brief Initializes the hardware for operation and starts the thread when DoInitialize returns true.
		 */
		bool Initialize();
		/**
		 * @brief Un-initializes the hardware.
		 */
		bool Uninitialize();
		/**
		 * @brief Virtual destructor for derived classes.
		 */
		virtual ~TController();
		/**
		 * @brief Attaches the event handler for events.
		 */
		void HookEventHandler(TControllerEvent handler);
		/**
		 * @brief Detaches the event handler for events.
		 */
		void UnhookEventHandler(TControllerEvent handler);
		/**
		 * @brief Writes the non exported parameters to profile.
		 */
		bool ReadWriteSettings(bool rd);

		/**
		 * @brief Interface class to access Axis properties and functions.
		 */
		class _GMI_CLASS TAxis
		{
			public:
				/**
				 * @brief Returns the location of the axis in the constellation of axes.
				 */
				EAxisLocation GetLocation() const;
				/**
				 * @brief Returns the possible movements the is instance can make.
				 */
				AxisMovements GetMovements() const;
				/**
				 * @brief Returns the name of this instance.
				 */
				const char* GetName() const;
				/**
				 * @brief Returns the description of this instance.
				 */
				const char* GetDescription() const;
				/**
			 * @brief Returns the current position/velocity/acceleration.
			 */
				double GetCurrent(EAxisValueType avt) const;
				/**
				 * @brief Sets the current position/velocity/acceleration. Only when not moving.
				 */
				bool SetCurrent(EAxisValueType avt, double);
				/**
				 * @brief Returns the accuracy for velocity and position.
				 */
				double GetAccuracy() const;
				/**
				 * @brief Returns the resolution of the axis.
				 */
				double GetResolution() const;
				/**
				 * @brief Gets the current extremes from this axis.
				 */
				double GetMinMax(EAxisMinMax amm) const;
				/**
				 * @brief Set the position offset.
				 */
				bool SetOffset(double ofs);
				/**
				 * @brief Get the position offset.
				 */
				double GetOffset() const;
				/**
				 * @brief Returns the current velocity.
				 */
				double GetTarget(EAxisValueType avt) const;
				/**
				 * @brief Sets the current velocity.
				 */
				bool SetTarget(EAxisValueType avt, double);
				/**
				 * @brief Sets the current position.
				 */
				bool SetPosition(double);
				/**
				 * @brief Normalize the passed position for all unlimited rotation axes.
				 * Uses accuracy to find the actual position to normalize.
				 * The passed value is also returned.
				 */
				double Normalized(double pos) const;
				/**
				 * @brief Sets the axis mode for operation. Returns false on failure.
				 */
				bool SetMode(EAxisMode);
				/**
				 * @brief Returns the mode of operation.
				 */
				EAxisMode GetMode() const;
				/**
				 * @brief Returns true if the mode can be changed.
				 */
				bool CanModeChange(EAxisMode cur_mode, EAxisMode new_mode, bool pos_cmplt, bool vel_cmplt, bool con_cmplt);
				/**
				 * @brief Easy to use vector for an array of Axis pointers.
				 */
				typedef TVector<TAxis*> TPtrVector;
				/**
				 * @brief Easy to use vector for an iterator for array of Axis pointers.
				 */
				typedef TIterator<TAxis*> TPtrIterator;

			protected:
				/**
				 * @brief Constructor that registers itself at the passed controller.
				 */
				TAxis(TController* mc, EAxisLocation);
				/**
				 * @brief Virtual destructor for possible derived destructors.
				 */
				virtual ~TAxis();
				/**
				 * @brief Holds the possible movements.
				 */
				AxisMovements FMovements;

			private:
				/**
				 * @brief Holds the controller this axis belongs to.
				 */
				TController& FController;
				/**
				 * @brief Holds the location of the axis in the constellation of axes.
				 */
				EAxisLocation FLocation;

				friend TController;
		};

		/**
		 * @brief Start homing procedure and returns true when successfully started.
		 * When skip has been passed the implementation tries to skip homing.
		 */
		bool HomeAxes(bool skip);
		/**
		 * @brief Enables or disables the joystick mode of the controller.
		 * @returns True when the transition was successful.
		 */
		bool SetJoystick(EJoystickCmd jsc);
		/**
		 * @brief Returns the joystick state. Off=0, Stop=1 and On=2.
		 */
		EJoystickCmd GetJoystick() const;
		/**
		 * @brief Returns the current selected axis on the joystick control.
		 * This does not mean it is enabled.
		 */
		EAxisLocation GetJoystickAxis() const;
		/**
		 * @brief Sets the selected axis for the joystick enabled or not.
		 */
		bool SetJoystickAxis(EAxisLocation al);
		/**
		 * @brief Returns the current selected axis on the pop event source.
		 */
		EAxisLocation GetPopAxis() const;
		/**
		 * @brief Sets the selected axis for the pop event source.
		 */
		bool SetPopAxis(EAxisLocation al);
		/**
		 * @brief Sets the trigger enable.
		 */
		bool SetTriggerEnable(bool enable);
		/**
		 * @brief Returns the trigger enable status.
		 */
		bool GetTriggerEnable() const;
		/**
		 * @brief Sets the trigger internal frequency generator and returns the clipped or rounded value.
		 * @return On error -1
		 */
		double SetTriggerFreq(double freq);
		/**
		 * @brief Returns the trigger internal frequency generator.
		 * @return On error -1.
		 */
		double GetTriggerFreq() const;

		//
		// Movement execution related functions and properties.
		//

		/**
		 * @brief Executes a passed move position command.
		 * @return True on succes and false on failure and see #GetLastErrorText() function for more information.
		 */
		bool SetMovePos(EMovePosCmd mpc);
		/**
		 * @brief Returns the current active position command. When moving position the command returned will eventually become mcSTOP.
		 */
		EMovePosCmd GetMovePos() const;
		/**
		 * @brief Executes a passed move velocity command.
		 */
		bool SetMoveVel(EMoveVelCmd mvc);
		/**
		 * @brief Returns the current active velocity command.
		 */
		EMoveVelCmd GetMoveVel() const;
		/**
		 * @brief Executes a passed move continuous command.
		 */
		bool SetMoveCon(EMoveConCmd mcc);
		/**
		 * @brief Returns the current active velocity command.
		 * @return
		 */
		EMoveConCmd GetMoveCon() const;
		/**
		 * @brief Gets a reference to the axis at the specified location.
		 */
		TAxis& GetAxis(int axis_loc);
		/**
		 * @brief Gets a const reference to the axis at the specified location.
		 */
		const TAxis& GetAxis(int axis_loc) const;
		/**
		 * @brief Returns the physical axis location of the passed axis number.
		 */
		TAxis& GetPhysAxis(int axis_num);
		/**
		 * @brief Adds controller specific property pages to the passed sheet.
		 */
		virtual void AddPropertyPages(PropertySheetDialog* sheet);
		/**
		 * @brief Gets the current axes values.
		 * @param avt Axis value type.
		 * @param coord Axes coordinate containing the values.
		 * @return True if successful and False on failure where the last error contains more information.
		 */
		bool GetCurrent(EAxisValueType avt, TAxesCoord& ac) const;
		/**
		 * @brief Sets the current axes values.
		 * @param avt Axis value type.
		 * @param coord Axes coordinate containing the values.
		 * @return True if successful and False on failure where the last error contains more information.
		 */
		bool SetCurrent(EAxisValueType avt, const TAxesCoord& coord);

		/**
		 * @brief Gets the target set for this axis.
		 * @param avt Axis value type.
		 * @param coord Axes coordinate containing the values.
		 * @return True if successful and False on failure where the last error contains more information.
		 */
		bool GetTarget(EAxisValueType avt, TAxesCoord& coord) const;
		/**
		 * @brief Set the target value type.
		 * @param avt Axis value type.
		 * @param coord Axes coordinate containing the values.
		 * @return True if successful and False on failure where the last error contains more information.
		 */
		bool SetTarget(EAxisValueType avt, const TAxesCoord& coord);
		/**
		 * @brief Set the target
		 * @param avt Axis value type.
		 * @param vel Axes velocities.
		 * @param acc Axes accelerations.
		 * @param linear
		 * @return True if successful and False on failure where the last error contains more information.
		 */
		bool SetTarget(const TAxesCoord& pos, const TAxesCoord& vel, const TAxesCoord& acc, bool linear);
		/**
		 * @brief Sets the mode according the in the coord specified axes.
		 * @param am Axis mode.
		 * @param ac
		 * @param amdef Default value for the axes when not specified.
		 * @return False on failure.
		 */
		bool SetMode(EAxisMode am, const TAxesCoord& ac, EAxisMode amdef = amDISABLED);
		/**
		 * @brief Sets the axis which pulses are used to derive the trigger pulses.
		 * @returns True on success.
		 */
		bool SetTriggerAxis(EAxisLocation al);
		/**
		 * @brief Gets the current selected trigger axis.
		 * @return The triggering axis.
		 */
		EAxisLocation GetTriggerAxis() const;
		/**
		 * @brief Gets the density of measurements in radians or meters depending on selected axis.
		 */
		double GetTriggerDensity() const;
		/**
		 * @brief Sets the density of measurements in rads or m depending on selected axis.
		 */
		bool SetTriggerDensity(double td);
		/**
		 * @brief Sets Trigger mode.
		 */
		bool SetTriggerMode(bool intern);
		/**
		 * @brief Gets the trigger mode.
		 */
		bool GetTriggerMode() const;
		/**
		 * @brief Sets chuck-jaw state.
		 */
		bool SetChuckJaw(EChuckJaw cj);
		/**
		 * @brief Gets chuck-jaw state.
		 */
		EChuckJaw GetChuckJaw() const;
		/**
		 * @brief Returns true if the last position move is completed.
		 */
		bool IsMovePosCompleted() const;
		/**
		 * @brief Returns true if the velocity move is completed.
		 */
		bool IsMoveVelCompleted() const;
		/**
		 * @brief Returns true if the contineous move is completed.
		 */
		bool IsMoveConCompleted() const;
		/**
		 * @brief Function testing for movement of position and velocity mode.
		 * When there is an abort state of-any-of the motion groups the axis is considered as not moving.
		 */
		bool IsMoving() const;
		/**
		 * @brief Sets the current position of single axis.
		 */
		bool SetPosition(EAxisLocation al, double);
		/**
		 * @brief Sets the current position of all axis.
		 */
		bool SetPosition(const TAxesCoord& ac);
		/**
		 * @brief Normalize the passed position for all unlimited rotation axes.
		 * Uses accuracy to find the actual position to normalize.
		 * The passed value is also returned.
		 * @param pos Axes coordinate to modify.
		 * @return The reference to the passed axes coordinate.
		 */
		TAxesCoord& Normalize(TAxesCoord& pos) const;
		/**
		 * @brief Normalize the passed position for all unlimited rotation axes.
		 * Uses accuracy to find the actual position to normalize.
		 * The passed value is also returned.
		 * @param pos Axes coordinate to modify.
		 * @return The modified axes coordinate.
		 */
		TAxesCoord Normalized(const TAxesCoord& dist) const;
		/**
		 * @brief Gets a bitmap set of radial unlimited axis locations.
		 */
		AxisLocations GetRadialUnlimted() const;
		/**
		 * @brief Opens a debugger dialog for the motioncontroller.
		 */
		virtual void OpenDebugger() {}

		// Information retrieval functions.
		/**
		 * @brief States the controller can have.
		 */
		enum EStatus : int
		{
			/** @brief Error occurred. */
			csERROR = -1,
			/** @brief Not initialized. */
			csUNINIT = 0,
			/** @brief Initialized. */
			csINIT,
			/** @brief Axes are not homed yet. */
			csUNHOMED,
			/** @brief Is currently homing. */
			csHOMING,
			/** @brief Is ready for action. */
			csREADY,
		};
		/**
		 * @brief Returns true if the current status is READY.
		 */
		inline bool IsReady() const;
		/**
		 * @brief Return the name of the axis.
		 */
		static const char* GetAxisName(int axis_loc);
		/**
		 * @brief Return the description of the axis.
		 */
		static const char* GetAxisDescription(int axis_loc);
		/**
		 * @brief Returns the status name of the current status.
		 */
		static const char* GetStatusName(EStatus status);
		/**
		 * @brief Returns the current status as a name.
		 */
		const char* GetStatusName() const;
		/**
		 * @brief Returns the amount real implemented axes.
		 */
		unsigned GetAxisCount() const;
		/**
		 * @brief Returns the last  error if there is one.
		 */
		std::string GetLastErrorText() const;

		//
		// Parameter manipulation and retrieval functions.
		//

		/**
		 * @brief Retrieve information about the passed param id.
		 */
		bool GetParamInfo(int id, TParamInfo& info) const;
		/**
		 * @brief Returns the value of the specified parameter id.
		 * When the ID does not exist it returns false.
		 */
		bool GetParam(int id, Value& value) const;
		/**
		 * @brief Sets an interface parameter and notifies hooked user when skip_event is false.
		 */
		bool SetParam(int id, const Value& value, bool skip_event);
		/**
		 * @brief Sets and immediately gets the same value again clipped or not. When skip event is true the CallParamHook is not called.
		 */
		bool SetGetParam(int id, Value& value, bool skip_event);
		/**
		 * @brief This function must be overloaded to handle the interface parameters.
		 * When info in non-null the parameter info must be filled in.
		 * When the set or get value is non-null the value is set and/or retrieved.
		 */
		virtual bool HandleParam(int id, TParamInfo* info, const Value* setval, Value* getval) = 0;
		/**
		 * @brief Gets the ID of the parameter for the specified gate.
		 * Value  std::numeric_limits<unsigned int>::max() tells the implementation to ignore the parameter.
		 */
		virtual int GetParamId(EParam param, unsigned int axis = std::numeric_limits<unsigned int>::max()) const = 0;
		/**
		 * @brief Enumerate interface parameters ids.
		 */
		virtual bool EnumParamIds(TIdList& ids) const = 0;
		/**
		 * @brief Sets a procedure hook for the interface implementation to be called when the value changes as a result of the implementation itself.
		 */
		void SetParamHook(TNotifyProc proc, void* data);
		/**
		 * @brief Sets a procedure hook for the interface implementation to be called when there ias data result has data.
		 */
		void SetResultHook(TNotifyProc proc, void* data);
		/**
		 * @brief Gets the ID of the result for the specified gate.
		 * Value std::numeric_limits<unsigned int>::max() tells the implementation to ignore the result.
		 */
		virtual int GetResultId(EResult result, unsigned int axis = std::numeric_limits<unsigned int>::max()) const = 0;
		/**
		 * @brief Enumerate interface results ids.
		 */
		virtual bool EnumResultIds(TIdList& ids) const = 0;
		/**
		 * @brief Retrieve information about the passed id.
		 */
		bool GetResultInfo(int id, TResultInfo& info) const;
		/**
		 * @brief Retrieve information about the passed id.
		 */
		virtual bool HandleResult(int id, TResultInfo* info, TBufferInfo* bufinfo) = 0;
		/**
		 * @brief Gets the result buffer associated with the result ID passed in the result hook at the time of the call.
		 * @return True on succes.
		 */
		bool GetResultBuffer(int id, TBufferInfo& buf_info);
		/**
		 * @brief Returns the accuracy of positions of the implementation for a coords compare.
		 */
		bool GetAccuracy(TAxesCoord& accuracy) const;
		/**
		 * @brief Returns the resolution on all axis for the implementation.
		 */
		bool GetResolution(TAxesCoord& resolution) const;
		/**
		 * @brief Returns the current extremes.
		 */
		bool GetMinMax(EAxisMinMax amm, TAxesCoord& coord) const;
		/**
		 * @brief Sets the position offset for all postion related parameters of all axes.
		 */
		bool SetOffset(const TAxesCoord& coord);
		/**
		 * @brief Returns the current offset for all axes.
		 */
		bool GetOffset(TAxesCoord& coord) const;
		/**
		 * @brief Returns the configuration profile path of this implementation.
		 */
		std::string GetProfilePath() const;

	protected:
		/**
		 * @brief Initialization function which must be overloaded by a derived class.
		 * Should check the hardware configuration.
		 */
		virtual bool DoInitialize(bool) = 0;
		/**
		 * @brief Can be overloaded by a derived class.
		 * Sets the current position.
		 */
		virtual bool DoSetPosition(EAxisLocation al, double);
		/**
		 * @brief Does the real execution of the home function and must be overloaded.
		 * When skip has been passed the implementation tries to skip homing.
		 */
		virtual bool DoHomeAxes(bool skip);
		/**
		 * @brief Derived classes can set the error text for the function.
		 */
		void SetLastErrorText(const char* text);
		/**
		 * @brief Used by derived classes to set the controllers status.
		 */
		void SetStatus(EStatus status);
		/**
		 * @brief Gets the status value.
		 */
		EStatus GetStatus() const;
		/**
		 * @brief Calls the hooked function if it exists.
		 * Passing the set data pointer and the ID of the effected parameter.
		 */
		void CallParamHook(int id);
		/**
		 * @brief Calls the hooked function if it exists.
		 * Passing the set data pointer and the ID of the effected result.
		 */
		void CallResultHook(int id);
		/**
		 * @brief Sends a POP event to the hooked event handlers.
		 * Is to be called by the implementation to signal that a new rotation has begun.
		 */
		void SendPopEvent();

	private:
		/**
		 * @brief Sends an event to the hooked event handlers.
		 */
		void SendEvent(EControllerEvent ce);
		/**
		 * @brief This functions only use by this base class to intercept changes.
		 * When the axis number is std::numeric_limits<unsigned int>::max()) no axis is selected.
		 */
		bool SetParam(EParam param, unsigned int axis, const Value& value, bool skip_event);
		/**
		 * @brief Returns the value of the specified parameter id.
		 * @returns False when the ID does not exist.
		 */
		bool GetParam(EParam param, unsigned int axis, Value& value) const;
		/**
		 * @brief Function to be called when an implementation creates an axis which is added to the motion control.
		 * @returns False when this addition is faulty. (e.g. twice the same axis...)
		 */
		void Attach(TAxis* axis);
		/**
		 * @brief Returns the pointer the axis property.
		 */
		TAxis* GetAxisPtr(int axis_loc);
		/**
		 * @brief Flag indication the controller is being destroyed.
		 */
		bool FlagDestroying;
		/**
		 * @brief Holds all pointer to existing axes.
		 */
		TAxis::TPtrVector FAxes;
		/**
		 * @brief Holds the axis map.
		 */
		TAxis* FAxesMap[alLAST_ENTRY];
		/**
		 * @brief Axis to return when there is actually none available.
		 */
		TAxis FNullAxis;
		/**
		 * @brief Holds the last error text.
		 */
		std::string FLastErrorText;
		/**
		 * @brief Holds the current status.
		 */
		EStatus FStatus;
		/**
		 * @brief Holds the pointer to the parameter change procedure.
		 */
		TNotifyProc ParamNotifyProc;
		/**
		 * @brief Holds the pointer to the result change procedure.
		 */
		TNotifyProc ResultNotifyProc;
		/**
		 * @brief Holds the data set with the hook.
		 */
		void* ParamNotifyData;
		/**
		 * @brief Holds the data set with the hook.
		 */
		void* ResultNotifyData;
		/**
		 * @brief Is set by SetGetParam so that hooked users of the interface are not notified.
		 */
		bool FNoEventGeneration;
		/**
		 * @brief Holds the registered event handlers.
		 */
		typedef TVector<TControllerEvent> TEventList;
		/**
		 * @brief Holds the registered event handlers.
		 */
		TEventList FMotionEventList;

		// Declarations of static functions and data members to be able to create registered RSA implementations.
		SF_DECL_IFACE(TController, TController::Parameters, Interface)
		//
		friend TAxis;
};

inline bool TController::IsReady() const
{
	return FStatus == csREADY;
}

inline bool TController::GetResultInfo(int id, TResultInfo& info) const
{
	return const_cast<TController*>(this)->HandleResult(id, &info, NULL);
}

inline bool TController::GetResultBuffer(int id, TBufferInfo& bi)
{
	return HandleResult(id, nullptr, &bi);
}

inline bool TController::GetParamInfo(int id, TParamInfo& info) const
{
	return const_cast<TController*>(this)->HandleParam(id, &info, NULL, NULL);
}

inline bool TController::GetParam(int id, Value& value) const
{
	return const_cast<TController*>(this)->HandleParam(id, NULL, NULL, &value);
}

inline void TController::SendPopEvent()
{
	SendEvent(cePOPEVENT);
}

inline void TController::SetParamHook(TNotifyProc proc, void* data)
{
	ParamNotifyProc = proc;
	ParamNotifyData = data;
}

inline void TController::SetResultHook(TNotifyProc proc, void* data)
{
	ResultNotifyProc = proc;
	ResultNotifyData = data;
}

inline bool TController::IsMovePosCompleted() const
{
	return GetMovePos() == mpcCOMPLETE;
}

inline bool TController::IsMoveVelCompleted() const
{
	return GetMoveVel() == mvcCOMPLETE;
}

inline bool TController::IsMoveConCompleted() const
{
	return GetMoveCon() == mccCOMPLETE;
}

inline unsigned TController::GetAxisCount() const
{
	return FAxes.count();
}

inline TController::TAxis* TController::GetAxisPtr(int axis_loc)
{
	return &GetAxis(axis_loc);
}

inline TController::EStatus TController::GetStatus() const
{
	return FStatus;
}

inline std::string TController::GetLastErrorText() const
{
	return FLastErrorText;
}

inline void TController::SetLastErrorText(const char* text)
{
	FLastErrorText = text;
}

inline const char* TController::GetStatusName() const
{
	return GetStatusName(FStatus);
}

inline TAxesCoord TController::Normalized(const TAxesCoord& dist) const
{
	TAxesCoord ac(dist);
	return Normalize(ac);
}

inline EAxisLocation TController::TAxis::GetLocation() const
{
	return FLocation;
}

inline AxisMovements TController::TAxis::GetMovements() const
{
	return FMovements;
}

}// namespace sf::gmi
