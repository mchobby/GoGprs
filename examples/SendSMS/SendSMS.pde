// Code de test pour le GPRS/GSM Shield vendu par MCHobby
//
// Ce code vérifie si le module SIM900 est alimenté et renvoi
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
// This code check if the SIM900 module is powered et returns 
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
  
  /* Test when the Module is ready */ 
  if( gsm.isCallReady() ){
    Serial.println( "REGISTERED ON NETWORK!" );
    // Give some time to GPRS to get stable on Mobile Network.
    // SMS send too shortly after the network registration are usually lost on the the network. 
    delay( 2000 ); 
    if( gsm.sendSms( "+32477665544", "GOF GPRS Shield is Great!" ) ) {
      Serial.println( "SMS Send" );
    }
    else {
      Serial.println( "SMS not sent!" );
    }
    // Avoids the scketch to continuously send sms.
    Serial.println( "Program stopped!" );
    while( true ) ;
  } 
  Serial.println( "Not registered on Network" );
  delay(500);
}
  
