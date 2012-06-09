/*
 * Autor:  J. Kerdels 
 * Lizenz: CC BY 3.0
*/

#ifndef DISCOVERY_EX_H
#define DISCOVERY_EX_H


//------------------------------------------------------------------------

/* In diesem einfachen Beispiel werden die 4 LEDs des discovery boards
eingeschaltet, wenn der User-Button des boards gedrückt wird.*/
void led_and_button_example(void);



//------------------------------------------------------------------------

/* Dieses Beispiel zeigt, wie man die Grundkonfiguration eines Timers
durchführt und wie man den Timer auf sehr einfach Weise nutzen kann. */
void led_and_timer_example(void);



//------------------------------------------------------------------------

/* In diesem Beispiel soll die Verwendung eines Interrupts erläutert 
   werden. */
void timer_irq_example(void);



//------------------------------------------------------------------------

/* In diesem Beispiel soll die Verwendung eines Timers zur PWM-Generierung
erläutert werden. */
void pwm_led_example(void);



//------------------------------------------------------------------------

/* Das folgende Beispiel demonstriert, wie man mithilfe des DMA controllers
   gänzlich ohne Unterbrechung der Hauptprogrammschleife die LEDs weich ein-
   und ausblenden lassen kann.*/
void dma_pwm_led_example(void);

#endif
