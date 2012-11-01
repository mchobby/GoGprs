// Code de test pour le GPRS/GSM Shield vendu par MCHobby
//
// Demande le numéro IMEI (identification unique) au module SIM900.
// Envoi la résultat sur le Moniteur Serie d'Arduino IDE.
//
// Testé et fonctionnel pour le GSM/GPRS de GeekOnFire
//    ------> http://mchobby.be/PrestaShop/product.php?id_product=63
// Soutenez le mouvement Open-Source, Open-Harware et Open-Translate en 
//   achetant votre exemplaire sur le WebShop de MCHobby (www.mchobby.be)
//
// ----------------------------------------------------------------------
// Test code for the GPRS/GSM Shield sold by MCHobby
//
// Ask IMEI number to the SIM900 module. 
// Send information to the Serial Monitor of Arduino IDE.
//
// Tested and works great with the GSM/GPRS shield of GeekOnFire
//    ------> http://mchobby.be/PrestaShop/product.php?id_product=63
// Pick one up today at the MC Hobby webshop and help support open-source,
// open-hardware & open-translate! -Dominique
//
//Written by Dominique Meurisse for MCHobby. 
//BSD license, check the license.txt for more information

#include <GoGprs.h>
#include <SoftwareSerial.h>

#define PIN_CODE "7663" // SIM CARD PinCode

// Serial connection to GPRS shield
SoftwareSerial mySerial(PIN_GPRS_RX, PIN_GPRS_TX);
GoGprs gsm = GoGprs(&mySerial);

void setup() {
  // Another serial connexion to Arduino IDE
  // for showing usefull messages
  Serial.begin(19200);
  
  // initiate GPRS Shield communication 
  gsm.begin( PIN_CODE ); 
}   

void loop() {
  gsm.execute();
  
  /* Ask the IMEI number to cell phone */ 
  if( gsm.isReady() ) {
    String theIMEI = gsm.IMEI();
    Serial.print( "IMEI = " );
    Serial.println( theIMEI );
  } 
  delay(500);  
}
  
