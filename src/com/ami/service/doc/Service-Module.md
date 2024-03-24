# Service Design {#sf-gii-Service-Design}

## Server Side

### Connection Established

The classes `::sf::Variable` and `::sf::ResultData` are not multi-thread safe and can only run in the main thread.
Relaying threads to the main thread is thus required. 

1. A `::sf::ServerConnection` instance is created by the listening `::sf::SocketServer` instance and thread.
2. The `::sf::ServerConnection` has a member `::sf::ServerConnector` which collects all exported generic 
information instances (`::sf::Variable` and `::sf::ResultData`).
3. A instance of `::sf::VariableCollector` is created for collecting `::sf::Variable` and capturing its events. 
  Variables are collected by the export flag set  in the instance.
4. All `::sf::Variable` instances are hooked to a handler for events.
5. Variable event information as setup string, value and flags is are stored into a multi-thread safe fifo and a 
thread is triggered to process.  
6. The socket processing thread is notified that the fifo has been filled 