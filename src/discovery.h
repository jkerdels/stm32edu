#ifndef DISCOVERY_H
#define DISCOVERY_H

/*
 * Autor:  J. Kerdels 
 * Lizenz: CC BY 3.0
*/


/* Die Methode discovery_basic_init() initialisiert zunächst nur die
4 LEDs und den User-Button des discovery boards. */
void discovery_basic_init(void);

/* Die Methode discovery_acc_init() führt die Grundkonfiguration des
Beschleunigungssensors auf dem discovery board durch. */
void discovery_acc_init(void);

#endif
