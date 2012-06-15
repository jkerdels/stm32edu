/*
 * Autor:  J. Kerdels 
 * Lizenz: CC BY 3.0
*/

// Definition der standard Integer-Typen
#include <stdint.h>

#include "rcc.h"

void rcc_init(void) {

/* Die Methode rcc_init() initialisiert den Clock-Tree zu Beginn der
der Verarbeitung. Die entsprechenden Schritte werden in [1] ab Seite
84 erläutert. Im Folgenden soll eine kurze Zusammenfassung dieser
Schritte erfolgen und in diesem Rahmen die interne Taktung des M4
beschreiben. Es ist hilfreich, sich nun Abbildung 9 auf Seite 85 des
Reference Manuals [1] zur Hand zu nehmen.

Die zentralen Komponenten des STM32 Cortex M4 können Über drei 
verschiedene Taktquellen mit einem Takt versorgt werden. Der sogenannte
Systemtakt wird hierbei als SYSCLK bezeichnet. In Abb. 9 findet man den 
Ursprung der SYSCLK etwa in der Mitte des Diagramms. Darunter findet 
sich die Anmerkung, dass die SYSCLK mit maximal 168 MHz betrieben 
werden kann. Links daneben ist ein Multiplexer eingezeichnet, Über den 
eine der zur Verfügung stehenden Taktquellen ausgewählt werden kann.
Diese sind mit HSI, HSE und PLLCLK bezeichnet.

-- HSI steht für das "High Speed Internal clock signal". Verfolgt man 
   die HSI-Leitung (Abb. 9) etwas nach links-oben, dann findet man die 
   Quelle des HSI: einen 16MHz schnellen, internen RC-Oszillator. 

-- HSE steht für das "High Speed External clock signal". Verfolgt man
   die HSE-Leitung zur Mitte nach links, dann erkennt man, dass diese
   ihren Ursprung an den externen Pins OSC_OUT und OSC_IN hat. An diesen
   Anschlüssen kann ein externer Quarzoszillator mit 4 bis 26 MHz 
   betrieben werden. Im Rahmen dieses Beispiels gehen wir von einem 8 MHz
   Quarz aus, so wie er beim STM32f4-discovery board anzutreffen ist. In
   rcc.h findet sich die zugehörige Definition F_HSE, die die Frequenz
   des externen Quarzoszillator beschreibt.

-- Die PLLCLK hat ihren Ursprung in einem der beiden PLLs (phase-locked 
   loop) des STM32 Cortex M4. Das Signal entspringt dem mit P bezeichneten
   Ausgang. Ein PLL ermöglicht die Vervielfachung eines eingehenden 
   Taktsignals. Betrachtet man den Eingang des PLLs (der Zweifach-Multi-
   plexer rechts vom PLL-Modul), so sieht man, dass sowohl das interne
   Taktsignal HSI als auch das externe Taktsignal HSE als Input für das
   PLL-Modul dienen können.

Nach dem Einschalten ist zunächst immer das HSI-Signal als Taktquelle für
die SYSCLK ausgewählt. Dies hat einen einfachen Grund. Nicht alle 
Komponenten des Mikrocontrollers können mit der maximalen Taktrate von
168 MHz betrieben werden. Nach dem Start tuckelt unser System also erst
einmal mit gemütlichen 16 MHz aus dem internen RC-Oszillator daher. Bevor
man nun richtig Gas geben kann, müssen erst an diversen Stellen des
Mikrocontrollers ein paar Taktteiler eingestellt werden, so dass die langsa-
men Komponenten nicht ins straucheln kommen, wenn man die Taktfrequenz auf 
168 MHz anhebt.

Diese Einstellungen werden - wie bei Mikrocontrollern allgemein üblich - über
sogenannte Register vorgenommen. Angesprochen werden diese Register über 
feste Speicheradressen. Die Register für die Konfiguration der RCC-Komponen-
ten werden in [1] ab Seite 93 beschrieben. An dieser Stelle werden jedoch
nur die relativen "address offsets" dieser Register erwähnt. Um direkt auf
die Register zugreifen zu können, muss man noch die sogenannte Basisadresse
der beschriebenen Register in Erfahrung bringen. Diese findet man in [1] auf
Seite 50 (Tabelle 1). Die dort beschriebene "Memory Map" gibt Auskunft 
darüber, welche Speicheradressen bzw. Adressbereiche für die Register der 
verschiedenen Komponenten des Mikrocontrollers verwendet werden. Für die 
RCC-Register wird dort die Basisadresse 0x4002 3800 angegeben. Um auf diese
Basisadresse im weiteren Verlauf zugreifen zu können, legen wir diese 
schonmal als Konstante an: */

static const uint32_t RCC_BASE = 0x40023800;

/* Wir benötigen später noch eine weitere Basisadresse. Es ist die Adresse
der FLASH Steuerungsregister: */

static const uint32_t FLASH_R_BASE = 0x40023C00;

/* Normalerweise verwendet man diese Adressen nicht direkt im Programmcode. 
Sie sind üblicherweise in einer dem jeweiligen Mikrocontrollermodell ent-
sprechenden Header-Datei ausgelagert (z.B. stm32f4xx.h) und über passend 
definierte Bezeichnungen zugreifbar. Man kann jedoch auch ganz händisch
auf die Register zugreifen -- und genau dies soll im Folgenden geschehen. 

Das erste Register für die Einstellung des sogenannten "Clock-Trees" ist 
das RCC clock control register (RCC_CR). Es ist 32-Bit breit und hat einen 
relativen Address-Offset von 0x00, d.h. es ist direkt an der oben beschriebenen
Basisadresse (0x40023800) zu finden. Um auf das Register zugreifen zu können,
definieren wir eine Zeigervariable, die auf die passende Speicheradresse 
zeigt: */

volatile uint32_t *RCC_CR = (uint32_t*)(RCC_BASE);

/* Das Schlüsselwort "volatile" ist hierbei eine Anweisung an den Compiler,
bitte keine Optimierungen, z.B. in Form von Caching, an dieser Variable vor-
zunehmen. Wir wollen ja schließlich immer die Speicheradresse manipulieren
und nicht irgendeinen gecachten Wert.

Zu Beginn der RCC-Initialisierung wird explizit noch einmal sichergestellt,
dass wir die SYSCLK gerade über den internen RC-Oszillator HSI betreiben.
Hierzu sind vier Schritte nötig:

- HSI einschalten 
- warten bis der HSI stabil läuft
- SYSCLK auf HSI stellen
- warten auf Bestätigung, dass SYSCLK auf HSI läuft

Für den ersten Schritt müssen wir laut Dokumentation auf Seite 93 ([1]) in 
Bit 0 des RCC_CR Registers eine 1 schreiben. Der Operator |= verodert dabei den bisherigen 
Inhalt von *RCC_CR mit 0x00000001 */ 

// HSI einschalten
*RCC_CR |= 0x00000001;

/* Für den zweiten Schritt müssen wir darauf warten, dass Bit 1 des RCC_CR 
Registers von der Hardwareseite aus auf 1 gesetzt wird. Wir können also auf 
Bit 1 nur lesend zugreifen. */

// warten bis HSI stabil läuft
while ((*RCC_CR & 0x00000002) == 0);


/* Für Schritte drei und vier benötigen wir ein weiteres Register: das RCC clock 
configuration register (RCC_CFGR). Es hat einen Address-Offset von 0x08 und 
wird damit wie folgt definiert.   */

volatile uint32_t *RCC_CFGR = (uint32_t*)(RCC_BASE + 0x08);

/* Die Dokumentation dieses Registers findet sich auf Seite 97. Um die SYSCLK
auf HSI zu stellen, müssen in diesem Register die Bits 0 + 1 auf 0 gesetzt
werden. Um zu melden, dass das Umschalten der SYSCLK auf HSI erfolgreich
war, werden die Bits 2 + 3 des RCC_CFGR Registers von der Hardware auf 0
gesetzt. Damit ergibt sich für die letzten beiden Schritte also der folgende 
Programmablauf: */

// SYSCLK auf HSI stellen
*RCC_CFGR &= 0xFFFFFFFC;

// warten auf Bestätigung, dass SYSCLK auf HSI läuft
while ((*RCC_CFGR & 0x0000000C) != 0);


/* Bevor wir mit der Einstellung der verschiedenen Taktquellen und Taktteiler
fortfahren, ist es empfohlen, den Flash-Speicher des Mikrocontrollers auf
die bevorstehende Geschwindigkeitsanpassung des Systems vorzubereiten. Wie
auf Seite 55 in [1] beschrieben, muss bei einer hohen Taktrate eine passende
Anzahl von "Wait States" (WS) eingestellt werden. Im Falle des STM32f4-
discovery board steht eine Versorgungsspannung von 2.7 V bis 3.6 V zur 
Verfügung. Aus Tabelle 3 in [1] (S.55) kann entnommen werden, dass wir dem-
entsprechend 5 WS (Wait States) für den Flash-Speicher konfigurieren müssen. 
Laut Seite 56ff in [1] kann dies über das "Flash access control register" 
(FLASH_ACR) erfolgen. Es hat einen relativen Address-Offset von 0x00, d.h. 
es ist direkt an der FLASH_R-Basisadresse (0x40023C00) zu finden:
( Waitstates sind Wartezyklen, die der Prozessor einlegen muss, weil der 
Speicher langsamer ist als der Prozessor. )
 */

volatile uint32_t *FLASH_ACR = (uint32_t*)(FLASH_R_BASE);

/* Die Wait States werden in den drei niederwertigsten Bits (0 bis 2) des 
FLASH_ACR Registers als Binärwert eingestellt. Für 5 WS müssen wir also eine
101 and die Bits 0 bis 2 schreiben. Hierzu setzen wir die Bits 0 bis 2 zunächst
gezielt auf 0 und schreiben anschließend den Wert 101: */

// Bits 0 bis 2 "freiräumen"
*FLASH_ACR &= 0xFFFFFFF8;

// 5 WS konfigurieren
*FLASH_ACR |= 0x00000005;


/* Als nächstes bereiten wir die korrekte Taktung der Realtime Clock (RTC) 
vor. Wie auf Seite 98 in [1] beschrieben steht, muss für die RTC ein 
sogenannter "prescaler" festgelegt werden. Der Prescaler ist ein Divisor,
der die Frequenz F_HSE (8 MHz) des externen Quarzoszillators so teilt, dass
die RTC mit einer Frequenz von 1 MHz betrieben wird. Im konkreten Fall 
benötigen wir also einen Prescaler von HSE/8. Für die Konfiguration des 
Prescalers werden die Bits 16 bis 20 des RCC_CFGR (s.o.) Registers verwendet.
Um den passenden Prescaler an die richtige Stelle zu schreiben, gehen wir in 
zwei Schritten vor. Wir setzen zunächst gezielt die Bits 16 bis 20 auf 0 und 
schreiben anschließend den gewünschten Prescaler an diese Stelle:*/

// Bits 16 bis 20 "freiräumen"
*RCC_CFGR &= 0xFFE0FFFF;

// passenden Prescaler setzen
*RCC_CFGR |= ((F_HSE / 1000000L) << 16) & 0x001F0000;


/* Im nächsten Schritt stellen wir die Geschwindigkeit der Haupt-Busmatrix
AHB ein. Diese Busmatrix verbindet alle wichtigen Komponenten des Systems,
z.B. den Prozessorkern mit dem Speicher oder den Peripheriebussen. Auch
die DMA-Controller hängen an der AHB-Matrix. Die Matrix kann mit 168 MHz
betrieben werden. Der Takt der AHB-Matrix wird als HCLK bezeichnet, und wird
über einen Prescaler von der SYSCLK gespeist (Abb.9). Da wird die AHB-Matrix
mit der vollen Geschwindigkeit betreiben wollen, setzen wir den Prescaler 
auf SYSLCK/1. Die Konfiguration des AHB-Prescalers ist auf Seite 99 in [1] 
beschrieben und erfolgt über die Bits 4 bis 7 vom RCC_CFGR Register. Für 
einen Prescaler von SYSLCK/1 setzen wir diese Bits nun auf 0:*/

// AHB-Prescaler auf 1 (prescaler off)
*RCC_CFGR &= 0xFFFFFF0F;


/* Nun wenden wir uns den beiden Peripheriebussen APB1 und APB2 zu. Betrachtet
man Abbildung 5 in [2] (S.17), so erkennt man, dass an diesen beiden Bussen 
fast die gesamte Hardware mit Ausnahme der GPIOs des STM32F4 angebunden ist. 
Der große APB1 auf der rechten Seite (Abb.5 [2]) kann mit maximal 42 MHz ge-
taktet werden, der etwas kleinere APB2 auf der linken Seite läuft mit maximal
84 MHz. Aus Abbildung 9 in [1] (S.85) geht hervor, dass beide Busse ("APBx")
ihren Takt von der Haupt-Bus-Matrix AHB beziehen. Diese haben wir zuvor auf
die maximale Frequenz von 168 MHz konfiguriert. Dementsprechend muss nun der
Prescaler von APB1 auf AHB/4 und der Prescaler von APB2 auf AHB/2 gestellt 
werden. Diese Konfiguration erfolgt ebenfalls im Register RCC_CFGR. Wie auf
Seite 98 [1] beschrieben, müssen wir für einen Prescaler von AHB/4 für APB1 
die Bits 10 bis 12 auf 101 setzen. Für den Prescaler AHB/2 für APB2 müssen die
Bits 13 bis 15 auf 100 gesetzt werden:*/

// Bits 10 bis 15 "freiräumen"
*RCC_CFGR &= 0xFFFF03FF;

// Bits 10 bis 12 auf 101 und Bits 13 bis 15 auf 100
*RCC_CFGR |= 0x00009400;


/* Bald ist es geschafft! Wir müssen nur noch das PLL-Modul konfigurieren.
Hierfür benötigen wir das "PLL configuration register" (RCC_PLLCFGR). Dies hat
(S.95 in [1]) einen Address-Offset von 0x04: */

volatile uint32_t *RCC_PLLCFGR = (uint32_t*)(RCC_BASE + 0x04);

/* Bevor wir das PLL-Modul konfigurieren, stellen wir sicher, dass es erstmal
ausgeschaltet ist. Dies erfolgt über das RCC_CR Register. Da die Eingänge des
Haupt-PLL-Moduls und des PLLI2S-Moduls miteinander gekoppelt sind (Abb. 9), 
müssen wir beide PLL-Module abschalten. Hierzu setzen wir Bit 24 und Bit 26
in RCC_CR auf 0: */

// PLL und PLLI2S ausschalten
*RCC_CR &= 0xFAFFFFFF;

/* Schauen wir uns erneut Abbildung 9 in [1] (S.85) an. Das PLL-Modul befindet
sich auf der linken Seite etwas unterhalb der Mitte. Als Eingang hat es einen
2fach-Multiplexer, der entweder HSI oder HSE als Takteingang für das PLL-Modul
auswählen kann. Über Bit 22 des RCC_PLLCFGR-Registers kann durch ein schreiben
von 1 der externe Oszillator HSE ausgewählt werden: */

// aktiviere HSE als PLL Takteingang
*RCC_PLLCFGR |= 0x00400000;

/* Links vom 2fach-Multiplexer (Abb.9) ist der erste Taktteiler "M", den wir zu
beachten haben. Wie auf Seite 96 in [1] beschrieben steht, soll der Taktteiler
so gewählt werden, dass das PLL-Modul mit 1 bis 2 MHz Takt versorgt wird. 
Hierbei sollte der Takt nach Möglichkeit 2 MHz betragen, um den PLL jitter (ein
"zittern" der Phase des Taktsignals) zu minimieren. Wir benötigen also einen
Prescaler von HSE/4 bei unserem 8 MHz Oszillator. Eingestellt wird der 
Prescaler M über die Bits 0 bis 5 des RCC_PLLCFGR-Registers: */

// Bits 0 bis 5 "freiräumen"
*RCC_PLLCFGR &= 0xFFFFFFC0;

// Bits 0 bis 5 auf den passenden Prescaler einstellen (binär)
*RCC_PLLCFGR |= (F_HSE / 2000000L) & 0x0000003F;

/* Die nächsten beiden Werte, die es einzustellen gilt, sind der Multiplika-
tor N und der Divisor P. Zusammen bestimmen Sie den Systemtakt SYSCLK, wenn 
dieser das PLL-Modul als Taktquelle benutzt. Die erzielte Taktfrequenz ergibt
sich hierbei aus den 2MHz am Eingang multipliziert mit N und dividiert durch P.
Aus der Beschreibung auf Seite 96 in [1] können die Werte N = 168 und P = 2 als
passende Werte entnommen werden. Der Wert von N wird als Binärwert in die Bits
6 bis 14 des RCC_PLLCFGR-Registers geschrieben. Für die Einstellung von P = 2
müssen die Bits 16 und 17 im gleichen Register auf 0 gesetzt werden: */

// Bits 6 bis 14 sowie Bits 16 und 17 "freiräumen" -> implizit P = 2
*RCC_PLLCFGR &= 0xFFFC803F;

// N = 168 setzen in Bits 6 bis 14
*RCC_PLLCFGR |= (168L << 6) & 0x00007FC0;

/* Der letzte Divisor "Q", der im PLL-Modul eingestellt werden muss, ist der 
Ausgang für die sogenannte PLL48CK. Diese Taktleitung wird von einigen Peri-
pheriegeräten genutzt, die zwingend einen Takt von 48 MHz benötigen. Dement-
sprechend muss ein Divisor gewählt werden, der aus 336 MHz (2 MHz * N) 48 MHz 
macht. Der Wert 7 scheint geeignet. Über die Bits 24 bis 27 kann dieser Wert 
als Binärwert im Register RCC_PLLCFGR konfiguriert werden: */

// Bits 24 bis 27 "freiräumen"
*RCC_PLLCFGR &= 0xF0FFFFFF;

// Wert Q = 7 in Bits 24 bis 27
*RCC_PLLCFGR |= 0x07000000;

/* Um nun die RCC-Konfiguration abzuschließen fehlen nur noch 6 Schritte. Über
das Bit 16 im Register RCC_CR schalten wir erst einmal das externe Taktsignal
ein. Dann warten wir darauf (Bit 17), dass das Taktsignal bereit steht. 
Anschließend starten wir mit Bit 24 das PLL-Modul und warten (Bit 25) darauf,
dass sich das PLL-Modul stabilisiert hat. Zu guter Letzt verwenden wir die
Bits 0 und 1 des RCC_CFGR-Registers, um den Systemtakt auf das PLL-Modul 
umzuschalten und warten (Bits 2 und 3) auch hier, bis wir eine hardwareseitige 
Bestätigung für diesen Vorgang erhalten:*/

// HSE über Bit 16 in RCC_CR einschalten
*RCC_CR |= 0x00010000;

// Warten bis HSE an ist (Bit 17)
while ((*RCC_CR & 0x00020000) == 0);

// PLL-Modul über Bit 24 einschalten
*RCC_CR |= 0x01000000;

// Warten bis PLL stabil (Bit 25)
while ((*RCC_CR & 0x02000000) == 0);

// PLL als Taktquelle für SYSCLK auswählen (10 in Bits 0 und 1 des RCC_CFGR)
*RCC_CFGR &= 0xFFFFFFFC; // "freiräumen"
*RCC_CFGR |= 0x00000002; // "10" schreiben

// warten bis die SYSCLK umgestellt ist (Bits 2 und 3 müssen 10 werden)
while ((*RCC_CFGR & 0x0000000C) != 0x00000008);


/* Damit ist die Konfiguration des Clock-Trees abgeschlossen und das System
läuft nun mit 168 MHz! */


}


