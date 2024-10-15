#include "ISE_mux.h"


Mux::Mux(int A, int B, int C, int INH, int chmap[]){
    mux_A = A; 
    mux_B = B; 
    mux_C = C; 
    mux_INH = INH; 
    ch_map = chmap;
}

void Mux::disable_mux(){

    digitalWrite(mux_INH, HIGH);  // Channel remapping from schematic (e.g. ISE Channel 1 on the PCB is connected to ISE MUX Channel 4)  
}


void Mux::switch_channel(int ch_num){  // 10 ms delay to avoid rapid channel switching
    
    int channel = 0; 
    
    digitalWrite(mux_INH, LOW);  // Channel remapping from schematic (e.g. ISE Channel 1 on the PCB is connected to ISE MUX Channel 4)  

      if(0 < ch_num < 9){
        
        channel = ch_map[ch_num - 1];
      }
      
      if(ch_num == -1)
        {
          channel = -1;
          //Serial.println("Null Channel");

      }  
        
  // Connecting desired channel from logic table in SN74LV4051A datasheet  
 
  if(channel == 1){
    digitalWrite(mux_C, LOW);
    digitalWrite(mux_B, LOW);
    digitalWrite(mux_A, LOW);  }
  else if(channel == 2){
    digitalWrite(mux_C, LOW);
    digitalWrite(mux_B, LOW);
    digitalWrite(mux_A, HIGH);
  }
  else if(channel == 3){
    digitalWrite(mux_C, LOW);
    digitalWrite(mux_B, HIGH);
    digitalWrite(mux_A, LOW);
  }
    else if(channel == 4){
    digitalWrite(mux_C, LOW);
    digitalWrite(mux_B, HIGH);
    digitalWrite(mux_A, HIGH);
  }
    else if(channel == 5){
    digitalWrite(mux_C, HIGH);
    digitalWrite(mux_B, LOW);
    digitalWrite(mux_A, LOW);
  }
    else if(channel == 6){
    digitalWrite(mux_C, HIGH);
    digitalWrite(mux_B, LOW);
    digitalWrite(mux_A, HIGH);
  }
    else if(channel == 7){
    digitalWrite(mux_C, HIGH);
    digitalWrite(mux_B, HIGH);
    digitalWrite(mux_A, LOW);
  }
  else if(channel == 8){
    digitalWrite(mux_A, HIGH);
    digitalWrite(mux_B, HIGH);
    digitalWrite(mux_C, HIGH);
  }  // Channel -1 disables the MUX
  else if(channel == -1){
    digitalWrite(mux_A, LOW);
    digitalWrite(mux_B, LOW);
    digitalWrite(mux_C, LOW);
    digitalWrite(mux_INH, HIGH);
  }  // If the user gives an invalid channel tell them
  else{
    Serial.println("requested channel does not exist");
  }
 
  }
