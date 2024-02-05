#include "Wire.h"
#include "SPI.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BMP3XX.h"

// Precisa verificar os pinos corretos do módulo em uso
#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

// Definindo portas de saida do sinal para valvulas
#define VAL_REAGENTE 1
#define VAL_INIBIDOR 1

#define TEMPO_DE_REACAO 10000

// Precisa testar pra ver se o ESP32 tem hardware SPI, eu desconfio que sim
//Adafruit_BMP3XX bmp; // I2C
Adafruit_BMP3XX bmp(BMP_CS); // hardware SPI
//Adafruit_BMP3XX bmp(BMP_CS, BMP_MOSI, BMP_MISO, BMP_SCK);  // Software SPI

int altitude[2];
int sea_level_pressure;
int velocidade[2];
int aceleracao;
bool is_done;

void setup () {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Adafruit BMP388 / BMP390 test");

    //if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
    //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
        Serial.println("Could not find a valid BMP3 sensor, check wiring!");
        while (1);
    }

    // Set up oversampling and filter initialization
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);

    // Inicializando saidas PWM
    pinMode(VAL_REAGENTE, OUTPUT);
    pinMode(VAL_INIBIDOR, OUTPUT);

    // Inicializando altimetro
    if (!bmp.performReading()) {
        Serial.println("Failed to perform reading :(");
        return;
    }
    sea_level_pressure = bmp.pressure / 100;
    altitude[0] = bmp.readAltitude(sea_level_pressure);
    altitude[1] = bmp.readAltitude(sea_level_pressure);
    velocidade[0] = 0;
    velocidade[1] = 0;
    aceleracao = 0;

    is_done = false;
}

void loop () {
    altitude[0] = altitude[1];
    altitude[1] = bmp.readAltitude(sea_level_pressure);
    
    velocidade[0] = velocidade[1];
    velocidade[1] = altitude[1] - altitude[0];

    aceleracao = velocidade[1] - velocidade[0];

    // Modo standby
    if (is_done || altitude[1] < 50) {
        delay(100);
        return;
    }

    // Estado de microgravidade
    if (aceleracao <= 0) {
        is_done = true;
        // Abrir valvula do reagente
        digitalWrite(VAL_REAGENTE, HIGH);
        // Esperar tempo de reacao
        delay(TEMPO_DE_REACAO);
        // Abrir valvula do inibidor
        digitalWrite(VAL_INIBIDOR, HIGH);
    }
}
