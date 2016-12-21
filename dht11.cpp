/*
 *  dht11.c:
 *  Simple test program to test the wiringPi functions
 *  DHT11 test
 *  Original obtained from http://www.uugear.com/portfolio/dht11-humidity-temperature-sensor-module/
 *  Compile with:   gcc dht11.cpp -lwiringPi -o dht11
 *  TODO: automatic pin checking....
 */

#include <wiringPi.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#define MAXTIMINGS  85
#define DHTPIN_DEFAULT    15

int DHTPIN = DHTPIN_DEFAULT;
int dht11_dat[5] = { 0, 0, 0, 0, 0 };
typedef struct Command_line_Arguments {
    int loop;
    int no_humidity;
    int no_temp;
    int welcome_to_1724;
    int reads;
    int simple;
    int wait;
    int help;
    int pin;
} Command_line_Arguments;

typedef struct DHT_Reading {
  int temp1;
  int temp2;
  int hum1;
  int hum2;
  int error;
} DHT_Reading;


DHT_Reading read_dht11_dat( Command_line_Arguments cmd_arg)
{
  uint8_t laststate  = HIGH;
  uint8_t counter    = 0;
  uint8_t j    = 0, i;
  float  f; /* fahrenheit */
  
  if (cmd_arg.pin != 0){
    DHTPIN = cmd_arg.pin;
  }

  dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

  /* pull pin down for 18 milliseconds */
  pinMode( DHTPIN, OUTPUT );
  digitalWrite( DHTPIN, LOW );
  delay( 18 );
  /* then pull it up for 40 microseconds */
  digitalWrite( DHTPIN, HIGH );
  delayMicroseconds( 40 );
  /* prepare to read the pin */
  pinMode( DHTPIN, INPUT );

  /* detect change and read data */
  for ( i = 0; i < MAXTIMINGS; i++ )
  {
    counter = 0;
    while ( digitalRead( DHTPIN ) == laststate )
    {
      counter++;
      delayMicroseconds( 1 );
      if ( counter == 255 )
      {
        break;
      }
    }
    laststate = digitalRead( DHTPIN );

    if ( counter == 255 )
      break;

    /* ignore first 3 transitions */
    if ( (i >= 4) && (i % 2 == 0) )
    {
      /* shove each bit into the storage bytes */
      dht11_dat[j / 8] <<= 1;
      if ( counter > 16 )
        dht11_dat[j / 8] |= 1;
      j++;
    }
  }

  /*
   * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
   * print it out if data is good
   */
  DHT_Reading dhtr = { 0,0,0,0,1 };

  if ( (j >= 40) &&
       (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
  {
    f = dht11_dat[2] * 9. / 5. + 32;
    printf( "Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n",
      dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f );
    dhtr.temp1 = dht11_dat[0];
    dhtr.temp2 = dht11_dat[1];
    dhtr.hum1  = dht11_dat[2];
    dhtr.hum2  = dht11_dat[3];
    dhtr.error = 0;
  }else  {
    printf( "Data not good, skip\n" );
  }
  return dhtr;
}


void print_usage(){
  printf("Usage: dht11 [OPTION...]\n");
  printf("\n");
  printf("Without options the values are outputted once in a human readable format. \n");
  printf("\n");
  printf("  -l, --loop             keep reading the dht11 and outputting the results\n");
  printf("  -h, --no-humidity      do not output the humidity given by the sensor\n");
  printf("  -t  --no-temperature   do not output the temperature given by the sensor\n");
  printf("  -f                     show temperature in Fahrenheit instead of Celcius\n");
  printf("  -r, --reads=NUMBER     number of reads from the sensor before outputting\n");
  printf("                         This makes the reading more reliable. Standard amount is 3\n");
  printf("  -s, --simple           output the sensor readings in [TEMPERATURE] [HUMIDITY] format\n");
  printf("  -w  --wait=NUMBER      time to wait between readings in seconds. Only useful with --loop\n");
  printf("  -p, --pin              the signal pin to output and read from. This program uses \n");
  printf("                         the WiringPi pin numbering. If you have WiringPi installed,\n");
  printf("                         run 'gpio readall' to check your pin layout.\n");
  printf("                         The default pin is %d (WiringPi numbering).\n", DHTPIN);
  printf("  --help                 print this description and exit\n");
}


int main( int argc, char *argv[] ) {
  //printf( "Raspberry Pi wiringPi DHT11 Temperature test program\n" );

  static  int loop;
  static  Command_line_Arguments cmd_arg = {0,0,0,0,0,0,0,0};
  int opt = 0;
  int long_index =0;

  static struct option long_options[] = {
    {"loop",           no_argument,       0, 'l' },
    {"no-humidity",    no_argument,       0, 'h' },
    {"no-temperature", no_argument,       0, 't' },
    {"welcome-to-1724", no_argument,      0, 'f' },
    {"reads",          required_argument, 0, 'r' },
    {"simple",         no_argument,       0, 's' },
    {"help",           no_argument,       &cmd_arg.help, 1},
    {"wait",           required_argument, 0, 'w' },
    {"pin",            required_argument, 0, 'p' },
    {0,                0,                 0,  0  }
  };

  while ((opt = getopt_long(argc, argv,"lhtfr:sw:p:", long_options, &long_index )) != -1) {
    switch (opt) {
      case 0:
        if (long_options[long_index].flag != 0) {
          break;
        }
      case 'l': cmd_arg.loop = 1;             break;
      case 'h': cmd_arg.no_humidity = 1;      break;
      case 't': cmd_arg.no_temp = 1;          break;
      case 'f': cmd_arg.welcome_to_1724 = 1;  break;
      case 'r': cmd_arg.reads = atoi(optarg); break;
      case 's': cmd_arg.simple = 1;           break;
      case 'w': cmd_arg.wait = atoi(optarg);  break;
      case 'p': cmd_arg.pin =  atoi(optarg);  break;

      default: print_usage(); 
         exit(1);
    }
  }

  if (cmd_arg.help != 0){
      print_usage();
      exit(0);
  }

  if (cmd_arg.wait < 1 || cmd_arg.wait >= INT_MAX){
    cmd_arg.wait = 1;
  }
  if (cmd_arg.reads > 100 || cmd_arg.reads < 1){
    cmd_arg.reads = 3;
  }

  printf("loop: %d, no-hum: %d  no-temp: %d\n",  cmd_arg.loop, cmd_arg.no_humidity, cmd_arg.no_temp);
  printf("fah:  %d, reads:  %d  simple: %d\n",  cmd_arg.welcome_to_1724, cmd_arg.reads, cmd_arg.simple);
  printf("help: %d, wait:  %d  \n",  cmd_arg.help, cmd_arg.wait);

  if ( wiringPiSetup() == -1 )
    exit( 1 );

  do {
    DHT_Reading dhtr = read_dht11_dat(cmd_arg);
    printf("Humidity = %d.%d %% Temperature = %d.%d *C     Error: %d \n", dhtr.temp1, dhtr.temp2, dhtr.hum1, dhtr.hum2, dhtr.error);
    delay( cmd_arg.wait * 1000 );
  } while ( cmd_arg.loop );

  return(0);
}
