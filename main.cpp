#include "mbed.h"
#include "Adafruit_SSD1306.h"
#include "ds3231.h"
#include "HTU21D.h"

//Declaracion Varialbes
float sample_ctemp;
float sample_humid;
int xz, yz;
float x=15.45, y=26.97;
//Declaracion I2C
I2C i2c(D14,D15);
Ds3231 rtc(D14, D15);

//Declaracion Pantalla Oled
Adafruit_SSD1306_I2c myOled(i2c, D13,0x78,64,128);
HTU21D temphumid(D14, D15); //Temp humid sensor || SDA, SCL

//Prototipos de las funciones
void led1_thread();
void led2_thread();
void led3_thread();

//Declaracion de hilos o tareas
Thread thread1(osPriorityNormal1, 2*OS_STACK_SIZE, NULL, "Reloj");
Thread thread2(osPriorityNormal2, 2*OS_STACK_SIZE, NULL, "Publicar");

//Declaracion de puertos
BufferedSerial pc(USBTX,USBRX);
DigitalOut  led1(PA_3);
DigitalOut  led2(PA_4);
DigitalOut  led_vegeta(PB_12);
DigitalOut  led_flora(PB_13);
DigitalOut  led_ventila(PB_14);

//Declaracion tiempo
time_t epoch_time;  // declare variable for absolute time (in seconds) from Jan 1, 1970 12mn
ds3231_time_t timeKeep; // declare time variable
ds3231_calendar_t dateKeep; // declare calendar variable
char buffer[32];
char bufferReloj[32];

//Definicion Funciones
void interfaz();
void reloj();
void publicar();
void fotoperiodo();

int intDec(float z);
// main() runs in its own thread in the OS
int main()
{   
    led_flora = true;
    led_vegeta = true;
    led_ventila = true;
    ThisThread::sleep_for(2s);
    set_time(rtc.get_epoch());
    myOled.clearDisplay();
    myOled.display();
    //Instanciar hilos o tareas
    thread1.start(reloj);
    thread2.start(publicar);
    interfaz();
}

void interfaz(){
    myOled.clearDisplay();
    //myOled.setTextSize(1);
    //hora
    //myOled.setTextCursor(0,0);
    //myOled.printf("12:00:00 AM\r");
    myOled.drawFastHLine(0,9,128, WHITE);
    myOled.drawRect(5, 16, 29, 21, WHITE);
    myOled.setTextCursor(9,18);
    myOled.setTextSize(2);
    myOled.printf("T1");
    myOled.setTextCursor(40,18);
    myOled.printf("00.00 ");
    myOled.setTextSize(1);
    myOled.printf("o");
    myOled.setTextSize(2);
    myOled.printf("C");
    //humedad
    myOled.drawRect(5, 35, 29, 21, WHITE);
    myOled.setTextCursor(9,39);
    myOled.setTextSize(2);
    myOled.printf("H1");
    myOled.setTextCursor(40,39);
    myOled.printf("00.00 %%");
    myOled.display();
}
void ventilador() {
    if (timeKeep.minutes > 30) {
        led_ventila = false;
    } else {
        led_ventila = true;
    }
}

void fotoperiodo() {
    if (timeKeep.hours>=6 && timeKeep.hours<=18) {
        led_vegeta = false;
        led_flora = false;
        ventilador();
    } else if (timeKeep.hours>18) {
        led_vegeta = false;
        led_flora = true;
        ventilador();
    } else {
        led_vegeta = true;
        led_flora = true;
        led_ventila = true;
    }

}


void reloj() {
    while (1) {
        led1 = !led1;
        //time_t seconds = time(NULL);
        //strftime(bufferReloj, 32, "%I:%M:%S %p       BBT", localtime(&seconds));
        myOled.setTextCursor(0,0);
        myOled.setTextSize(1);
        rtc.get_time(&timeKeep);
        myOled.printf("%02u:%02u:%02u          BBT",timeKeep.hours,timeKeep.minutes,timeKeep.seconds);
        //myOled.printf("%s", bufferReloj);
        //myOled.printf("  %i:%i", hora, minuto);
        myOled.setTextSize(2);
        myOled.display();
        fotoperiodo();
        ThisThread::sleep_for(1s);
        
    }
}

void publicar(){
    while(1){
        char sfloat[80];
        myOled.setTextCursor(9,18);
        myOled.setTextSize(2);
        myOled.printf("T1");
        myOled.setTextCursor(40,18);
        myOled.printf("%d.%d ", int(x), intDec(x));    
        myOled.setTextSize(2);
        myOled.setTextCursor(9,39);
        myOled.printf("H1");
        myOled.setTextCursor(40,39);
        //sprintf(sfloat, "%f\n", 3.1522);
        //printf("%s", sfloat);
        myOled.printf("%d.%d %%", int(y), intDec(y));
        myOled.setTextCursor(9,17);
        myOled.setTextSize(2);
        myOled.display();
        ThisThread::sleep_for(1s);
        myOled.setTextCursor(9,18);
        myOled.printf("T2");
        myOled.setTextCursor(40,18);
        myOled.printf("%d.%d ", int(y), intDec(y));
        myOled.setTextCursor(9,39);
        myOled.printf("H2");
        myOled.setTextCursor(40,39);
        myOled.printf("%d.%d %%", int(x), intDec(x));
        myOled.display();
        ThisThread::sleep_for(1s);     
    }
}


int intDec(float z) {
    int x1 = z*100;
    int xz = z;
    int yz = x1%100;
    return yz;
}




