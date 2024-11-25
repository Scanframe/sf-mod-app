#pragma once
#include <gmi/iface/GmiInterface.h>
#include <misc/gen/TClassRegistration.h>
#include <misc/qt/PropertySheetDialog.h>

namespace sf::gmi
{

/**
 * @brief Pure virtual class for implementation of motion controllers.
 */
class _GMI_CLASS Controller
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
		Controller(const Parameters&);

		/**
		 * @brief Virtual destructor for derived classes.
		 */
		virtual ~Controller();

		/**
		 * @brief Must be called from a derived class in the destructor before anything.
		 */
		void destroy();

		/**
		 * @brief Initializes the hardware for operation and starts the thread when doInitialize returns true.
		 */
		bool initialize();

		/**
		 * @brief Un-initializes the hardware.
		 */
		bool uninitialize();

		/**
		 * @brief Attaches the event handler for events.
		 */
		void hookEventHandler(ControllerEvent handler);

		/**
		 * @brief Detaches the event handler for events.
		 */
		void unhookEventHandler(ControllerEvent handler);

		/**
		 * @brief Reads or writes the non exported parameters/settings to profile.
		 * @param rd When true settings are read.
		 * @return True on success.
		 */
		bool settingsReadWrite(bool rd);

		/**
		 * @brief Interface class to access Axis properties and functions.
		 */
		class _GMI_CLASS Axis
		{
			public:
				/**
				 * @brief Returns the location of the axis in the constellation of axes.
				 */
				EAxisLocation getLocation() const;

				/**
				 * @brief Returns the possible movements the is instance can make.
				 */
				AxisMovements getMovements() const;

				/**
				 * @brief Returns the name of this instance.
				 */
				const char* getName() const;

				/**
				 * @brief Returns the description of this instance.
				 */
				const char* getDescription() const;

				/**
			 * @brief Returns the current position/velocity/acceleration.
			 */
				double getCurrent(EAxisValueType avt) const;

				/**
				 * @brief Sets the current position/velocity/acceleration. Only when not moving.
				 */
				bool setCurrent(EAxisValueType avt, double val);

				/**
				 * @brief Returns the accuracy for velocity and position.
				 */
				double getAccuracy() const;

				/**
				 * @brief Returns the resolution of the axis.
				 */
				double getResolution() const;

				/**
				 * @brief Gets the current extremes from this axis.
				 */
				double getMinMax(EAxisMinMax amm) const;

				/**
				 * @brief Set the position offset.
				 */
				bool setOffset(double ofs);

				/**
				 * @brief Get the position offset.
				 */
				double setOffset() const;

				/**
				 * @brief Returns the current velocity.
				 */
				double getTarget(EAxisValueType avt) const;

				/**
				 * @brief Sets the current velocity.
				 */
				bool setTarget(EAxisValueType avt, double val);

				/**
				 * @brief Sets the current position.
				 */
				bool setPosition(double value);

				/**
				 * @brief normalize the passed position for all unlimited rotation axes.
				 * Uses accuracy to find the actual position to normalize.
				 * The passed value is also returned.
				 */
				double normalized(double pos) const;

				/**
				 * @brief Sets the axis mode for operation. Returns false on failure.
				 */
				bool setMode(EAxisMode mode);

				/**
				 * @brief Returns the mode of operation.
				 */
				EAxisMode getMode() const;

				/**
				 * @brief Returns true if the mode can be changed.
				 */
				bool canModeChange(EAxisMode cur_mode, EAxisMode new_mode, bool pos_cmplt, bool vel_cmplt, bool con_cmplt);

				/**
				 * @brief Easy to use vector for an array of Axis pointers.
				 */
				typedef TVector<Axis*> PtrVector;

			protected:
				/**
				 * @brief Constructor that registers itself at the passed controller.
				 */
				Axis(Controller* mc, EAxisLocation);

				/**
				 * @brief Virtual destructor for possible derived destructors.
				 */
				virtual ~Axis();

				/**
				 * @brief Holds the possible movements.
				 */
				AxisMovements _movements;

			private:
				/**
				 * @brief Holds the controller this axis belongs to.
				 */
				Controller& _controller;
				/**
				 * @brief Holds the location of the axis in the constellation of axes.
				 */
				EAxisLocation _location;

				friend Controller;
		};

		/**
		 * @brief Start homing procedure and returns true when successfully started.
		 * When skip has been passed the implementation tries to skip homing.
		 */
		bool homeAxes(bool skip);

		/**
		 * @brief Enables or disables the joystick mode of the controller.
		 * @returns True when the transition was successful.
		 */
		bool setJoystick(EJoystickCmd jsc);

		/**
		 * @brief Returns the joystick state. Off=0, Stop=1 and On=2.
		 */
		EJoystickCmd getJoystick() const;

		/**
		 * @brief Returns the current selected axis on the joystick control.
		 * This does not mean it is enabled.
		 */
		EAxisLocation getJoystickAxis() const;

		/**
		 * @brief Sets the selected axis for the joystick enabled or not.
		 */
		bool setJoystickAxis(EAxisLocation axis_loc);

		/**
		 * @brief Returns the current selected axis on the pop event source.
		 */
		EAxisLocation getPopAxis() const;

		/**
		 * @brief Sets the selected axis for the pop event source.
		 */
		bool setPopAxis(EAxisLocation al);

		/**
		 * @brief Sets the trigger enable.
		 */
		bool setTriggerEnable(bool enable);

		/**
		 * @brief Gets the trigger enable status.
		 */
		bool getTriggerEnable() const;

		/**
		 * @brief Sets the trigger internal frequency generator and returns the clipped or rounded value.
		 * @return On error -1
		 */
		double setTriggerFreq(double freq);

		/**
		 * @brief Returns the trigger internal frequency generator.
		 * @return On error -1.
		 */
		double getTriggerFreq() const;

		//
		// Movement execution related functions and properties.
		//

		/**
		 * @brief Executes a passed move position command.
		 * @return True on succes and false on failure and see #getLastErrorText() function for more information.
		 */
		bool setMovePos(EMovePosCmd mpc);

		/**
		 * @brief Returns the current active position command. When moving position the command returned will eventually become mcSTOP.
		 */
		EMovePosCmd getMovePos() const;

		/**
		 * @brief Executes a passed move velocity command.
		 */
		bool setMoveVel(EMoveVelCmd mvc);

		/**
		 * @brief Returns the current active velocity command.
		 */
		EMoveVelCmd getMoveVel() const;

		/**
		 * @brief Executes a passed move continuous command.
		 */
		bool setMoveCon(EMoveConCmd mcc);

		/**
		 * @brief Returns the current active velocity command.
		 * @return
		 */
		EMoveConCmd getMoveCon() const;

		/**
		 * @brief Gets a reference to the axis at the specified location.
		 */
		Axis& getAxis(int axis_loc);

		/**
		 * @brief Gets a const reference to the axis at the specified location.
		 */
		const Axis& getAxis(int axis_loc) const;

		/**
		 * @brief Returns the physical axis location of the passed axis number.
		 */
		Axis& getPhysicalAxis(int axis_num);

		/**
		 * @brief Adds controller specific property pages to the passed sheet.
		 */
		virtual void addPropertyPages(PropertySheetDialog* sheet);

		/**
		 * @brief Gets the current axes values.
		 * @param avt Axis value type.
		 * @param coord Axes coordinate containing the values.
		 * @return True if successful and False on failure where the last error contains more information.
		 */
		bool getCurrent(EAxisValueType avt, AxesCoord& ac) const;

		/**
		 * @brief Sets the current axes values.
		 * @param avt Axis value type.
		 * @param coord Axes coordinate containing the values.
		 * @return True if successful and False on failure where the last error contains more information.
		 */
		bool setCurrent(EAxisValueType avt, const AxesCoord& coord);

		/**
		 * @brief Gets the target set for this axis.
		 * @param avt Axis value type.
		 * @param coord Axes coordinate containing the values.
		 * @return True if successful and False on failure where the last error contains more information.
		 */
		bool getTarget(EAxisValueType avt, AxesCoord& coord) const;

		/**
		 * @brief Set the target value type.
		 * @param avt Axis value type.
		 * @param coord Axes coordinate containing the values.
		 * @return True if successful and False on failure where the last error contains more information.
		 */
		bool setTarget(EAxisValueType avt, const AxesCoord& coord);

		/**
		 * @brief Set the target
		 * @param avt Axis value type.
		 * @param vel Axes velocities.
		 * @param acc Axes accelerations.
		 * @param linear
		 * @return True if successful and False on failure where the last error contains more information.
		 */
		bool setTarget(const AxesCoord& pos, const AxesCoord& vel, const AxesCoord& acc, bool linear);

		/**
		 * @brief Sets the mode according the in the coord specified axes.
		 * @param am Axis mode.
		 * @param ac
		 * @param amdef Default value for the axes when not specified.
		 * @return False on failure.
		 */
		bool setMode(EAxisMode am, const AxesCoord& ac, EAxisMode amdef = amDISABLED);

		/**
		 * @brief Sets the axis which pulses are used to derive the trigger pulses.
		 * @returns True on success.
		 */
		bool setTriggerAxis(EAxisLocation al);

		/**
		 * @brief Gets the current selected trigger axis.
		 * @return The triggering axis.
		 */
		EAxisLocation getTriggerAxis() const;

		/**
		 * @brief Gets the density of measurements in radians or meters depending on selected axis.
		 */
		double getTriggerDensity() const;

		/**
		 * @brief Sets the density of measurements in rads or m depending on selected axis.
		 */
		bool setTriggerDensity(double td);

		/**
		 * @brief Sets Trigger mode.
		 */
		bool setTriggerMode(bool intern);

		/**
		 * @brief Gets the trigger mode.
		 */
		bool getTriggerMode() const;

		/**
		 * @brief Sets chuck-jaw state.
		 */
		bool setChuckJaw(EChuckJaw cj);

		/**
		 * @brief Gets chuck-jaw state.
		 */
		EChuckJaw getChuckJaw() const;

		/**
		 * @brief Returns true if the last position move is completed.
		 */
		bool isMovePosCompleted() const;

		/**
		 * @brief Returns true if the velocity move is completed.
		 */
		bool isMoveVelCompleted() const;

		/**
		 * @brief Returns true if the contineous move is completed.
		 */
		bool isMoveConCompleted() const;

		/**
		 * @brief Function testing for movement of position and velocity mode.
		 * When there is an abort state of-any-of the motion groups the axis is considered as not moving.
		 */
		bool isMoving() const;

		/**
		 * @brief Sets the current position of single axis.
		 */
		bool setPosition(EAxisLocation al, double value);

		/**
		 * @brief Sets the current position of all axis.
		 */
		bool setPosition(const AxesCoord& ac);

		/**
		 * @brief normalize the passed position for all unlimited rotation axes.
		 * Uses accuracy to find the actual position to normalize.
		 * The passed value is also returned.
		 * @param pos Axes coordinate to modify.
		 * @return The reference to the passed axes coordinate.
		 */
		AxesCoord& normalize(AxesCoord& pos) const;

		/**
		 * @brief normalize the passed position for all unlimited rotation axes.
		 * Uses accuracy to find the actual position to normalize.
		 * The passed value is also returned.
		 * @param pos Axes coordinate to modify.
		 * @return The modified axes coordinate.
		 */
		AxesCoord normalized(const AxesCoord& dist) const;

		/**
		 * @brief Gets a bitmap set of radial unlimited axis locations.
		 */
		AxisLocations getRadialUnlimited() const;

		/**
		 * @brief Opens a debugger dialog for the motion controller if any.
		 */
		virtual void openDebugger() {}

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
		inline bool isReady() const;

		/**
		 * @brief Gets the name of the axis.
		 */
		static const char* getAxisName(int axis_loc);

		/**
		 * @brief Gets the description of the axis.
		 */
		static const char* getAxisDescription(int axis_loc);

		/**
		 * @brief Gets the status name of the current status.
		 */
		static const char* getStatusName(EStatus status);

		/**
		 * @brief Gets the current status as a name.
		 */
		const char* getStatusName() const;

		/**
		 * @brief Gets the amount real implemented axes.
		 */
		unsigned getAxisCount() const;

		/**
		 * @brief Gets the last error if there is one.
		 */
		std::string getLastErrorText() const;

		//
		// Parameter manipulation and retrieval functions.
		//

		/**
		 * @brief Retrieve information about the passed param id.
		 */
		bool getParamInfo(IdType id, ParamInfo& info) const;

		/**
		 * @brief Returns the value of the specified parameter id.
		 * When the ID does not exist it returns false.
		 */
		bool getParam(IdType id, Value& value) const;

		/**
		 * @brief Sets an interface parameter and notifies hooked user when skip_event is false.
		 */
		bool setParam(IdType id, const Value& value, bool skip_event);

		/**
		 * @brief Sets and immediately gets the same value again clipped or not. When skip event is true the callParamHook is not called.
		 */
		bool setGetParam(IdType id, Value& value, bool skip_event);

		/**
		 * @brief This function must be overloaded to handle the interface parameters.
		 * When info in non-null the parameter info must be filled in.
		 * When the set or get value is non-null the value is set and/or retrieved.
		 */
		virtual bool handleParam(IdType id, ParamInfo* info, const Value* setval, Value* getval) = 0;

		/**
		 * @brief Gets the ID of the parameter for the specified gate.
		 * Value  std::numeric_limits<int>::max() tells the implementation to ignore the parameter.
		 */
		virtual IdType getParamId(EParam param, int axis = std::numeric_limits<int>::max()) const = 0;

		/**
		 * @brief Enumerate interface parameters ids.
		 */
		virtual bool enumParamIds(IdList& ids) const = 0;

		/**
		 * @brief Sets a procedure hook for the interface implementation to be called when the value changes as a result of the implementation itself.
		 */
		void setParamHook(NotifyProc proc, void* data);

		/**
		 * @brief Sets a procedure hook for the interface implementation to be called when there ias data result has data.
		 */
		void setResultHook(NotifyProc proc, void* data);

		/**
		 * @brief Gets the ID of the result for the specified gate.
		 * Value std::numeric_limits<int>::max() tells the implementation to ignore the result.
		 */
		virtual IdType getResultId(EResult result, int axis = std::numeric_limits<int>::max()) const = 0;

		/**
		 * @brief Enumerate interface results ids.
		 */
		virtual bool enumResultIds(IdList& ids) const = 0;

		/**
		 * @brief Retrieve information about the passed result id.
		 */
		bool getResultInfo(IdType id, ResultInfo& info) const;

		/**
		 * @brief This function must be overloaded to handle the interface results.
		 */
		virtual bool handleResult(IdType id, ResultInfo* info, BufferInfo* buf_info) = 0;

		/**
		 * @brief Gets the result buffer associated with the result ID passed in the result hook at the time of the call.
		 * @return True on succes.
		 */
		bool getResultBuffer(IdType id, BufferInfo& buf_info);

		/**
		 * @brief Gets the accuracy of positions of the implementation for a coords compare.
		 */
		bool getAccuracy(AxesCoord& accuracy) const;

		/**
		 * @brief Gets the resolution on all axis for the implementation.
		 */
		bool getResolution(AxesCoord& resolution) const;

		/**
		 * @brief Gets the current axes extremes.
		 */
		bool getMinMax(EAxisMinMax amm, AxesCoord& coord) const;

		/**
		 * @brief Sets the position offset for all position related parameters of all axes.
		 */
		bool setOffset(const AxesCoord& coord);

		/**
		 * @brief Returns the current offset for all axes.
		 */
		bool getOffset(AxesCoord& coord) const;

		/**
		 * @brief Returns the configuration profile path of this implementation.
		 */
		std::string getProfilePath() const;

	protected:
		/**
		 * @brief Initialization function which must be overloaded by a derived class.
		 * Should check the hardware configuration.
		 */
		virtual bool doInitialize(bool) = 0;

		/**
		 * @brief Can be overloaded by a derived class.
		 * Sets the current position.
		 */
		virtual bool doSetPosition(EAxisLocation al, double);

		/**
		 * @brief Does the real execution of the home function and must be overloaded.
		 * When skip has been passed the implementation tries to skip homing.
		 */
		virtual bool doHomeAxes(bool skip);

		/**
		 * @brief Derived classes can set the error text for the function.
		 */
		void setLastErrorText(const char* text);

		/**
		 * @brief Used by derived classes to set the controllers status.
		 */
		void setStatus(EStatus status);

		/**
		 * @brief Gets the status value.
		 */
		EStatus getStatus() const;

		/**
		 * @brief Calls the hooked function if it exists.
		 * Passing the set data pointer and the ID of the effected parameter.
		 */
		void callParamHook(IdType id);

		/**
		 * @brief Calls the hooked function if it exists.
		 * Passing the set data pointer and the ID of the effected result.
		 */
		void callResultHook(IdType id);

		/**
		 * @brief Sends a POP event to the hooked event handlers.
		 * Is to be called by the implementation to signal that a new rotation has begun.
		 */
		void sendPopEvent();

	private:
		/**
		 * @brief Sends an event to the hooked event handlers.
		 */
		void sendEvent(EControllerEvent ce);

		/**
		 * @brief This functions only use by this base class to intercept changes.
		 * When the axis number is std::numeric_limits<unsigned int>::max()) no axis is selected.
		 */
		bool setParam(EParam param, int axis, const Value& value, bool skip_event);

		/**
		 * @brief Returns the value of the specified parameter id.
		 * @returns False when the ID does not exist.
		 */
		bool getParam(EParam param, int axis, Value& value) const;

		/**
		 * @brief Function to be called when an implementation creates an axis which is added to the motion control.
		 * @returns False when this addition is faulty. (e.g. twice the same axis...)
		 */
		void attach(Axis* axis);

		/**
		 * @brief Gets the pointer to the axis instance of the passed axis location.
		 */
		Axis* getAxisPtr(int axis_loc);

		/**
		 * @brief Flag indication the controller is being destroyed.
		 */
		bool _flagDestroying;
		/**
		 * @brief Holds all pointer to existing axes.
		 */
		Axis::PtrVector _axes;
		/**
		 * @brief Holds the axis map.
		 */
		Axis* _axesMap[alLAST_ENTRY];
		/**
		 * @brief Axis to return when there is actually none available.
		 */
		Axis _nullAxis;
		/**
		 * @brief Holds the last error text.
		 */
		std::string _lastErrorText;
		/**
		 * @brief Holds the current status.
		 */
		EStatus _status;
		/**
		 * @brief Holds the pointer to the parameter change procedure.
		 */
		NotifyProc _paramNotifyProc;
		/**
		 * @brief Holds the pointer to the result change procedure.
		 */
		NotifyProc _resultNotifyProc;
		/**
		 * @brief Holds the data set with the hook.
		 */
		void* _paramNotifyData;
		/**
		 * @brief Holds the data set with the hook.
		 */
		void* _resultNotifyData;
		/**
		 * @brief Is set by setGetParam so that hooked users of the interface are not notified.
		 */
		bool _noEventGeneration;
		/**
		 * @brief Holds the registered event handlers.
		 */
		typedef TVector<ControllerEvent> EventList;
		/**
		 * @brief Holds the registered event handlers.
		 */
		EventList _motionEventList;

		friend Axis;

		// Declarations of static functions and data members to be able to create registered RSA implementations.
		SF_DECL_IFACE(Controller, Controller::Parameters, Interface)
};

inline bool Controller::isReady() const
{
	return _status == csREADY;
}

inline bool Controller::getResultInfo(IdType id, ResultInfo& info) const
{
	return const_cast<Controller*>(this)->handleResult(id, &info, NULL);
}

inline bool Controller::getResultBuffer(IdType id, BufferInfo& buf_info)
{
	return handleResult(id, nullptr, &buf_info);
}

inline bool Controller::getParamInfo(IdType id, ParamInfo& info) const
{
	return const_cast<Controller*>(this)->handleParam(id, &info, NULL, NULL);
}

inline bool Controller::getParam(IdType id, Value& value) const
{
	return const_cast<Controller*>(this)->handleParam(id, NULL, NULL, &value);
}

inline void Controller::sendPopEvent()
{
	sendEvent(cePOPEVENT);
}

inline void Controller::setParamHook(NotifyProc proc, void* data)
{
	_paramNotifyProc = proc;
	_paramNotifyData = data;
}

inline void Controller::setResultHook(NotifyProc proc, void* data)
{
	_resultNotifyProc = proc;
	_resultNotifyData = data;
}

inline bool Controller::isMovePosCompleted() const
{
	return getMovePos() == mpcCOMPLETE;
}

inline bool Controller::isMoveVelCompleted() const
{
	return getMoveVel() == mvcCOMPLETE;
}

inline bool Controller::isMoveConCompleted() const
{
	return getMoveCon() == mccCOMPLETE;
}

inline unsigned Controller::getAxisCount() const
{
	return _axes.count();
}

inline Controller::Axis* Controller::getAxisPtr(int axis_loc)
{
	return &getAxis(axis_loc);
}

inline Controller::EStatus Controller::getStatus() const
{
	return _status;
}

inline std::string Controller::getLastErrorText() const
{
	return _lastErrorText;
}

inline void Controller::setLastErrorText(const char* text)
{
	_lastErrorText = text;
}

inline const char* Controller::getStatusName() const
{
	return getStatusName(_status);
}

inline AxesCoord Controller::normalized(const AxesCoord& dist) const
{
	AxesCoord ac(dist);
	return normalize(ac);
}

inline EAxisLocation Controller::Axis::getLocation() const
{
	return _location;
}

inline AxisMovements Controller::Axis::getMovements() const
{
	return _movements;
}

}// namespace sf::gmi
