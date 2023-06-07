void webpageLoggedData(int Req) {
          CPL("HTTP/1.1 200 OK"); // Sende HTML Header Daten
          CPL("Content-type:text/html");
          CPL("Connection: close");
          CPL();
          // Hier wird nun die HTML Seite angezeigt:
          CPL("<!DOCTYPE html><html>");
          CPL("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          CPL("<link rel=\"icon\" href=\"data:\">");
          CPL("<body>");
          if(Req==4) year_last=year_-1; // Vorjahr:  year_last wird für den Dateinamen benutzt und muss bei Vorjahr Jahr - 1 sein
          InitFFS();
          if(Req==4) year_last=year_; // wieder zurückändern auf aktuelles Jahr          
          File file = SPIFFS.open(ChrDatName);
          CP("<table><tr align=right><td>");
          unsigned long lastSentTime=millis();
          unsigned long TmOut=2500;
          FileDataSet = "";
          HTMLLine = "";
          bool FirstRow=true;
          while(file.available() and (millis()<lastSentTime+TmOut)){ // wenn File vorhanden und kein Timeout, dann SPIFFS-Daten lesen und ausgeben
            lastSentTime=millis();
            char chr=file.read();
            byte bchr = chr+127;   // konvertieren von Char in Byte zum Auswerten
            //Serial.print(chr); Serial.print(" "); Serial.println(String(bchr));
            if (chr==';') {
              //CP("</td><td>"); // nächstes Feld
              HTMLLine+="</td><td>";      // nächstes Feld
              FileDataSet+=";";
            }
            if (bchr==137) {  // Zeilenende bzw Datensatzende 
              //CP("</td></tr><tr align=right><td>"); // 137 ist im Dateisystem das Endw eines Datensatzes
              HTMLLine+="</td></tr><tr align=right><td>";      // nächstes Feld
              FileDataSet+=";";
              int MM,DD,Pos1,Pos2,Pos3;
              Pos1=FileDataSet.indexOf(";");  // Position des zweiten Datenpunktes finden
              if(Pos1>=0){ // ";" im Datensatz gefunden
                Pos2=FileDataSet.indexOf(";",Pos1+1);       
                Pos3=FileDataSet.indexOf(";",Pos2+1);     
                MM=FileDataSet.substring(Pos1+1,Pos2).toInt(); // MM liegt zwischen Pos1 und Pos 2
                DD=FileDataSet.substring(Pos2+1,Pos3).toInt(); // DD liegt zwischen Pos2 und Pos 3
                //Serial.println();Serial.println(Pos1); Serial.println(Pos2);   Serial.println(Pos3); Serial.println(MM); Serial.println(DD);
                if ((FirstRow) and !(Req>=3)){ // Zeilenüberschrift ausgeben, aber nicht bei Jahreswerten, da dort ohnehin alles ausgegeben wird
                  CP(HTMLLine); 
                  FirstRow=false;
                }
                if (Req>=3) CP(HTMLLine);                      // bei Jahr alle Datensätze ausgeben
                if((Req==2) and (MM==month_)) CP(HTMLLine);    // Monat angefragt, richtiger Monat des Datensatzes     
                if((Req==1) and (MM==month_) and (DD==day_)) {           // heute angefragt auch richtiger Tag des Datensatzes        
                  CP(HTMLLine);   
                }                         
                FileDataSet = "";
                HTMLLine = "";
              }
            }
            if (bchr>=64 and (chr!=';')) {
              //CP(chr);      // Nur druckbare Zeichen ausgeben    
              HTMLLine+=chr;      // nächstes Feld
              FileDataSet+=chr;
            }
            CheckTimeForNewInverterValue();  // Checke, ob wieder Powerwerte an den Inverter zu senden sind (nur alte, da keine neue Zählerabfrage gestartet wird)          
            //Serial.println("In sent schleife");             
            lastokSMLReading = millis();   // Setzen, damit es keine Fehlalarm gibt
          }  // Ende Datei ausgeben
          file.close();
          if(millis()<lastSentTime+TmOut) { // wenn kein Timeout, dann HTML normal beenden 
            CP("</td></tr><table>");
            CPL("</body></html>");
          }
          else Serial.println("Timeout der Webverbindung");
}
