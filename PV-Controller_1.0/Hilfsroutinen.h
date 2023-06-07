
void EEPROMWriteByte(int Adresse, byte Wert){
  EEPROM.write(Adresse, Wert);   
  EEPROM.commit();
}
void EEPROMWriteUInt(int Adresse, unsigned int  Wert){
  EEPROM.write(Adresse, Wert/256);     // Hi
  EEPROM.write(Adresse+1, Wert%256);   // Lo
  EEPROM.commit();
}
byte EEPROMReadByte(int Adresse){
  return EEPROM.read(Adresse);   
}
unsigned int EEPROMReadUInt(int Adresse){
  unsigned int Wert = 0;
  Wert = EEPROM.read(Adresse) * 256;     // Hi
  Wert+= EEPROM.read(Adresse+1);         // lo
  return Wert;   
}

void ReadConfigFromEEPROM(){
  EHZType =          EEPROMReadByte(1); // EEPROM Adr 1
  InverterType =     EEPROMReadByte(2); // EEPROM Adr 2 
  InverterZahl =     EEPROMReadUInt(3); // EEPROM Adr 3+4
  MaxWattInverter =  EEPROMReadUInt(5); // EEPROM Adr 5+6
  WattsCorr =        EEPROMReadUInt(7); // EEPROM Adr 7+8
  InverterInterval = EEPROMReadUInt(9); // EEPROM Adr 9+10
  InverterAdress =   EEPROMReadUInt(11);// EEPROM Adr 11+12
  MinNetInput =      EEPROMReadUInt(13);// EEPROM Adr 13+14    
  LastWechselDelay = EEPROMReadUInt(15);// EEPROM Adr 15+16   
  Inv_Baudrate =     EEPROMReadUInt(17);// EEPROM Adr 17+18                  // Baudrate des Wechselrichters - noch nicht implementiert
  EHZ_Baudrate =     EEPROMReadUInt(19);// EEPROM Adr 19+20                   // Baudrate des Einheitenzähleres - noch nicht implementiert
  EHZ_Send_IR =      EEPROMReadByte(21);// EEPROM Adr 21   ;                 // Sende IR Anfrage an EHZ; 0= Nein; 1; Ja - noch nicht implementiert
}

void WriteConfigToEEPROM(){
  // wenn keine Werte im EEPROM vorhanden sind, wird mit diesen Werten initialiert und gespeichert
  Serial.println("Konfiguration wird auf EEPRPOM geschrieben");
  EEPROMWriteByte( 0,EEPROM_ConfigMarker);// EEPROM Adr 0; 27 ist Marker, dass es schon eine Initialisierung des EEProm mit Werten gegeben hat   
  EEPROMWriteByte( 1,EHZType);            // EEPROM Adr 1 
  EEPROMWriteByte( 2,InverterType);       // EEPROM Adr 2 
  EEPROMWriteUInt( 3,InverterZahl);       // EEPROM Adr 3+4
  EEPROMWriteUInt( 5,MaxWattInverter);    // EEPROM Adr 5+6
  EEPROMWriteUInt( 7,WattsCorr);          // EEPROM Adr 7+8
  EEPROMWriteUInt( 9,InverterInterval);   // EEPROM Adr 9+10
  EEPROMWriteUInt(11,InverterAdress);     // EEPROM Adr 11+12
  EEPROMWriteUInt(13,MinNetInput);        // EEPROM Adr 13+14 
  EEPROMWriteUInt(15,LastWechselDelay);   // EEPROM Adr 15+16 
  EEPROMWriteUInt(17,Inv_Baudrate);       // EEPROM Adr 17+18               Baudrate des Wechselrichters - noch nicht implementiert
  EEPROMWriteUInt(19,EHZ_Baudrate);       // EEPROM Adr 19+20               Baudrate des Einheitenzähleres - noch nicht implementiert
  EEPROMWriteByte(21,EHZ_Send_IR);           // EEPROM Adr 21                  Sende IR Anfrage an EHZ; 0= Nein; 1; Ja - noch nicht implementiert
}

bool islocalClient(){   // True, wenn IP locale Adresse, sonst falsde
  bool Loc =false;
  int i=ClientIP.toString().indexOf("192.168.");
  if(i==0) Loc=true;
  return Loc;
}


void S2C(String InStrg){  // Schreibt String Dateinamen in Array ChrDatName
  //InStrg+="\0";
  int ssid_len = InStrg.length() + 1;      
  InStrg.toCharArray(ChrDatName, ssid_len);  
}

String parseString(String ReadHeader,String SearchString) { // lies den Wert
  String ResultString="";
  String Zeichen="";
  String tempStr="";
  int headerPos = ReadHeader.indexOf(SearchString); // suche Aufruf "ShowDataPage" im Header
  if(headerPos>=0) { // es wurde die Datenausgabeseite angefragt (Button gedrückt)
     headerPos+=SearchString.length()+1; // auf ersten Wert setzen
     Zeichen=ReadHeader.substring(headerPos,headerPos+1);
     while (Zeichen!=" " and Zeichen!="&")  {
       //Serial.println();Serial.print("Zeichen: ");Serial.println(Zeichen);Serial.println();
       ResultString+=Zeichen;
       headerPos+=1;
       Zeichen=ReadHeader.substring(headerPos,headerPos+1);
     }
  }
  //Serial.println(ResultString);
  return ResultString;
}

long parseVal(String ReadHeader,String SearchString) { // lies den Wert
  long Ergebnis=0;
  Ergebnis= parseString(ReadHeader, SearchString).toInt();
   //  Ergebnis = ResultString.toInt();  
  //Serial.println(Ergebnis);  
  return Ergebnis; 
}
