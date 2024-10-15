#include <Arduino.h>
#include "Wire.h"

class davispcbv2
{

public: 

    float read_pair(int ISE_ch, int RE_ch, int d_time = 3000);   


};
