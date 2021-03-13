# TVariable Example {#sf-gii-variable}

Example 1: Linking an instance to another class. 

### Class declaration

```c++
class TClientOrServer
{
    public:
        /**
         * Default constructor.
         */
        virtual TClientOrServer(bool client);

    private:
        /**
         * Variable instance used as client.
         */
        TVariable FVariable;
        /**
         * Variable link to this class.
         */
        TVariableLinkHandler<TClientOrServer> FEventLink;
        /**
         * Event handler function for hooking to a VarLink.
         */
        void EventHandler
        (
            TVariable::EEvent event,
            const TVariable& caller,
            TVariable& link,
            bool sameinst
        );
};
```

### Class implementation.

```c++
TClientOrServer::TClientOrServer(TComponent* owner)
  :FEventLink(this, &TClientOrServer::EventHandler)
{
    // Link handler to the variable instance.
    Fvariable->SetLink(&FEventLink);
    if (client)
    {
        // Setup instance as a client which triggers events.
        Fvariable.Setup(0x98001);
    }
    else
    {
        // Setup instance as a server which triggers events.
        Fvariable.Setup
        (
            "0x98001,Calibrate|Blue Pos,%,ASP,"
            "Blue position for calibrating the C-scan plot.,"
            "FLOAT,,"
            "0.1,50,0,100"
        );
    }
}

void TClientOrServer::EventHandler
(
    TVariable::EEvent event,
    const TVariable& caller,
    TVariable& link,
    bool sameinst
)
{
    switch (event)
    {
        case TVariable::veFLAGSCHANGE:
        // Handle something
        break;
        
        case TVariable::veVALUECHANGE:
        // Handle something
        break;
        
        case TVariable::veCONVERTED:
        // Handle something
        break;
        
        case Tvariable::veIDCHANGED:
        // Handle something
        break;
    }
}
```
