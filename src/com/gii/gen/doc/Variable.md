# Variable Event Example {#sf-gii-Variable}

Example 1: Linking an instance to another class. 

### Class declaration

```c++
class ClientOrServer
{
    public:
        /**
         * Default constructor.
         */
        virtual ClientOrServer(bool client);

    private:
        /**
         * Variable instance used as client.
         */
        Variable _variable;
        /**
         * Variable link to this class.
         */
        TVariableHandler<ClientOrServer> _eventHandler;
        /**
         * Event handler function for hooking to a VarLink.
         */
        void eventHandler
        (
            Variable::EEvent event,
            const Variable& caller,
            Variable& link,
            bool sameinst
        );
};
```

### Class implementation.

```c++
ClientOrServer::ClientOrServer()
  :_eventHandler(this, &ClientOrServer::eventHandler)
{
    // Link handler to the variable instance.
    Fvariable->setHandler(&_eventHandler);
    if (client)
    {
        // Setup instance as a client which triggers events.
        _variable.setup(0x98001);
    }
    else
    {
        // Setup instance as a server which triggers events.
        _variable.setup
        (
            "0x98001,Calibrate|Blue Pos,%,ASP,"
            "Blue position for calibrating the C-scan plot.,"
            "FLOAT,,"
            "0.1,50,0,100"
        );
    }
}

void ClientOrServer::eventHandler
(
    Variable::EEvent event,
    const Variable& caller,
    Variable& link,
    bool sameinst
)
{
    switch (event)
    {
        case Variable::veFlagsChange:
        // Handle something
        break;
        
        case Variable::veValueChange:
        // Handle something
        break;
        
        case Variable::veConverted:
        // Handle something
        break;
        
        case Tvariable::veIdChanged:
        // Handle something
        break;
    }
}
```
