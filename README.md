Date: 2017-10-15

以下内容没有使用，原因是有错或者没有完成，可以作为备用代码。

# send char

~~~~

~~~~



# send number

Can not complie. MUMBER_LENGTH digital number.

~~~~
#include <stdio.h>

#define NUMBER_LENGTH 3

/******************************************************************************
 * Send number
 *****************************************************************************/
void send_number(unsigned int a)
{
    unsigned char char_buffer[NUMBER_LENGTH];
    
    sprintf(char_buffer, "%d", a);
    
    unsigned char i;
    for (i = 0; i <= 3; i ++)
    {
        send_char(char_buffer[i]);
    }
}
~~~~


three digital number

~~~~
#include <stdio.h>

/******************************************************************************
 * Send number
 *****************************************************************************/
void send_number(unsigned int a)
{
    int s;
    int base;
    
    base = 100;
    
    while (base > 0)
    {
        s = a / base;
        send_char((unsigned char) (s + '0'));
        a = a % base;
        base = base / 10;
    }
}
~~~~


# send string


~~~~
/******************************************************************************
 * send_string
 *****************************************************************************/
void  send_string(unsigned char p[])
{
    TI = 1;
    printf("%s", p);
    while (TI == 0)
    {
    }
    TI = 0;
}
~~~~


# pricise delay

~~~~

~~~~


# Software delay

~~~~
/******************************************************************************
 * Delay function, unit is 100 us.
 *****************************************************************************/
void delay100u(unsigned char t)
{
    unsigned char i;
    for( ; t > 0; t --)
    {
        for(i = 44; i > 0; i --)
        {
        }
    }
}


/******************************************************************************
 * Delay function, unit is ms.
 *****************************************************************************/
void delay(unsigned int t)
{
    unsigned char i, j;
    for(; t > 0; t --)
    {
        for(i = 102; i > 0; i --)
        {
            for(j = 3; j > 0; j --)
            {
            }
        }
    }
}
~~~~



# led blink

~~~~
sbit led   = P0^1;

/******************************************************************************
 * Led blink.
 *****************************************************************************/
void led_blink(unsigned char num)
{
    unsigned char i;
    for(i = 0; i < num; i ++)
    {
        led = 0;
        delay(5000);
        led = 1;
        delay(5000);
    }
}
~~~~


# Speed?

~~~~
unsigned char speed = 0xFF;

void process_data()
{
    // What is the use of speed?
    if (speed < 0xFF)
    {
        speed ++;
    }
    else
    {
    }
}
~~~~


# Serial port timeout reset byte_count

~~~~
byte_count_last = byte_count_current;

if (TR0 == 0)
{
    TR0 = 1;
}
else
{
}

if (timer0_number >= 20000)
{
    TR0 = 0;
    timer0_number = 0;
    if (byte_count_current > byte_count_last)
    {
    }
    else
    {
        byte_count = 0;
    }
}
else
{
}
~~~~