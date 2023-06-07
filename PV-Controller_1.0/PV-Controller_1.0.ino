// 5.2 Inverter-Ansterungsintervalle in eigener Surbroutine; Ansteuerung beim Web-Daten-Senden; 
// 5.4.Timeout für Datenseite
// 6.1 Wifi Manager and Timeout ohne Wifi Connection beim Start => 
// 8.1 Letzer hochgeladene Version, 8.2 Zählerstand auf Vorstundenwert beim Speichern
// 9.0 Neue NTP Zeit mit Sommer- / Winterzeit; Netzzeit wird täglich aktualisiert, wenn nicht gefunden, alle 10 Min neuer Versuch (NetTimeRetry)
//10.0 Umbau mit inlcude Dateien zur besseren Lesbarkeit
//11.0 Konfigurationsseite integriert
//13.0 SML Lib integriert
// 0.9.3 Auzeige der eingespeisten Gesamtleistung (2.8.0) vom Zweirichtungszähler im Seriellen Monitor
// 0.9.4 Anzeige der eingespeisten Gesamtleistung auch im WebUI; Bereinigung des Parsers - nur noch einmal Aufruf
// 1.0  Anzeige der Versionsnummer, Manueller Festwert für Wechselrichterleistung zum Testen oder ungeregelte Nachteinspeisung

// Hinweis: es müssen 9 Bibliotheken (s. Z. 52) eingebunden und 8 weitere Hilfsdateien (*.h) im Root liegen, s. Zeile ca. 195 und ca. 290

//  -----------  P I N B E L E G U N G  D..   für ESP32 WROOM    --------------
                                      //  1 : TX von serieller Kommunikation => mgl. nicht benutzen
#define OB_LED_Pin     2              //  2 : Built in LED für Anzeige von Datenempfang
                                      //  3 : RX von serieller Kommunikation => mgl. nicht benutzen
#define RS485_RX_Pin   4              //  4 : RX bzw RO von RS485; (Lesen wird hier nicht verwendet, Modeumschaltung Lesen/Schreiben auch nicht
#define RS485_TX_Pin   5              //  5 : TX bzw DI von  RS485; 
#define Buzzer_Pin    13              // 13 : Buzzer für Alarm keine Daten seit x sec; muss nicht eingebaut werden
#define RS485_Tog_Pin 15              // 15 : Pin zum Umschalten von RX (high) auf RX (Low)
#define IRHead_RX_Pin 16              // 16 : RX von IRHead
#define IRHead_TX_Pin 17              // 17 : TX von IRHead (wird nicht verwendet)
#define IR_Green_Pin  18              // 18 : Led Grün: erfolgreich gelesen
#define IR_Red_Pin    19              // 19 : Led rot: Lesefehler
#define SDA_Pin       21              // 21 : SDA für 1602 Display   ; muss nicht eingebaut werden    Display: 22 = SCL; 21 = SDA
#define SCL_Pin       22              // 22 : SCL für 1602 Display 
#define Touch_Pin     27              // 27 : touch pin Taster für LCD Beleuchtung  ; muss nicht eingebaut werden
// Pin zum Umschalten Lesen /Schreiben des RS485 (nicht definiert und benutzt, da nur Schreiben verwendet und Hard verkabelt (auf Plus) 

// --------------- I N I T I A L -  K O N F I G U R A T I O N  -------------------------
String VersionsNummer="1.0.0";        // Versionsnummer - wird auf WebUI angezeigt     
byte EHZType = 1;                     // 1 = Einrichtungszähler; 2 = 2 Zweirichtungszähler EEPROM Adr 1
byte InverterType = 1;                // 1 = Soyosource, 2 = GoodWe; 3 = Growatt;  EEPROM Adr 2 
int InverterZahl= 1;                  // Anzahl der Inverter/Soyosource => errechnete Ansteurungsleistung wird durch diese Zahl geteilt; EEPROM Adr 3+4
int Inv_Baudrate = 4800;              // Baudrate des Wechselrichters - noch nicht implementiert
int EHZ_Baudrate =9600;               // Baudrate des Einheitenzähleres - noch nicht implementiert
byte EHZ_Send_IR = 0;                 // Sende IR Anfrage an EHZ; 0= Nein; 1; Ja - noch nicht implementiert
int MaxWattInverter = 900;            // Maximale Inverter Leistung in Watt insgesamt;  EEPROM Adr 5+6
int WattsCorr = 1100;                 // korrekturfator für korrekte Leistungsausgabe;  EEPROM Adr 7+8
int InverterInterval = 1800;          // Zeit in ms nach der Inverter erneut mit Daten angesteuert wird; 1500 = 1,5 Sek; EEPROM Adr 9+10
int LastWechselDelay = 2000;          // Zusatverzögerung für Ansteuerung in ms/mW ; z.B 2000 = 2000 ms/kW als0 bei 2000ms bei 1000W Laständerung pro WR oder 2ms/W; EEPROM Adr 15+16    
int InverterAdress=1;                 // Inverter Adresse für Modbus Commando;  EEPROM Adr 11+12
int MinNetInput = 13;                 // MindestNetzverbrauch, auf den ausgeregelt wird;  EEPROM Adr 13+14
int manualInverterPower = 0;          // Manueller Festwert für Wechselrichterleistung zum Testen oder ungeregelte Nachteinspeisung
long  FirstDelay = 2050;              // Zeit, bei erstem Wert, bis Soyo das erste mal regelt
#define DownWatt 50                   // Wert, um den die Leistung im ersten Schritt mindestens verringert wird, wenn der Verbrauch 0 ist
#define DownWattProz 10               // %Wert, um den die Leistung im ersten Schritt verringert wird, wenn der Verbrauch 0 ist
float DownFaktor = 2;                 // Teler, durch den die Leistung geteilt wird bei Nulleinspeisung
long LCD_light_on_duration = 60;      // LCD bleibt x Sec an
#define TouchTeiler 2                 // Verhältnis des Touchpins zwischen Drücken und nicht Drücken, gew 1..4
#define GMT_Plus_h 1                  // lokale Zeit ist GMT + 2 im Sommer, 1 im Winter
#define MaxPower_Check_tm 600         // Zeit in Sekunden, die abgewartet wird bevor die Leistung zum Test reduziert wird
#define NetTimeRetry 600000           // wenn Netzzeit nicht gefunden neuer Versuch alle 600.000 msek = 10 min
#define NetTimeRefreshTime 86400000   // ein mal am Tag (in ms) wird die Netzeit erneut
#define WebReqTimeoutTime 3000        // Timeout in ms für Webserveranfragen
#define CP client.print               // CP  für Kurzscheibweise von client.print
#define CPL client.println            // CPL für Kurzscheibweise von client.println
#define EEPROM_SIZE 500               // EEPROM Größe (max=512)
#define EEPROM_ConfigMarker 27        // Bytewert zur Erkennung, dass Konfigurationsdaten abgelegt wurden - ist hoffentlich nicht zufällig gesetz ;-

// Bibliotheken, die über Libraries zuerst noch eingebunden werden müssen
#include <ESP32Tone.h>                // "ESP32Servo" aus Libraries einfügen
#include <LiquidCrystal_I2C.h>        // "LiquidCrystal_I2C" aus Libraries einbinden 
#include <WiFiManager.h>              // "WifiManger" by TZAPU aus Libraries einfügen  (Quelle: https://github.com/tzapu/WiFiManager  )

// Bibliotheken, die normalerweise bereits vorhanden sind
#include <time.h>                     // time() ctime()
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "FS.h"
#include <EEPROM.h>
#include "SPIFFS.h"

#define MAX_STR_MANUF 5   // Anzahl der max OBIS Kennwerte für den SML Parser
unsigned char manuf[MAX_STR_MANUF];
double T1Wh = -2, SumWh = -2, Sum2NetWh = -2, Watt = -2;  // Vorgabewerte zum Erkennen von Falsch- / Nichtlesen

WiFiServer server(80);
WiFiClient client;
WiFiManager wm; // Öffnet Accesspoint, in dem man sein Netzwerk angeben kann
IPAddress ClientIP; // Hier wird die IP Adresse des Client gespeichert, wenn die Webseite angefordert wird
LiquidCrystal_I2C lcd1(0x27, SDA_Pin, SCL_Pin); // Display: 21 = SDA; 22 = SCL

// https://www.msxfaq.de/sonst/bastelbude/smartmeter_d0_sml_protokoll.htm  : nachfolgend Auszug
// https://www.stefan-weigert.de/php_loader/sml.php
// 77 07 01 00 01 08 01 FF 01 01 62 1E 52 FF 56 00 02 BE DD 30 01               // wäre Standard bei Zähler, der 01.08.01 Wirk Energie Tarif 1 Bezug anzeigt
// .......................................FF 56 00 02 BE DD 30 01               // hier ist die Leistung dann in 5 Byte angegeben (00 02 ..)

// Diese Werte werden gesucht 24/25 Byte für den Zähler DVS 7420
// Wirkverbrauch Auufbau des Datensatzes:
// 77 07 01 00 10 07 00 FF 01 72 62 01 62 00 62 1B 52 FE : Header, dann:
// 54  oder 53; 54 = 3 Byte Wert; 53 = 2 Byte Wert
// 3 Byte: 02 8F B9 Watt als 3 Byte Long (/100); dieser Wert muss geparst werden!!
// 3 Byte:  23 79  Watt als unsigned integer (/100); dieser Wert muss geparst werden!!
// Bsp:
// 77 07 01 00 10 07 00 FF 01 72 62 01 62 00 62 1B 52 FE                  // Header für Wirkverbrauch, dann...
//                                                       54 02 8F B9 01 01 01   3 Byte bis 655W oder
// ...................................................   53 23 79 01 01 01      2 Byte darüber (oder auch FE 52 XX) = 1 Byrte
//  HdStr[1] = "77070100100700FF017262016200621B52FE";                   // Header für Wirkleistung DVZ 7420, definiert in Zeile 212

// Gesamtverbrauch Aufbau des Datensatzes:
// 77 07 01 00 01 08 00 FF 64 1D 49 04 72 62 01 62 00 62 1E 52 FF                        // Header für Gesamtverbrauch 1.8.0. wie hier im Programm verwendet mit Anpassung "Joker"
//                                                                 65 03 7C AF 11 01     // 65 = 4 Bytes mit Werte ; Werten in 0,1W =>/10 ;64 für 3 Bytes etc => 61 abziehen für Anzahl Byte mit Wert
// HdStr[2]=  "77070100010800FF64FFFF047262016200621E52FF"; dieser Header wird unten verwendet

#define LEDAN HIGH                  // Wert, um onboard LED anzuschalten
#define LEDAUS LOW                  // Wert, um onboard LED auszuschalten
#define MaxArray     2000           // Maximale Anzahl von Bytes die von IR gelesen und gespeichert werden können
#define TIMEOUT        15           // timeout für Lesen der IR Daten typisch 20ms
unsigned long LoopTm = 10;          // Intervall für Loop Durchlauf in msec
unsigned long LastLoopTm;           // Zeit des letzten Loop Durchlauf
char cmd[8];                        // Für RS485 Comand Soyosoucre
byte SID[MaxArray];                 // Für Header der SML ID für den aktuellen Verbrauch
String HdStr[5];                    // Hier werden die SML Header abgelegt
byte Subtr[5];                      // Speicher für Werte, der beim Parsen abgezogen werden muss, um die Anzahl der Bytes zu erhalten
int Hd_lgh;                         // SML - Header Länge
const byte MaxHdLength = 25;        // maximale SML - Header Länge
int PowerTeiler[5];                 // jeweilge Dividenten der Wattwerte
int CP_lgth = 0;                    // Anzahl der Byte die für den Verbrauchswert gelesen werden; 2 oder 3)
int cnt = 0;                        // counter für Zeilenumbruch beim Printen
int ID_act = 0;                     // counter für korrekt erkannte ID Positionen
int IR_Dataset_length;              // Länge des Datensatzs
bool Hd_found = false;              // header gefunden in SML Daten
bool Parsing_Success = false;       // fertig => Daten gelesen
bool Send_complete = false;         // wenn alle Daten von IR empfangen wurden
unsigned long tm_IR_Data;           // letzte Zeipunkt von gelesenen Daten
unsigned long lastokSMLReading;     // Letzter Programmdurchlauf
unsigned long lastCsptAdd=0;        // Letzte Zeit bei der der Verbrauch in den Verbrauchszähler addiert wurde
bool firstdown=true;                // bei negativ erster Schritt mit Minus x Watt, dann geteilt durch x
long InverterPower = 0;             // Leistungswert in Watt, der an den Inverter gesendet wird; Startwert 0 Watt
long InverterPowerOld = 0;          // zuvor gesendeter Leistungswert
long DownWatt2=0;                   // Leistungsreduzierung Stufe 2; wird berechnet
long WaitInverterPwr = 0;           // Zeit, bis zu der mit der Ausgabe neuer Leistungswerte für den Inverter gewartet werden soll, damit dieser Zeit zum Regeln hat
bool LCD_avail = false;             // LCD angeschlossen ja/nein
bool LCD_light_on = true;           // LCD angeschlossen ja/nein
long LCD_light_onlastInfo;          // letzte Einschaltzeit der LCD Beleuchtung
int NoTouchVal;                     // Ruhewert für Touchpin, der beim Start gemessen wird
long  TouchSum;                     // 10 TOuchwerte werden addiert und gemittelt, um Fehlfunktion zu vermeiden
long  TouchCnt;                     // zählt summierten die Touchwerte
String header;                      // Header beim request vom WebClient
String FileDataSet;                 // Eine ganze Zeile Daten (1 Datensatz) beim Lesen und Ausgeben
String HTMLLine;                    // Eine ganze Zeile Daten (1 Datensatz) zum Ausgeben als HTML Zeile
long NetPowerAct = 0, NetPowerAct_Display = 0; // Netzbezug und Netzbezugsspeicher für vergangenen Wert für Anzeige in Webseite
long ConsmptTot = 0, ConsmptTot_last_h =0;     // Zählerstand (Gesamtverbrauch des Zählers) 
long Pow2NetTot = 99999999;          // Gesamteinspeisung vom Zähler  - Anzeigewert, wenn kein Wert gelesen werden konnte
#define MY_NTP_SERVER "de.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"   // Europe/Berlin   von  https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
time_t now;    // this is the epoch
tm tm;         // the structure tm holds time information in a more convenient way

unsigned long unix_epoch;    // Unix Zeit
byte last_second, second_, minute_,hour_last, hour_, day_, month_, month_last; // Zeitwerte für Min etc und die vorherigen
int  year_, year_last;
String DatumString;
bool Batterie_empty=false;  // Flag, ob Batterie als leer erkannt
bool MaxPower=false ;   // wird true gesetzt, sobald MaxPower augegeben wird
unsigned long MaxPowerStart=0; // speichert, seit wann maximal Power ausgegeben wird ; nach x min wird geprüft, ob Verbrauch bei Änderung reagiert, dann ist Batterie noch geladen und aktiv
unsigned long LastNetTime; // wann das letzte Mal erfolgreich die Netzzeit bezogen wurde
unsigned long LastWifiCheck=millis();  // Letzte Zeit der Wifi überprüfung
unsigned long currentWebReqTime, previousWebReqTime;
unsigned long lastInverterSetTime; // Zeit der letzten Inverteransteuerung
bool WifiConnectedOnStart = false;   
#define RebootTime 3600000  // Reboot jede Stunde (3600000 msec) , wenn beim Start nicht mit Wifi verbunden wurde 
bool NetTimeFound = false;    // Netzzeit gefunden - nein
long Consmpt_lastday=0;
long EinspeisungWattSek=0;
long Einspeisung_day=0, Einspeisung_lastday=0;
long ConsmptTot_lastday=0;
byte day_last=23;
byte Minute_Vortag;
String Systemstartzeit;
char ChrDatName[100]; // char array zum Speichern des Pfades für die Datei
char WriteChar[100];  // char array für den zu speichernden Wert
#define FORMAT_SPIFFS_IF_FAILED true

#define DEBUG // auskommentieren, wenn keine Debuggingwerte angezeigt werden sollen
#ifdef DEBUG
  #define SERIALBEGIN(...)   Serial.begin(__VA_ARGS__)
  #define DPRINT(...)        Serial.print(__VA_ARGS__)
  #define DPRINTLN(...)      Serial.println(__VA_ARGS__)
  #else
  #define SERIALBEGIN(...)   //blank line
  #define DPRINT(...)        //blank line
  #define DPRINTLN(...)      //blank line
#endif

#define SHOWRESULT  // auskommentieren, wenn kein Ergebnis angezeigt werden soll
#ifdef SHOWRESULT
  #define RPRINT(...)        Serial.print(__VA_ARGS__)
  #define RPRINTLN(...)      Serial.println(__VA_ARGS__)
  #else
  #define RPRINT(...)        //blank line
  #define RPRINTLN(...)      //blank line
#endif

//eigene Includdateien; ins root legen und  mit Routinen einbinden
#include "Hilfsroutinen.h"
#include "SPIFFSroutinen.h"
#include "sml.h"                   // ins Root legen (wo ino Datei liegt, sowie sml.cpp und smlCrcTable.h
// sie auch weiter unten #include "webpageConfig.h", #include "webpageLoggedData.h", #include "webpageVerbrauch.h"



void setup() {
// Pins konfigurieren
  pinMode (OB_LED_Pin, OUTPUT);   // on Board LED
  pinMode (IR_Green_Pin, OUTPUT); // grüne LED
  pinMode (IR_Red_Pin, OUTPUT);   // rote LED
  pinMode (RS485_Tog_Pin, OUTPUT);// RX / TX Umschalter für RS485
  digitalWrite(RS485_Tog_Pin,HIGH);  //  Setzt den RS485 Umschalter auf High für Senden
  Serial.begin(115200); // für Seriellen Monitor
  Serial1.begin(4800, SERIAL_8N1, RS485_RX_Pin, RS485_TX_Pin); // statt IRHead.begin(9600); // RS485 Schnittstelle starten
  Serial2.begin(9600, SERIAL_8N1, IRHead_RX_Pin, IRHead_TX_Pin); // statt IRHead.begin(9600); // IR Schnittstelle starten

  //pinMode(IRHead_RX_Pin, INPUT_PULLUP); // Test für Aufbau ohne Widerstand und Poti nur mit internem Pullup

  // LEDs und Buzzer testen
  digitalWrite(IR_Red_Pin, LEDAN); 
  delay(300);
  digitalWrite(IR_Red_Pin, LEDAUS);
  digitalWrite(IR_Green_Pin, LEDAN); 
  delay(300);
  digitalWrite(IR_Green_Pin, LEDAUS); 
  tone(Buzzer_Pin,784,100);
  tone(Buzzer_Pin,988,100);
  tone(Buzzer_Pin,2*784,100);
    
  NoTouchVal = touchRead(Touch_Pin);
   
  EEPROM.begin(EEPROM_SIZE);
  if(EEPROMReadByte(0)!=EEPROM_ConfigMarker) WriteConfigToEEPROM();   // EEProm einmalig mit Standard-Konfigwerten beschreiben
  else ReadConfigFromEEPROM();                        //wenn konfiguriert, Konfiguration aus EEProm auslesen
  
  cmd[0] = 0x24; cmd[1] = 0x56; cmd[2] = 0x00; cmd[3] = 0x21; cmd[6] = 0x80; // Cmd Werte sind konstant
  
  /* Wird seit Integration des SML Parsers nicht mehr benötigt
  HdStr[1] = "77070100100700FF017262016200621B52FE";      // Header für Wirkleistung DVZ 7420
  Subtr[1] = 0x51;// diess muss von Bytezahl abgezogen werden für richtige Anzahl der Bytes für Leistung
  PowerTeiler[1] = 100; // ermittelter Wert / 100 ergibt Watt
  HdStr[2]=  "77070100010800FF64FFFF047262016200621E52FF";  // Header für Gesamtverbrauch hier 2 x FF bei 010800F64 FFFF 04 als "Joker" statt 1C01
  //  HdStr[2]=  "77070100010800FF641C01047262016200621E52FF";  // urspr. Header für Gesamtverbrauch wie ausgelesen, es gibt jedoch Abweichungen, s.o. daher Joker "FF"
  Subtr[2] = 0x61;// diess muss von Bytezahl abgezogen werden für richtige Anzahl der Bytes für Leistung
  PowerTeiler[2] = 10; // ermittelter Wert / 10 ergibt Watt
  // nachfolgend ggf weiteren Zählerwert zum Auslesen definieren
  //HdStr[3] =  "77070100010800FF641C41047262016200621E52FF"; // Header für Gesamtverbrauch
  //Subtr[3] = 0x61;// diess muss von Bytezahl abgezogen werden für richtige Anzahl der Bytes für Leistung
  //PowerTeiler[3] = 10; // ermittelter Wert / 10 ergibt Watt
  */
   
  wm.setClass("invert"); // Wifi Manager Start
  wm.setConfigPortalTimeout(300); // auto close configportal after n seconds
  if(!wm.autoConnect("ESP-PV-AP")) {
      Serial.println("Failed to connect");
      // das Programm startet dennoch... - kein Reset
      //ESP.restart();
      for(int i=1;i<6;i++){ digitalWrite(OB_LED_Pin, LEDAN); delay(100);digitalWrite(OB_LED_Pin, LEDAUS);  delay(100);} // 5 x blinken
  } 
  else {       //if you get here you have connected to the WiFi    
    WifiConnectedOnStart = true;
    Serial.println("");
    Serial.println("WLAN verbunden.");
    Serial.print("IP Adresse: ");
    Serial.println(WiFi.localIP());
    server.begin();
  } // WIFI Manger und WLAN verbinden Ende
  
  Wire.begin();
  Wire.beginTransmission(39); // LCD Display address 0x27
  if (Wire.endTransmission () == 0) {
    LCD_avail = true;
    lcd1.begin(); //Display 1 wird gestartet und die Größe des Displays wird angegeben(16 Zeichen, zwei Zeilen)
    if (LCD_light_on)  lcd1.backlight();   //Beleuchtung Display 1 einschalten
    else lcd1.noBacklight();               //Beleuchtung Display 1 ausschalten
    lcd1.clear();
    lcd1.setCursor(0, 0); //Text soll beim ersten Zeichen in der ersten Reihe
    Anzeige(1, 1, "Keine IR-Daten!");
  }
  Serial.println("Touch-Ruhewert: "+String(NoTouchVal));
  // Per WLAN mit dem Netzwerk verbinden
  //Serial.print("Connecting to ");   Serial.println(ssid);

  GetNetTime(); // hole Netzzeit
  UpdateTime(); // Setze die Zeit Variablen 
  Minute_Vortag=minute_;
  hour_last=hour_;
  day_last=day_;
  month_last=month_;
  year_last=year_;
  SetDatumString();
  Systemstartzeit="<font color=white>Letzter Systemstart: "+String(DatumString)+" ";
  if(hour_<10) Systemstartzeit+="0";
  Systemstartzeit+=String(hour_)+":";
  if(minute_<10) Systemstartzeit+="0";
  Systemstartzeit+=String(minute_)+":";
  if(second_<10) Systemstartzeit+="0";
  Systemstartzeit+=String(second_);
  lastCsptAdd=millis(); // Startwert setzen
  LastLoopTm = millis();
}

//eigene Webseiten einbinden
#include "webpageConfig.h"
#include "webpageLoggedData.h"
#include "webpageVerbrauch.h"

typedef struct {
  const unsigned char OBIS[7];
  void (*Handler)();
} OBISHandler;

void Manufacturer() { smlOBISManufacturer(manuf, MAX_STR_MANUF); }

void PowerT1() { smlOBISWh(T1Wh); }

void PowerSum() { smlOBISWh(SumWh); }

void PowerW() { smlOBISW(Watt); }

void Power2NetSum() { smlOBISWh(Sum2NetWh); }

OBISHandler OBISHandlers[] = {
  {{ 0x81, 0x81, 0xc7, 0x82, 0x03, 0xff }, &Manufacturer}, /* 129-129:199.130.3*255 */
  {{ 0x01, 0x00, 0x01, 0x08, 0x01, 0xff }, &PowerT1},      /*   1-  0:  1.  8.1*255 (T1) */
  {{ 0x01, 0x00, 0x01, 0x08, 0x00, 0xff }, &PowerSum},     /*   1-  0:  1.  8.0*255 (T1 + T2) */
  //  {{ 0x01, 0x00, 0x0F, 0x07, 0x00, 0xff }, &PowerW},       /*   1-  0: 15.  7.0*255 (Watt) */
  {{ 0x01, 0x00, 0x10, 0x07, 0x00, 0xff }, &PowerW},       /*   1-  0: 15.  7.0*255 (Watt) aktuelle Wirkleistung */ 
  {{ 0x01, 0x00, 0x02, 0x08, 0x00, 0xff }, &Power2NetSum},     /*   1-  0:  1.  8.0*255 (T1 + T2)   Wirkenergie Einspeisung 07 01 00 02 08 00 ff  */  
  {{ 0, 0 }}
};

void loop() {   //                 ***************************** H A U P T P R O G R A M M  *****************************
  CheckTouchPin();                                    // Touchpin für LCD Beleuchtung checken
  CheckPowermeter();                                  // auf neue Daten prüfen, wenn vorhanden
  CheckTimeForNewInverterValue();                     // prüfe Zeit und sende neue Daten an Inverter 
  CheckFreshPowerDataSentToInverter();                // prüfen, ob neue Daten vorhanden sind, sonst Fehler auf LCD ausgeben und piepen
  CheckBatterie();                                    // prüfen, ob Batterie einspeist, Batterie_empty Status setzen; bei Leer Einspeisezähler korrigieren
  UpdateTime();                                       // Uhrzeitvariablen updaten

  if(millis()>(LastLoopTm + LoopTm)){                 // Webabfrage nur alle ca 15 ms aufrufen, da sonst manchmal Webabfragen hängen bleiben
    LastLoopTm = millis();
    CheckWebRequest();                                // auf Webanfrage checken
  }
  if(WifiConnectedOnStart){
    CheckNewHour();                                   // jede neue Stunde Datensatz auf SPIFFS speichern
    WifiCheck();                                      // alle 5 Minuten Wifi Status überprüfen und ggf reconnect senden
  } 
  else if(millis() > RebootTime) ESP.restart();       // Restart jede Stunde, wenn beim Start kein Wifi verbunden wurde
}                                  //***************************** Ende Hauptprogramm ***********************************



void WifiCheck(){
  if((millis()>LastWifiCheck+300000) and  (WiFi.status() != WL_CONNECTED)) { // check Wifi every 5 Minutes
    tone(Buzzer_Pin,400,50);          // niedriger Ton für Verbindung verloren
    WiFi.disconnect();
    WiFi.reconnect();
    if (WiFi.status() == WL_CONNECTED) tone(Buzzer_Pin,800,200); // Hocher Ton für erfolgreich verbunden
    LastWifiCheck=millis();
  }
}

void CheckFreshPowerDataSentToInverter(){
  if (millis() > lastokSMLReading + 2000) { // 2 Sek keine SML Daten erhalten
    RPRINTLN("Keine IR-Daten!");
    NetPowerAct=0; // Leistung auf Null setzen
    //ChangeInverterPower(0); 
    if (LCD_avail) {
      Anzeige(1, 1, "Keine IR-Daten! ");
      Anzeige(1, 2, "Zähler o. Poti! ");
      Parsingfehler();
    }
    //SML Daten sind aktuell
    lastokSMLReading = millis();
  }
}

void CheckTouchPin(){  // Frage den Touchpin 10 x ab und ermittlel den Mittelwert, um Backlight anzuschalten oder nach def Zeit wieder aus
  TouchSum+=touchRead(Touch_Pin);
  TouchCnt+=1;
  if(TouchCnt==10){
    TouchCnt=0;
    if ((TouchSum/10) < (NoTouchVal/TouchTeiler)){
      LCD_light_onlastInfo = millis();
      LCD_light_on = true;
      lcd1.backlight();
    }
    TouchSum=0;
  }
  if (((LCD_light_onlastInfo + LCD_light_on_duration * 1000) < millis()) and LCD_light_on)  { // LCD Licht ausschalten
    LCD_light_on = false;
    lcd1.noBacklight(); //Beleuchtung Display 1 ausschalten
  }
}


void CheckNewHour(){
  if(not(hour_last==hour_)) { // neue Stunde => Schreibe Werte der vergangenen Stunde in SPIFFS Datei
  //if(not(Minute_Vortag==minute_)) { // {// nur zum Testen
  //if(not(day_==day_last)) {// neuer Tag => Schreibe Werte in SPIFFS Datei
    InitFFS();
    //deleteFile(SPIFFS, ChrDatName);
    if(!existFile(SPIFFS, ChrDatName)) writeFile(SPIFFS, ChrDatName, "YY;MM;DD;hh;ZaehlerWh;NetzWh;EinspgWh; \n");
    String Dat=String(year_-2000)+";"+String(month_last)+";"+String(day_last)+";";
    Dat+=String(hour_last)+";";
    Dat+=String(ConsmptTot_last_h)+";";
    Dat+=String(ConsmptTot-ConsmptTot_last_h)+";"; 
    ConsmptTot_last_h=ConsmptTot;
    long Einspeis;
    Einspeis=_max(EinspeisungWattSek/(3600),0); // Umrechnung Wattsekunden in Wattstunden; negative Falschberechnungen durch Korrekturen vermeiden
    Einspeisung_day+=Einspeis;
    //Einspeis=EinspeisungWattSek/60; // Umrechnung Wattsekunden in Wattminuten
    Dat+=String(Einspeis)+";\n";     // Datensatz zuende 
    int ssid_len = Dat.length() + 1; char WriteChar[ssid_len]; Dat.toCharArray(WriteChar, ssid_len);
    appendFile(SPIFFS, ChrDatName, WriteChar);      
    //Serial.println();Serial.println();Serial.println();    Serial.print("Neuer Datensatz: ");    Serial.println(String(WriteChar));    Serial.println();
    SetDatumString(); // Setze das Datum auf den neuen Tag
    Minute_Vortag=minute_;
    hour_last=hour_;
    EinspeisungWattSek=0;
    if(day_last!=day_) {  // bei neuer Tag Verbrauch des Vortages berechnen 
      Consmpt_lastday=ConsmptTot-ConsmptTot_lastday;
      ConsmptTot_lastday=ConsmptTot;
      Einspeisung_lastday=Einspeisung_day;
      Einspeisung_day=0;
      day_last=day_;  
      month_last=month_;
      year_last=year_;
    }
  } 
}    // ****** Ende neue Stunde ********


void CheckPowermeter() {
  if (Serial2.available()) { // neue IR-Daten vorhanden
     digitalWrite(OB_LED_Pin, LEDAN); 
    if (ReadIRData()) {       // Auslesen
      digitalWrite(OB_LED_Pin, LEDAUS); 
      Get_Cur_Power(); // Werte aus Datensatz ermitteln
      if (Parsing_Success) { // Wattwerte wurde erfolgreich ermittelt
        //digitalWrite(IR_Green_Pin, LEDAN); 
        Print_Power(NetPowerAct); // Daten erfolgreich gelesen
        lastokSMLReading = millis();
        long restzeit = WaitInverterPwr - (millis());
        //Serial.println("Noch warten: " + String(restzeit) + " ms.");
        RPRINT("Zählerstand: ");
        RPRINTLN(String(ConsmptTot) + " Watt");
        
        if(ConsmptTot_lastday==0) {     // beim ersten Start den Vortagszähler einmalig auf aktuellen Zählerstand setzen, damit erster Wert nicht Gesamtverbrauch
          ConsmptTot_lastday=ConsmptTot;       
          ConsmptTot_last_h= ConsmptTot;
        }
      }
      else Parsingfehler();     // Parsing fehler Grün aus und 2 x Rot blinken          
    }     // Ende IR Daten erfolgreich gelesen 
  }// Ende Serial Available
}

void CheckTimeForNewInverterValue(){
// Inverter ansteuern
  if(millis()>lastInverterSetTime+InverterInterval)  { //Mindestzeit verstrichen verstrichen=> neu ansteuern
    if(millis()>WaitInverterPwr){ // Leistungs-Änderungswartezeit verstrichen => mit neuem Wert ansteuern
      RPRINT("Zeit seit letzter Ansteuerung: ");
      RPRINTLN(String(millis()-lastInverterSetTime));
      ChangeInverterPower(NetPowerAct); // Inverter Watt-Logik mit neuem Wert ansteuern
      WaitInverterPwr = millis() + FirstDelay + InverterInterval + abs(InverterPowerOld - InverterPower)/InverterZahl * LastWechselDelay/1000; // 0,9 sek Warten + WattDiff*10ms / /oder Wert 1040 für alle 2 Sek und stablier
      if(FirstDelay>0) FirstDelay=0;// Einmaliges Extra-Warten beim ersten Inverter Setting wird danach auf null gesetz
      InverterPowerOld = InverterPower;
    }
    else SetSoyoPower(InverterPowerOld); // sonst mit altem Wert ansteuern (MinNetInput-MinNetInput=0)(
    
    lastInverterSetTime=millis();
  }
  //if(Batterie_empty) DPRINTLN("Batterie ist leer"); else  DPRINTLN("Batterie ist nicht leer");
  if (NetPowerAct < MinNetInput) Batterie_empty=false; // Minimalverbrauch erreicht bedeutet, dass die Batterie geladen ist
}   // Ende Void Check Powermeter


void CheckBatterie(){ //Hier wird überprüft, ob die Batterie einspeist. Wenn der Inverter lange einspeist, wird dazu die Leistung reduriert und geprüft, ob die Leistungsaufnahme entsprechend ansteigt
  if(InverterPower==MaxWattInverter) { // nachfolgend wird bei MaxPower geprüft, ob noch aus der Batterie eingespeist wird
    if(!MaxPower) {//MaxPower wurde gerade neu erreicht
      MaxPower=true;
      MaxPowerStart=millis();
      RPRINTLN("Verbrauch ist jetz auf Maximum");
    }
    else {// MaxPower ist schon länger aktiv)
      if(((millis()-MaxPowerStart)/1000) > MaxPower_Check_tm) { // MaxPower liegt geforderte Zeit zum Testen an => Verbrauch senken zum Chekcen
        long _NetPowerAct=NetPowerAct;
        //tone(Buzzer_Pin,200,100);
        RPRINTLN("Verbrauchsreduzierung wird durchgeführt, um Batterie zu testen!");
        RPRINTLN("Netzbezug: "+ String(NetPowerAct));
        SetSoyoPower(0);
        delay(4000);    //Warten bis Inverter auf 0
        while(Serial2.available()) Serial2.read(); // Read buffer leeren
        while (!Serial2.available());              // warten bis neue IR-Daten vorhanden
        digitalWrite(OB_LED_Pin, LEDAN); 
        if (ReadIRData()) {       // Auslesen
          digitalWrite(OB_LED_Pin, LEDAUS);
          Get_Cur_Power();
          RPRINTLN("Netzbezug nach Red: "+ String(NetPowerAct));
          long Abw=abs((NetPowerAct-_NetPowerAct)-MaxWattInverter);
          RPRINTLN("Die Abweichung beträgt:  "+String(Abw)+" Watt");
          lastokSMLReading=millis();  // Setzen, um Fehlalarm zu vermeiden
          InverterPower=0;
          MaxPower=false; // Erkennung Maxpower auf Start zurücksetzten, da InverterPower nun auf Null gesetzt und neuer Erkennungszyklus beginnen kann
          if (Abw>(MaxWattInverter/4)){     
            if(!Batterie_empty) EinspeisungWattSek-=MaxWattInverter*MaxPower_Check_tm; // fälschlich gezählte Einspeise-WattSekunden werden 1 x wieder subtrahiert
            Batterie_empty=true; //wenn die Abweichung als die halbe reduzierte Leistung ist, ist die Batterie leer
          }
          else Batterie_empty=false;
        }
      }
    }
  } 
  else MaxPower=false;
}   // Ende Batterieprüfung


void Parsingfehler() { // 3 x rot blinken und piepen
  int dauer =50;
  digitalWrite(IR_Green_Pin, LEDAUS); 
  lcd1.backlight();
  for(int i=1;i<6;i++) {
    digitalWrite(IR_Red_Pin, LEDAN);  
    delay(dauer );
    tone(Buzzer_Pin,400,dauer );
    digitalWrite(IR_Red_Pin, LEDAUS);
    delay(dauer);   
  }
  lcd1.noBacklight();
}

void ChangeInverterPower(long Power) {
  if ((Power <= 0) and (EHZType==1)) {                                                    // bei 0 Verbrauch Leistung reduzieren - nur für Einrichtungszähler
    if (firstdown) {                                                   // Stufe 1: im ersten Schritt nur ein paar Watt reduzieren zb Licht aus
      InverterPower = InverterPower - _max(DownWatt,InverterPower*DownWattProz/100);   
      firstdown=false;
      DownWatt2  = _max(InverterPower / DownFaktor, DownWatt); //Stufe 2 Reduktion berechnen, Teilverbrauch, mindestens aber Minmalwert
    }
    else  InverterPower = InverterPower - DownWatt2;                    // Stufe 2: wenn , nach Stufe 1 Reduktion immer noch Nullverbauch, um Stufe 2 reduzieren
    
  }
  else {               // es wird noch Leistung vom Netz bezogen
    InverterPower = InverterPower + Power*WattsCorr/1000 - MinNetInput; // Leistung des Inverters um aktuellen Verbrauch erhöhen
    InverterPower = _min(InverterPower, MaxWattInverter);
    firstdown=true;                                                     // bei negativ dann wieder ersten Schritt mit Minus x Watt
  }
  if (InverterPower < 0) InverterPower = 0;
  SetSoyoPower(InverterPower);
  RPRINT("Neue Inverteransteuerung: ");   
  RPRINTLN(String(InverterPower));
  if(!Batterie_empty) EinspeisungWattSek+=InverterPower*(millis()-lastCsptAdd)/1000;
  lastCsptAdd=millis();
}

void SetSoyoPower(long power) { // Soyosouce mit Power ansteuern; von Array cmd[i] (mit Dank an BavarianSuperGuy!)
  digitalWrite(IR_Green_Pin, LEDAN); // LED an
  digitalWrite(OB_LED_Pin, LEDAN);
  //tone(Buzzer_Pin,1500,20);
  if(manualInverterPower>0) { // wenn manueller Wert gesetzt ist wird dieser statt berechnetem Wert verwendet
    InverterPower=manualInverterPower;
    power=manualInverterPower;
  }
  NetPowerAct_Display = NetPowerAct; //  in NetPowerAct_Display den lezten Verbrauchsver speichern, für besseren Match aus Einspeisung und resuiterendem Netzbezug (wegen Regelverzögerung nicht gleichzuetig messen, sondern versetzt)
  power=power/InverterZahl;   // Leistung wird auf alle Soyos aufgeteilt
  byte WattsHi, WattsLo;
  WattsHi = power / 256; 
  WattsLo = power % 256;
  cmd[7] = (264 - WattsHi - WattsLo) & 255;             //Checksumme
  cmd[4] = WattsHi; 
  cmd[5] = WattsLo;                                     // Watt setzen
  for (int i = 0; i <= 7; i++) Serial1.write(cmd[i]);   // Werte senden
  DPRINTLN("Soyo:  " + String(InverterPower) + " Watt");
  if(Batterie_empty) Anzeige(1, 1, "Batterie leer!  ");
  else {
    Anzeige(1, 1, "Einspsg:       " + String(InverterPower) + " W");
    if (InverterPower>= 1000) Anzeige(14 - log10(InverterPower+ 1) , 1, String(InverterPower/ 1000) + ".000"); // Formatieren die Ausgabe mit 1.000
    int XPos = 15 - _max(log10((InverterPower+ 1) % 1000), 1) ; 
    Anzeige(XPos , 1, String(InverterPower% 1000) + " W");
  }
  digitalWrite(IR_Green_Pin, LEDAUS); 
  digitalWrite(OB_LED_Pin, LEDAUS);
}

void Anzeige(int Spalte, int Zeile, String Text) { // Ausgabe eines Textes an definierter Position auf LCD
  if (LCD_avail) {
    //while (Text.length() <= 17 - Spalte) Text = Text + " "; // Text mit Leerzeichen auffüllen bis 16 Zeichen
    Text.replace("ä", "\xE1"); // Sonderzeichen umwandeln
    Text.replace("ö", "\xEF");
    Text.replace("ü", "\xF5");
    Text.replace("ß", "\xE2");
    lcd1.setCursor(Spalte - 1, Zeile - 1);
    lcd1.print(Text);
  }
}

void Print_Power(long Power) { // Gib akutellen Verbrauch auf LCD aus
  RPRINTLN();    RPRINT("Netzbezug: ");   RPRINTLN(String(Power) + " Watt");

  if (LCD_avail) {
    Anzeige(1, 2, "Netzbzg:          ");
    if (Power >= 1000) Anzeige(14 - log10(Power + 1) , 2, String(Power / 1000) + ".000"); // Formatieren die Ausgabe mit 1.000
    int XPos = 15 - _max(log10((Power + 1) % 1000), 1) ; 
    //delay(50);
    Anzeige(XPos , 2, String(Power % 1000) + " W");
  }
}

bool ReadIRData() { // True wenn erfolgreich mehr als 200 Zeichen gelesen; false wenn Timeout
  IR_Dataset_length = 0;
  cnt = 0;
  tm_IR_Data = millis();
  while ((millis() < (tm_IR_Data + TIMEOUT))) { // wiederhole bis Timeout oder alles gelesen
    if (Serial2.available() ) { //alle Zeichen einlesen
      tm_IR_Data = millis();
      SID[cnt] = Serial2.read(); // Zeichen lesen
      if (cnt < MaxArray) cnt++;
    }
  }
  IR_Dataset_length = cnt;
  DPRINTLN(); DPRINTLN(); DPRINTLN("Neuer Datensatz mit " + String(IR_Dataset_length) + " Bytes:");
  for (int i=0; i<cnt; i++) {
    DPRINT("0x"); 
    DPRINTLN(SID[i], HEX);
  };
  DPRINTLN(); // Zeichen ausgeben auf SerialMonitor und Zeilenumbruch
  if (cnt > 200) {// Datensatz gelesen
    //IR_Dataset_length=cnt;
    return true;
  }
  else return false;
}

void Get_Cur_Power()  //1 = Wirkleistung, 2 = Gesamtverbrauch, 3 = Gesamteinspeisung
{
  int i = 0, iHandler = 0;
  unsigned char c;
  sml_states_t s;
  for (i = 0; i < IR_Dataset_length; ++i) {
    c = SID[i];
    s = smlState(c);
    if (s == SML_START) {
      /* reset local vars */
      manuf[0] = 0;
      T1Wh = -3;
      SumWh = -3;
      Sum2NetWh = 9999999;
    }
    if (s == SML_LISTEND) {
      /* check handlers on last received list */
      for (iHandler = 0; OBISHandlers[iHandler].Handler != 0 &&
                         !(smlOBISCheck(OBISHandlers[iHandler].OBIS));
           iHandler++)
        ;
      if (OBISHandlers[iHandler].Handler != 0) {
        OBISHandlers[iHandler].Handler();
      }
    }
    if (s == SML_UNEXPECTED) {
      printf(">>> Unexpected byte >%02X<! <<<\n", c);
      Parsing_Success = false;
    }
    if (s == SML_FINAL) {
      printf(">>> FINAL! Checksum OK\n");
      printf(">>> Manufacturer.............: %s\n", manuf);
      printf(">>> Power T1    (1-0:1.8.1)..: %.3f Wh\n", T1Wh);
      printf(">>> Power T1+T2 (1-0:1.8.0)..: %.3f Wh\n", SumWh);
      printf(">>> Watt        (1-0:15.7.0).: %.3f W\n\n", Watt);
      printf(">>> Power 2 Net (1-0:2.8.0)..: %.3f Wh\n", Sum2NetWh);
      cnt = 0;
      Parsing_Success = true;
      NetPowerAct = Watt;        // Wirkleistung zurück
      ConsmptTot  = SumWh;       // Gesamtverbrauch zurück
      Pow2NetTot  = Sum2NetWh;   // Gesamtverbrauch zurück
    }
  }
}

int HdAtPos(String SuchStr, int Pos) { // Wandelt einen Hex an Pos in einem String um
  String HexStr;
  HexStr = SuchStr.substring(Pos * 2, Pos * 2 + 2);
  return strtoll(&HexStr[0], NULL, 16);
}

void GetNetTime() {
  configTime(0, 0, MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line
  if(!getLocalTime(&tm)){
    Serial.println("  Netzzeit nicht gefunden ");
    NetTimeFound=false;
  }
  else {
    Serial.println("  Netzzeit gefunden ");
    NetTimeFound=true;
  }
  setenv("TZ", MY_TZ, 1); // Set environment variable with your time zone
  tzset();

  //StartMillis = millis();
  LastNetTime = millis();
}

void SetDatumString(){
  DatumString=String(day_)+"."+String(month_)+"."+String(year_);
}

void UpdateTime() {
  time(&now); // read the current time
  localtime_r(&now, &tm); // update the structure tm with the current time
  year_   = tm.tm_year + 1900; // years since 1900
  month_  = tm.tm_mon + 1; // January = 0 (!)
  day_    = tm.tm_mday; // day of month
  hour_   = tm.tm_hour; // hours since midnight 0-23
  minute_ = tm.tm_min; // minutes after the hour 0-59
  second_ = tm.tm_sec; // seconds after the minute 0-61*
  // tm.tm_wday; // days since Sunday 0-6
  //if (tm.tm_isdst == 1) // Daylight Saving Time flag  Serial.print("\tDST"); else Serial.print("\tstandard");
 
  if((!NetTimeFound) and (millis()>(LastNetTime + NetTimeRetry))) GetNetTime(); // versuche neue Netzteit zu beziehen wenn bisher erfolglos 
  if ((NetTimeFound) and (millis()>(LastNetTime + NetTimeRefreshTime))) GetNetTime(); // neue Netzteit  beziehen nach vorgegebenem Intervall
    
}

void CheckWebRequest() {
  client = server.available();                 // Client auf Anfrage prüfen
  if (client) {                                // Falls sich ein neuer Client verbindet,
    ClientIP = client.remoteIP();              // Client IP Adrese speichern
    currentWebReqTime = millis();
    previousWebReqTime = currentWebReqTime;
    //client.setTimeout(2); //### neu  war 3000 und hing
    while (client.connected() && (currentWebReqTime - previousWebReqTime <= WebReqTimeoutTime)) {  // loop while the client's connected
      currentWebReqTime = millis();            // wiederholen so lange der Client verbunden ist
      if (client.available()) {                // Fall ein Byte zum lesen da ist,
        char c = client.read();                // lese das Byte, und dann
        Serial.write(c);                       // Serial.print(" ");       // gib es auf dem seriellen Monitor aus
        header += c;
      }
      else { // alle Zeichen gelesen; Auswertung und Aktionen starten
        int headerPos = header.indexOf("GET /get?ShowDataPage="); // suche Aufruf "ShowDataPage" im Header
        
        if(headerPos>=0) { // es wurde die Datenausgabeseite angefragt (Button gedrückt)
          int Req = parseVal(header,"ShowDataPage");  // // Zahl des Buttons ermitteln (1=heute,2=aktMonat,3=Jahr, 4=Vorjahr)
          for(int i=1; i<=Req; i++){tone(Buzzer_Pin,988,30);delay(70);} // Ausgabeton x mal, dass Daten angefordert wurden je nach Button Nummer  
          webpageLoggedData(Req);                                                          // Datenseite anzeigen mit entspr. Parameter
        }
        else if((header.indexOf("GET /get?EHZType")>=0) and islocalClient()) ConfigSave();   // Konfigurationswerte der Konfigseite speichern

        else if((header.indexOf("GET /get?ConfigPage=1")>=0) and islocalClient())  webPageConfig();   // Konfigurationsseite anzeigen

        else webpageVerbrauch();                                                           //  Haupt-Webseite "Verbrauch" anzeigen 
        
        //  Nach Ausgabe der Webseite Client stoppen und Header löschen
        client.stop();
        header = "";    
      }    // Ende alle Aktionen
    }      // Connection closed
  }        // no Client available
  header = "";    // Die Header-Variable für den nächsten Durchlauf löschen
  // Die Verbindung schließen
}

void ConfigSave() {
  long val;
  String t=" au&szlig;erhalb des g&uuml;ltigen Bereichs";
  
  // Webantwort begin ...
  CPL("HTTP/1.1 200 OK");
  CPL("Content-type:text/html");
  CPL("Connection: close");
  CPL();
  CPL("<!DOCTYPE html><html>");
  CPL("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  CPL("<link rel=\"icon\" href=\"data:\">");
  CPL("<body> <h3>Daten gespeichert</h3> ");
  
  
  Serial.println();Serial.println();
  Serial.println("Gesendete Werte:");
  val=parseVal(header,"EHZType"); Serial.println("ZählerType: "+String(val));
  EHZType=val;

  val=parseVal(header,"EHZ_Baudrate"); Serial.println("EHZ Baudrate: "+String(val));
  EHZ_Baudrate=val;

  val=parseVal(header,"EHZ_Send_IR"); Serial.println("EHZ_Send_IR: "+String(val));
  EHZ_Send_IR=val;
  
  val=parseVal(header,"InvType"); Serial.println("InvType: "+String(val));
  InverterType=val;

  val=parseVal(header,"Inv_Baudrate"); Serial.println("Inverter-Baudrate: "+String(val));
  Inv_Baudrate=val;
  
  if(InverterType==1)Inv_Baudrate = 4800; //Soyosource muss auf 4800 gesetzt werden

  val=parseVal(header,"Adress"); Serial.println("Adresse: "+String(val));
  if(val<0 or val>255) CP("<p>Adresse"+ t + "</p>"); 
  else InverterAdress=val;
  
  val=parseVal(header,"InverterZahl"); Serial.println("InverterZahl: "+String(val));
  if(val<1 or val>255) CP("<p>Inverterzahl"+ t + "</p>"); 
  else InverterZahl=val;

  val=parseVal(header,"InverterLeistung"); Serial.println("InverterLeistung: "+String(val));
  if(val<0 or val>32000) CP("<p>Inverterleistung"+ t + "</p>"); 
  else MaxWattInverter=val;
  
  val=parseVal(header,"InverterFaktor"); Serial.println("InverterFaktor: "+String(val));
  if(val<1 or val>9000) CP("<p>Inverterfaktor"+ t + "</p>"); 
  else WattsCorr=val;
  
  val=parseVal(header,"SendInterval"); Serial.println("SendInterval: "+String(val));
  if(val<1 or val>10000) CP("<p>Sendeintervall"+ t + "</p>"); 
  else InverterInterval=val;

  val=parseVal(header,"LastWechselDelay"); Serial.println("LastWechselDelay: "+String(val));
  if(val<1 or val>30000) CP("<p>LastWechselDelay"+ t + "</p>"); 
  else LastWechselDelay=val;
  
  val=parseVal(header,"MinWert"); Serial.println("Min Netz: "+String(val));
  if(val<0 or val>1000) CP("<p>Minimalwert f&uuml;r Netzbezug"+ t + "</p>"); 
  else MinNetInput=val;

  val=parseVal(header,"manualInverterPower"); Serial.println("Fixe Wechselrichterleistung: "+String(val));
  if((val<0) or (val>MaxWattInverter)) CP("<p>Wert f&uuml;r Fixe Wechselrichterleistung"+ t + "</p>"); 
  else manualInverterPower=val;

  WriteConfigToEEPROM(); 
  
  CPL("<br>");
  CPL("<p><a href=\"/\"><button>Startseite</button></a></p>");
  CPL("</body>  </html>");


/*
  // Schnittstellen neu starten mit gespeicherten Baudrate - funktionert nicht
  delay(500);
  Serial1.flush();delay(1); Serial1.end();
  Serial1.begin(Inv_Baudrate, SERIAL_8N1, RS485_RX_Pin, RS485_TX_Pin);   // statt IRHead.begin(9600); // RS485 Schnittstelle starten
  Serial2.flush();delay(1); Serial2.end();
  Serial2.begin(EHZ_Baudrate, SERIAL_8N1, IRHead_RX_Pin, IRHead_TX_Pin); // statt IRHead.begin(9600); // IR Schnittstelle starten
*/  
}




/*
 * I have added this line "gpio_matrix_out(K_OUT, SIG_GPIO_OUT_IDX, false, false);" and now it works.
So the pin is now working as a GPIO and a UART TX line.

Serial2.end();
  delay(1000);
  gpio_matrix_out(K_OUT, SIG_GPIO_OUT_IDX, false, false);
  pinMode(K_OUT, OUTPUT);
  digitalWrite(K_OUT, HIGH);
  delay(300);
  digitalWrite(K_OUT, LOW);
  delay(25);
  digitalWrite(K_OUT, HIGH);
  delay(25);
 */
