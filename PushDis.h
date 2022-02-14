#ifndef PUSHDIS_H
#define PUSHDIS_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

struct DisplayParameter {
  String name;
  String value;
};

class Token {

  public:
    Token(const Token &token);
    Token(String tokenString);

    String toString();

  private:
    String m_tokenString;
  
};

class PushDisConnector {

  public:
    PushDisConnector(WiFiClientSecure &wifiClient, String connectorCode, String myName);
    
    static PushDisConnector* reconnect(WiFiClientSecure &wifiClient, String connectorCode, Token token);

    void connect();
    void pushParams(String title, std::vector<DisplayParameter> params);
    void pushAlert(String title, String body);

    Token* getToken();

  private:
    PushDisConnector(WiFiClientSecure* wifiClient, String connectorCode, Token token);

    WiFiClientSecure* m_wifiClient;
    HTTPClient m_httpClient;

    String m_pushDisCode;
    String m_deviceName;
    Token* m_token;
    String m_authHeader;
    
    void reconnect();
};

#endif
