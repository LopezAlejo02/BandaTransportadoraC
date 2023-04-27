/* 
 * File:   LCD.c
 * Author: User
 *
 * Created on 15 de abril de 2023, 09:53 PM
 */

//Archivo de cabecera
#include "LCD.h"

//Librerias adicionales
#include <xc.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#define FCY 4000000
#include <libpic30.h>

//Funciones

void sendData(char data, char mode){
    RS = mode; //Selección de modo (0 - comando; 1 - dato)
    //Envio parte alta del dato
    D7 = ((data>>7)&1); // Se realiza un corrimiento a la derecha de data para luego hacer una and con un 1 (...00001) y así asginar ese valor de data a D7
    D6 = ((data>>6)&1); // Se realiza un corrimiento a la derecha de data para luego hacer una and con un 1 (...00001) y así asginar ese valor de data a D6
    D5 = ((data>>5)&1); // Se realiza un corrimiento a la derecha de data para luego hacer una and con un 1 (...00001) y así asginar ese valor de data a D5
    D4 = ((data>>4)&1); // Se realiza un corrimiento a la derecha de data para luego hacer una and con un 1 (...00001) y así asginar ese valor de data a D4

    E = 1; // Enviar comando
    __delay_ms(10); // Espera mientras se ejecuta el comando
    E = 0; // Apagar envio de dato
   
    //Envio parte baja del dato
    D7 = ((data>>3)&1); // Se realiza un corrimiento a la derecha de data para luego hacer una and con un 1 (...00001) y así asginar ese valor de data a D7
    D6 = ((data>>2)&1); // Se realiza un corrimiento a la derecha de data para luego hacer una and con un 1 (...00001) y así asginar ese valor de data a D6
    D5 = ((data>>1)&1); // Se realiza un corrimiento a la derecha de data para luego hacer una and con un 1 (...00001) y así asginar ese valor de data a D5
    D4 = ((data>>0)&1); // Se realiza un corrimiento a la derecha de data para luego hacer una and con un 1 (...00001) y así asginar ese valor de data a D4
    
    E = 1; // Enviar comando
    __delay_ms(10); // Espera mientras se ejecuta el comando
    E = 0; // Apagar envio de dato
}

void onLcd(){
    RS = 0; //Selección de modo comando
    //Enviar comando de encendido confidurado por hardware
    D7 = 0;
    D6 = 0;
    D5 = 0;
    D4 = 0;
    
    E = 1; // Enviar comando
    __delay_ms(10); // Espera mientras se ejecuta el comando
    E = 0; // Apagar envio de dato
}

void initLcd(){
    RS = 0; //Selección de modo comando
    //Enviar comando de configuración para 4 bits
    D7 = 0;
    D6 = 0;
    D5 = 1;
    D4 = 0;
    
    E = 1; // Enviar comando
    __delay_ms(10); // Espera mientras se ejecuta el comando
    E = 0; // Apagar envio de dato
}

void clearLcd(){
        sendData(1,0); //Envía el valor "1" en modo comando que representa el comando de limpiar pantalla
}

void goTo(char y, char x){
    // Desplazarse por las posiciones del LCD. y representa la fila del display y x la columna
    if((y == 1)&&(x < 17)){
        sendData((128 + (x - 1)),0);// 128 en binario 1000 0000 + x en binario. Esto para ir a alguna posicion en la primera fila
    }
    else if((y == 2)&&(x < 17)){
        sendData((192 + (x - 1)),0);// 192 en binario 1100 0000 + x en binario. Esto para ir a alguna posicion en la primera fila
    }
}

void sendStr(const char *dat){
    //Recibe una cadena de texto y la imprime caractér por caractér
    int i = 0;
    while(dat[i]){
        __delay_ms(10);
        sendData(dat[i++],1);
    }
}

void sendNum(int num){
    //Recibe una número y lo imprime 
    char buf[32]; //Crea un buffer para almacenar la conversión del numero a ascii
    sprintf(buf, "%d", num); //Convierte el número recibido a ascii
    sendStr(buf);
}