#include <Arduino.h>

class Mux
{
private:
int mux_A;
int mux_B; 
int mux_C; 
int mux_INH; 
int* ch_map;

public: 
    Mux(int mux_A, int mux_B, int mux_C, int mux_INH, int ch_map[]);

    void switch_channel(int ch_num); 

    void disable_mux();
};