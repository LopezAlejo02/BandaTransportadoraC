/* 
 * File:   LCD.h
 * Author: User
 *
 * Created on 16 de abril de 2023, 09:37 AM
 */

//Esta librería es para el uso de una pantalla LCD 2x16 en modo de 4 bits

#ifndef LCD_H
#define	LCD_H

//Configuracion de pines
#define RS LATBbits.LATB10
#define E  LATBbits.LATB11
#define D4 LATBbits.LATB0
#define D5 LATBbits.LATB1
#define D6 LATBbits.LATB2
#define D7 LATBbits.LATB3

//Funciones

void sendData(char data, char mode); // Envía un dato al LCD y se selecciona el modo, si es un dato para imprimir (mode = 1) o es un comando (mode = 0)
void onLcd(); //Envía el comando de encendido fijado por harware externo
void initLcd(); //Envía el comando para fijar la configuración en 4 bits
void clearLcd(); //Limpia la pantalla
void goTo(char y, char x); //Ubica el cursor en la fila y, columna x
void sendStr(const char *dat); // Imprimir strings
void sendNum(int num); // Imprimir numeros

#endif	/* LCD_H */

