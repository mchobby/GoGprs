// Code de test pour le GPRS/GSM Shield vendu par MCHobby
//
// Effectue un appel téléphonique vocal.
// Raccroche automatiquement au bout de 8 + 15 sec.
// Detecte si le destinataire raccroche prématurément.
// Envoi des informations sur le Serial Monitor d'Arduino IDE!
//
// Testé et fonctionnel pour le GSM/GPRS de GeekOnFire
//    ------> http://mchobby.be/PrestaShop/product.php?id_product=63
// Soutenez le mouvement Open-Source, Open-Harware et Open-Translate en 
//   achetant votre exemplaire sur le WebShop de MCHobby (www.mchobby.be)
//
// ----------------------------------------------------------------------
// Test code for the GPRS/GSM Shield sold by MCHobby
//
// This program perform a voice call.
// It automatically hangs-up within 8-15 seconds.
// It can also detect when the target cell phone hang-up the call. 
// It sends information to the Serial Monitor of Arduino IDE.
//
// Tested and works great with the GSM/GPRS shield of GeekOnFire
//    ------> http://mchobby.be/PrestaShop/product.php?id_product=63
// Pick one up today at the MC Hobby webshop and help support open-source,
// open-hardware & open-translate! -Dominique
//
//Written by Dominique Meurisse for MCHobby. 
//BSD license, check the license.txt for more information

//Note: Should be improved a bit. Call() function sometime return "false"
//      even if the call is current establishing :-/

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
  unsigned long currentMillis;

  gsm.execute();
  
    
  if( gsm.isCallReady() ){
    // Wait a few before initiating call... this seems to be important
    delay( 2000 );
    // Start Call
    Serial.println( "Initiating Call" );
    if( gsm.call( "+32477665544" ) )
      Serial.println( "Call initiated" );
    else {
      Serial.println( "Call refused!" );
      while( true ) ; // program end
    }
    
    // Wait a time because isCalling() is also true while 
    // remote host is ringing
    Serial.println( "wait 8 sec to establish communication" );
    delay( 8000 ); 
        
    // Is the voice call established?
    if( not( gsm.isCalling() ) ){
      Serial.println( "User did not responded to call" ); 
      while( true ); // program end
    }
    Serial.println( "Call established (or still ringing)." );

    // 15 seconds OR communcation ends by Host
    Serial.println( "   wait end of call by host OR 15 sec max" );    
    currentMillis = millis();
    while( gsm.isCalling() && ((millis()-currentMillis) < 15000) )
      delay(200);
      
    // If still communicating then ENDS THE CALL
    if( gsm.isCalling() ) {
      Serial.println( "Still under call... HANG UP communication by software" );
      if( gsm.hangUp() ) 
        Serial.println( "Hang-Up accepted" );
      else 
        Serial.println( "Hang-Up refused!" );
    }
    else 
      Serial.println( "Remote host did HANG-UP the phone call" );    
            
     while( true ); // stop program
     
  } // if gsm.isCallReady()
  else 
    Serial.println( "Not registered on network" );
  delay(500);  
}
  
