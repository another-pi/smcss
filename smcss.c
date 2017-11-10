/* Description:
 *
 * Author:
 */

#include <reg51.h>
#include <stdio.h>

#define NAME        "Step Motor Control System Server"
#define VERSION     "1.4"

#define MAX_CODE_LENGTH  48
#define CODE_BODY_LENGTH 9
#define CODE_FOOT_LENGTH 3

unsigned char xdata RES[MAX_CODE_LENGTH] = 0;
unsigned char byte_count = 0;
unsigned char byte_count_current = 0;

unsigned char xdata code_data[MAX_CODE_LENGTH] = 0;
unsigned char code_length = 0;
unsigned char code_id = 0;

unsigned int timer0_number = 0;

sbit pwm0 = P1^1;
sbit pwm1 = P1^2;

sfr ISP_DATA    = 0xE2;
sfr ISP_ADDRH   = 0xE3;
sfr ISP_ADDRL   = 0xE4;
sfr ISP_CMD     = 0xE5;
sfr ISP_TRIG    = 0xE6;
sfr ISP_CONTR   = 0xE7;


/******************************************************************************
 * Delay function use timer with flow time 100 us, unit is 100 us.
 *****************************************************************************/
void delay100u(unsigned char t)
{
    TR0 = 1;
    while (timer0_number < (t * 1))
    {
    }
    TR0 = 0;

    timer0_number = 0;
}


/******************************************************************************
 * Delay function use timer with flow time 100 us, unit is ms.
 *****************************************************************************/
void delay(unsigned char t)
{
    TR0 = 1;
    while (timer0_number < (t * 10))
    {
    }
    TR0 = 0;

    timer0_number = 0;
}


/******************************************************************************
 * ISP functions
 *****************************************************************************/
void ISP_ERA()
{
    ISP_CMD     = 0x03;
    ISP_TRIG    = 0x46;
    ISP_TRIG    = 0xb9;
    delay(10);
}

unsigned char ISP_READ(unsigned char x)
{
    ISP_ADDRL   = x;
    ISP_CMD     = 0x01;
    ISP_TRIG    = 0x46;
    ISP_TRIG    = 0xb9;
    delay(1);
    return(ISP_DATA);
}

void ISP_WRITE(unsigned char y, unsigned char add)
{
    ISP_DATA    = y;
    ISP_ADDRL   = add;
    ISP_CMD     = 0x02;
    ISP_TRIG    = 0x46;
    ISP_TRIG    = 0xb9;
    delay(10);
}


/******************************************************************************
 * send_char
 *****************************************************************************/
void send_char(unsigned char a)
{
    ES = 0;

    if (a == '\n')
    {
        SBUF = 0x0D;
        while(TI != 1)
        {
        }
        TI = 0;

        SBUF = 0x0A;
        while(TI != 1)
        {
        }
        TI = 0;
    }
    else
    {
        SBUF = a;
        while(TI != 1)
        {
        }
        TI=0;
    }

    ES = 1;
}


/******************************************************************************
 * Send string
 *****************************************************************************/
void  send_string(unsigned char * p)
{
    while (* p != '\0')
    {
        send_char(* p);
        p = p + 1;
    }
}


/******************************************************************************
 * Send string in interrupt.
 *****************************************************************************/
void  send_string_interrupt(unsigned char * p)
{
    while (* p != '\0')
    {
        send_char(* p);
        p = p + 1;
    }
}

/******************************************************************************
 * Send number as a string.
 *****************************************************************************/
void send_number(unsigned char p)
{
    TI = 1;
    printf("%bd", p);
    while (TI == 0)
    {
    }
    TI = 0;
}


/******************************************************************************
 * Send a hex as a string.
 *****************************************************************************/
void  send_hex(unsigned char p)
{
    TI = 1;
    printf("%02bX", p);
    while (TI == 0)
    {
    }
    TI = 0;
}


/******************************************************************************
 * Welcome message.
 *****************************************************************************/
void welcome()
{
    send_string("================================================================================\n");
    send_string("     ");
    send_string(NAME);
    send_string("\n");
    send_string("\n");

    send_string("Version            : ");
    send_string(VERSION);
    send_string("\n");
    send_string("Max code length    : ");
    send_number(MAX_CODE_LENGTH);
    send_string("\n");
    send_string("Code body length   : ");
    send_number(CODE_BODY_LENGTH);
    send_string("\n");
    send_string("\n");
    send_string("For more detial information, please read the program manual in OUYANG Lab.\n");
    send_string("================================================================================\n");
    send_string("\n");
}

/******************************************************************************
 * RAM code information
 *****************************************************************************/
void code_information_ram()
{
    unsigned char i;

    send_string("RAM code information\n");
    send_string("    code id    : ");
    send_number(code_id);
    send_string("\n");

    send_string("    code length: ");
    send_number(code_length);
    send_string("\n");

    send_string("    code data  : ");
    for (i = 0; i < code_length; i ++)
    {
        send_hex(code_data[i]);
        send_string(" ");
    }
    send_string("\n");
    send_string("\n");
}

/******************************************************************************
 * EEPROM code information
 *****************************************************************************/
void code_information_eeprom()
{
    unsigned char i;
    unsigned char code_eeprom_length;

    code_eeprom_length = ISP_READ(0xFF);

    send_string("EEPROM code information\n");
    send_string("    code length: ");
    send_number(code_eeprom_length);
    send_string("\n");

    send_string("    code data  : ");
    for (i = 0; i < code_eeprom_length; i ++)
    {
        send_hex(ISP_READ(i));
        send_string(" ");
    }
    send_string("\n");
    send_string("\n");
}


/******************************************************************************
 * Read data from EEPROM and save in global array "code_data".
 *****************************************************************************/
void load_code()
{
    unsigned char i;

    code_length = ISP_READ(0xFF);

    if (
            (code_length == 0xFF) ||
            (code_length < (CODE_BODY_LENGTH + CODE_FOOT_LENGTH))
    )
    {
        code_length = 0;

        send_string("Storage is empty.\n");
        send_string("\n");
    }
    else
    {
        for (i = 0; i < code_length; i ++)
        {
            code_data[i] = ISP_READ(i);
        }

        send_string("Code loaded.\n");
        send_string("\n");
    }
}


/******************************************************************************
 * Tempary save data.
 *****************************************************************************/
void save_code(unsigned char l_data[], unsigned char l_length)
{
    unsigned char i;
    
    code_id = code_id + 1;
    code_length = l_length;

    for (i = 0; i < l_length; i ++)
    {
        code_data[i] = l_data[i];
    }

    send_string("Code saved.\n");
    send_string("\n");
}

/******************************************************************************
 * Write data to eeprom.
 *****************************************************************************/
void write_code(unsigned char l_data[], unsigned char l_length)
{
    unsigned char i;

    ISP_ERA();
    ISP_WRITE(l_length, 0xFF);

    for (i = 0; i < l_length; i ++)
    {
        ISP_WRITE(l_data[i], i);
    }

    send_string("Code written.\n");
    send_string("\n");
}


/******************************************************************************
 * Drive stepping motor according data from global array -- code_data.
 *****************************************************************************/
void run(unsigned char l_data[], unsigned char l_length)
{
    unsigned char i;
    unsigned char cycle_count;
    unsigned char step_count;
    unsigned char part_interval_high_count;
    unsigned char back_step_count;
    unsigned char back_part_interval_high_count;

    send_string("Running ...\n");

    for (i = 0; i < (l_length - CODE_FOOT_LENGTH); i = i + CODE_BODY_LENGTH)
    {
        for(cycle_count = 0; cycle_count < l_data[i+4]; cycle_count ++)
        {
            for(step_count = 0; step_count < l_data[i+1]; step_count ++)
            {
                if((l_data[i] & 0xF0) == 0xF0)
                {
                    /*
                     * Direction is "F", mark as -CW
                     */
                    pwm0 = 0;
                    delay100u(l_data[i] & 0x0F);
                    pwm0 = 1;
                    delay100u(l_data[i] & 0x0F);
                }
                else
                {
                    /*
                     * Direction is "0", mark as +CW
                     */
                    pwm1 = 0;
                    delay100u(l_data[i]);
                    pwm1 = 1;
                    delay100u(l_data[i]);
                }
            }

            /*
             * Delay.
             */

            for(part_interval_high_count = 0; part_interval_high_count < l_data[i+2]; part_interval_high_count ++)
            {
                delay(255);
                delay(1);
            }
            delay(l_data[i+3]);

            if ((l_data[i] & 0xF0) == 0xF0)
            {
                /*
                 * When the direction is "F", execute back action.
                 */
                for (back_step_count = 0; back_step_count < l_data[i+6]; back_step_count ++)
                {
                    pwm1 = 0;
                    delay100u(l_data[i+5]);
                    pwm1 = 1;
                    delay100u(l_data[i+5]);
                }

                /*
                 * Delay.
                 */

                for (back_part_interval_high_count = 0; back_part_interval_high_count < l_data[i+7]; back_part_interval_high_count ++)
                {
                    delay(255);
                    delay(1);
                }
                delay(l_data[i+8]);
            }
        }
    }

    send_string("Running finished.\n");
    send_string("\n");
}


/******************************************************************************
 * Process data in global array "RES".
 *****************************************************************************/
void process_data()
{
    // Catch current byte count for follow judgement.
    byte_count_current = byte_count;

    if (
            (byte_count_current >= (CODE_BODY_LENGTH + CODE_FOOT_LENGTH)) &&
            (((byte_count_current - CODE_FOOT_LENGTH) % CODE_BODY_LENGTH) == 0) &&
            (
                ((RES[0] & 0xF0) == 0xF0) ||
                ((RES[0] & 0xF0) == 0x00)
            )
    )
    {
        /*
         * This is a command for store code data.
         */
        if (
                (RES[byte_count_current-1] == 0x00) &&
                (RES[byte_count_current-2] == 0x00) &&
                (RES[byte_count_current-3] == 0x00)
        )
        {
            /*
             * This is a command for temparily store code data.
             */
            byte_count = 0;
            save_code(RES, byte_count_current);
        }
        else if (
                (RES[byte_count_current-1] == 0x11) &&
                (RES[byte_count_current-2] == 0x11) &&
                (RES[byte_count_current-3] == 0x11)
        )
        {
            /*
             * This is a command for immediately execute code data.
             */
            byte_count = 0;
            run(RES, byte_count_current);
        }
        else
        {
            /*
             * Sorry, we do not know these code.
             */
             byte_count = 0;
        }
    }
    else if (
            (byte_count_current >= 5) &&
            (RES[byte_count_current-1] == 0xEE) &&
            (RES[byte_count_current-2] == 0xEE) &&
            (RES[byte_count_current-3] == 0xEE) &&
            (RES[byte_count_current-4] == 0xEE) &&
            (RES[byte_count_current-5] == 0xEE)
    )
    {
        /*
         * This is a command for reset byte count.
         */
        byte_count = 0;
        send_string("Reset send byte count.\n");
        send_string("\n");
    }
    else if (
            (byte_count_current >= 5) &&
            (RES[byte_count_current-1] == 0xEF) &&
            (RES[byte_count_current-2] == 0xEF) &&
            (RES[byte_count_current-3] == 0xEF) &&
            (RES[byte_count_current-4] == 0xEF) &&
            (RES[byte_count_current-5] == 0xEF)
    )
    {
        /*
         * This is a command for start action.
         */
        byte_count = 0;
        run(code_data, code_length);
    }
    else if (
            (byte_count_current >= 5) &&
            (RES[byte_count_current-1] == 0xE0) &&
            (RES[byte_count_current-2] == 0xE0) &&
            (RES[byte_count_current-3] == 0xE0) &&
            (RES[byte_count_current-4] == 0xE0) &&
            (RES[byte_count_current-5] == 0xE0)
    )
    {
        /*
         * This is a command for display ram code information.
         */
        byte_count = 0;
        code_information_ram();
    }
    else if (
            (byte_count_current >= 5) &&
            (RES[byte_count_current-1] == 0xE1) &&
            (RES[byte_count_current-2] == 0xE1) &&
            (RES[byte_count_current-3] == 0xE1) &&
            (RES[byte_count_current-4] == 0xE1) &&
            (RES[byte_count_current-5] == 0xE1)
    )
    {
        /*
         * This is a command for display eeprom code information.
         */
        byte_count = 0;
        code_information_eeprom();
    }
    else if (
            (byte_count_current >= 5) &&
            (RES[byte_count_current-1] == 0xE5) &&
            (RES[byte_count_current-2] == 0xE5) &&
            (RES[byte_count_current-3] == 0xE5) &&
            (RES[byte_count_current-4] == 0xE5) &&
            (RES[byte_count_current-5] == 0xE5)
    )
    {
        /*
         * This is a command for Write data to EEPROM.
         */
        byte_count = 0;
        write_code(code_data, code_length);
    }
    else if (
            (byte_count_current >= 5) &&
            (RES[byte_count_current-1] == 0xE6) &&
            (RES[byte_count_current-2] == 0xE6) &&
            (RES[byte_count_current-3] == 0xE6) &&
            (RES[byte_count_current-4] == 0xE6) &&
            (RES[byte_count_current-5] == 0xE6)
    )
    {
        /*
         * This is a command for load data from EEPROM.
         */
        byte_count = 0;
        load_code();
    }
    else
    {
        /*
         * Give more.
         */
    }
}


/******************************************************************************
 * Initial
 *****************************************************************************/
void initial()
{
    // Timer 1 mode 2
    TMOD = 0x20;

    // Timer 1, serial port bitrate 9600
    TH1 = 0xFD;
    TL1 = TH1;

    // Serial port mode 1
    SCON = 0x50;
    PCON = 0x00;

    // Serial port interrupt open.
    ES = 1;

    // Timer 0 mode 2
    TMOD = TMOD | 0x02;

    // Timer 0, flow time is 50 us
    TH0 = 0xA4;
    TL0 = TH0;

    // Timer0 interrupt
    ET0 = 1;

    // Main interrupt open.
    EA = 1;

    // Timer 1 begin to count.
    TR1 = 1;

    // ISP
    ISP_CONTR = 0x9A;
    ISP_ADDRH = 0x21;
    ISP_ADDRL = 0x00;
}


/******************************************************************************
 * Main
 *****************************************************************************/
void main()
{
    initial();

    welcome();

    load_code();

    while (1)
    {
        process_data();
    }
}


/******************************************************************************
 * Interrupt for serial port UART
 *****************************************************************************/
void uart_interrupt0() interrupt 4
{
    if (RI == 1)
    {
        /*
         * Is a recieve interrupt.
         */
        RI = 0;

        RES[byte_count] = SBUF;
        byte_count = byte_count + 1;

        if (byte_count > MAX_CODE_LENGTH)
        {
            byte_count = 0;
        }
        else
        {
        }

        if (
                (byte_count >= 5) &&
                (RES[byte_count-1] == 0xEA) &&
                (RES[byte_count-2] == 0xEA) &&
                (RES[byte_count-3] == 0xEA) &&
                (RES[byte_count-4] == 0xEA) &&
                (RES[byte_count-5] == 0xEA)
        )
        {
            /*
             * MCU reset, even though when running.
             */
            byte_count = 0;
            send_string_interrupt("**** RESET ****\n");
            send_string_interrupt("\n");
            ISP_CONTR = ISP_CONTR | 0x20;
        }
        else
        {
        }
    }

    if (TI == 1)
    {
        /*
         * Is a transport interrupt.
         */
    }
}

/******************************************************************************
 * Interrupt for Timer 0 overflow
 *****************************************************************************/
void timer0_interrupt() interrupt 1
{
    timer0_number = timer0_number + 1;
}