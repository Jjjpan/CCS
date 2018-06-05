#include <msp430.h>
//#include <MacroAndConst.h>
//#include <msp430.h>         //引入MSP430标准函数库
#include "DELAY.h"
#include "Paper_Display.h"
#include "key.h"
#define TimesNewRoman 0
#define Arial 1
#define ArialBlack 2
#define size8 0
#define size16 1
#define standard 80

double length;

void initClock()
{
     UCSCTL6 &= ~XT1OFF; //启动XT1
     P5SEL |= BIT2 + BIT3; //XT2引脚功能选择
     UCSCTL6 &= ~XT2OFF;          //打开XT2
     __bis_SR_register(SCG0);
     UCSCTL0 = DCO0+DCO1+DCO2+DCO3+DCO4;
     UCSCTL1 = DCORSEL_4;       //DCO频率范围在28.2MHZ以下
     UCSCTL2 = FLLD_5 + 1;       //D=16，N=1
     UCSCTL3 = SELREF_5 + FLLREFDIV_3;    //n=8,FLLREFCLK时钟源为XT2CLK；DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
     UCSCTL4 = SELA_4 + SELS_3 +SELM_3;    //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
     UCSCTL5 = DIVA_5 +DIVS_1;      //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
                 //最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ

//   __bic_SR_register(SCG0);                   //Enable the FLL control loop

}
void InitAD()
{
    ADC12CTL0 |= ADC12MSC;                                                                              //自动循环采样转换
    ADC12CTL0 |= ADC12ON;                                                                                   //启动ADC12模块
    ADC12CTL1 |= ADC12CONSEQ_3 ;                                                                        //选择序列通道多次循环采样转换
    ADC12CTL1 |= ADC12SHP;                                                                              //采样保持模式
    ADC12CTL1  |= ADC12CSTARTADD_0;
    ADC12MCTL0 |=ADC12INCH_0;   //通道选择
    ADC12MCTL1 |=ADC12INCH_1;
    ADC12MCTL2 |=ADC12INCH_2;
    ADC12MCTL3 |=ADC12INCH_3;
    ADC12MCTL4 |=ADC12INCH_4;
    ADC12MCTL5 |=ADC12INCH_5+ADC12EOS;
    ADC12CTL0 |= ADC12ENC;
}

void IOinit(void)
{
    P6DIR |= BIT3+BIT4;
        P3DIR |= BIT5+BIT7;
        P7DIR |= BIT4;
        P8DIR |= BIT1;

        P1REN |= BIT3;
        P1OUT |= BIT3;
        P1IES &= ~BIT3;
        P1IFG &= ~BIT3;
        P1IE |= BIT3;

        P2REN |= BIT6;
        P2OUT |= BIT6;
        P2IES &= ~BIT6;
        P2IFG &= ~BIT6;
        P2IE |= BIT6;

        return;

    return;
}

void trig(void)
{
    P1DIR|=BIT2;
    P1OUT|=BIT2;
    Delay_mS(1);
    P1OUT^=BIT2;
    return;
}

int getAD(void)
{
    ADC12CTL0 |= ADC12SC;
    unsigned int value = ADC12MEM1;
    return value;
}

double distanceCalc(void)
{
    double distance = length*0.667;
    return distance;
}

void echo(void)
{
    while(getAD()<standard);
    //__enable_interrupt();
     unsigned int time_loop=0;
    while(getAD()>standard)
    {
        time_loop++;
        Delay_10nS(1);
    }
    length=(double)time_loop;
    return;
}

void led(double value)
{
    if(value > 30)
        P8OUT |= BIT1;
    else
        P8OUT &= ~BIT1;
    if(value >= 40)
        P3OUT |= BIT7;
    else
        P3OUT &= ~BIT7;
    if(value >= 50)
        P7OUT |= BIT4;
    else
        P7OUT &= ~BIT4;
    if(value >= 180)
        P6OUT |= BIT3;
    else
        P6OUT &= ~BIT3;
    if(value >= 200)
        P6OUT |= BIT4;
    else
        P6OUT &= ~BIT4;
    if(value >= 400)
        P3OUT |= BIT5;
    else
        P3OUT &= ~BIT5;
}

double getDistance(void)
{
    TA0CTL |= MC_1 + TASSEL_2 + TACLR;
    IOinit();
    InitAD();
    trig();
    echo();
    double distance=distanceCalc();
    led(distance);
    return distance;
}

double speedCalc(double d1,double d2,int delay)
{
    return d1>d2?(d1-d2)/delay*1000:(d2-d1)/delay*1000;
}

double getSpeed(void)
{
    int delayTime=100;//100*10ns
    double D1=getDistance();
    Delay_10nS(100);
    double D2=getDistance();
    double speed=speedCalc(D1,D2,delayTime);
    return speed;
}

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
    sprintf(buffer,"%f",output);
    display(buffer, 100, 0,TimesNewRoman,size8,1,0);
    DIS_IMG(1);
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
    IOinit();
    InitAD();

    while(1)
    {
        if((P1IN & BIT3)==0)
            __enable_interrupt();

        if((P2IN & BIT6)==0)
            __enable_interrupt();
    }
    return 0;
}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void)
{

    trig();
    echo();
    double dis=distanceCalc();
    led(dis);
}
#pragma vector = PORT2_VECTOR
__interrupt void Port_2(void)
{
    InkInit();
    double speed=getSpeed();
    showUp(speed);
}
