#include "PushDis.h"


Token::Token(const Token &token) {
  m_tokenString = token.m_tokenString;
}

Token::Token(String tokenString) {
  m_tokenString = tokenString;
}

String Token::toString() {
  return m_tokenString;
}

PushDisConnector::PushDisConnector(WiFiClientSecure &wifiClient, String connectorCode, String myName) {
  m_wifiClient = &wifiClient;
  m_pushDisCode = connectorCode;
  m_deviceName = myName;
  m_httpClient.setTimeout(5000);
}


PushDisConnector::PushDisConnector(WiFiClientSecure* wifiClient, String connectorCode, Token token) {
  m_wifiClient = wifiClient;
  m_pushDisCode = connectorCode;
  m_httpClient.setTimeout(5000);
  m_token = new Token(token);
  m_authHeader = "Bearer " + m_token->toString();
}

PushDisConnector* PushDisConnector::reconnect(WiFiClientSecure &wifiClient, String connectorCode, Token token) {
  PushDisConnector* pdc = new PushDisConnector(&wifiClient, connectorCode, token);

  pdc->reconnect();

  return pdc;
}

void PushDisConnector::reconnect() {
  String getAuthStatusRequestUrl = "https://api.pushdis.com/api/v1/connector/getAuthStatus/?conCode=" + m_pushDisCode;

  m_httpClient.begin(*m_wifiClient, getAuthStatusRequestUrl); 
  m_httpClient.addHeader("Authorization", m_authHeader);
  auto httpCode = m_httpClient.GET();
  
  if (httpCode == HTTP_CODE_OK) {
 
    if (m_httpClient.getString() == "GRANTED") {
        Serial.println("Reconnect successful. Connection established.");
        
    } else {
        Serial.println("Error during reconnect.");
        Serial.println(m_httpClient.getString());
    }
  
  }
}

void PushDisConnector::connect() {

    String url_base = "https://api.pushdis.com";
    String url_auth = url_base + "/api/v1/connector/auth/";
    String url_getAuthStatus = url_base + "/api/v1/connector/getAuthStatus/";

    String authRequestUrl = url_auth + "?conCode=" + m_pushDisCode;
    String authRequestJson = "{\"deviceName\":\"" + m_deviceName + "\"}";
    
    String getAuthStatusRequestUrl = url_getAuthStatus + "?conCode=" + m_pushDisCode;

    Serial.print("Initializing PushDis connection with connector code ");
    Serial.println(m_pushDisCode);

    String conStatus = "DENIED";

    while (conStatus != "GRANTED") {

       if (conStatus == "DENIED") {
          Serial.println("Ping PushDis connector");
          m_httpClient.begin(*m_wifiClient, authRequestUrl);
          m_httpClient.addHeader("Content-Type", "application/json");
          auto httpCode = m_httpClient.POST(authRequestJson); 
          
          if (httpCode == HTTP_CODE_OK) {
            m_token = new Token(m_httpClient.getString());
            m_authHeader = "Bearer " + m_token->toString();
            conStatus = "PENDING";
            Serial.println("Connector accepted connection. Waiting for user confirmation.");
          }
          else if (httpCode == HTTP_CODE_FORBIDDEN) {
            conStatus = "DENIED";
            String body = m_httpClient.getString();
            Serial.println(body);
          }
          else {
            Serial.println("Connection failed with error:");
            Serial.println(m_httpClient.errorToString(httpCode));
            String body = m_httpClient.getString();
            Serial.println(body);
          }
          m_httpClient.end();
       }  
       else if (conStatus == "PENDING") {
          Serial.print(".");
          
          m_httpClient.begin(*m_wifiClient, getAuthStatusRequestUrl); 
          m_httpClient.addHeader("Authorization", m_authHeader);
          auto httpCode = m_httpClient.GET();
          
          if (httpCode == HTTP_CODE_OK) {
            conStatus = m_httpClient.getString();
          
          } else if (httpCode == HTTP_CODE_FORBIDDEN) {
            conStatus = "DENIED";
            Serial.println("Connection request denied.");
            String body = m_httpClient.getString();
            Serial.println(body);
        
          } else {
            Serial.println("Connection failed with error:");
            String body = m_httpClient.getString();
            Serial.println(body);
            Serial.println(m_httpClient.errorToString(httpCode));
          }
          m_httpClient.end();
      
        }

        delay(5000);
    } // while loop end
    
    if (conStatus == "GRANTED") {
      Serial.println("Connection established.");
    }
}

void PushDisConnector::pushParams(String title, std::vector<DisplayParameter> params) {

    String url_postPushLive = "https://api.pushdis.com/api/v1/display/pushLiveParams/";
    String pushRequest = url_postPushLive + "?conCode=" + m_pushDisCode;

    String paramsJson = "{\"title\":\"" + title + "\", \"parameters\":[";

    int paramSize = params.size();
    for (int i=0; i< paramSize; i++) {
        paramsJson = paramsJson + 
                            "{\"name\":\"" + params[i].name + "\", \"value\":\""  + params[i].value + "\"}";
                            
        if (i != paramSize-1) { 
          paramsJson = paramsJson + ",";
        }
    }
    paramsJson = paramsJson + "]}";
     
    m_httpClient.begin(*m_wifiClient, pushRequest); 

    m_httpClient.addHeader("Authorization", m_authHeader);   
    m_httpClient.addHeader("Content-Type", "application/json");
    auto httpCode = m_httpClient.POST(paramsJson); 
     
    if (httpCode != HTTP_CODE_OK) {
        String error = m_httpClient.getString();
        Serial.println("Connection failed with error:");
        Serial.println(m_httpClient.errorToString(httpCode));
        Serial.println(error);
    }
    
    m_httpClient.end(); 
}


void PushDisConnector::pushAlert(String title, String body) {
    
    String url_postPushAlert = "https://api.pushdis.com/api/v1/display/pushAlert/";
    String pushRequest = url_postPushAlert + "?conCode=" + m_pushDisCode;
    String pushAlert = "{\"title\":\"" + title + "\", \"body\":\"" + body + "\"}";
 
    m_httpClient.begin(*m_wifiClient, pushRequest);     
    m_httpClient.addHeader("Authorization", m_authHeader);
    m_httpClient.addHeader("Content-Type", "application/json");
    auto httpCode = m_httpClient.POST(pushAlert); 

    if (httpCode != HTTP_CODE_OK) {
        String error = m_httpClient.getString();
        Serial.println("Connection failed with error:");
        Serial.println(m_httpClient.errorToString(httpCode));
        Serial.println(error);
    }
    
    m_httpClient.end(); 
}

Token* PushDisConnector::getToken() {
  return m_token;
}
