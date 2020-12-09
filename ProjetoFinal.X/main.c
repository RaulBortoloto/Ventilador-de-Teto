#include "pic18f4520.h"
#include "config.h"
#include "pwm1.h"
#include "keypad.h"
#include "ssd.h"
#include "lcd.h"
#include "timer.h"
#include "bits.h"

void main(void) {

    unsigned char tecla, temp, i, Tela = 1, VelocidadeVentoinha = 1;
    unsigned char EstadoVentoinha = 0, EstadoTimer = 0, EstadoLuminaria = 0;
    unsigned long timerSSD = 0;

    lcdInit();
    kpInit();
    pwmInit();
    ssdInit();
    timerInit();

    for (;;) {
        timerReset(10000);
        kpDebounce();

        if (EstadoTimer == 1) {
            ssdUpdate();
            ssdDigit(((timerSSD / 60000) % 6), 0);
            ssdDigit(((timerSSD / 6000) % 10), 1);
            ssdDigit(((timerSSD / 1000) % 6), 2);
            ssdDigit(((timerSSD / 100) % 10), 3);
            timerSSD--;

            if (timerSSD == 0) {
                pwmSet1(0);
                EstadoTimer = 0;
            }
        }

        if (temp != kpRead()) {
            temp = kpRead();
            tecla = kpRead();

            if (bitTst(tecla, 3)) {
                if ((EstadoVentoinha % 2) == 0) {               // Se nao estiver ligado, liga na vel. baixa
                    pwmSet1(32);
                    VelocidadeVentoinha = 1;
                } else if ((EstadoVentoinha % 2) != 0) {        // Se estiver ligado, desliga
                    pwmSet1(0);
                    VelocidadeVentoinha = 1;
                }
                EstadoVentoinha++;
            }

            if (bitTst(tecla, 7)) {
                if (VelocidadeVentoinha == 1) { // Velocidade baixa
                    EstadoVentoinha = 1;
                    pwmSet1(0);
                    pwmSet1(32);
                    VelocidadeVentoinha++; // Na prox clicada vel. media
                } else if (VelocidadeVentoinha == 2) { // Velocidade media ou ligado no baixo
                    EstadoVentoinha = 1;
                    pwmSet1(0);
                    pwmSet1(64);
                    VelocidadeVentoinha++; // Na prox clicada vel. alta
                } else if (VelocidadeVentoinha == 3) { // Velocidade alta
                    EstadoVentoinha = 1;
                    pwmSet1(0);
                    pwmSet1(96);
                    VelocidadeVentoinha = 1; // na proxima clicada vel. baixa
                }
            }

            if (bitTst(tecla, 6)) { //Timer 5 min
                EstadoTimer = 1;
                timerSSD = 30000;
            }
            if (bitTst(tecla, 5)) { //Timer 10 min
                EstadoTimer = 1;
                timerSSD = 60000;
            }
            if (bitTst(tecla, 4)) { //Timer 30 min
                EstadoTimer = 1;
                timerSSD = 180000;
            }

            if (bitTst(tecla, 2)) { // Tecla 7 = Tela 1
                lcdCommand(0x01);
                char Tela1[32] = "1-Vent. ON/OFF\n2-Mudar Veloc.";
                for (i = 0; i < 32; i++) {
                    if (Tela1[i] == '\n') {
                        lcdCommand(0xC0);
                        continue;
                    }
                    lcdData(Tela1[i]);
                }
            }
            if (bitTst(tecla, 1)) { // Tecla * = Tela 2
                lcdCommand(0x01);
                char Tela2[32] = "5-Timer 5 min\n8-Timer 10 min";
                for (i = 0; i < 32; i++) {
                    if (Tela2[i] == '\n') {
                        lcdCommand(0xC0);
                        continue;
                    }
                    lcdData(Tela2[i]);
                }
            }
            if (bitTst(tecla, 0)) { // Tecla 0 = Tela 3
                lcdCommand(0x01);
                char Tela3[32] = "0-Timer 30 min";
                for (i = 0; i < 32; i++) {
                    if (Tela3[i] == '\n') {
                        lcdCommand(0xC0);
                        continue;
                    }
                    lcdData(Tela3[i]);
                }
            }
        }
        timerWait();
    }
}