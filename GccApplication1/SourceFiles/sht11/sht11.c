/*
 * sht11.c
 *
 * Created: 17/06/2015 9:36:51
 *  Author: Diego
 */ 


#include "../sht11/sht11.h"
#include <util/delay.h>


/**
 * Generates a transmission start
 *       _____         ________
 * DATA:      |_______|
 *           ___     ___
 * SCK : ___|   |___|   |______
 */
static void Sht11Start(void);

/* Reads a byte form the Sensibus and gives an acknowledge in case of "ack=1" */
static uint8_t Sht11Read(uint8_t ack);

/* Writes a byte on the Sensibus and checks the acknowledge */
static uint8_t Sht11Write(uint8_t value);


/* Makes a measurement (humidity/temperature) with checksum */
static uint8_t Sht11MeasureParam(uint16_t *p_value, uint8_t *p_checksum, uint8_t mode);

void Sht11Init() {
	//ioport_init();  TODO: mirar si hace falta esto.
	
	//TODO: comprobar si hace falta configurar 
	// el pin como pin digital.	
    //SHT_DATA_CNF = 1; 
    //SHT_SCK_CNF = 1;
	ioport_set_pin_dir(SHT_SCK,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(SHT_DATA,IOPORT_DIR_OUTPUT);
    //SHT_SCK_DDR = 0;
    //SHT_DATA_DDR = 0;
	ioport_set_pin_level(SHT_DATA,IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_LOW);
	//SHT_DATA = 1;
    //SHT_SCK = 0;
}

uint8_t Sht11Measure(ShtData* data) {
    uint8_t error = 0;
    // Get measures
    error += Sht11MeasureParam(&data->temperature.i, &data->tempChk,
            SHT_MEASURE_TEMP);
    error += Sht11MeasureParam(&data->humidity.i, &data->humiChk,
            SHT_MEASURE_HUMI);
    return error;
}

static uint8_t Sht11Write(uint8_t value) {
    uint8_t i, error = 0;
    //shift bit for masking
    for (i = 0x80; i > 0; i /= 2) {
        if (i & value) {
            ioport_set_pin_level(SHT_DATA,IOPORT_PIN_LEVEL_HIGH); //masking value with i , write to SENSI-BUS
        } else {
            ioport_set_pin_level(SHT_DATA,IOPORT_PIN_LEVEL_LOW);
        }
        _delay_us(5); //observe setup time
        ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_HIGH); //clk for SENSI-BUS
        _delay_us(5); //pulswith approx. 5 us
        ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_LOW);
        _delay_us(5); //observe hold time
    }
    ioport_set_pin_level(SHT_DATA,IOPORT_PIN_LEVEL_HIGH); //release SHT_DATA-line
    _delay_us(5); //observe setup time
    ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_HIGH);  //clk #9 for ack
    _delay_us(5);
    // To read SHT ack(low), change pin direction
    ioport_set_pin_dir(SHT_DATA,IOPORT_DIR_INPUT);
    // Read pin
    error = ioport_get_pin_level (SHT_DATA); //check ack (SHT_DATA will be pulled down by SHT11)
    ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_LOW);
    return error; //error=1 in case of no acknowledge
}

static uint8_t Sht11Read(uint8_t ack) {
    uint8_t i, val = 0;
    ioport_set_pin_dir(SHT_DATA,IOPORT_DIR_INPUT);
	//SHT_DATA_DDR = 1;
    //TODO: si es de entrada porq lo escribo
	ioport_set_pin_level(SHT_DATA,IOPORT_PIN_LEVEL_HIGH);
	//SHT_DATA = 1;
    for (i = 0x80; i > 0; i /= 2) //shift bit for masking
    {
        _delay_us(5);
         ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_HIGH);//clk for SENSI-BUS
        if (ioport_get_pin_level(SHT_DATA)==1) {
            val = (val | i); //read bit
        }
        _delay_us(5);
        ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_LOW);
    }

    if (ack) {
        ioport_set_pin_dir(SHT_DATA,IOPORT_DIR_OUTPUT);
        ioport_set_pin_level(SHT_DATA,IOPORT_PIN_LEVEL_LOW); //in case of "ack==1" pull down SHT_DATA-Line
    }
    _delay_us(5); //observe setup time
     ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_HIGH); //clk #9 for ack
    _delay_us(5);//pulswith approx. 5 us
    ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_LOW);
    _delay_us(5); //observe hold time
    return val;
}

//----------------------------------------------------------------------------------

static void Sht11Start(void) {

	   ioport_set_pin_level(SHT_DATA,IOPORT_PIN_LEVEL_HIGH);
	   ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_LOW); //Initial state
	   _delay_us(5);
	   ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_HIGH);
	   _delay_us(5);
	   ioport_set_pin_level(SHT_DATA,IOPORT_PIN_LEVEL_LOW);
	   _delay_us(5);
	   ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_LOW);
	   _delay_us(5);
	   ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_HIGH);
	   _delay_us(5);
	   ioport_set_pin_level(SHT_DATA,IOPORT_PIN_LEVEL_HIGH);
	   _delay_us(5);
	   ioport_set_pin_level(SHT_SCK,IOPORT_PIN_LEVEL_LOW);

}

static uint8_t Sht11MeasureParam(uint16_t *p_value, uint8_t *p_checksum, uint8_t mode) {
    uint8_t error = 0;
    ioport_set_pin_dir(SHT_DATA,IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(SHT_DATA,IOPORT_PIN_LEVEL_HIGH);
    Sht11Start(); //transmission start
    switch (mode) { //send command to sensor
        case SHT_MEASURE_TEMP: error += Sht11Write(SHT_MEASURE_TEMP);
            break;
        case SHT_MEASURE_HUMI: error += Sht11Write(SHT_MEASURE_HUMI);
            break;
        default: break;
    }
    _delay_ms(1); // TODO: comprobar este delay.
    ioport_set_pin_dir(SHT_DATA,IOPORT_DIR_INPUT);
    while (ioport_get_pin_level(SHT_DATA) == 1);
    // Read two bytes response
    *(p_value) = Sht11Read(SHT_ACK); //read the first byte (MSB)
    *(p_value) = *(p_value) << 8;
    *(p_value) += Sht11Read(SHT_ACK); //read the second byte (LSB)
    *p_checksum = Sht11Read(SHT_NACK); //read checksum

    return error;
}
