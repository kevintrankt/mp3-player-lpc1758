#include "lcdDriver.hpp"
#include "LabUart.hpp"
#include "utilities.h" //delay_ms

labUART x;
int lcdDriver::line = 0;
int lcdDriver::position = 0;
//int lcdDriver::size = 0;

//static bool scroll = false; //flag
//static bool enditer = false;

lcdDriver::lcdDriver() //constructor
{
    line = 0;
    position = 0;
    //size = 0;

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 25; j++) //for some longer file names
        {
            lcd[i][j] = {0};
        }
    }
}

void lcdDriver::initLCD() //initializes LCD screen
{
    x.init(19200); //Baud rate = 19200

    /*TURN BACKLIGHT ON*/
    //x.transmit(0x11);

    /*TURN BACKLIGHT OFF*/
    x.transmit(0x12);

    /*DEFAULT: send command to turn display on with curson on and no blink*/
    //x.transmit(0x18);
}

void lcdDriver::clear() //Clears screen and repositions cursor in upper-left corner
{
    x.transmit(0x0C); //Form Feed
        //size = 0;
    line = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 25; j++)
        {
            lcd[i][j] = {0};
        }
    }
    position = 0;

    delay_ms(500); //Users must pause 5ms after this command
}

void lcdDriver::setCursor(int row, int col) //Re-positions cursor to set location
{
    if (row == 0)
    {
        if (col == 0)
        {
            x.transmit(0x80); //move cursor to line0, position0
        }
        else if (col == 1)
        {
            x.transmit(0x81); //move cursor to line0, position1
        }
        else if (col == 2)
        {
            x.transmit(0x82); //move cursor to line0, position2
        }
        else if (col == 3)
        {
            x.transmit(0x83); //move cursor to line0, position3
        }
        else if (col == 4)
        {
            x.transmit(0x84); //move cursor to line0, position4
        }
        else if (col == 5)
        {
            x.transmit(0x85); //move cursor to line0, position5
        }
        else if (col == 6)
        {
            x.transmit(0x86); //move cursor to line0, position6
        }
        else if (col == 7)
        {
            x.transmit(0x87); //move cursor to line0, position7
        }
        else if (col == 8)
        {
            x.transmit(0x88); //move cursor to line0, position8
        }
        else if (col == 9)
        {
            x.transmit(0x89); //move cursor to line0, position9
        }
        else if (col == 10)
        {
            x.transmit(0x8A); //move cursor to line0, position10
        }
        else if (col == 11)
        {
            x.transmit(0x8B); //move cursor to line0, position11
        }
        else if (col == 12)
        {
            x.transmit(0x8C); //move cursor to line0, position12
        }
        else if (col == 13)
        {
            x.transmit(0x8D); //move cursor to line0, position13
        }
        else if (col == 14)
        {
            x.transmit(0x8E); //move cursor to line0, position14
        }
        else if (col == 15)
        {
            x.transmit(0x8F); //move cursor to line0, position15
        }
    }
    else if (row == 1)
    {
        if (col == 0)
        {
            x.transmit(0x94); //move cursor to line1, position0
        }
        else if (col == 1)
        {
            x.transmit(0x95); //move cursor to line1, position1
        }
        else if (col == 2)
        {
            x.transmit(0x96); //move cursor to line1, position2
        }
        else if (col == 3)
        {
            x.transmit(0x97); //move cursor to line1, position3
        }
        else if (col == 4)
        {
            x.transmit(0x98); //move cursor to line1, position4
        }
        else if (col == 5)
        {
            x.transmit(0x99); //move cursor to line1, position5
        }
        else if (col == 6)
        {
            x.transmit(0x9A); //move cursor to line1, position6
        }
        else if (col == 7)
        {
            x.transmit(0x9B); //move cursor to line1, position7
        }
        else if (col == 8)
        {
            x.transmit(0x9C); //move cursor to line1, position8
        }
        else if (col == 9)
        {
            x.transmit(0x9D); //move cursor to line1, position9
        }
        else if (col == 10)
        {
            x.transmit(0x9E); //move cursor to line1, position10
        }
        else if (col == 11)
        {
            x.transmit(0x9F); //move cursor to line1, position11
        }
        else if (col == 12)
        {
            x.transmit(0xA0); //move cursor to line1, position12
        }
        else if (col == 13)
        {
            x.transmit(0xA1); //move cursor to line1, position13
        }
        else if (col == 14)
        {
            x.transmit(0xA2); //move cursor to line1, position14
        }
        else if (col == 15)
        {
            x.transmit(0xA3); //move cursor to line1, position15
        }
    }
}

void lcdDriver::write(char data) //Writes char to the LCD screen
{
    // static int s_line = 0;
    // static int s_position = 0;

    // if(scroll == true)
    // {
    //     lcd[s_line][s_position] = data; //store data into lcd array
    //     printf("write scrolling at (%i, %i): %c\n", s_line, s_position, lcd[s_line][s_position]);
    //     x.transmit(lcd[s_line][s_position]); //transmits the data to actual LCD

    //     if(s_position < 15 && enditer == false) //if not at end of line and iteration not over
    //     {
    //         s_position += 1; // moves cursor 1 to the right
    //     }
    //     else if(s_position < 15 && enditer == true) //if not at end of line and iteration over
    //     {
    //         s_line = 0; //reset
    //         s_position = 0;
    //         enditer = false;
    //     }
    // }
    //else if (scroll == false) //if not writing to scroll screen
    //{
    lcd[line][position] = data; //store data into lcd array

    //size += 1;
    //printf("writing at (%i, %i): %c\n", line, position, lcd[line][position]);
    x.transmit(lcd[line][position]); //transmits the data to actual LCD

    if (position < 15) //if not at end of line
    {
        position += 1; // moves cursor 1 to the right
    }
    else if ((position = 15) && (line == 0)) //if at the end of line 0
    {
        line = 1;     //next line
        position = 0; //reset position to beginning of next line
        setCursor(1, 0);
    }
    else if ((position = 15) && (line == 1)) //if at the end of line 1
    {
        line = 0;     //go back to line 0
        position = 0; //reset position to 0
        setCursor(0, 0);
    }
    //}
}

void lcdDriver::writeL1(char data) //writes to Line 1 of LCD screen
{
    line = 0;

    lcd[line][position] = data; //store data into lcd array

    //size += 1;
    //printf("writing at (%i, %i): %c\n", line, position, lcd[line][position]);

    x.transmit(lcd[line][position]); //transmits the data to actual LCD

    if (position < 15) //if not at end of line
    {
        position += 1; // moves cursor 1 to the right
    }
    else if ((position = 15) && (line == 0)) //if at the end of line 0
    {
        position = 0; //reset position to beginning of line
        setCursor(0, 0);
    }
    else if ((position = 15) && (line == 1)) //if at the end of line 1
    {
        line = 0;
        position = 0; //reset position to 0
        setCursor(0, 0);
    }
}

void lcdDriver::writeL2(char data) //writes to Line 2 of LCD screen
{
    line = 1;

    lcd[line][position] = data; //store data into lcd array

    //size += 1;
    //printf("writing at (%i, %i): %c\n", line, position, lcd[line][position]);

    x.transmit(lcd[line][position]); //transmits the data to actual LCD

    if (position < 15) //if not at end of line
    {
        position += 1; // moves cursor 1 to the right
    }
    else if ((position == 15) && (line == 0)) //if at the end of line 0
    {
        line = 1;
        position = 0; //reset position to beginning of line
        setCursor(1, 0);
    }
    else if ((position == 15) && (line == 1)) //if at the end of line 1
    {
        position = 0; //reset position to 0
        setCursor(1, 0);
    }
}

void lcdDriver::displayTimeElapsed(int elapsed)
{
    /*displaying remaining percentage*/
    char time[3] = {0, 0, 0};
    itoa(elapsed, time, 10);

    setCursor(1, 6);

    if (strnlen(time, 100) == 1)
    {
        write(time[0]);
        setCursor(1, 7);
        write('%');
    }
    else if (strnlen(time, 100) == 2)
    {
        write(time[0]);
        setCursor(1, 7);
        write(time[1]);
        setCursor(1, 8);
        write('%');
    }
    else if (strnlen(time, 100) == 3)
    {
        write(time[0]);
        setCursor(1, 7);
        write(time[1]);
        setCursor(1, 8);
        write(time[2]);
        setCursor(1, 9);
        write('%');
    }
}

//void lcdDriver::scrollDisplayLeft() //scrolls 1st line of display (text & cursor) continuously left
//{
// scroll = true; //set flag
// int k = 0;
// int ptr = 0;
// int s = size;

// char lcd_copy[2][16];

// for(int i = 0; i < 2; i++) //clear copy
// {
//     for(int j = 0; j < 16; j++)
//     {
//         lcd_copy[i][j] = {0};
//     }
// }

// printf("copy cleared\n");
// printf("size of lcd display = %i\n", size);
// printf("in lcd array:");
// for(int z = 0; z < size; z++)
// {
//     printf("%c", lcd[0][z]);
// }
// printf("\n");
// printf("s-1 = %i\n", s-1);

// for(int i = size-1; i > 0; i--)
// {
//     k = ptr;

//     for(int j = 0; j < s-1; j++)
//     {
//         lcd_copy[0][j] = lcd[0][k+1];
//         //printf("writing %c\n", lcd_copy[0][j]);
//         write(lcd_copy[0][j]);
//         k += 1;
//     }

//     delay_ms(1000);
//     printf("delayed 1s\n");

//     enditer = true;
//     s -= 1;
//     ptr += 1;
//     clear(); //Clear screen, cursor set to (0,0)
// }

//}

//use scrollDisplayLeft to have text go off screen <--- and then come back on screen <---
// EX: https://www.arduino.cc/en/Tutorial/LiquidCrystalScroll
