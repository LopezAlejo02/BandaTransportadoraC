/* 
 * File:   main.c
 * Author: Mateo López
 *
 * Created on 9 de marzo de 2023, 02:49 PM
 */
#include "config.h"
//#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#define FCY 4000000
#include <libpic30.h>

/*
 * 
 */
 main() {
    //Configuración del micro
    AD1PCFG = 0xFFF; //Todo como digital
    TRISB = 0; // Puerto B como salidas
    TRISA = 1; // Puerto A como entradas
    
    
    //Declaración de variables
    int unidades_metalicas = 1;
    int unidades_no_metalicas = 1;
    int decenas_metalicas = 0;
    int decenas_no_metalicas = 0;
    int UD = 0;
    
    //Ciclo infinito
    while(1){
        
        if(PORTAbits.RA0 == 0){
            
            while(!PORTAbits.RA0);
            //Incrementar unidades
            unidades_metalicas = incrementar(unidades_metalicas);
            
            if(unidades_metalicas == 10){
                unidades_metalicas = 0;
                decenas_metalicas = incrementar(decenas_metalicas);
            }
            
        }
        else if(PORTAbits.RA1 == 0){
            
            while(!PORTAbits.RA1);
            //Incrementar unidades
            unidades_no_metalicas = incrementar(unidades_no_metalicas);
            if(unidades_no_metalicas == 10){
                unidades_no_metalicas = 0;
                decenas_no_metalicas = incrementar(decenas_no_metalicas);
            }
            
        }
        
        if(PORTAbits.RA2 == 0){
        visualizacion(UD,unidades_metalicas,decenas_metalicas);
        }
        else{
        visualizacion(UD,unidades_no_metalicas,decenas_no_metalicas);
        }
        UD = !UD;
        LATB = 0;
    }
}
 
 void visualizacion(int UD, int unidades, int decenas){
        if(UD == 1){
            mostrarNumero(unidades);
            encenderDisplay(UD);
        }
        if(UD == 0){
            mostrarNumero(decenas);
            encenderDisplay(UD);
        }
 }

 void encenderDisplay(int bandera){
     if(bandera == 0){
         LATBbits.LATB10 = 1;
         LATBbits.LATB9 = 0;
     }
     if(bandera == 1){
         LATBbits.LATB9 = 1;
         LATBbits.LATB10 = 0;
     }
     __delay_ms(5);
 }
 
 int incrementar(int numero){
     numero++; //incrementa el número
     return numero;
 }
 
 void mostrarNumero(int numero){
     switch(numero){
         case 0:
             zero();
         break;
         case 1:
             uno();
         break;
         case 2:
             dos();
         break;
         case 3:
             tres();
         break;
         case 4:
             cuatro();
         break;
         case 5:
             cinco();
         break;
         case 6:
             seis();
         break;
         case 7:
             siete();
         break;
         case 8:
             ocho();
         break;
         case 9:
             nueve();
         break;
     }
     
 }
 
 void zero(){
     LATB = 0b00111111; //g-fedcba
 }
 void uno(){
     LATB = 0b00000110; //g-fedcba
 }
 void dos(){
     LATB = 0b10011011; //g-fedcba
 }
 void tres(){
     LATB = 0b10001111; //g-fedcba
 }
 void cuatro(){
     LATB = 0b10100110; //g-fedcba
 }
 void cinco(){
     LATB = 0b10101101; //g-fedcba
 }
 void seis(){
     LATB = 0b10111101; //g-fedcba
 }
 void siete(){
     LATB = 0b00000111; //g-fedcba
 }
 void ocho(){
     LATB = 0b10111111; //g-fedcba
 }
 void nueve(){
     LATB = 0b10100111; //g-fedcba
 }