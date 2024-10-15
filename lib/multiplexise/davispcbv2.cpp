#include "davispcbv2.h"
#include "ISE_mux.h"
#include "Wire.h"

// Default I2C Address for ADC (ADS1110)
#define ads1110 0x48


// Default Initialization of ISE and RE multiplex parameters
Mux initialize_ISEMux(int* channelmap_ise, int ISE_A = 14, int ISE_B = 32,int ISE_C = 15, int ISE_INH = 33){

    return Mux(ISE_A, ISE_B, ISE_C, ISE_INH, channelmap_ise);

}

Mux initialize_REMux(int* channelmap_re, int RE_A = 27, int RE_B = 12,int  RE_C = 21, int RE_INH = 4){

    return Mux(RE_A, RE_B, RE_C, RE_INH, channelmap_re);

}

// INPUTS: ISE_mux: Mux object which controls the ISE input, ISE_ch: channel to read (1 - 8) (-1 to disable mux), RE_mux: Mux object which controls RE input
// RE_ch: integer (1 - 8) reference electrode channel to read (-1 to disable mux)
// OUTPUTS: voltage in volts measured at the ADC 
float davispcbv2::read_pair(int ISE_ch, int RE_ch, int d_time){  

    float voltage, data;
    byte highbyte, lowbyte, configRegister;  
    
    Wire.begin();

    //Fresh initialization of multiplexer
    int remap[] = {3,2,1,4,5,7,6,8};
   
    int isemap[] = {4,1,2,3,5,7,8,6};
    Mux RE_mux = initialize_REMux(remap);
    Mux ISE_mux = initialize_ISEMux(isemap);

    //Configure the multiplexers to the desired channels 
   
    RE_mux.switch_channel(RE_ch); 
    
    ISE_mux.switch_channel(ISE_ch);

    delay(d_time);  // **Important delay code built due to the LPF time constant** 
    
    //request data from ADC 
    Wire.requestFrom(ads1110, 3);

   // receive data from ADC 
    while(Wire.available()) 
    {
        highbyte = Wire.read(); // high byte * B11111111
        lowbyte = Wire.read(); // low byte
        configRegister = Wire.read();
    }  
  
    // Convert data received from I2C wire to digital code 
    data = highbyte * 256;
    data = data + lowbyte; 
    if(data == 0){
        Serial.print("No Data Recieved");

    }
    //Data to voltage conversion (Depends on ADC gain config.) 
    voltage = data * 2.048 ;
    voltage = voltage / 32768.0;  
    
    return voltage;

}
