/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Google_Sheet_Client.h>
#include <stdlib.h>
#include "time.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80); // listens for incoming HTTP requests on port 80

// Network credentials
const char* ssid = "Berkeley-IoT";
const char* password = "r7ubCMR!";

// Google Sheet to be written to 
// Make sure access is "anyone with link can edit"
String spreadsheetId = "1KyccwrxmbhHYBc3ppK5N1OD_RTGPX2c8j6nK0z7BGNE";
String spreadsheetURL = "https://docs.google.com/spreadsheets/d/1KyccwrxmbhHYBc3ppK5N1OD_RTGPX2c8j6nK0z7BGNE/edit?usp=sharing";

// Google API params (service account)
#define PROJECT_ID "lucasproj"
#define CLIENT_EMAIL "esp32-readings@lucasproj.iam.gserviceaccount.com"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCnIW1H+he+ifna\nagpwkheDGRep7a1jJqVDGzNFDRIVXmqZRipF25Eyp4WiLFhmCCBZlBXfVYay2avy\nq1m6gnAEh2wh/WLqsS8XB2jrmbZ15OZ35iPKdoylpQGsmDKmBIQ3owwS+8cohlmP\nJCOyaKMN5HHz7lbaps7o/Rjrj0sLbprcH1KksCO+PDVkNpDQj8gYtv/l5CNSl2Ac\nfEBXcVI5SzdFlV2ncZkrFJnymPUHAmQyHEp2U8z1wh0L7D5FxLyiLx/fEWbc3Abj\n2hLHFiHbTfg5c4doqahL/KIypI4qJkJGx6FXapEmJiera893f2vW3aFRQW1StYEO\nJodY28RBAgMBAAECggEAF/7vI0nvlc5Wnrrh49a95Hc6VA4JenZmfPOPUWc2hPXo\n5TfTQvNq7fRqdeXbDU82YdYmq8mqTMx+6RkcCXDH54aQCnNIe/zTj4FjWYxYy6WB\nR4617dBGklPWITM69mYom5pf1evrZNChVDFypJ9sZca6a7T5KGyRwZbxOIT619me\nlKpx2Yzm/1dHdceEXdaMhUWakeLPJN618ftpGpAYxSn77a973G9ZseDgva2jx0T5\nNQpa4i0ZIzLCpp0TNUfrffG7SzX52xDsE77bLvgfALk75DB2Hwm/GQ2Zqr0sJw32\nxqJ64I4uwU4f6s8paWz+HVwuW+/A9WoVaea0gzLm/wKBgQDXm81qD+6vpGdg8Vr4\nAEzTz+2UkmqKlh0rmtt+y6L6TWb+63rIuSHlcjyaQmNYgP2l8bwl5abcGzrEQLq1\nePgrkP9+/FaI3fzybsOWG87tPDPkpIB0wfh6XfPHd7d5cx4/uADSQZEROqFR8q9D\noSL7j7wstCRjemVAnLtNwe1rUwKBgQDGcLTDWVtfjafc5QtXJ4rdb4oEyO3O/qzC\niGt/NCQd2we1Y34TKGs0Trx54o7x/s8Jv88wPGPh+ii6AZK/iyHybfDQz8J4Nvoz\ncOnXzViXXzNqkDE+tk/whDkScKEfiXiDgB5w3jzgcvvKF7ZgwN7aIQGasx7WQbzZ\n8Vq5/lTzmwKBgGcPBAr0fqTaMsf11gPBMPf15ZwkuGRyCJN1PAHJmKeII/kp2dDk\nHT2tcyZRIWeGkLenasW6lh2Trm1se1KO9iDs+BNczogxg9zl8PtpY2mmYy+hKf2l\nUf47tnDb2olk60qNy2a1DIBtw/qFxBzRgwU32TyNBu+963QM9sLeNXwdAoGAVzy3\nCuYemExDDoYWzTLSBnUBfQcEHKqzMjYc4+f9NK7UnbZmd2+eaXlvvDmQU+yMAiIz\nqGX0FyUNgHAunkxBGT2hz3p2aLjql3C9LKTWnH06sk8/hu1JCVhnaqbe/mlF9LF/\nMGlS4+1Iqdhw0eoYTlaSSPsTxGdT6ZAELZ4UpJ8CgYEAg36krdtOfww0T1NLmq0+\nISgxIT8tJz8K3bK23DC9MQEORpl2fkljgadDVN/DWy92xIN5kvvZV+zClSkpVvWO\nluezFlXsqGlPXhDfTEL1rDv1CiaJa1aZ/iWQM1BpomgWpzysz4YFBkXWTFJvixbZ\nLIWPj8kx/ysR0Hiqqc3mwdk=\n-----END PRIVATE KEY-----\n";

// Objects for GSheet API input and ouput
FirebaseJson response; // Create the object to handle responses from the API calls
FirebaseJson valueRange; // Object to send through API calls

// Connecting the times
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -8 * 3600; // PST
const int daylightOffset_sec = 3600; // 3600 (if there is daylight savings), 0 if normal PST

// Variables for writing timestamp into entries
struct tm timeinfo;
char timeStringBuff[50];
String asString;
char buffer[40];

void tokenStatusCallback(TokenInfo info); // header for a debug function

// Variables to be changed by user input on webpage
const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";
//const char* PARAM_INPUT_3 = "input3";
String inputMessage;
String inputParam;
int inputValue;
bool startSequence = false;

// Loop var
int iter = 0;

// HTML, AJAX, and JavaScript web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    // Function to send AJAX request
    function submitForm(inputName, inputValue) {
      const xhr = new XMLHttpRequest();
      xhr.open("GET", `/get?${inputName}=${inputValue}`, true);
      xhr.onload = function () {
        if (xhr.status === 200) {
          alert(`Task initiated with ${inputName}: ${inputValue}`);
        } else {
          alert('Failed to send request');
        }
      };
      xhr.send();
    }

    // Handler for form submission
    function handleSubmit(event, inputName) {
      event.preventDefault();  // Prevent page reload
      const inputValue = event.target.querySelector('input[type="text"]').value;
      submitForm(inputName, inputValue);
    }
  </script>
</head>
<body>
  <h2>ESP32 Input Form</h2>
  <form onsubmit="handleSubmit(event, 'input1')">
    input1: <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form><br>

  <form onsubmit="handleSubmit(event, 'input2')">
    input2: <input type="text" name="input2">
    <input type="submit" value="Submit">
  </form><br>
//
//  <form onsubmit="handleSubmit(event, 'input3')">
//    input3: <input type="text" name="input3">
//    <input type="submit" value="Submit">
//  </form>
</body>
</html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Begin WiFi connection
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(300);
  }
  Serial.println(" Connected with IP: " + WiFi.localIP().toString());

  // Begin the time synchronization to get timestamps
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  delay(2000);  // Wait for NTP sync

  // Set the callback for Google API access token generation status (for debug only)
  GSheet.setTokenCallback(tokenStatusCallback);

  // Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
  GSheet.setPrerefreshSeconds(10 * 60);

  // Begin the access token generation for Google API authentication
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

  // Try to add column headers to an existing spreadsheet
  valueRange.set("values/[0]/[0]", "Timestamp");
  valueRange.set("values/[0]/[1]", "Ch1");
  valueRange.set("values/[0]/[2]", "Ch2");
  GSheet.values.update(&response /* returned response */, spreadsheetId /* spreadsheet Id to update */, "Sheet1" /* range to update */, &valueRange /* data to update */);
  response.toString(Serial, true);
  Serial.println();

  // Serve the webpage with input fields
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // Handle GET request with input parameters
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {

    if (request->hasParam("input1")) {
      inputMessage = request->getParam("input1")->value();
      inputValue = inputMessage.toInt();
      inputParam = "input1";
      startSequence = true;
      iter = 0;
    } 
    else if (request->hasParam("input2")) {
      inputMessage = request->getParam("input2")->value();
      inputParam = "input2";
      startSequence = false;
    } 
//    else if (request->hasParam("input3")) {
//      inputMessage = request->getParam("input3")->value();
//      inputParam = "input3";
//    } 
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    
    Serial.println(inputMessage);
    request->send(200, "text/plain", "Success");  // Just return a success message
  });

  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  bool ready = GSheet.ready();
  if (startSequence && iter < inputValue) {
    if (ready) {
      if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
            return;
        }

      strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
      asString = timeStringBuff;

      // Create a new row to append
      FirebaseJson valueRange;
      valueRange.set("values/[0]/[0]", asString);
      
      valueRange.set("values/[0]/[1]", "first val");

      valueRange.set("values/[0]/[2]", "second val");

      // Append data to the first available empty row
      GSheet.values.append(&response, spreadsheetId, "Sheet1!A:C", &valueRange);
      response.toString(Serial, true);  // Print the API response for debugging
      Serial.println();

      // Update loop var
      iter++;

      delay(500);  // Optional: Avoid hitting rate limits
    } else {
      Serial.println("Google Sheets client not ready. Retrying...");
      delay(1000);  // Retry after a brief delay
    }
  }
}


void tokenStatusCallback(TokenInfo info) {
  if (info.status == esp_signer_token_status_error) {
    Serial.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    Serial.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
  } else {
    Serial.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
  }
}
