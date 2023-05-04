/* 
 * File:   main.c
 * Author: User
 *
 * Created on 16 de abril de 2023, 12:12 PM
 */

/*
 Timer 1 -> Tiempo promedio
 Timer 2 -> ADC
 Timer 3 -> PWM
 Timer 4 -> Sensor de color
 */
//Caracteristicas del hardware
#define FOSC 8000000
//Librerias
#include <stdio.h>
#include <stdlib.h>
#define FCY (FOSC/2)
#include <libpic30.h>
#include "config.h"
#include "LCD.h"
#include <math.h>

//Defines
#define Sensor_metalico PORTAbits.RA1
#define Sensor_no_metalico PORTAbits.RA2
#define mode PORTAbits.RA3
#define CH 0 //Definición del canal de lectura del ADC ANx
//#define lectura_color 
#define S2 PORTBbits.RB13
#define S3 PORTBbits.RB14

//Variables globales
int segundos = 0;

//Funciones de interrupciones
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void){
    segundos++;
    IFS0bits.T1IF = 0;
};

//void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
//    IFS0bits.T3IF = 0;
//};
//
//void __attribute__((__interrupt__, no_auto_psv)) _OC1Interrupt(void){   
//    IFS0bits.OC1IF = 0;
//}

//Funciones de configuración de periféricos

void initTimer1(int prescaler, int periodo, int interrupcion){
        int presc_select = 0b00;
        if(prescaler == 1){
            presc_select = 0b00;
        }
        else if(prescaler == 8){
            presc_select = 0b01;
        }
        else if(prescaler == 64){
            presc_select = 0b10;
        }
        else if(prescaler == 256){
            presc_select = 0b11;
        }
        T1CONbits.TON = 0; // Apagar el timer 1
        T1CONbits.TCS = 0; //Selección del oscilador interno como fuente
        T1CONbits.TCKPS = presc_select; //Configuración del prescaler 
        TMR1 = 0; //Limpiar el registro del timer
        PR1 = periodo; //Fijamos el valor del periodo para que el timer cuente hasta la variable "periodo"
        IFS0bits.T1IF = 0; //Bandera de timer a cero
        IEC0bits.T1IE = interrupcion; //Habilitación de la interrupción
        T1CONbits.TON = 1; // Encender el timer 1
}

void initTimer2(int prescaler, int periodo, int interrupcion){
        int presc_select = 0b00;
        if(prescaler == 1){
            presc_select = 0b00;
        }
        else if(prescaler == 8){
            presc_select = 0b01;
        }
        else if(prescaler == 64){
            presc_select = 0b10;
        }
        else if(prescaler == 256){
            presc_select = 0b11;
        }
        T2CONbits.TON = 0; // Apagar el timer 2
        T2CONbits.TCS = 0; //Selección del oscilador interno como fuente
        T2CONbits.TCKPS = presc_select; //Configuración del prescaler 
        TMR2 = 0; //Limpiar el registro del timer
        PR2 = periodo; //Fijamos el valor del periodo para que el timer cuente hasta la variable "periodo"
        IFS0bits.T2IF = 0; //Bandera de timer a cero
        IEC0bits.T2IE = interrupcion; //Habilitación de la interrupción
        T2CONbits.TON = 1; // Encender el timer 2
}

void initTimer3(int prescaler, int periodo, int interrupcion){
        int presc_select = 0b00;
        if(prescaler == 1){
            presc_select = 0b00;
        }
        else if(prescaler == 8){
            presc_select = 0b01;
        }
        else if(prescaler == 64){
            presc_select = 0b10;
        }
        else if(prescaler == 256){
            presc_select = 0b11;
        }
        T3CONbits.TON = 0; // Apagar el timer 3
        T3CONbits.TCS = 0; //Selección del oscilador interno como fuente
        T3CONbits.TCKPS = presc_select; //Configuración del prescaler 
        TMR3 = 0; //Limpiar el registro del timer
        PR3 = periodo; //Fijamos el valor del periodo para que el timer cuente hasta la variable "periodo"
        IFS0bits.T3IF = 0; //Bandera de timer a cero
        IEC0bits.T3IE = interrupcion; //Habilitación de la interrupción
        T3CONbits.TON = 1; // Encender el timer 3
}

void initTimer4(int prescaler, int periodo){
    int presc_select = 0b00;
    if(prescaler == 1){
        presc_select = 0b00;
    }
    else if(prescaler == 8){
        presc_select = 0b01;
    }
    else if(prescaler == 64){
        presc_select = 0b10;
    }
    else if(prescaler == 256){
        presc_select = 0b11;
    }
    T4CONbits.TON = 0; // Apagar el timer 2
    T4CONbits.TCS = 0; //Selección del oscilador interno como fuente
    T4CONbits.TCKPS = presc_select; //Configuración del prescaler 
    TMR4 = 0; //Limpiar el registro del timer
    PR4 = periodo; //Fijamos el valor del periodo para que el timer cuente hasta la variable "periodo"
    T4CONbits.TON = 1; // Encender el timer 2
}

void initAdc1(){
    AD1PCFG = 0b1111111111111110; //Se seleccionan los pines analógicos
    AD1CON1 = 0; //Se selecciona el control manual de la secuencia de conversión
    AD1CSSL = 0; //No se requiere scaneo
    AD1CON2 = 0; //Se selecciona el MUX A, y AVss y AVdd son usados como Vref+/-
    AD1CON3 = 0x1F01; //Se selecciona el Tad = 2* Tcy (Tad > 75ns)
    AD1CON1bits.ADON = 1; //Se enciende el ADC
}

void initPwm(void)
{   
    // ENFLT0 disabled; ENFLT1 disabled; OCFLT2 disabled; ENFLT2 disabled; OCSIDL disabled; OCM Edge-Aligned PWM mode; OCFLT1 disabled; OCFLT0 disabled; OCTSEL TMR3; TRIGMODE Only Software; 
    OC1CON1 = 0x406;
    // SYNCSEL TMR3; TRIGSTAT disabled; DCB Start of instruction cycle; OCINV disabled; OCTRIG Trigger; OC32 disabled; FLTOUT disabled; OCTRIS disabled; FLTMD Cycle; FLTTRIEN disabled; 
    OC1CON2 = 0x8D;
    // OC1RS 1999; 
    OC1RS = 1999;
    // OC1R 600; 
    OC1R = 1000;
    // OC1TMR 0; 
    OC1TMR = 0x00;
    RPOR7bits.RP15R = 0x0012;    //RB15->OC1:OC1
}

//Funciones generales

void cambiarDuty(int duty){
    
    OC1R = (int)((1999/100)*(duty));
}

long calcDutyPwm(long val){
    long resolucion = 1023; //La resolucion verdadera es 1024, pero se fija en 1023 para que el duty cycle pueda ser 100%, si se deja en 1024 el duty cycle máximo es de 99%
    return (long)((val*100)/resolucion);
}

int leerAdc(int an){
    AD1CHS = an; //Selecciona el canal de entrada analogo
    AD1CON1bits.SAMP = 1; //Inicia el muestreo
    TMR2 = 0; //Resetea el contador del timer 2
    IFS0bits.T2IF = 0; //Pone la bandera del timer 2 en 0
    while(!IFS0bits.T2IF); //Espera hasta que la bandera del timer 2 se ponga en 1 (cuenta hasta 6.25us)
    AD1CON1bits.SAMP = 0; //Empieza la conversión
    while(!AD1CON1bits.DONE); //Espera hasta que la conversión se complete
    return ADC1BUF0; //Retorna el valor de la conversión
}

int calcularTProm(int tiempo_promedio, int total_objetos, int tiempo_tramo){
    tiempo_promedio = (((total_objetos - 1)*tiempo_promedio) + tiempo_tramo)/total_objetos;
    return (int)(tiempo_promedio);
}

int incrementar(int numero){
    numero++; //incrementa el número
    return numero;
}

//long leerColor(int num_muestras){
//    T4CONbits.TON = 1;
//    long periodo = 0;
//    int i = 0;
//    while(!(i == num_muestras)){
//        if(lectura_color){
//            while(lectura_color);
//            periodo += TMR4;
//            TMR4 = 0;
//            i++;
//        }
//    }
//    T4CONbits.TON = 0;
//    return (long)(periodo/num_muestras);
//}
//
//int initPeriodo(int S_2, int S_3){
//    S2 = S_2;
//    S3 = S_3;    
//    return leerColor(100);
//}

void incrementarColor(int periodo, int periodo_ref_rojo, int periodo_ref_azul, int periodo_ref_verde, int objetos_rojos){
    if((periodo_ref_rojo - periodo)>(periodo_ref_azul - periodo)){
        if((periodo_ref_rojo - periodo)>(periodo_ref_verde - periodo)){
            objetos_rojos++;
        }
    }
}

void Mode(int mod, int num_metalicos, int num_no_metalicos, int total_objetos, int tiempo_promedio, int dutyCycle, int lec_adc){
    if(mod == 0){
       clearLcd();
       goTo(1,1);
       sendStr("Metalicos: ");
       sendNum(num_metalicos);
       __delay_ms(2);
       goTo(2,1);
       sendStr("No metal.: ");
       sendNum(num_no_metalicos);
    }
    else if(mod == 1){
       __delay_ms(2);
       clearLcd();
       goTo(1,1);
       __delay_ms(1);
       sendStr("t promedio: ");
       sendNum(tiempo_promedio);
       __delay_ms(2);
       goTo(2,1);
       __delay_ms(1);
       sendStr("Tot. obj.: ");
       __delay_ms(1);
       goTo(2,12);
       __delay_ms(1);
       sendNum(total_objetos);
    }
    else if(mod == 2){
       __delay_ms(2);
       clearLcd();
       goTo(1,1);
       __delay_ms(1);
       sendStr("Lectura ADC: ");
       sendNum(lec_adc);
       __delay_ms(2);
       goTo(2,1);
       __delay_ms(1);
       sendStr("Duty cycle: ");
       __delay_ms(1);
       goTo(2,12);
       __delay_ms(1);
       sendNum(dutyCycle);
    }
    else if(mod == 3){
       __delay_ms(2);
       clearLcd();
       goTo(1,1);
       __delay_ms(1);
       sendStr("Frec.: ");
       sendNum(tiempo_promedio);
       __delay_ms(2);
       goTo(2,1);
       __delay_ms(1);
       sendStr("Test: ");
       __delay_ms(1);
       goTo(2,12);
       __delay_ms(1);
       sendNum(total_objetos);
    }
}

int main() {
    //Configuración de puertos
        AD1PCFG = 0b1111111111111110; //Todo como digital
        TRISB = 0b1000000000000000; // Puerto B como salidas menos RB13
        TRISA = 1; // Puerto A como entradas
    //Configuración de periféricos
        //Timer1 cuenta hasta 1s
          initTimer1(64, 62500, 1);
        //Timer cuenta hasta 6.25us
          initTimer2(1, 25, 0);
        //Timer cuenta hasta 6.25us
          initTimer3(1, 1999, 0);
        //Timer cuenta hasta 1ms
          initTimer4(1, 4000);
          T4CONbits.TON = 0;
          TMR4 = 0;
        //ADC1  
          initAdc1();
        //PWM
          initPwm();
    //Configuración de dispositivos adicionales
        //LCD
        onLcd();
        initLcd();
    
    //Declaración de variables
        int num_metalicos = 0;
        int num_no_metalicos = 0;
        int mod = 0;
        int total_objetos;
        int tiempo_promedio;
        int lec_adc;
        long dutyCycle;
        long periodo = 0;
        long periodo_ref_rojo = 0;
        long periodo_ref_azul = 0;
        long periodo_ref_verde = 0;
    //Bienvenida
        __delay_ms(1000);
        goTo(1,4);
        sendStr("Bienvenido");
        goTo(2,4);
        sendStr("al sistema");
        __delay_ms(1000);
        clearLcd();
        __delay_ms(1000);
        goTo(1,2);
        sendStr("Sincronizando.");
        goTo(2,7);
        sendStr("....");
        //periodo_ref_rojo = initPeriodo(0,0);
        __delay_ms(100);
        //periodo_ref_azul = initPeriodo(0,1);
        __delay_ms(100);
        //periodo_ref_verde = initPeriodo(1,0);
        __delay_ms(100);
        clearLcd();
        goTo(1,1);
        sendStr("Metalicos: ");
        sendNum(num_metalicos);
        goTo(2,1);
        sendStr("No metal.: ");
        sendNum(num_no_metalicos);
     
    while (1){
        lec_adc = leerAdc(CH);
        dutyCycle = calcDutyPwm(lec_adc);
        cambiarDuty(dutyCycle);
//        if(dutyCycle > 50){
//            cambiarDuty(1800);
//        }
//        else{
//            cambiarDuty(400);
//        }
        if(Sensor_metalico == 1){  
            //periodo = leerColor(100);
            while(Sensor_metalico);
            num_metalicos = incrementar(num_metalicos);
            total_objetos = incrementar(total_objetos);
            tiempo_promedio = calcularTProm(tiempo_promedio, total_objetos, segundos);
            segundos = 0;
            if(mod == 0){
                goTo(1,12);
                sendNum(num_metalicos);
            }
        }
        else if(Sensor_no_metalico == 1){
            //periodo = leerColor(100);
            while(Sensor_no_metalico);
            num_no_metalicos= incrementar(num_no_metalicos);
            total_objetos = incrementar(total_objetos);
            tiempo_promedio = calcularTProm(tiempo_promedio, total_objetos, segundos);
            segundos = 0;
            if(mod == 0){
                goTo(2,12);
                sendNum(num_no_metalicos);
            }
        }
        if(mode){
            while(mode);
            mod++;            
            if(mod == 4){
                mod = 0;
            }
           Mode(mod, num_metalicos, num_no_metalicos, total_objetos, tiempo_promedio, dutyCycle, lec_adc);
        }
        if(mod == 1){
           goTo(1,13);
           sendNum(tiempo_promedio);
           sendStr("  ");
           goTo(2,12);
           sendNum(total_objetos);
           sendStr("  ");
        }
        if(mod == 2){
           goTo(1,13);
           sendNum(lec_adc);
           sendStr("  ");
           goTo(2,12);
           sendNum(dutyCycle);
           sendStr("  ");
        }
        if(mod == 3){
           goTo(1,8);
           sendNum(periodo);
           sendStr("  ");
           goTo(2,12);
           sendNum(segundos);
           sendStr("  ");
        }
    }
        return 0;
}