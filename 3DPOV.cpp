#include "mbed.h"
#include "Servo.h"

Servo ESC(p21);
DigitalOut Data(p5);
DigitalOut Latch(p6);
DigitalOut Clock(p7);
DigitalOut RPM_LED(p25);
DigitalOut RPM_FOTO(p23);

InterruptIn RPM(p30);
Timer timer;

uint16_t seq1[30] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,16384,8192,4096,2048,1024,512,256,128,64,32,16,8,4,2};
uint16_t data_array[120][15];

int i, k, j, degree;
volatile int rad_count = 0;

int rad_div = 120;
int rad_div_half = rad_div/2;
int num_tiers = 15;
int spacing = rad_div/8;
int sample_size = 200;
int degree_delay_us = 0;
int timing_offset_us = 6;

bool rpm_check = true;

void light_it_up(int);
void write_data_array(int, int, int);
void rad_increment();
void timing();

int main()
{
    ESC.position(-180);     //Arm ESC
    wait(5);                //Wait for Arming to complete
    ESC.position(1);       //Output speed signal

    uint16_t SQUARE1[] = {
        0, 2, 5, 10, 20, 25, 27
    };
    uint16_t SQUARE2[] = {
        16,32,64,128,128,64,32
    };
    for(j = 0; j < 4; j++) {
        for(int l = 0; l < 7; l++) {
            write_data_array(2, SQUARE1[l] + rad_div*j/4, SQUARE2[l]);
            write_data_array(10, SQUARE1[l] + rad_div*j/4, SQUARE2[l]);
        }
        write_data_array(3,30*j,16);
        write_data_array(4,30*j,16);
        write_data_array(5,30*j,16);
        write_data_array(6,30*j,16);
        write_data_array(7,30*j,16);
        write_data_array(8,30*j,16);
        write_data_array(9,30*j,16);
    }
    
    
    
    timing();
    


    //Main display loop;
    while(1) {
        for(degree = 0; degree < rad_div; degree++) {
            light_it_up(degree);
            wait_us(degree_delay_us);
        }
    }





}

void light_it_up(int deg) //Outputs LED pattern for the given radial degree.
{
    Latch = 0;
    for(i = 0; i < 15; i++) {
        for(k = 0; k < 16; k++) {
            Data = (data_array[deg][i] & (1 << k));
            Clock = 0;
            Clock = 1;
        }
    }
    i = 14;
    if(deg < rad_div_half) {
        for(; i >= 0; i--) {
            for(k = 0; k < 16; k++) {
                Data = (data_array[deg + rad_div_half][i] & (1 << k));
                Clock = 0;
                Clock = 1;
            }
        }
    } else {
        for(; i >= 0; i--) {
            for(k = 0; k < 16; k++) {
                Data = (data_array[deg - rad_div_half][i] & (1 << k));
                Clock = 0;
                Clock = 1;
            }
        }
    }
    Latch = 1;
}

void write_data_array(int tier, int degree, int num) //Writes to data_array given the tier, degree, and num coordinates. Converts 
{
    data_array[(rad_div + degree - spacing*(tier%4))%rad_div][tier] = num;
}

void clear_data_array()
{
    for(int t_rad = 0; t_rad < rad_div; T_rad++){
        for(int t_tier = 0; t_tier < num_tiers; t_tier++){
            data_array[t_rad][t_tier] = 0;
        }
    }
}

void timing() //Calculates the delay between each degree.
{
    int begin, end, overhead;
    timer.start();
    begin = timer.read_us();

    for(j = 0; j < sample_size; j++) {
        for(degree = 0; degree < rad_div; degree++) {
            light_it_up(degree);
            wait_us(degree_delay_us);
        }
    }

    end = timer.read_us();
    overhead = end - begin;

    wait(10);

    timer.reset();
    RPM_LED = 1;
    RPM_FOTO = 1;
    rad_count = 0;

    begin = timer.read_us();
    RPM.fall(&rad_increment);
    while(rad_count < sample_size);
    end = timer.read_us();
    RPM.fall(NULL);
    RPM_LED = 0;
    RPM_FOTO = 0;
    degree_delay_us = (end - begin - overhead)/rad_div/sample_size + timing_offset_us;
}

void rad_increment()
{
    rad_count++;
}