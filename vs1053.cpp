#include "vs1053.hpp"

VS1053::VS1053()
{
  // Intentionally left empty
}

bool VS1053::init(LabGPIO_0 *cs, LabGPIO_0 *dreq, LabGPIO_0 *rst, LabGPIO_0 *xdcs)
{
  // 1. Get all songs from micro-sd card and store into an array.
  number_songs = 0;
  song_playing = 0;
  getSongs();
  max_songs = number_songs;

  // 2. Initialize GPIO
  _cs = cs;
  _cs->setAsOutput();
  _cs->setHigh();

  _dreq = dreq;
  _dreq->setAsInput();

  _rst = rst;
  _rst->setAsOutput();
  _rst->setLow();

  _xdcs = xdcs;
  _xdcs->setAsOutput();
  _xdcs->setHigh();

  // 3. Initialize SPI
  ssp0_init(24);

  // 4. Reset VS1053
  _rst->setHigh();

  delay_ms(50);

  // 5. Set SCI Mode register to 0x0810 and enable test mode
  sciWrite(SCI_MODE, (1 << 5) | 0x0810);
  // 6. Set SCI Bass register to 0x0000
  sciWrite(SCI_BASS, 0x0000);
  // 7. Set SCI AUDATA register to 0xAC45
  sciWrite(SCI_AUDATA, 0xAC45);
  // 8. Set SCI Clkf register to 0x6000
  sciWrite(SCI_CLOCKF, 0x6000);
  // 9. Set volume to maximum value
  volume = 0;
  setVolume(volume, volume);

  // 10. Initialize playback flags
  next_song_flag = false;
  prev_song_flag = false;
  shuffle = true;
  playing = true;

  return (sciRead(SCI_STATUS) >> 4 & 0x04);
}

void VS1053::getSongs()
{
  DIR Dir;
  FILINFO Finfo;
  FRESULT returnCode;
  char Lfname[128];
  // Open directory
  f_opendir(&Dir, "1:");
  for (;;)
  {
    Finfo.lfname = Lfname;
    Finfo.lfsize = sizeof(Lfname);
    // Read directory contents
    returnCode = f_readdir(&Dir, &Finfo);
    if ((FR_OK != returnCode) || !Finfo.fname[0])
    {
      break;
    }
    // If filename ends with .mp3, store song title in an array
    if (strstr(Finfo.lfname, ".mp3") || strstr(Finfo.lfname, ".MP3"))
    {
      int len = strnlen(Finfo.lfname, 128);
      songs[number_songs] = new char[len + 1];
      strcpy(songs[number_songs], Finfo.lfname);
      number_songs++;
    }
  }
}

void VS1053::sciWrite(uint8_t reg_addr, uint16_t value)
{
  while (!readyForData())
    ;
  _cs->setLow();
  ssp0_exchange_byte(0x02);     // write
  ssp0_exchange_byte(reg_addr); // reg address
  ssp0_exchange_byte(value >> 8);
  ssp0_exchange_byte(value & 0xff);
  _cs->setHigh();
}

uint16_t VS1053::sciRead(uint8_t reg_addr)
{
  while (!readyForData())
    ;
  uint16_t data;
  _cs->setLow();
  ssp0_exchange_byte(0x03);     // read
  ssp0_exchange_byte(reg_addr); //reg address
  delay_ms(10);
  data = ssp0_exchange_byte(0x00);
  data <<= 8;
  data |= ssp0_exchange_byte(0x00);
  _cs->setHigh();
  return data;
}

bool VS1053::readyForData()
{
  return _dreq->getLevel();
}

void VS1053::dumpRegs()
{
  u0_dbg_printf("\nMode = 0x%x", sciRead(SCI_MODE));
  u0_dbg_printf("\nStatus = 0x%x", sciRead(SCI_STATUS));
  u0_dbg_printf("\nClkF = 0x%x", sciRead(SCI_CLOCKF));
  u0_dbg_printf("\nVol = 0x%x", sciRead(SCI_VOLUME));
}

void VS1053::setVolume(uint8_t left, uint8_t right)
{
  volume = left;
  uint16_t v;
  v = left;
  v <<= 8;
  v |= right;
  sciWrite(SCI_VOLUME, v);
}

int VS1053::getVolume()
{
  return volume;
}

void VS1053::setXDCSHigh()
{
  _xdcs->setHigh();
}

void VS1053::setXDCSLow()
{
  _xdcs->setLow();
}

void VS1053::sdiWrite(uint8_t *data, uint32_t size)
{
  while (!readyForData())
    ;
  _xdcs->setLow();
  for (int i = 0; i < size; i++)
  {
    while (!readyForData())
      ;
    ssp0_exchange_byte(data[i]);
  }

  if (size < 32)
    for (int i = 0; i < (32 - size); i++)
      ssp0_exchange_byte(0x00);
  _xdcs->setHigh();
}

void VS1053::sineTest()
{
  uint8_t sineTest_on[] = {0x53, 0xEF, 0x6E, 0x44, 0, 0, 0, 0};
  uint8_t sineTest_off[] = {0x45, 0x78, 0x69, 0x74, 0, 0, 0, 0};
  sdiWrite(sineTest_on, 8);
  delay_ms(200);
  sdiWrite(sineTest_off, 8);
}

void VS1053::PlayFile(FIL file)
{
  int buffer_size = 512;
  int buffer_ofs = 0;
  unsigned char buffer[buffer_size] = {};

  unsigned int file_size = f_size(&file);
  unsigned int bytes_read;

  while (buffer_ofs < file_size && !next_song_flag && !prev_song_flag)
  {
    if (playing)
    {
      int buffer_pos = 0;
      unsigned char *p;
      f_read(&file, buffer, buffer_size, &bytes_read);
      p = buffer;
      while (buffer_pos < buffer_size)
      {
        while (!readyForData())
        {
          setXDCSHigh();
        }
        setXDCSLow();
        ssp0_exchange_byte(*p++);
        buffer_pos++;
      }
      setXDCSHigh();

      buffer_ofs += 512;
    }
  }
  f_close(&file);
  if (prev_song_flag)
  {
    prev_song_flag = false;
    decSong();
    decSong();
  }
  else if (next_song_flag)
  {
    next_song_flag = false;
    decSong();
    nextSong();
  }
  playing = true;
}

void VS1053::pause()
{
  playing = false;
}

void VS1053::play()
{
  playing = true;
}

bool VS1053::isPlaying()
{
  return playing;
}

void VS1053::nextSong()
{
  if (shuffle)
  {
    int temp_song = song_playing;
    while (song_playing == temp_song)
    {
      song_playing = rand() % max_songs;
    }
  }
  else
  {
    song_playing++;
    if (song_playing >= max_songs)
    {
      song_playing = 0;
    }
  }
}

void VS1053::decSong()
{
  if (song_playing != 0)
  {
    song_playing--;
  }
  else
  {
    song_playing = max_songs - 1;
  }
}

void VS1053::skipSong()
{
  next_song_flag = true;
}

void VS1053::prevSong()
{
  prev_song_flag = true;
}

bool VS1053::getNextSongFlag()
{
  return next_song_flag;
}

bool VS1053::getPrevSongFlag()
{
  return prev_song_flag;
}

void VS1053::clearNextPrevFlags()
{
  prev_song_flag = false;
  next_song_flag = false;
}

char *VS1053::getCurrentSongName()
{
  return songs[song_playing];
}

void VS1053::toggleShuffle()
{
  shuffle = !shuffle;
}

bool VS1053::getShuffle()
{
  return shuffle;
}
