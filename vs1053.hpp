#ifndef VS1053_H
#define VS1053_H

#include "LPC17xx.h"
#include "LabGPIO_0.hpp"
#include "LabSPI.hpp"
#include "printf_lib.h"
#include "utilities.h"
#include "ssp0.h"
#include "ff.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

#define SCI_MODE 0x00
#define SCI_STATUS 0x01
#define SCI_BASS 0x02
#define SCI_CLOCKF 0x03
#define SCI_DECODETIME 0x04
#define SCI_AUDATA 0x05
#define SCI_WRAM 0x06
#define VSCI_WRAMADDR 0x07
#define SCI_HDAT0 0x08
#define SCI_HDAT1 0x09
#define SCI_VOLUME 0x0B

class VS1053
{
private:
  LabGPIO_0 *_cs;      // VS1053 Chip Select
  LabGPIO_0 *_dreq;    // VS1053 DREQ Signal
  LabGPIO_0 *_rst;     // VS1053 Reset
  LabGPIO_0 *_xdcs;    // VS1053 D Chip Select
  bool playing;        // bool to check if the decoder is playing or not
  uint8_t volume;      // current volume of the VS1053
  int number_songs;    // number of songs retrieved from micro-sd card
  char *songs[100];    // array of song titles
  int song_playing;    // index of song playing
  int max_songs;       // maximum number of songs for index
  bool next_song_flag; // flag to detect if skipSongISR is triggered
  bool prev_song_flag; // flag to detect if prevSongISR is triggered
  bool shuffle;

public:
  bool init(LabGPIO_0 *cs, LabGPIO_0 *dreq, LabGPIO_0 *rst, LabGPIO_0 *xdcs);
  /*-------------------------------------------------------------------------|
  | -- init(LabGPIO_0 *cs, LabGPIO_0 *dreq, LabGPIO_0 *rst, LabGPIO_0 *xdcs) |
  |--------------------------------------------------------------------------|
  | Initializes the VS1053 and MP3 functionality.                            |
  |  1. Get all songs from micro-sd card and store into an array.            |
  |  2. Initialize GPIO                                                      |
  |  3. Initialize SPI                                                       |
  |  4. Reset VS1053                                                         |
  |  5. Set SCI Mode register to 0x0810 and enable test mode                 |
  |  6. Set SCI Bass register to 0x0000                                      |
  |  7. Set SCI AUDATA register to 0xAC45                                    |
  |  8. Set SCI Clkf register to 0x6000                                      |
  |  9. Set volume to maximum value                                          |
  |  10. Initialize playback flags                                           |
  |  11. Check version from SCI Status register to confirm proper            |
  |      initialization                                                      |
  |--------------------------------------------------------------------------|
  | @param:                                                                  |
  |    {LabGPIO_0 *} cs - Chip Select GPIO object                            |
  |    {LabGPIO_0 *} dreq - Dreq GPIO Object                                 |
  |    {LabGPIO_0 *} rst - RST GPIO Object                                   |
  |    {LabGPIO_0 *} xdcs - D Chip Select GPIO object                        |
  | @return:                                                                 |
  |    {bool} - bool if VS1053 is initialized properly                       |
  |-------------------------------------------------------------------------*/
  void getSongs();
  /*-------------------------------------------------------------------------|
  | -- getSongs()                                                            |
  |--------------------------------------------------------------------------|
  | Get all songs from micro-sd card and store into an array.                |
  | Song titles are loaded by using ff.h f_opendir and f_readdir and storing |
  | files that end with .mp3 in a char * array.                              |
  |-------------------------------------------------------------------------*/
  void sciWrite(uint8_t reg_addr, uint16_t value);
  /*-------------------------------------------------------------------------|
  | -- sciWrite(uint8_t reg_addr, uint16_t value)                           |
  |--------------------------------------------------------------------------|
  | Writes to VS1053 SCI registers                                           |
  |  1. Wait until VS1053 is ready for data transfer                         |
  |  2. Set chip select to low to enable SPI transfer                        |
  |  3. SPI transfer 0x02 to write to SCI register                           |
  |  4. SPI transfer register address                                        |
  |  5. SPI transfer 16 bits of data to specified SCI register               |
  |  6. Set chip select to high to disable SPI transfer                      |
  |--------------------------------------------------------------------------|
  | @param:                                                                  |
  |    {uint8_t} reg_addr - Register address                                 |
  |    {uint16_t} value - Value to write to register address                 |
  |-------------------------------------------------------------------------*/
  uint16_t sciRead(uint8_t reg_addr);
  /*-------------------------------------------------------------------------|
  | -- sciRead(uint8_t reg_addr)                                            |
  |--------------------------------------------------------------------------|
  | Reads from VS1053 SCI registers                                          |
  |  1. Wait until VS1053 is ready for data transfer                         |
  |  2. Set chip select to low to enable SPI transfer                        |
  |  3. SPI transfer 0x03 to read to SCI register                            |
  |  4. SPI transfer register address                                        |
  |  5. SPI transfer 0x0000 to VS1053 to retrieve 16 bit data from registers |
  |  6. Set chip select to high to disable SPI transfer                      |
  |--------------------------------------------------------------------------|
  | @param:                                                                  |
  |    {uint8_t} reg_addr - Register address                                 |
  | @return:                                                                 |
  |    {uint16_t} - Data stored in register address                          |
  |-------------------------------------------------------------------------*/
  bool readyForData();
  /*-------------------------------------------------------------------------|
  | -- readyForData()                                                        |
  |--------------------------------------------------------------------------|
  | Returns whether VS1053 is ready for data or not by checking dreq signal  |
  |--------------------------------------------------------------------------|
  | @param:                                                                  |
  |    {uint8_t} reg_addr - Register address                                 |
  | @return:                                                                 |
  |    {uint16_t} - Data stored in register address                          |
  |-------------------------------------------------------------------------*/
  void dumpRegs();
  /*-------------------------------------------------------------------------|
  | -- dumpRegs()                                                            |
  |--------------------------------------------------------------------------|
  | Prints Mode, Status, Clk, and Volume registers                           |
  |-------------------------------------------------------------------------*/
  void setVolume(uint8_t left, uint8_t right);
  /*-------------------------------------------------------------------------|
  | -- setVolume(uint8_t left, uint8_t right)                               |
  |--------------------------------------------------------------------------|
  | Sets the volume in SCI registers for left and right channels             |
  |--------------------------------------------------------------------------|
  | @param:                                                                  |
  |    {uint8_t} left - Left Volume                                          |
  |    {uint8_t} right - Right Volume                                        |
  |-------------------------------------------------------------------------*/
  int getVolume();
  /*-------------------------------------------------------------------------|
  | -- getVolume()                                                           |
  |--------------------------------------------------------------------------|
  | Returns volume last set to the VS1053                                    |
  |--------------------------------------------------------------------------|
  | @return:                                                                 |
  |    {int} - Volume                                                        |
  |-------------------------------------------------------------------------*/
  void setXDCSHigh();
  /*-------------------------------------------------------------------------|
  | -- setXDCSHigh()                                                       |
  |--------------------------------------------------------------------------|
  | Set XDCS pin GPIO pin as high                                            |
  |-------------------------------------------------------------------------*/
  void setXDCSLow();
  /*-------------------------------------------------------------------------|
  | -- setXDCSLow()                                                        |
  |--------------------------------------------------------------------------|
  | Set XDCS pin GPIO pin as low                                             |
  |-------------------------------------------------------------------------*/
  void sdiWrite(uint8_t *data, uint32_t size);
  /*-------------------------------------------------------------------------|
  | -- sdiWrite(uint8_t *data, uint32_t size)                               |
  |--------------------------------------------------------------------------|
  | Writes data to the VS1053. This function writes each byte given until the|
  | specified max size is reached.                                           |
  |--------------------------------------------------------------------------|
  | @param:                                                                  |
  |    {uint8_t *} data - Array of bytes to be sent                          |
  |    {uint32_t} size - Size of array                                       |
  |-------------------------------------------------------------------------*/
  void sineTest();
  /*-------------------------------------------------------------------------|
  | -- sineTest()                                                           |
  |--------------------------------------------------------------------------|
  | Turns on the VS1053 sine test for 200 ms then is turned off              |
  |-------------------------------------------------------------------------*/
  void PlayFile(FIL file);
  /*-------------------------------------------------------------------------|
  | -- PlayFile(FIL file)                                                    |
  |--------------------------------------------------------------------------|
  | Send and play an MP3 file. Playback is paused if playing = false.        |
  | Playback is stopped f next_song_flag or prev_song_flag = true.           |
  | Chunks of 512 bytes are read and sent to the VS1053 until EOF            |
  |--------------------------------------------------------------------------|
  | @param:                                                                  |
  |    {FIL} file - FIL object of MP3 file                                   |
  |-------------------------------------------------------------------------*/
  void pause();
  /*-------------------------------------------------------------------------|
  | -- pause()                                                               |
  |--------------------------------------------------------------------------|
  | Set the playing flag as false                                            |
  |-------------------------------------------------------------------------*/
  void play();
  /*-------------------------------------------------------------------------|
  | -- play()                                                                |
  |--------------------------------------------------------------------------|
  | Set the playing flag as true                                             |
  |-------------------------------------------------------------------------*/
  bool isPlaying();
  /*-------------------------------------------------------------------------|
  | -- isPlaying()                                                           |
  |--------------------------------------------------------------------------|
  | Returns playing flag                                                     |
  |--------------------------------------------------------------------------|
  | @return:                                                                 |
  |    {bool} - playing flag                                                 |
  |-------------------------------------------------------------------------*/
  void nextSong();
  /*-------------------------------------------------------------------------|
  | -- nextSong()                                                            |
  |--------------------------------------------------------------------------|
  | Increments the song_playing index                                        |
  |-------------------------------------------------------------------------*/
  void decSong();
  /*-------------------------------------------------------------------------|
  | -- decSong()                                                             |
  |--------------------------------------------------------------------------|
  | Decrements the song_playing index                                        |
  |-------------------------------------------------------------------------*/
  void skipSong();
  /*-------------------------------------------------------------------------|
  | -- skipSong()                                                            |
  |--------------------------------------------------------------------------|
  | Set next_song_flag as true                                               |
  |-------------------------------------------------------------------------*/
  void prevSong();
  /*-------------------------------------------------------------------------|
  | -- prevSong()                                                            |
  |--------------------------------------------------------------------------|
  | Set prev_song_flag as true                                               |
  |-------------------------------------------------------------------------*/
  bool getNextSongFlag();
  /*-------------------------------------------------------------------------|
  | -- getNextSongFlag()                                                     |
  |--------------------------------------------------------------------------|
  | Returns next_song_flag                                                   |
  |--------------------------------------------------------------------------|
  | @return:                                                                 |
  |    {bool} - next_song_flag                                               |
  |-------------------------------------------------------------------------*/
  bool getPrevSongFlag();
  /*-------------------------------------------------------------------------|
  | -- getPrevSongFlag()                                                     |
  |--------------------------------------------------------------------------|
  | Returns prev_song_flag                                                   |
  |--------------------------------------------------------------------------|
  | @return:                                                                 |
  |    {bool} - prev_song_flag                                               |
  |-------------------------------------------------------------------------*/
  void clearNextPrevFlags();
  /*-------------------------------------------------------------------------|
  | -- clearNextPrevFlags()                                                  |
  |--------------------------------------------------------------------------|
  | Set prev_song_flag and next_song_flag as false                           |
  |-------------------------------------------------------------------------*/
  char *getCurrentSongName();
  /*-------------------------------------------------------------------------|
  | -- getCurrentSongName()                                                  |
  |--------------------------------------------------------------------------|
  | Returns current song file name                                           |
  |--------------------------------------------------------------------------|
  | @return:                                                                 |
  |    {char *} - current song file name                                     |
  |-------------------------------------------------------------------------*/
  void toggleShuffle();
  /*-------------------------------------------------------------------------|
  | -- toggleShuffle     ()                                                  |
  |--------------------------------------------------------------------------|
  | Toggles shuffle flag                                                     |
  |-------------------------------------------------------------------------*/
  bool getShuffle();
  /*-------------------------------------------------------------------------|
  | -- getShuffle()                                                          |
  |--------------------------------------------------------------------------|
  | Returns shuffle flag                                                     |
  |--------------------------------------------------------------------------|
  | @return:                                                                 |
  |    {boo} - shuffle flag                                                  |
  |-------------------------------------------------------------------------*/
  VS1053();
};

#endif
