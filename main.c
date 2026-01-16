#include <LPC17xx.h>
#include <stdio.h>

/* ===================== PIN DEFINITIONS ===================== */

/* Relay & Buzzer */
#define FAN_RELAY_PIN   (1<<28)    // P1.28 (Fan Relay)
#define BUZZER_PIN      (1<<27)    // P1.27 (Buzzer)

/* LCD (16x2, 4-bit mode) - GPIO0 */
#define RS (1<<10)
#define EN (1<<11)
#define D4 (1<<19)
#define D5 (1<<20)
#define D6 (1<<21)
#define D7 (1<<22)

/* ===================== THRESHOLDS ===================== */
#define GAS_ON_THRESHOLD     30.0f   // Fan ON when gas > 30%
#define GAS_OFF_THRESHOLD    25.0f   // Fan OFF when gas < 25%

#define TEMP_ON_THRESHOLD    45.0f   // Fan ON when Temp > 45C
#define TEMP_OFF_THRESHOLD   40.0f   // Fan OFF when Temp <= 40C

/* ===================== DELAY ===================== */
void wait_loop(volatile unsigned int t)
{
    while(t--)
    {
        volatile unsigned int i = 10000;
        while(i--) {}
    }
}

/* ===================== LCD FUNCTIONS ===================== */
void LCD_Pulse(void)
{
    LPC_GPIO0->FIOSET = EN;
    wait_loop(1);
    LPC_GPIO0->FIOCLR = EN;
}

void LCD_Send4(unsigned int nibble)
{
    LPC_GPIO0->FIOCLR = D4 | D5 | D6 | D7;

    if(nibble & 0x1) LPC_GPIO0->FIOSET = D4;
    if(nibble & 0x2) LPC_GPIO0->FIOSET = D5;
    if(nibble & 0x4) LPC_GPIO0->FIOSET = D6;
    if(nibble & 0x8) LPC_GPIO0->FIOSET = D7;
}

void LCD_Command(unsigned char cmd)
{
    LPC_GPIO0->FIOCLR = RS;

    LCD_Send4((cmd >> 4) & 0x0F);
    LCD_Pulse();

    LCD_Send4(cmd & 0x0F);
    LCD_Pulse();

    wait_loop(10);
}

void LCD_Data(unsigned char data)
{
    LPC_GPIO0->FIOSET = RS;

    LCD_Send4((data >> 4) & 0x0F);
    LCD_Pulse();

    LCD_Send4(data & 0x0F);
    LCD_Pulse();

    wait_loop(5);
}

void LCD_Init(void)
{
    LPC_GPIO0->FIODIR |= RS | EN | D4 | D5 | D6 | D7;

    wait_loop(200);

    LCD_Command(0x28);  // 4-bit, 2 line
    LCD_Command(0x0C);  // Display ON, Cursor OFF
    LCD_Command(0x06);  // Entry mode
    LCD_Command(0x01);  // Clear LCD
    wait_loop(50);
}

void LCD_String(const char *s)
{
    while(*s) LCD_Data(*s++);
}

void LCD_SetCursor(int row, int col)
{
    LCD_Command((row == 0 ? 0x80 : 0xC0) + col);
}

/* ===================== ADC FUNCTIONS ===================== */
/*
    Gas Sensor  -> P0.23 -> AD0.0
    LM35 Temp   -> P0.25 -> AD0.2
*/
void ADC_Init(void)
{
    LPC_SC->PCONP |= (1<<12); // Power to ADC

    /* ADC Pin Select */
    LPC_PINCON->PINSEL1 |= (1<<14); // P0.23 -> AD0.0
    LPC_PINCON->PINSEL1 |= (1<<18); // P0.25 -> AD0.2

    /* ADC Control: PDN=1, CLKDIV=7 */
    LPC_ADC->ADCR = (1<<21) | (7<<8);
}

unsigned int ADC_ReadChannel(unsigned int channel)
{
    LPC_ADC->ADCR &= ~0xFF;           // Clear channel selection
    LPC_ADC->ADCR |= (1<<channel);    // Select channel
    LPC_ADC->ADCR |= (1<<24);         // Start conversion

    while(!(LPC_ADC->ADGDR & (1U<<31))); // Wait until DONE

    return (LPC_ADC->ADGDR >> 4) & 0xFFF;
}

/* ===================== SENSOR FUNCTIONS ===================== */
float LM35_GetTempC(void)
{
    unsigned int adc = ADC_ReadChannel(2);  // AD0.2
    return ((float)adc * 3.3f / 4095.0f) * 100.0f;
}

float Gas_GetPercent(void)
{
    unsigned int sum = 0;

    for(int i = 0; i < 5; i++)
        sum += ADC_ReadChannel(0); // AD0.0

    float avg = (float)sum / 5.0f;
    return (avg / 4095.0f) * 100.0f;
}

/* ===================== MAIN ===================== */
int main(void)
{
    char line[17];
    int fan_state = 0;

    ADC_Init();
    LCD_Init();

    /* Output pins */
    LPC_GPIO1->FIODIR |= FAN_RELAY_PIN | BUZZER_PIN;

    /* Initially OFF (assuming Active LOW relay module, Active HIGH buzzer) */
    LPC_GPIO1->FIOSET = FAN_RELAY_PIN;  // Fan OFF
    LPC_GPIO1->FIOCLR = BUZZER_PIN;     // Buzzer OFF

    LCD_SetCursor(0,0);
    LCD_String("Air Quality Mon");

    wait_loop(300);

    while(1)
    {
        float temp = LM35_GetTempC();
        float gas_percent = Gas_GetPercent();

        /* ---------- FAN + BUZZER CONTROL WITH HYSTERESIS ---------- */
        if((gas_percent > GAS_ON_THRESHOLD) || (temp > TEMP_ON_THRESHOLD))
        {
            fan_state = 1;
        }
        else if((gas_percent < GAS_OFF_THRESHOLD) && (temp <= TEMP_OFF_THRESHOLD))
        {
            fan_state = 0;
        }

        if(fan_state)
        {
            LPC_GPIO1->FIOCLR = FAN_RELAY_PIN;  // Fan ON (Active LOW)
            LPC_GPIO1->FIOSET = BUZZER_PIN;     // Buzzer ON
        }
        else
        {
            LPC_GPIO1->FIOSET = FAN_RELAY_PIN;  // Fan OFF
            LPC_GPIO1->FIOCLR = BUZZER_PIN;     // Buzzer OFF
        }

        /* ---------- LCD UPDATE ---------- */
        LCD_SetCursor(0,0);
        sprintf(line, "T:%2.1fC       ", temp);
        LCD_String(line);

        LCD_SetCursor(1,0);
        sprintf(line, "Gas:%2.1f%%     ", gas_percent);
        LCD_String(line);

        wait_loop(200);
    }
}
