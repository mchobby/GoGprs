// Code de test pour le GPRS/GSM Shield vendu par MCHobby
//
// Effectue un appel vocal. 
// Attends 20 secondes puis interrompt l'appel (raccroche)
// des informations sur le Serial Monitor d'Arduino IDE!
//
// Testé et fonctionnel pour le GSM/GPRS de GeekOnFire
//    ------> http://mchobby.be/PrestaShop/product.php?id_product=63
// Soutenez le mouvement Open-Source, Open-Harware et Open-Translate en 
//   achetant votre exemplaire sur le WebShop de MCHobby (www.mchobby.be)
//
// ----------------------------------------------------------------------
// Test code for the GPRS/GSM Shield sold by MCHobby
//
// Perform a voice call. Waits 20 sec then hang-up the call.  
// information to the Serial Monitor of Arduino IDE.
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
  
  /* Initiate a Phone call then Hangup */
  if( gsm.isCallReady() ){
    Serial.println( "calling..." );
    if( gsm.call( "+32477665544" ) )
      Serial.println( "Call initiated" );
    else 
      Serial.println( "Call refused!" );
    
    delay( 20000 ); // wait 20 seconds
    
    if( gsm.hangUp() ) 
      Serial.println( "Hang-Up accepted" );
    else 
      Serial.println( "Hang-Up refused!" );
      
    while( true ); // stop program
  }
  else {
    Serial.println( "Not registered on network" );
  }
  delay(500);  
}
  
