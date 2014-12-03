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
int spacing = rad_div/8;
int sample_size = 200;
int degree_delay_us = 0;

bool rpm_check = true;

void update_display(int);
void store_data(int, int, int);
void rad_increment();

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
            store_data(SQUARE2[l],SQUARE1[l] + rad_div*j/4,2);
            store_data(SQUARE2[l],SQUARE1[l] + rad_div*j/4,10);
        }
        store_data(16,30*j,3);
        store_data(16,30*j,4);
        store_data(16,30*j,5);
        store_data(16,30*j,6);
        store_data(16,30*j,7);
        store_data(16,30*j,8);
        store_data(16,30*j,9);
    }

    int begin, end, overhead;
    timer.start();
    begin = timer.read_us();

    for(j = 0; j < sample_size; j++) {
        for(degree = 0; degree < rad_div; degree++) {
            update_display(degree);
            wait_us(degree_delay_us);
        }
    }

    end = timer.read_us();
    overhead = end - begin;

    wait(20);

    timer.reset();
    RPM_LED = 1;
    RPM_FOTO = 1;
    begin = timer.read_us();
    RPM.fall(&rad_increment);
    while(rad_count < sample_size);
    end = timer.read_us();
    RPM.fall(NULL);
    RPM_LED = 0;
    RPM_FOTO = 0;
    degree_delay_us = (end - begin - overhead)/rad_div/sample_size + 7;


    


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

void store_data(int num, int degree, int tier)
{
    data_array[(degree + spacing*(tier%4))%rad_div][tier] = num;
}

void rad_increment()
{
    rad_count++;
}