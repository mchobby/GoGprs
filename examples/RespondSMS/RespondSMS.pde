// Code de test pour le GPRS/GSM Shield vendu par MCHobby
//
// Attend l'arrivée d'un SMS et renvoi le contenu du message
//   modifié à l'emetteur du message. 
// Envoi aussi des informations sur le Serial Monitor d'Arduino IDE!
//
// Testé et fonctionnel pour le GSM/GPRS de GeekOnFire
//    ------> http://mchobby.be/PrestaShop/product.php?id_product=63
// Soutenez le mouvement Open-Source, Open-Harware et Open-Translate en 
//   achetant votre exemplaire sur le WebShop de MCHobby (www.mchobby.be)
//
// ----------------------------------------------------------------------
// Test code for the GPRS/GSM Shield sold by MCHobby
//
// Waits for incoming messages then modify it and resend it to sender.  
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
  
  /* Receive SMS and resend message to callee (with additionnal text) */ 
  if( gsm.isCallReady() ){
    Serial.print(".");    // waiting something....
    if( gsm.hasSms() ) {
      Serial.println(""); // Write carriage (improve display)
      String phoneNr = String( gsm.smsCallee() );
      String msg = String( "You said "+gsm.smsText() );
      gsm.clearSms();
      if( gsm.sendSms( phoneNr, msg ) )
        Serial.println( "Successfully replied "+msg+" to "+phoneNr );
      else 
        Serial.println( "Reply "+msg+" to "+phoneNr+ "did failed!" );
    }
    
  } 
  else
    Serial.println( "Not registered on network" );
  delay(500);  
}
  
