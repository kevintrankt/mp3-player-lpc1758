#include <stdio.h>
#include "FreeRTOSConfig.h"
#include "tasks.hpp"
#include "utilities.h"
#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "LabGPIO_0.hpp"
#include "printf_lib.h"
#include "vs1053.hpp"
#include "LabGPIOInterrupts.hpp"
#include "adc0.h"
#include "lcdDriver.hpp"
#include "semphr.h"

SemaphoreHandle_t spi_bus_lock; // Semaphore to protect SPI transfer
VS1053 mp3; // VS1053 Driver
lcdDriver lcd; // Parralax 16x2 Driver
int elapsed;
char * song_title;
SoftTimer debouncer(200);

void pauseToggleISR()
/*-------------------------------------------------------------------------|
| -- pauseToggleISR()                                                      |
|--------------------------------------------------------------------------|
| When interrupt on P0.1 is detected, this ISR pauses or resumes playback  |
|-------------------------------------------------------------------------*/
{
  if(mp3.isPlaying()){
    mp3.pause();
  } else {
    mp3.play();
  }
}

void skipSongISR(){
/*-------------------------------------------------------------------------|
| -- skipSongISR()                                                         |
|--------------------------------------------------------------------------|
| When interrupt on P2.4 is detected, this ISR skips to the next song      |
|-------------------------------------------------------------------------*/
  mp3.skipSong();
}

// void prevSongISR(){
// /*-------------------------------------------------------------------------|
// | -- prevSongISR()                                                         |
// |--------------------------------------------------------------------------|
// | When interrupt on P2.7 is detected, this ISR plays the previous song     |
// |-------------------------------------------------------------------------*/
//   mp3.prevSong();
// }

void prevSongISR(void * pvParameters)
/*-------------------------------------------------------------------------|
| -- prevSongISR()                                                         |
|--------------------------------------------------------------------------|
| When interrupt on P2.7 is detected, this ISR plays the previous song     |
|-------------------------------------------------------------------------*/
{
  LabGPIO_0 prev(2, 2);
  prev.setAsInput();
  while(1){
    if(debouncer.expired())
    {
      if(prev.getLevel()){
        mp3.prevSong();
        debouncer.reset();
      }
    }
  vTaskDelay(100);
  }
}

void shuffleISR(void * pvParameters){
/*-------------------------------------------------------------------------|
| -- shuffleISR()                                                          |
|--------------------------------------------------------------------------|
| When interrupt on P2.6 is detected, this ISR toggles shuffle mode        |
|-------------------------------------------------------------------------*/
  mp3.toggleShuffle();
  LabGPIO_0 shuf(2, 6);
  shuf.setAsInput();
  while(1){
    if(debouncer.expired())
    {
      if(shuf.getLevel()){
        mp3.toggleShuffle();
        debouncer.reset();
      }
    }
  vTaskDelay(100);
  }
}

void setVolume(void * pvParameters)
/*-------------------------------------------------------------------------|
| -- setVolume(void * pvParameters)                                        |
|--------------------------------------------------------------------------|
| This task reads the value from the ADC channel 5 and converts it to an 8 |
| bit integer. The converted value is then written to the SCI register to  |
| change the volume. The task uses a mutex to protect the SPI bus.        |
|--------------------------------------------------------------------------|
| @param:                                                                  |
|    {void *} pvParameters - Parameters passed to task                     |
|-------------------------------------------------------------------------*/
{
  while(1)
  {
    if(xSemaphoreTake(spi_bus_lock, portMAX_DELAY)) {
      uint16_t reading = 0;
      reading = adc0_get_reading(5);
      uint8_t volume = reading * 254.0 / 4095;
      mp3.setVolume(volume,volume);
      xSemaphoreGive(spi_bus_lock);
    }
    vTaskDelay(500);
  }
}

void playSong(void * pvParameters)
/*-------------------------------------------------------------------------|
| -- playSong(void * pvParameters)                                         |
|--------------------------------------------------------------------------|
| This task controls playback for the VS1053 MP3 decoder. It retrieves the |
| name of the song to be played and concatenates it with "1:" so it can be |
| opened using f_open. A buffer size of 24 is set to read only 24 bytes    |
| from the file before SPI transferring to the VS1053. This size is chosen |
| to minimize CPU utilization. The task will continuously read from the    |
| file and write bytes to the decoder to play music. If a song is paused,  |
| reading and sending is halted. When an interrupt is detected to skip or  |
| previous song is detected, reading and sending is stopped and the next   |
| mp3 song title is loaded. The task uses a mutex to protect the SPI bus.  |
|--------------------------------------------------------------------------|
| @param:                                                                  |
|    {void *} pvParameters - Parameters passed to task                     |
|-------------------------------------------------------------------------*/
{
  while(1)
  {
    FIL file;
    char title[100];
    song_title = mp3.getCurrentSongName();
    strcpy(title, "1:");
    strcat(title, mp3.getCurrentSongName());
    f_open(&file,title,FA_OPEN_EXISTING|FA_READ);
    int buffer_size = 24;
    int buffer_ofs = 0;
    unsigned char buffer[buffer_size] = {};

    unsigned int file_size = f_size(&file);
    unsigned int bytes_read;

    while(buffer_ofs < file_size && !mp3.getNextSongFlag() && !mp3.getPrevSongFlag()){
      if(xSemaphoreTake(spi_bus_lock, portMAX_DELAY)) {
        if (mp3.isPlaying()){
          int buffer_pos = 0;
          unsigned char *p;
          f_read(&file, buffer, buffer_size ,&bytes_read);
            p = buffer;
            while (buffer_pos < buffer_size) {
              while(!mp3.readyForData());
              mp3.setXDCSLow();
              ssp0_exchange_byte(*p++);
              buffer_pos++;
            }
          mp3.setXDCSHigh();
          buffer_ofs += buffer_size;
        }
        elapsed = ((float)buffer_ofs/file_size)*100;
        xSemaphoreGive(spi_bus_lock);
      }
      vTaskDelay(1);
    }
    f_close(&file);
    if(mp3.getPrevSongFlag()){
      mp3.decSong();
    } else {
      mp3.nextSong();
    }
    mp3.clearNextPrevFlags();
    mp3.play();
  }
}

void displayName(void *pvParameters)
{
/*-------------------------------------------------------------------------|
| -- displayName(void * pvParameters)                                      |
|--------------------------------------------------------------------------|
| This task displays the filename. If the file name is too long, scrolling |
| is automatically implemented by the task.                                |
|--------------------------------------------------------------------------|
| @param:                                                                  |
|    {void *} pvParameters - Parameters passed to task                     |
|-------------------------------------------------------------------------*/
  lcd.clear();

  while(1)
  {
    int reference = 0;
    int end = 16;
    lcd.setCursor(0,0);
    char* entireT = song_title;
    if(strnlen(entireT, 100) > 15)
      {
        while(entireT == song_title)
        {
          char *subT;
          lcd.setCursor(0,0);
          subT=strndup(entireT+reference, end); //substring of first 16 char of entire title
            for(int i = 0; i < strnlen(subT, 100); i++) //strnlen for thread safety
            {
              if(i>15){
                break;
              }
              lcd.write(subT[i]);
            }
            lcd.displayTimeElapsed(elapsed);
            if(mp3.getShuffle()){
              lcd.setCursor(1,0);
              lcd.write({'S'});
            }
            vTaskDelay(2000); //2 seconds for readability
            lcd.clear();
            if (end > strnlen(entireT, 100)) //if end > length of substring -> exit
            {
             break;
            }
            reference += 16; //next 16 letters
            end +=16;
        }
      }
      else
      {
        while(entireT == song_title){
          for(int i = 0; i < strnlen(song_title, 100); i++)
          {
              lcd.write(entireT[i]);
          }
          lcd.displayTimeElapsed(elapsed);
          if(mp3.getShuffle()){
            lcd.setCursor(1,0);
            lcd.write({'S'});
          }
          vTaskDelay(2000); //2 seconds for readability
          lcd.clear();
        }
      }
    vTaskDelay(100);
  }
}

int main(void)
{
  // Create SPI Mutex
  spi_bus_lock = xSemaphoreCreateMutex();

  // Initialize the LCD
  lcd.initLCD();

  // Initialize MP3 Decoder
  LabGPIO_0 _rst(0, 29);
  LabGPIO_0 _xdcs(1, 19);
  LabGPIO_0 _cs(0, 30);
  LabGPIO_0 _dreq(1, 20);
  bool mp3_success = mp3.init(&_cs, &_dreq, &_rst, &_xdcs);
  mp3.sineTest();

  // PINSEL for ADC5 (Used for Volume Control)
  LPC_PINCON->PINSEL3 |= (0x3<<30) ; //ADC5

  // Interrupt for Play/Pause/Skip/Previous
  LabGPIOInterrupts* x = LabGPIOInterrupts::getInstance();
  x->init();
  bool attach;
  InterruptCondition_E int1 = rising;
  attach = x->attachInterruptHandler(0,1,&pauseToggleISR,int1);
  attach = x->attachInterruptHandler(2,0,&skipSongISR,int1);
  // attach = x->attachInterruptHandler(2,2,&prevSongISR,int1);
  // attach = x->attachInterruptHandler(2,6,&shuffleISR,int1);


  FIL file;
  f_open(&file,"1:song.mp3",FA_OPEN_EXISTING|FA_READ);

  // Create tasks for scheduler
  scheduler_add_task(new terminalTask(PRIORITY_HIGH));
  xTaskCreate(prevSongISR, "Prev Song", 1024, ( void * ) 1, 2, NULL );
  xTaskCreate(shuffleISR, "Shuff Song", 1024, ( void * ) 1, 2, NULL );
  xTaskCreate(playSong, "Play Song", 1024, ( void * ) 1, 3, NULL );
  xTaskCreate(setVolume, "Volume", 1024, ( void * ) 1, 1, NULL );
  xTaskCreate(displayName, "Display Name", 1024, (void *) 1, 1, NULL);

  scheduler_start();

  return 0;
}
