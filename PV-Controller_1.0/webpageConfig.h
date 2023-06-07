void webPageConfig() { 
  String cStr[4];cStr[1]=" ";cStr[2]=" ";cStr[3]=" "; // Platzhalter für Radiobutton Checked String für Invertertyp
  cStr[InverterType]=" checked";
  String EStr[3];EStr[1]=" ";EStr[2]=" ";             // Platzhalter für Radiobutton Checked String für EHZ
  EStr[EHZType]=" checked";

  //Serial.println("Webseite ausgeben");
  CPL("HTTP/1.1 200 OK");
  CPL("Content-type:text/html");
  CPL("Connection: close");
  CPL();
  // Hier wird nun die HTML Seite angezeigt:
  CP("<!DOCTYPE html><html>");
  CP("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  CP("<link rel=\"icon\" href=\"data:\">");
  
  CPL("<body> <h1>Konfiguration</h1> ");
  CPL("<p><b> Stromz&aumlhler-Typ (EHZ):</b></p>");
  CPL(" <form action=\"/get\" method =\"GET\">");
  CPL("<p> <label><input type=radio id=EHZ1 name=EHZType value=1 " + EStr[1]+ "> Einrichtungsz&aumlhler</label></p>");
  CPL("<p> <label><input type=radio id=EHZ2 name=EHZType value=2 " + EStr[2]+ "> Zweirichtungsz&aumlhler</label> </p>");  
  
  CPL(" <label for=EHZ_BaudrateLabel><b>Baudrate EHZ:&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp</b></label>");
  CP(" <select name=EHZ_Baudrate id=EHZ_BaudrateLabel>");
  if(EHZ_Baudrate== 300)  CP("<option value=300 selected>300</option>");   else CP("<option value=300>300</option>");
  if(EHZ_Baudrate==1200)  CP("<option value=1200 selected>1200</option>"); else CP("<option value=1200>1200</option>");
  if(EHZ_Baudrate==4800)  CP("<option value=4800 selected>4800</option>"); else CP("<option value=4800>4800</option>");
  if(EHZ_Baudrate==9600)  CP("<option value=9600 selected>9600</option>"); else CP("<option value=9600>9600</option>");
  if(EHZ_Baudrate==19200)  CP("<option value=19200 selected>19200</option>"); else CP("<option value=19200>19200</option>");

  CPL("</select> ");  
  CPL("<br>");
  
  CP("<label for=SendIR_Label> Send Command an EHZ (nicht implementiert)</label>");
  if(EHZ_Send_IR==0) CP("<input type=checkbox id=SendIR_Label name=EHZ_Send_IR value=1>"); else CP("<input type=checkbox id=SendIR_Label name=EHZ_Send_IR value=1 checked>");
  
  CP("<p><b> Inverter Typ:</b></p>"); 
  CP("<p> <label><input type=radio id=Inv1 name=InvType value=1 " + cStr[1]+ "> Soyosource GTN 1000/1200</label></p>");
  /* CP("<p> <label><input type=radio id=Inv2 name=InvType value=2 " + cStr[2]+ "> GoodWe 3048/5048D - ES</label> </p>");
  CP("<p> <label><input type=radio id=Inv3 name=InvType value=3 " + cStr[3]+ "> Growatt SPH </label>");
  */
  CP(" <label for=Inv_BaudrateLabel><b>Baudrate Inverter:</b></label>");
   CP(" <select name=Inv_Baudrate id=Inv_BaudrateLabel>");
   if(Inv_Baudrate==1200)  CP("<option value=1200 selected>1200</option>");   else CP("  <option value=1200>1200</option>");
   if(Inv_Baudrate==4800)  CP("<option value=4800 selected>4800</option>");   else CP("  <option value=4800>4800</option>");
   if(Inv_Baudrate==9600)  CP("<option value=9600 selected>9600</option>");   else CP("  <option value=9600>9600</option>");
   if(Inv_Baudrate==19200) CP("<option value=19200 selected>19200</option>"); else CP("  <option value=19200>19200</option>");
  CPL("</select> ");   
  CPL("<br>");
  
  CP(" <p><label><b>Inverter Adresse:&nbsp</b><input type=text id=input0 name=Adress style='width:40px; text-align:right' value=" +String(InverterAdress)+"> dezimal</p></label>");
  CP(" <p><label><b>Anzahl Inverter: &nbsp</b><input type=text id=input1 name=InverterZahl style='width:40px;text-align:right'; value=" +String(InverterZahl)+"></P></label>");
  CP(" <p><label><b>Gesamtleistung: &nbsp&nbsp</b><input type=text id=input2 name=InverterLeistung style='width:40px; text-align:right'; value=" +String(MaxWattInverter)+"> Watt maximal</p></label>");
  CP(" <p><label><b>Ansteuerung mit:&nbsp</b><input type=text id=input3 name=InverterFaktor style='width:40px; text-align:right'; value=" +String(WattsCorr)+"> &#8240 </p></label>");
  CP(" <p><label><b>Ansteuerung alle: </b><input type=text id=input4 name=SendInterval style='width:40px; text-align:right' value=" +String(InverterInterval)+"> msec</p></label>");
  CP(" <p><label><b>Verz&oumlgerg. Lastw.: </b><input type=text id=input5 name=LastWechselDelay style='width:40px; text-align:right' value=" +String(LastWechselDelay)+"> msec/kW</p></label>");
  CP(" <p><label><b>Regelung auf:&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp</b><input type=text id=input6 name=MinWert style='width:40px; text-align:right' value=" +String(MinNetInput)+"> Watt Netzbezug</p></label>");
  CP(" <p><label><b>Leistung Fixwert:&nbsp</b><input type=text id=input7 name=manualInverterPower style='width:40px; text-align:right' value=" +String(manualInverterPower)+"> Watt (>0=fix)</p></label>");  
  CPL(" &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp<input type=submit value=\"Speichern\"style='width:150px; text-align:center'>");

  CPL("</form>  </body>  </html>");
}
