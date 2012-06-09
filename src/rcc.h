#ifndef RCC_H
#define RCC_H
/*
 * In den Dateien rcc.h und rcc.c finden sich u.a. die Methoden
 * zur Initialisierung des Clock-Trees. Das Kürzel RCC steht hierbei
 * für "Reset and Clock Control". Nähere Informationen finden sich
 * hierzu in [1] ab Seite 82. Empfohlen seien insbesondere die 
 * Abbildungen 5 und 6 in [2] (Seiten 17 und 20), sowie die Abbildung
 * 9 in [1] (Seite 85).
 
 * Autor:  J. Kerdels 
 * Lizenz: CC BY 3.0

 */

// Die Frequenz des externen Oszillators
#define F_HSE      8000000L

// Die "interne" Frequenz der CPU
#define F_CPU    168000000L

// Die Methode rcc_init() initialisiert den Clock-Tree.
// Details siehe in rcc.c
void rcc_init(void);

#endif
