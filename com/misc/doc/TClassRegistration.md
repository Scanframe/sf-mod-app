# Class Registration Example {#sf-TClassRegistration}

## Interface Dynamic Library

Create class having a structure for passing arguments to the derived class on creation. 

### Interface Header

```c++
/**
 * This class is the interface to all derived classes and could be pure virtual.
 */
class DL_EXPORT RuntimeIface
{
	public:
		struct Parameters
		{
			explicit Parameters(int h) : Handle(h) {}

			int Handle;
		};

		/**
		 * Required type of constructor passing a parameters structure.
		 */
		explicit RuntimeIface(const Parameters& params) = default;
		/**
		 * Virtual destructor required.
		 */
		virtual ~RuntimeIface();
		/**
		 * Method needed to implemented in the implementation of a derived class.
		 */
		virtual const char* getString() = 0;

	// Declare the interface function.
	SF_DECL_IFACE(RuntimeIface, RuntimeIface::Parameters, Interface)
};
```
### Interface Code

Only line using a macro for implementing the interface class. 

```c++
SF_IMPL_IFACE(RuntimeIface, RuntimeIface::Parameters, Interface)
```

## Implementation Dynamic Library

### Implementation Header

```c++
/**
 * Exporting is not needed.
 */
class RuntimeLibImplementationA : public RuntimeIface
{
	public:
		/**
		 * Required constructor passing a parameters structure.
		 */
		explicit RuntimeLibImplementationA(const Parameters& params);
		/**
		 * Implementing the pure virtual method by overriding.
		 */
		const char* getString() override;

	private:
		std::string FMessage;

};
```

### Implementation Code

This `_DL_INFORMATION(...)` library information into the target.<br>
See `::sf::DynamicLibraryInfo`

```c++
// Declaration of the dynamic library information.
SF_DL_INFORMATION("Implementation Library A",
	R"(This is the description of the library and what it contains.
	A second line of a multi lined description.)"
)

// Register this derived class.
SF_REG_CLASS
(
	RuntimeIface, RuntimeIface::Parameters, Interface,
	RuntimeLibImplementationA, "Class-A", "Actual name of the class."
)

RuntimeLibImplementationA::RuntimeLibImplementationA(const RuntimeIface::Parameters& params)
	: RuntimeIface(params)
{
	SF_RTTI_NOTIFY(DO_CLOG, "Constructed: " << sf::string_format("Handle(%d)", params.Handle))
}

const char* RuntimeLibImplementationA::getString()
{
	SF_RTTI_NOTIFY(DO_CLOG, "Calling(" << Interface().RegisterName(this) << "): " << __FUNCTION__)
	FMessage = sf::string_format("Greetings from Class '%s' created by name '%s' ", _RTTI_TYPENAME.c_str(),
		Interface().RegisterName(this).c_str());
	return FMessage.c_str();
}
```
