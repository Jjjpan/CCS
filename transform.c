#include <msp430.h>
#include <stdio.h>
#include <string.h>
//#include <MacroAndConst.h>
#include "Paper_Display.h"
/**
 * a function to transform a variable from double to char
 */
#define TimesNewRoman 0
#define Arial 1
#define ArialBlack 2
#define size8 0
#define size16 1
#define standard 80

double length;
char buffer[6];
char ERROR[6]="error";


void InkInit()
{
    P8DIR |= BIT1;
    P8OUT &=~ BIT1;
    P2IN|=BIT2;
    //initClock();
    PaperIO_Int();
    INIT_SSD1673();
    //Init_buff();
    Init_buff_write();
    return;
}

void showUp(double output)
{
    InkInit();
    char buffer[20];
    sprintf(buffer,"%d",output);
    display(buffer, 100, 0,TimesNewRoman,size8,1,0);
    DIS_IMG(1);
    return;
}

void bufferImport(double output)
{
    int i;int j;
        if(output>9999)
            memcpy(buffer,ERROR,sizeof(ERROR));
        else
        {
            for(i=1;i<output;i*=10);
            if(i==1)
            {
                buffer[0]='0';
                buffer[1]='.';
                for(j=2;j<5;j++)
                {
                    output*=10;
                    buffer[j]='0'+((int)output)%10;
                }
            }
            else
            {
                i/=10;j=0;
                while(1)
                {
                    buffer[j]=output/i+'0';
                    output-=((int)output)/10*10;
                    j++;

                    if(i==1)
                        break;
                    i/=10;
                }
                if(j==5)buffer[j]='\0';
                else
                {

                    buffer[j]='.';
                    j++;
                    while(j<5)
                    {
                        output*=10;
                        buffer[j]='0'+((int)output)%10;
                        j++;
                    }
                }
            }
            buffer[5]='\0';
        }
    return;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    TA0CTL |= MC_1 + TASSEL_2 + TACLR;
    P8DIR |= BIT1;
        P8OUT &=~ BIT1;
        P2IN|=BIT2;
        //initClock();
        PaperIO_Int();
        INIT_SSD1673();
        //Init_buff();
        Init_buff_write();
    bufferImport(31.4159);
    //sprintf(buffer,"%f",output);
    display(buffer, 85, 0,TimesNewRoman,size16,0,0);
    bufferImport(0.046);
    display(buffer, 85, 40,TimesNewRoman,size16,0,0);


    DIS_IMG(1);
}
