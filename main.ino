
/**
 * 
 * References: Example sketch by M.Merati (majid.merati.2012@gmail.com) 
 * on https://electropeak.com/learn/sending-data-from-esp32-or-esp8266-to-google-sheets-2-methods/
 */


#include <Arduino.h>
//#include <floatToString.h>
#include <WiFi.h>
#include <ESP_Google_Sheet_Client.h>
#include <stdlib.h>
#include "time.h"
#include "Wire.h"
#include <davispcbv2.h>
#include <ISE_mux.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>

// ISE readout board
#define ads1110 0x48// Logic Pins on ESP32 to Control RE/ISE MUX
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);

// Connecting the times
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -8 * 3600; // PST
const int daylightOffset_sec = 3600; // 3600 (if there is daylight savings), 0 if normal PST

// For SD/SD_MMC mounting helper
//#include <GS_SDHelper.h>

#define WIFI_SSID "Berkeley-IoT"
#define WIFI_PASSWORD "jF,55N8u"
#define PROJECT_ID "lucasproj"

// Service Account's client email
#define CLIENT_EMAIL "esp32-readings@lucasproj.iam.gserviceaccount.com"

// Service Account's private key
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCnIW1H+he+ifna\nagpwkheDGRep7a1jJqVDGzNFDRIVXmqZRipF25Eyp4WiLFhmCCBZlBXfVYay2avy\nq1m6gnAEh2wh/WLqsS8XB2jrmbZ15OZ35iPKdoylpQGsmDKmBIQ3owwS+8cohlmP\nJCOyaKMN5HHz7lbaps7o/Rjrj0sLbprcH1KksCO+PDVkNpDQj8gYtv/l5CNSl2Ac\nfEBXcVI5SzdFlV2ncZkrFJnymPUHAmQyHEp2U8z1wh0L7D5FxLyiLx/fEWbc3Abj\n2hLHFiHbTfg5c4doqahL/KIypI4qJkJGx6FXapEmJiera893f2vW3aFRQW1StYEO\nJodY28RBAgMBAAECggEAF/7vI0nvlc5Wnrrh49a95Hc6VA4JenZmfPOPUWc2hPXo\n5TfTQvNq7fRqdeXbDU82YdYmq8mqTMx+6RkcCXDH54aQCnNIe/zTj4FjWYxYy6WB\nR4617dBGklPWITM69mYom5pf1evrZNChVDFypJ9sZca6a7T5KGyRwZbxOIT619me\nlKpx2Yzm/1dHdceEXdaMhUWakeLPJN618ftpGpAYxSn77a973G9ZseDgva2jx0T5\nNQpa4i0ZIzLCpp0TNUfrffG7SzX52xDsE77bLvgfALk75DB2Hwm/GQ2Zqr0sJw32\nxqJ64I4uwU4f6s8paWz+HVwuW+/A9WoVaea0gzLm/wKBgQDXm81qD+6vpGdg8Vr4\nAEzTz+2UkmqKlh0rmtt+y6L6TWb+63rIuSHlcjyaQmNYgP2l8bwl5abcGzrEQLq1\nePgrkP9+/FaI3fzybsOWG87tPDPkpIB0wfh6XfPHd7d5cx4/uADSQZEROqFR8q9D\noSL7j7wstCRjemVAnLtNwe1rUwKBgQDGcLTDWVtfjafc5QtXJ4rdb4oEyO3O/qzC\niGt/NCQd2we1Y34TKGs0Trx54o7x/s8Jv88wPGPh+ii6AZK/iyHybfDQz8J4Nvoz\ncOnXzViXXzNqkDE+tk/whDkScKEfiXiDgB5w3jzgcvvKF7ZgwN7aIQGasx7WQbzZ\n8Vq5/lTzmwKBgGcPBAr0fqTaMsf11gPBMPf15ZwkuGRyCJN1PAHJmKeII/kp2dDk\nHT2tcyZRIWeGkLenasW6lh2Trm1se1KO9iDs+BNczogxg9zl8PtpY2mmYy+hKf2l\nUf47tnDb2olk60qNy2a1DIBtw/qFxBzRgwU32TyNBu+963QM9sLeNXwdAoGAVzy3\nCuYemExDDoYWzTLSBnUBfQcEHKqzMjYc4+f9NK7UnbZmd2+eaXlvvDmQU+yMAiIz\nqGX0FyUNgHAunkxBGT2hz3p2aLjql3C9LKTWnH06sk8/hu1JCVhnaqbe/mlF9LF/\nMGlS4+1Iqdhw0eoYTlaSSPsTxGdT6ZAELZ4UpJ8CgYEAg36krdtOfww0T1NLmq0+\nISgxIT8tJz8K3bK23DC9MQEORpl2fkljgadDVN/DWy92xIN5kvvZV+zClSkpVvWO\nluezFlXsqGlPXhDfTEL1rDv1CiaJa1aZ/iWQM1BpomgWpzysz4YFBkXWTFJvixbZ\nLIWPj8kx/ysR0Hiqqc3mwdk=\n-----END PRIVATE KEY-----\n";

float SensValue = 0; // Float to store the output of read_pair() 
char voltageStr[10]; // String to hold the 12 bit output of voltage reading (0 - 4096) 
bool taskComplete = false; // Boolean to stop readings

void tokenStatusCallback(TokenInfo info);

// Loop variables
bool collect_data = true;
int counter = 1; // The number of data points and the row number to input values in Gsheet
bool initialized = false;
FirebaseJson response; // Create the object to handle responses from the API calls
FirebaseJson valueRange; // Object to send through API calls
String spreadsheetId = "1NH1fdTd90tOHnppB2fGd5rUo1olu1A2CTlNbLp_ON3g";
String spreadsheetURL = "https://docs.google.com/spreadsheets/d/1NH1fdTd90tOHnppB2fGd5rUo1olu1A2CTlNbLp_ON3g/edit?usp=sharing";

// Variables for writing timestamp into entries
struct tm timeinfo;
char timeStringBuff[50];
String asString;
char buffer[40];

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // Being wifi connection
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(300);
  }
  Serial.println(" Connected with IP: " + WiFi.localIP().toString());

  // Begin the time synchronization to get timestamps
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  delay(2000);  // Wait for NTP sync

  // Make the ISE readout board available for reading
  Wire.begin();

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
  delay(5000);
  
}

void loop() {
  // Call ready() repeatedly in loop for authentication checking and processing
  bool ready = GSheet.ready();

  if (ready && counter <= 10) {
    Serial.printf("Updating row: %d\n", counter);

        Serial.println("\nUpdate spreadsheet values...");
        Serial.println("------------------------------");
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
            return;
        }
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
        strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
        asString = timeStringBuff;
        asString.replace(" ", "-");

        // Column 1 is time
        sprintf(buffer, "values/[%d]/[0]", counter);
        valueRange.set(buffer, asString);

        // Column 2 is channel 1
        SensValue = davispcbv2().read_pair(1, -1); // // Get Ch1 float
        dtostrf(SensValue, sizeof(voltageStr), 3, voltageStr);
        sprintf(buffer, "values/[%d]/[1]", counter);
        valueRange.set(buffer, voltageStr);

        // Column 3 is channel 2
        SensValue = davispcbv2().read_pair(2, -1); // Get Ch2 float
        dtostrf(SensValue, sizeof(voltageStr), 3, voltageStr); // convert to string
        sprintf(buffer, "values/[%d]/[2]", counter); // Set location in spreadsheet
        valueRange.set(buffer, voltageStr); // Save location and value to write 

        GSheet.values.update(&response /* returned response */, spreadsheetId /* spreadsheet Id to update */, "Sheet1" /* range to update */, &valueRange /* data to update */);
        response.toString(Serial, true);
        Serial.println();
        
        delay(500);
     }

     counter++;
}

void tokenStatusCallback(TokenInfo info) {
  if (info.status == esp_signer_token_status_error) {
    Serial.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    Serial.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
  } else {
    Serial.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
  }
}
