#include <ESP8266mDNS.h>
#include "webserver.h"
#include "FS.h"
#include "functions.h"

void Webserver::begin()
{   
  MDNS.begin("esp8266");

  this->server_object->on("/", HTTP_GET, [this]() { handleRoot();});
  this->server_object->on("/style.css", HTTP_GET, [this]() { handleStylesheet();});
  this->server_object->on("/logo.png", HTTP_GET, [this]() { handleLogo();});
  this->server_object->on("/configure", HTTP_POST, [this]() { handleConfiguration();});
  this->server_object->begin();
}

void Webserver::handleConfiguration()
{
  String ssid = server_object->arg("ssid");
  String password = server_object->arg("password");
    
  String content = "<html><head><meta charset='UTF-8'><title>Coffeere</title>";
  content += "<link rel='stylesheet' href='style.css'>";
  content += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  content += "</head><body><div class='logo'></div>";
  content += "<div class='login-block'><h1  style='margin-top: 10px'>";
  content += "Tentando conectar à rede '" + ssid + "'</h1>";
  content += "<span class='circle' style='background-color: #09ff00;'></span>Luz verde - Conectado com sucesso<br/>";
  content += "<div style='margin-top: 10px;'>";
  content += "<span class='circle' style='background-color: #ff0000;'></span>Luz vermelha - Falha ao conectar</div>";  
  content += "</div></body><footer>powered by <a target='blank' href='http://maxwellden.github.com'>MaXwEllDeN</a></footer></html>";

  server_object->send(200, "text/html", content);
  saveConfiguration(ssid, password, 0.0);
  
  restartApplet();
  return;
}

void Webserver::handleLogo()
{
  File webFile = SPIFFS.open("/logo.png", "r");

  String content = "";

  if (webFile) {
    while (webFile.available()) {
      content += (char) webFile.read();
    }

    webFile.close();
  }

  server_object->send(200, "image/png", content);
  return;
}

void Webserver::handleStylesheet()
{
  File webFile = SPIFFS.open("/style.css", "r");

  String content = "";

  if (webFile) {
    while (webFile.available()) {
      content += (char) webFile.read();
    }

    webFile.close();
  }

  server_object->send(200, "text/css", content);

  return;
}

void Webserver::handleRoot()
{
  File webFile = SPIFFS.open("/index.html", "r");

  String content = "";

  if (webFile) {
    while (webFile.available()) {
      content += (char) webFile.read();
    }

    webFile.close();
  }

  server_object->send(200, "text/html", content);

  return;
}
