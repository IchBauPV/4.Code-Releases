void webpageVerbrauch(){

  CPL("HTTP/1.1 200 OK");
  CPL("Content-type:text/html");
  CPL("Connection: close");
  CPL();
  CPL("<!DOCTYPE html><html>");
  CPL("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  CPL("<meta http-equiv='refresh' content='5'>");
  CPL("<link rel=\"icon\" href=\"data:\">");
  
  CP("<body><style type=text/css>.background {box-sizing: border-box; width: 380px; height: 750px; padding: 5px;");
  //CP("background-image: url(https://www.elektrotechnik-matt.de/wp-content/uploads/2020/07/Photovoltaik-Anlage-3_370x450_web.jpg);");
  CP("background-color:DarkBlue;"); 
  CP("border: 4px solid black; background-size: 100% 100%; }");
  CP("h1 {color: white; font-size: 2.2em; line-height: 0.0; margin-top: 1.0; margin-bottom: 0;} ");
  CP("p {color: white; font-size: 1.1em; line-height: 1.0;  margin-top: 0;  margin-bottom: 0;}");
  CP("h2 {color: white; font-size: 1.3em; line-height: 1.0; margin-top: 0; margin-bottom: 0;}  </style>");
  
  CP("<div class=background> <h1 align=center>STROMVERBRAUCH</h1>");
  CP("<table>");
  int InvPowerAnzeige;
  if(Batterie_empty) InvPowerAnzeige=0; else InvPowerAnzeige = InverterPowerOld;  //InverterPower; ggf (InverterPowerOld+InverterPower)/2 probieren, um besseren Match aus Einspeisungsvorgabe und aktuellem Netzbezug zu erreichen
  
  // *** Beginn Block aktuelle Werte ***
  DPRINT("Gesamtverbrauch :");
  DPRINTLN(String(NetPowerAct + InvPowerAnzeige));
  CP("<tr><td><b><h2>AKTUELL: </td>");          
  //CP("<tr><td><b><p>Gesamtverbrauch: </p></b></td>");
  CP("<td align=right><b><p>" + String(NetPowerAct_Display + InvPowerAnzeige) + " W</h2></b></p></td></tr>");//eigentlich NetPowerAct, aber InverterPowerOld ergibt aufgrund der Regelverzögerung einen besseren Gesamtwert, der nicht so stark springt

  DPRINT("Netzbezug: ");
  DPRINTLN((NetPowerAct));
  CP("<tr><td><p>Netzbezug: </p></td>");
  CP("<td align=right><p>" + String(NetPowerAct_Display) + " W</p></td></tr>");

  DPRINT("Einspeisung:");
  DPRINTLN((InverterPower));
  CP("<tr><td><p>Einspeisung: </p></td>");
  CP("<td align=right><p>" +String(InvPowerAnzeige)+ " W</p></td></tr>");
  CP("<tr><td><hr></td><td><hr></td></tr>"); // Absatz einfügeb

  DPRINT("Zaehlerstand:");
  DPRINTLN((ConsmptTot));
  CP("<tr><td><p>Z&aumlhler Bezug: </p></td><td align=right><p>");
  if(ConsmptTot >= 1000)    CP(String(ConsmptTot / 1000) + ","); 
  if(ConsmptTot % 1000<100) CP("0"); 
  if(ConsmptTot % 1000<10)  CP("0"); 
  CP(String(ConsmptTot % 1000) + " kWh</p></td></tr>");
  CP("<tr>");

  if(EHZType==2) { // Einspeisung anzeigen
    CP("<tr><td><p>Z&aumlhler Einspeisung: </p></td><td align=right><p>");
    if(Pow2NetTot >= 1000)    CP(String(Pow2NetTot / 1000) + ","); 
    if(Pow2NetTot % 1000<100) CP("0"); 
    if(Pow2NetTot % 1000<10)  CP("0"); 
    CP(String(Pow2NetTot % 1000) + " kWh</p></td></tr>");
    CP("<tr>");
  }
  CP("<td><hr></td><td><hr></td></tr>"); // Absatz einfügen 

  //****** ENDE Block Aktuelle Werte *********

  // *** Beginn Block Heute ***
  long Ensp=(Einspeisung_day+_max(EinspeisungWattSek/(3600),0));
  CP("<tr><td><b><h2>HEUTE GESAMT: </b></h2></td>");                    
  CP("<td align=right><b><p>"+String(ConsmptTot-ConsmptTot_lastday+Ensp) +" Wh</p></b></td>");
  CP("<tr><td><p>Netzbezug: </p></td>");
  CPL("<td align=right><p>" + String((ConsmptTot-ConsmptTot_lastday)) + "  Wh</p></td></tr>");

  DPRINT("Einspeisung Inverter :");
  DPRINTLN((InverterPower));
  CP("<tr><td><p>Einspeisung: </p></td>");
  CPL("<td align=right><p>" +String(Ensp)+ "  Wh</p></td></tr>");
  CP("<tr><td><hr></td><td><hr></td></tr>"); // Absatz einfügen
  // *** Ende Block Heute ***

  // *** Beginn Block Gestern *** 
//      if(Consmpt_lastday!=0) { // Werte vom Vortag liegen vor
  if(true) { // Werte vom Vortag liegen vor
    CP("<tr><td><b><h2>GESTERN GESAMT: </b></h2></td>");          
    CP("<td align=right><b><p>"+ String(Consmpt_lastday+Einspeisung_lastday) +" Wh</p></b></td>");
    CP("<tr><td><p>Netzbezug: </p></td>");
    CPL("<td align=right><p>" + String(Consmpt_lastday) + " Wh</p></td></tr>");
    CP("<tr><td><p>Einspeisung: </p></td>");
    CPL("<td align=right><p>" + String(Einspeisung_lastday) + " Wh</p></td></tr>");
    CP("<tr><td><hr></td><td><hr></td></tr>"); // Absatz einfügeb
  }
  // *** Ende Block Gestern ***           
  
  CPL("<br><hr>  ");
  
  CP("<tr><td><b><p>Batterie: </p></td><td align=right><b><p>");
  if(Batterie_empty) CPL("LEER</p></td></tr>"); else CPL("GELADEN</p></td></tr>");
  CP("<tr><td><hr></td><td><hr></td></tr>"); // Absatz einfügen
  CP("</b><hr><br>");
  CP("</table>");
  //CP("<br>");
  
  // Daten abrufen           
  CP("<p>Daten abrufen</p>");          
  CP(" <form action=\"/get\" method =\"GET\">");
  //CP("<form>"); 
  CPL("<button name=\"ShowDataPage\" value=1>Heute</button>");
  CPL("<button name=\"ShowDataPage\" value=2>Monat</button>");
  CPL("<button name=\"ShowDataPage\" value=3>Jahr</button>");
  CPL("<button name=\"ShowDataPage\" value=4>Vorjahr</button>");
  CP("</form>");        
  CP("<hr>");
  //CPL("<br><br>");     

  // Zeit und Datum ausgeben  
  CP("<p>Uhrzeit: " + String(hour_) + ":");
  if (minute_ < 10) CP("0");
  CP(String(minute_) + ":");
  if (second_ < 10) CP("0");
  CP(String(second_));
  CPL("</p> ");
  CPL("<p>Datum : " + String(DatumString));
  CPL("</p><br> ");             //delay(100);
  CPL(Systemstartzeit);
  CPL("<br>Client IP:");
  CPL(client.remoteIP()); 
  if(islocalClient()) CPL(" lokale Adresse"); else CPL(" Internet-Adresse");  

  CPL("<br>SW-Version: " +VersionsNummer);  

  // Konfigurations Button
  if(islocalClient()){ 
    CPL("<br>");          
    CP("<hr>");
//    CP("<form>"); 
    CP(" <form action=\"/get\" method =\"GET\">");
    CPL("<button name=\"ConfigPage\" value=1>Konfiguration</button>");
    CP("</form>");        
    CP("<hr>");
  }
  
  CPL("</div></body></html>");
}
