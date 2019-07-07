#ifndef _H_SERVER
#define _H_SERVER
#include <ESP8266WebServer.h>

class Webserver
{
private:
    ESP8266WebServer* server_object = NULL;
    
public:
    Webserver()
    {
        this->server_object = new ESP8266WebServer(80);
    };

    void begin();
    void handleRoot();
    void handleConfiguration();
    void handleLogo();
    void handleStylesheet();

    ESP8266WebServer* getServerObject()
    {
        return this->server_object;
    };
};

#endif