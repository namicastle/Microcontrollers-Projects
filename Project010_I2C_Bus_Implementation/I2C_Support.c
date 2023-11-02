#include <stdio.h>
#include <p18f4620.h>
#include "I2C_Support.h"
#include "I2C.h"

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02
#define ACK             1
#define NAK             0

extern unsigned char second, minute, hour, dow, day, month, year;
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
extern unsigned char alarm_second, alarm_minute, alarm_hour, alarm_date;
extern unsigned char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;

int DS1621_Read_Temp()                                                          // READ TEMPERATURE FUNCTION
{
  char Data_Ret;                                                                // constant declarations
  char Device = 0x48;                                                           // initialize Device with I2C address (0x48)
  char Cmd = READ_TEMP;                                                         // set Cmd equal to READ_TEMP
  I2C_Start();                                                                  // Start I2C protocol
  I2C_Write((Device << 1) | 0);                                                 // Device address
  I2C_Write(Cmd);                                                               // Send register address
  I2C_ReStart();                                                                // Restart I2C
  I2C_Write((Device << 1) | 1);                                                 // Initialize data read
  Data_Ret = I2C_Read(NAK);                                                     // 
  I2C_Stop();                                                                   // 
  return Data_Ret;                                                              // return the value of temperature from readout
}
void DS1621_Init()
{
    char Device = 0x48;                                                         // initialize Device with I2C address (0x48)
    I2C_Write_Cmd_Write_Data(Device, ACCESS_CFG, CONT_CONV); 
    I2C_Write_Cmd_Only(Device, START_CONV);
}
void DS3231_Read_Time()
{
    char Device = 0x68;                                                         // initialize Device with I2C address (0x68)
    char Address = 0x00;                                                        // initialize Address with an address (0x00) pointing to register 'second'
    char Data_Ret;                                                              // declare Data_Ret
    
    I2C_Start();                                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                                               // DS3231 address Write mode
    I2C_Write(Address);                                                         // Send register address
    I2C_ReStart();                                                              // Restart I2C
    I2C_Write((Device << 1) | 1);                                               // Initialize data read
    second = I2C_Read(ACK);                                                     // read register 'second' from DS3231 device
    minute = I2C_Read(ACK);                                                     // read register 'minute' from DS3231 device
    hour = I2C_Read(ACK);                                                       // read register 'hour' from DS3231 device
    dow = I2C_Read(ACK);                                                        // read register 'dow' from DS3231 device
    day = I2C_Read(ACK);                                                        // read register 'day' from DS3231 device
    month = I2C_Read(ACK);                                                      // read register 'month' from DS3231 device
    year = I2C_Read(NAK);                                                       // read register 'year' from DS3231 device
    I2C_Stop();                                                                 // end the function
}
void DS3231_Setup_Time()
{
    char Device = 0x68;                                                         // initialize Device with I2C address (0x68)
    char Address = 0x00;                                                        // initialize Address with an address (0x00) pointing to register 'second'

    second = 0x00;                                                              // second starts at 00 sec
    minute = 0x18;                                                              // minute at 18
    hour = 0x03;                                                                // hour at 3
    dow = 0x05;                                                                 // day of week at 5 (Thursday)
    day = 0x20;                                                                 // day at 20
    month = 0x04;                                                               // month at 4 because April
    year = 0x23;                                                                // Start I2C protocol
    I2C_Start();                                                                //
    I2C_Write((Device << 1) | 0);                                               // Device address Write mode
    I2C_Write(Address);                                                         // Send register address
    I2C_Write(second);                                                          // Send register second
    I2C_Write(minute);                                                          // Send register minute
    I2C_Write(hour);                                                            // Send register hour
    I2C_Write(dow);                                                             // Send register dow
    I2C_Write(day);                                                             // Send register day
    I2C_Write(month);                                                           // Send register month
    I2C_Write(year);                                                            // Send register year
    I2C_Stop();                                                                 // stop function
}


