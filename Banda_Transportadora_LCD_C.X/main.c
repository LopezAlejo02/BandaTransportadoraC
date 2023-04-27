/* 
 * File:   main.c
 * Author: User
 *
 * Created on 16 de abril de 2023, 12:12 PM
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
#define Sensor_metalico PORTAbits.RA0
#define Sensor_no_metalico PORTAbits.RA1
#define mode PORTAbits.RA2
#define CH 11 //Definición del canal de lectura del ADC ANx

//Variables globales
int segundos = 0;

//Funciones de interrupciones
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void){
    segundos++;
    IFS0bits.T1IF = 0;
};

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

void initAdc1(){
    AD1PCFG = 0b1111011111111111; //Seseleccionan los pines analógicos
    AD1CON1 = 0; //Se selecciona el control manual de la secuencia de conversión
    AD1CSSL = 0; //No se requiere scaneo
    AD1CON2 = 0; //Se selecciona el MUX A, y AVss y AVdd son usados como Vref+/-
    AD1CON3 = 0x1F01; //Se selecciona el Tad = 2* Tcy (Tad > 75ns)
    AD1CON1bits.ADON = 1; //Se enciende el ADC
}

//Funciones generales

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

void Mode(int mod, int num_metalicos, int num_no_metalicos, int total_objetos, int tiempo_promedio){
    
    if(mod == 1){
       clearLcd();
       goTo(1,1);
       sendStr("Metalicos: ");
       sendNum(num_metalicos);
       __delay_ms(2);
       goTo(2,1);
       sendStr("No metal.: ");
       sendNum(num_no_metalicos);
    }
    else{
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
}

int main() {
    //Configuración de puertos
        AD1PCFG = 0b1111111111111111; //Todo como digital
        TRISB = 0b0010000000000000; // Puerto B como salidas menos RB13
        TRISA = 1; // Puerto A como entradas
    //Configuración de periféricos
        //Timer1 cuenta hasta 1s
          initTimer1(64, 62500, 1);
        //Timer1 cuenta hasta 1s
          initTimer2(1, 25, 0);
        //ADC1
          initAdc1();
    //Configuración de dispositivos adicionales
        //LCD
        onLcd();
        initLcd();
    
    //Declaración de variables
        int num_metalicos = 0;
        int num_no_metalicos = 0;
        int mod = 1;
        int total_objetos;
        int tiempo_promedio;
        int lec_adc;
        long dutyCycle;

    //Bienvenida
        __delay_ms(1000);
        goTo(1,4);
        sendStr("Bienvenido");
        goTo(2,4);
        sendStr("al sistema");
        __delay_ms(1000);
        clearLcd();
        sendStr("Metalicos: ");
        sendNum(num_metalicos);
        goTo(2,1);
        sendStr("No metal.: ");
        sendNum(num_no_metalicos);
    while (1){
        lec_adc = leerAdc(CH);
        dutyCycle = calcDutyPwm(lec_adc);
        if(Sensor_metalico == 1){  
            while(Sensor_metalico);
            num_metalicos = incrementar(num_metalicos);
            total_objetos = incrementar(total_objetos);
            tiempo_promedio = calcularTProm(tiempo_promedio, total_objetos, segundos);
            segundos = 0;
            //obj_min = objetosPorMinuto(minutos, total_objetos);
            if(mod == 1){
                goTo(1,12);
                sendNum(num_metalicos);
            }
        }
        else if(Sensor_no_metalico == 1){
            
            while(Sensor_no_metalico);
            num_no_metalicos= incrementar(num_no_metalicos);
            total_objetos = incrementar(total_objetos);
            //obj_min = objetosPorMinuto(minutos, total_objetos);
            tiempo_promedio = calcularTProm(tiempo_promedio, total_objetos, segundos);
            segundos = 0;
            if(mod == 1){
                goTo(2,12);
                sendNum(num_no_metalicos);
            }
        }
        if(mode){
            while(mode);
            mod = !mod;
            Mode(mod, num_metalicos, num_no_metalicos, total_objetos, tiempo_promedio);
        }
        if(mod == 0){
           goTo(1,13);
           sendNum(lec_adc);
           sendStr("  ");
           goTo(2,12);
           sendNum(dutyCycle);
           sendStr("  ");
        }
    }
        return 0;
}