// Code de test pour le GPRS/GSM Shield vendu par MCHobby
//
// Vérifie l'état IsCallReady. Permet de savoir sur le module est
// enregistré sur le réseau mobile et capable d'envoyer/recevoir des
// SMS et appels téléphonique
//
// Testé et fonctionnel pour le GSM/GPRS de GeekOnFire
//    ------> http://mchobby.be/PrestaShop/product.php?id_product=63
// Soutenez le mouvement Open-Source, Open-Harware et Open-Translate en 
//   achetant votre exemplaire sur le WebShop de MCHobby (www.mchobby.be)
//
// ----------------------------------------------------------------------
// Test code for the GPRS/GSM Shield sold by MCHobby
//
// Check the state IsCallReady. With that, you can figure out if the 
// SIM900 is registered to a mobile netword and being able to send/receive
// SMS and calls.
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
  
  /* Test if the Module is registered to a Mobile Network */ 
  if( gsm.isCallReady() )
    Serial.println( "isCallReady = TRUE" );
  else
    Serial.println( "isCallReady = false" ); 
  
  delay(500);  
}
  
