#include "mbed.h"
#include "Servo.h"

Servo ESC(p21);
DigitalOut Data(p5);
DigitalOut Latch(p6);
DigitalOut Clock(p7);
DigitalOut RPM_LED(p10);

InterruptIn RPM(p20);
Timer timer;

uint16_t seq1[30] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,16384,8192,4096,2048,1024,512,256,128,64,32,16,8,4,2};
uint16_t data_array[120][15];

int i, k, j, degree;
volatile int rad_count = 0;

int rad_div = 120;
int rad_div_half = rad_div/2;
int sample_size = 500;
int degree_delay_us = 0;

bool rpm_check = true;

void update_display(int);
void rad_increment();

int main()
{
    ESC.position(-180);     //Arm ESC
    wait(5);                //Wait for Arming to complete
    ESC.position(80);       //Output speed signal


    int begin, end, overhead;
    timer.start();
    begin = timer.read_ms();

    for(j = 0; j < sample_size; j++) {
        for(degree = 0; degree < rad_div; degree++) {
            update_display(degree);
            wait_us(degree_delay_us);
        }
    }
    
    end = timer.read_ms();
    overhead = end - begin;

    wait(40);

    timer.reset();
    RPM_LED = 1;
    begin = timer.read_ms();
    RPM.rise(&rad_increment);
    while(rad_count < sample_size);
    end = timer.read_ms();
    RPM.rise(null);
    degree_delay_us = 1000*(begin - end - overhead)/rad_div/sample_size;


    uint16_t SQUARE1[] = {
        0, 2, 5, 10, 20, 25, 27
    };
    uint16_t SQUARE2[] = {
        16,32,64,128,128,64,32
    };
    for(j = 0; j < 4; j++) {
        for(int l = 0; l < 7; l++) {
            data_array[SQUARE1[l] + rad_div*j/4][2] = SQUARE2[l];
            data_array[SQUARE1[l] + rad_div*j/4][10] = SQUARE2[l];
        }
        data_array[30*i][3] = 16;
        data_array[30*i][4] = 16;
        data_array[30*i][5] = 16;
        data_array[30*i][6] = 16;
        data_array[30*i][7] = 16;
        data_array[30*i][8] = 16;
        data_array[30*i][9] = 16;
    }


    //Main display loop;
    while(1) {
        for(degree = 0; degree < rad_div; degree++) {
            update_display(degree);
            wait_us(degree_delay_us);
        }
    }
}

void update_display(int deg)
{
    Latch = 0;
    for(i = 0; i < 15; i++) {
        for(k = 0; k < 16; k++) {
            Data = ((data_array[deg][i] & (1 << k)) != 0);
            Clock = 0;
            Clock = 1;
        }
    }
    if(deg < rad_div/2) {
        for(; i > 0; --i) {
            for(k = 0; k < 16; k++) {
                Data = ((data_array[deg + rad_div_half][i] & (1 << k)) != 0);
                Clock = 0;
                Clock = 1;
            }
        }
    } else {
        for(; i > 0; --i) {
            for(k = 0; k < 16; k++) {
                Data = ((data_array[deg - rad_div_half][i] & (1 << k)) != 0);
                Clock = 0;
                Clock = 1;
            }
        }
    }
    Latch = 1;
}

void rad_increment()
{
    rad_count++;
}