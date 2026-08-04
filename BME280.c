// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// BMP280
// This code is designed to work with the BMP280_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Barometer?sku=BMP280_I2CSs#tabs-0-product_tabset-2

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <strings.h>
#include <unistd.h>

int day;
int hour;
int min;
int sec;
int bmeConfig(int file);
int getTempCal(int fd1, int *pCal);
int getPresCal(int fd1, int *pCal);
int getHumCal(int fd1, int *pCal);
float compensate_humidity(int32_t adc_H);
int pTempCal[3];
int pPresCal[9];
int pHumCal[6];
int read_print(int file, char *fstring);
u_int16_t dig_H1, dig_H3;
int16_t dig_H2, dig_H4, dig_H5, dig_H6;
int32_t g_t_fine; // Calculated during temperature compensation

void main() 
{
    // Create I2C bus
    int file;
    char *bus = "/dev/i2c-3";
    if((file = open(bus, O_RDWR)) < 0) 
    {
        printf("Failed to open the bus. \n");
        exit(1);
    }
    time_t ct = time(NULL);
    struct tm tm = *localtime(&ct);
    int day = tm.tm_mday;
    int hour = tm.tm_hour;
    int min = tm.tm_min;
    int sec = tm.tm_sec;
    char fstring[128];
    sprintf(fstring,"/home/rock/t_data_%d_%d_%d_%d",day,hour,min,sec);
    ioctl(file,I2C_SLAVE,0x76);
    getTempCal(file,pTempCal); 
    getPresCal(file,pPresCal); 
    getHumCal(file,pHumCal); 
    while(1)
    {
        read_print(file, fstring);
        sleep(1);
    }
}
int read_print(int file, char *fstring)
{

    FILE *fp1;
    FILE *fpLowTemp;
    FILE *fpHighTemp;
    float fLowTemp = 0;
    float fHighTemp = 0;
    char sLowTemp[32];
    char sHighTemp[32];
    char *pEnd;
    char data[24];

#ifdef NOT
    fpLowTemp = fopen("/home/rock/LowTemp","r+");
    fpHighTemp = fopen("/home/rock/HighTemp","r+");
    if (fpLowTemp)
    {
        fgets(sLowTemp,32,fpLowTemp);
    fLowTemp = strtof(sLowTemp,&pEnd);
    printf("Low temp is %2.2f\n", fLowTemp);
    }
    if (fpHighTemp)
    {
        fgets(sHighTemp,32,fpHighTemp);
    fHighTemp = strtof(sHighTemp,&pEnd);
    printf("High temp is %2.2f\n", fHighTemp);
    }
    fclose(fpLowTemp);
    fclose(fpHighTemp);

#endif // NOT
    fp1 = fopen(fstring,"a+");
    time_t ct = time(NULL);
    int epoch_time = ct;

    bmeConfig(file);
    
    // Read 8 bytes of data from register(0xF7)
    // pressure msb1, pressure msb, pressure lsb, temp msb1, temp msb, temp lsb, humidity lsb, humidity msb
    //
    char reg[1];
    reg[0] = 0xF7;
    write(file, reg, 1);
    if(read(file, data, 8) != 8)
    {
        printf("Error : Input/output Error \n");
        exit(1);
    }
    int nRawHum = (data[6] << 8) | data[7];
    float nHumPercent = compensate_humidity(nRawHum);
//    printf("humidty percent  = %2.2f\n",nHumPercent);   
    // Convert pressure and temperature data to 19-bits
    long adc_p = (((long)data[0] * 65536) + ((long)data[1] * 256) + (long)(data[2] & 0xF0)) / 16;
    long adc_t = (((long)data[3] * 65536) + ((long)data[4] * 256) + (long)(data[5] & 0xF0)) / 16;
        
    // Temperature offset calculations
    double var1 = (((double)adc_t) / 16384.0 - ((double)pTempCal[0]) / 1024.0) * ((double)pTempCal[1]);
    double var2 = ((((double)adc_t) / 131072.0 - ((double)pTempCal[0]) / 8192.0) *(((double)adc_t)/131072.0 - ((double)pTempCal[0])/8192.0)) * ((double)pTempCal[2]);
    g_t_fine = (int32_t)(var1 + var2);
    double t_fine = (long)(var1 + var2);
    double cTemp = (var1 + var2) / 5120.0;
    double fTemp = cTemp * 1.8 + 32;
        
    // Pressure offset calculations
    var1 = ((double)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)pPresCal[6]) / 32768.0;
    var2 = var2 + var1 * ((double)pPresCal[5]) * 2.0;
    var2 = (var2 / 4.0) + (((double)pPresCal[4]) * 65536.0);
    var1 = (((double) pPresCal[3]) * var1 * var1 / 524288.0 + ((double) pPresCal[2]) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)pPresCal[1]);
    double p = 1048576.0 - (double)adc_p;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double) pPresCal[9]) * p * p / 2147483648.0;
    var2 = p * ((double) pPresCal[8]) / 32768.0;
    double pressure = (p + (var1 + var2 + ((double)pPresCal[7])) / 16.0) / 100;
    
    // Output data to screen
    fprintf(fp1," %.2f hPa %.2f deg C %.2f hum %d\n", pressure, fTemp,nHumPercent, epoch_time);
    printf(" %.2f hPa %.2f deg C %.2f hum %d\n", pressure, fTemp,nHumPercent, epoch_time);
#ifdef NOT
    if (fTemp < fLowTemp)
    {
        printf("New low temp %2.2f\n", fTemp);
                fpLowTemp = fopen("/home/rock/LowTemp","w+");
        sprintf(sLowTemp,"%2.2f\n",fTemp);
        fputs(sLowTemp,fpLowTemp);
        fclose(fpLowTemp);

    }
    if (fTemp > fHighTemp)
    {
        printf("New High temp %2.2f\n", fTemp);
                fpHighTemp = fopen("/home/rock/HighTemp","w+");
        sprintf(sHighTemp,"%2.2f\n",fTemp);
        fputs(sHighTemp,fpHighTemp);
        fclose(fpHighTemp);
    }
#endif // NOT
    fclose(fp1);
    return file;
}
int getTempCal(int file, int *pCal)
{
    char reg[1] = {0x88};
    write(file, reg, 1);
    char data[24];
    bzero(data,24);
    read(file, data, 24);
    pCal[0] = data[1] * 256 + data[0];
    pCal[1] = data[3] * 256 + data[2];
    if(pCal[1] > 32767)
    {
        pCal[1] -= 65536;
    }
    pCal[2] = data[5] * 256 + data[4];
    if(pCal[2] > 32767)
    {
        pCal[2] -= 65536;
    }
    return 0;
}
int getPresCal(int file, int *pCal)
{
    char reg[1] = {0x88};
    write(file, reg, 1);
    char data[24];
    bzero(data,24);
    read(file, data, 24);
    pCal[1] = data[7] * 256 + data[6];
    pCal[2]  = data[9] * 256 + data[8];
    if(pCal[2] > 32767)
    {
        pCal[2] -= 65536;
    }
    pCal[3] = data[11]* 256 + data[10];
    if(pCal[3] > 32767)
    {
        pCal[3] -= 65536;
    }
    pCal[4] = data[13]* 256 + data[12];
    if(pCal[4] > 32767)
    {
        pCal[4] -= 65536;
    }
    pCal[5] = data[15]* 256 + data[14];
    if(pCal[5] > 32767)
    {
        pCal[5] -= 65536;
    }
    pCal[6] = data[17]* 256 + data[16];
    if(pCal[6] > 32767)
    {
        pCal[6] -= 65536;
    }
    pCal[7] = data[19]* 256 + data[18];
    if(pCal[7] > 32767)
    {
        pCal[7] -= 65536;
    }
    pCal[8] = data[21]* 256 + data[20];
    if(pCal[8] > 32767)
    {
        pCal[8] -= 65536;
    }
    pCal[9] = data[23]* 256 + data[22];
    if(pCal[9] > 32767)
    {
        pCal[9] -= 65536;
    }
    return 0;
}
int getHumCal(int file, int *pCal)
{    
    short d1[1];
    short d2[1];

    char reg[1] = {0xA1};
    write(file, reg, 1);
    char data[8];
    bzero(data,8);
    read(file, data, 1);
    dig_H1 = (u_char)(data[0]);
    char reg2[1] = {0xE1};
    write(file, reg2, 1);
    read(file, data, 6);

    dig_H2  = ((data[1] << 8) | (data[0]));
    dig_H3 = (u_char)(data[2]);
    dig_H4 = (data[3] << 4) | (data[4] & 0xf);
    dig_H4 = 300;
//    dig_H5 = 50;
    dig_H5 = (data[5] << 4) | (data[4] >> 4);
//    dig_H6 = 30;
    dig_H6 = (char)(data[6]);
    return 0;
}
int bmeConfig(int file)
{

    int nChipId = 0;

    char pReg[1] = {0xD0};
    char config[2] = {0};
    write(file, pReg, 1);
    char pId[1] = {0};
    read(file,pId,1);
    printf("chip id is %x\n",pId[0]);
    nChipId = (int)pId[0];
        
    // Select config register(0xF2)
    // set humidity oversampling = 1, = 001 (0x1)
    config[0] = 0xF2;
    config[1] = 0x1;
    write(file, config, 2);
    sleep(1);
    // Select control measurement register(0xF4)
    // Normal mode, temp and pressure over sampling rate = 1(0x27)
    config[0] = 0xF4;
    config[1] = 0x27;
    write(file, config, 2);
    
    // Select config register(0xF5)
    // Stand_by time = 1000 ms(0xA0)
    config[0] = 0xF5;
    config[1] = 0xA0;
    write(file, config, 2);
    sleep(1);

    return nChipId;
}
int16_t dig_H2, dig_H4, dig_H5, dig_H6;
// Example compensation function based on BME280 datasheet

float compensate_humidity(int32_t adc_H) {
    int32_t v_x1_u32r;
    v_x1_u32r = (g_t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H6) * v_x1_u32r)) +
        ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) *
        (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
        ((int32_t)dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    float h1 =  (float)(v_x1_u32r >> 12); // Result in %RH * 1024
    return (h1 / 1024.0);
}

