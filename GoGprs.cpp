/*******************************************
   Ceci est la libraire GoGprs de MCHobby
   Prévu pour fonctionner avec un SIM900

   MCHobby investi du temps et des ressources  
      pour fournir du code source ouvert et 
      de la documentation.

   Supportez MCHobby et nos efforts de 
      traductions/documentations en 
      achetant vos produits chez MCHobby!
      
      www.mchobby.be 

Ecrit par Dominique Meurisse pour MCHobby.
BSD license, vérifez license.txt pour plus 
d'informations
 -------------------------------------------
   This is the GoGprs Library of MCHobby
   It is designed to work with the SIM900.

   MCHobby invest lots of time and ressources to
      to produce open-source code and 
      documentations.

   Please, support MCHobby and our investment in
      documentation and translation work by 
      purchassing your products at MCHobby
      
      www.mchobby.be

Written by Dominique Meurisse for MCHobby.
BSD license, check license.txt  for more information
*******************************************
Tout le texte ci-dessus (dans les deux langues) 
  doit être inclus dans toutes les redistributions.
Text above in both languages must be included in
  any redistributions.
*******************************************/

#include "GoGprs.h"

//Description:
//  Constructor + 
//     Initialisation of Software Serial to GPRS shield
//
GoGprs::GoGprs(SoftwareSerial *ser): _gprsMsg(""), _reqRes1(""), _reqRes2(""), _smsText(""), _smsCallee("") {
  GG_DEBUG_PRINTLN( F("GoGprs constructor") );
  
  GG_DEBUG_PRINT( F("GPRS SoftwareSerial RX/TX link on ") );
  GG_DEBUG_PRINT2( PIN_GPRS_RX, DEC );
  GG_DEBUG_PRINT( F(", ") );
  GG_DEBUG_PRINTLN2( PIN_GPRS_TX, DEC );

  // gprsSerial( PIN_GPRS_RX, PIN_GPRS_TX ) ,
  gprsSerial = ser; // ...override gpsSwSerial with value passed.
  _gprsMsg.reserve( BUFFER_LEN_GPRS_MSG );
  _smsText.reserve( SMS_TEXT_LEN );
  _smsCallee.reserve( SMS_CALLEE_LEN );

  _isReady = false;
  _isCallReady = false;
  _lastReqID   = REQ_NONE;    // No request message has been send to SIM900
  _cpasStatus  = CPAS_UNKNOWN;// Result of AT+CPAS request. 
}

//Description:
//  Just clear the buffer containing the incomming GPRS message
//
void GoGprs::_clearGprsMsg(){
  _gprsMsg = "";
}

//Description:
//  Remember which is the current request (it is important when the
//  response will come)
void GoGprs::_setRequest( byte aRequest ){
  _lastReqID = aRequest;
  GG_DEBUG_PRINT( F("ReqID -> ") );
  GG_DEBUG_PRINTLN2( aRequest, DEC );
}; 

//Description:
//  Clear the "last request" information. Usually done when
//  gprsMsg containing the response to the request has been processed!
//  (see _processReqResp() for more information)
void GoGprs::_clearLastReq(){
  _lastReqID = REQ_NONE;
};     

//Description:
//  Indicates if a request has been posted to the GPRS... and that the 
//  response is still in waiting to be processed by the _processGprsMsg() (and
//  cascading call to _processReqRespt() ).
//
boolean GoGprs::_isReqPending(){
  return (_lastReqID != REQ_NONE);
}

void GoGprs::_setReady( boolean newStatus ){
  _isReady = newStatus;
}

void GoGprs::_setCallReady( boolean newStatus ){
  _isCallReady = newStatus;
  if( _isCallReady ){
    GG_DEBUG_PRINTLN( F("Call Ready: can process call and SMS") );
  }
}

//Description:
//  Set the _smsText variable.
//
void GoGprs::_setSmsText( String aText ){
  _smsText = aText;
}        
    
//Description:
//  Set the _smsCallee variable (who did send the SMS)
//
void GoGprs::_setSmsCallee( String aPhoneNr ) {
  _smsCallee = aPhoneNr;
}

//Description:
//  Set the flag indicating that we had received a SMS message
//  Only set it when the message has been readed from GPRS and stored inside the _smsText !
void GoGprs::_setHasSms( boolean newValue ){
  _hasSms = newValue;
}     

/* ------------------------------
       GPRS Message utilities 
   ------------------------------ */

void GoGprs::_sendPinCode(){
  GG_DEBUG_PRINTLN( F("*** NEED FOR SIM PIN CODE ***") );
  GG_DEBUG_PRINTLN( F("PIN CODE *** WILL BE SEND NOW") );

  if( _SIM_PIN_CODE.length() == 0 ){
    GG_DEBUG_PRINTLN( F("*** OUPS! you did not have provided a PIN CODE for your SIM CARD. ***") );
    GG_DEBUG_PRINTLN( F("*** Please, define the sPinCode when calling begin(). ***") );
    return;
  }
  gprsSerial->print("AT+CPIN=");
  gprsSerial->println( _SIM_PIN_CODE );  
  _reenterExecute( RES_TIME ); // read the response
}

//Description:
//  Send AT Commands to Init the communication between Arduino & GPRS
void GoGprs::_sendInitCmds(){ 
  _sendEchoOff();
  _sendTextModeSms();
}

//Description:
//  Sim900 placed on "Echo OFF" mode 
//  Echoing the incomming command did corrupt the response handling.
//  This is the raison why echo is set off
void GoGprs::_sendEchoOff(){
  GG_DEBUG_PRINTLN( F("Set Echo Off") );
  gprsSerial->println("ATE0");
  _reenterExecute( RES_TIME ); // reads the OK
}

//Description:
//  Ask SIM900 to process SMS as Text format (not binary)
//
void GoGprs::_sendTextModeSms(){
  GG_DEBUG_PRINTLN( F("Text mode SMS") );
  gprsSerial->println("AT+CMGF=1");
  _reenterExecute( RES_TIME ); // reads the "OK"
}

//Description:
//  Ask the SIM900 to delete the content of a particular SMS store
//
void GoGprs::_sendDeleteSmsStore( String smsStore ){
  GG_DEBUG_PRINT( F("Delete SMS Store ") );
  GG_DEBUG_PRINTLN( smsStore );
  
  gprsSerial->print( "AT+CMGD=" );
  gprsSerial->println( smsStore );
  _reenterExecute( RES_TIME ); // reads the "OK"
}

//Description:
//  Request for the unique cell-phone identifier
void GoGprs::_sendReq_Imei(){
  _setRequest( REQ_IMEI );
  gprsSerial->println("AT+CGSN");
}

//Description:
//  Request the reading of a given SMS store
void GoGprs::_sendReq_ReadSmsStore( String smsStore ){
  _setRequest( REQ_READ_SMS_STORE );
  gprsSerial->print( "AT+CMGR=" );
  gprsSerial->println( smsStore );
}

//Description:
// Request the "network registration status" on the fly. 
// Usefull if the GPRS is already up and running... and did already sends 
//  the "Call Ready" long time before your programme starts
// If the GPRS is already registered, he will respond someting like
//  +CREG: 0,1    ( the ",1" means REGISTERED )
void GoGprs::_sendReq_NetRegStatus(){
  _setRequest( REQ_CREG );
  gprsSerial->println("AT+CREG?");
  delay(RES_TIME);
}

//Description:
//  Request for call status
void GoGprs::_sendReq_Cpas(){
  _setRequest( REQ_CPAS );
  gprsSerial->println("AT+CPAS");
}


/* ------------------------------
       processGprsMsg
   ------------------------------ */

//Description:
//  A complete message has been received from GPRS Shield from execute()
//  the message is processed.
void GoGprs::_processGprsMsg(){
  GG_DEBUG_PRINT( F("processGprsMsg for ") );
  GG_DEBUG_PRINTLN( _gprsMsg );
  
  // Initialize Internal response status
  _isOk = false;
  _isError = false;
  
  // Initialize the Ready State 
  _setReady( _gprsMsg.indexOf( "POWER DOWN") < 0 );
  if( _gprsMsg.indexOf( "POWER DOWN")>=0 ){
    // If the module is going down -> not possible to call anymore!
    _setCallReady( false );
    // wait the power down to get completed 
    delay( 1500 ); 
    // waiting will avoids isReady() to send "AT" command
    // and to receive a valid response before power down getting complete
  }
  
  // Process Normal GPRS message
  if( _gprsMsg == "OK" ){
     //Not pertinent: GG_DEBUG_PRINTLN("Skip OK");
     _isOk = true;
  }
  else
  if( _gprsMsg.indexOf( "ERROR" ) == 0 ) {
    _isError = true;
  }
  else
  if( _gprsMsg.indexOf( "+CPIN: SIM PIN" ) >= 0 ){
     _sendPinCode();
  }
  else 
  if( _gprsMsg.indexOf( "RDY" ) == 0 ){
    _setReady( true );
    _setCallReady( false ); // GPRS will bind (or rebin) the Mobile Network
    _sendEchoOff();         // Send echo off
  }
  else
  if( _gprsMsg.indexOf( "Call Ready" ) >= 0 ){
    _setCallReady( true );
    _sendInitCmds();    // Initialise the communication between the Arduino & GPRS
  } 
  else
  if( _gprsMsg.indexOf( "+CMTI" ) >= 0 ) {
    _processSmsReceipt(); // Process the SMS reception (it is a long work... so, sub-routine!) 
  }
  else
  // If it is not a self explaining GPRS message... then it is perhaps
  // a response to a previous request ?
  // 
  // Will manage the _gprsMsg -> update _lastResID + _ReqResult1
  if( _lastReqID != REQ_NONE ){ 
    _processReqResp( _lastReqID, _gprsMsg );
  }
  
  _clearGprsMsg(); 
}

/* Manage the treatment of JUST RECEIVED SMS signaled by the 
   +CMTI spontaneous message.
   
   When the routine is called, the +CMTI message is still in the
   _gprsMsg buffer :-) */
void GoGprs::_processSmsReceipt(){
  GG_DEBUG_PRINTLN( F("*** SMS Received ***" ) );
  // Look for the coma in the full message (+CMTI: "SM",6)
  // In the sample, the SMS is stored at position 6  
  int iPos = _gprsMsg.indexOf( "," );
  String smsStorePos = _gprsMsg.substring( iPos+1 );
  GG_DEBUG_PRINT( F("SMS stored at " ) );
  GG_DEBUG_PRINTLN( smsStorePos );
          
  // Ask to read the SMS store
  _sendReq_ReadSmsStore( smsStorePos );
  
  // Manage the response from this routine
  _reqRes1 = "";
  _reqRes2 = "";
  _reenterExecute( RES_TIME );
  iPos = _reqRes1.indexOf( "\",\"" );
  if( iPos > 0 ){
    int iPos2 = _reqRes1.indexOf( "\",", iPos+1 );
    if( iPos2 > 0 ){
      _reqRes1 = _reqRes1.substring( iPos+3, iPos2 );
    }
  }
  
  // If no data found ...
  if( _reqRes1.length() == 0 )
    _setSmsCallee( "NONE" );
  else
    _setSmsCallee( _reqRes1 );
      
  // Read second part of the message
  _reenterExecute( RES_TIME );
  // GG_DEBUG_PRINT( "[XXX]" );
  // GG_DEBUG_PRINTLN( _reqRes2 );
  
  if( _reqRes2.length() > SMS_TEXT_LEN )
    _setSmsText( _reqRes2.substring(0,SMS_TEXT_LEN) );
  else 
    _setSmsText( _reqRes2 );
    
  // Read the final OK message
  _reenterExecute( RES_TIME );
  
  //_isOk should be true HERE
  
  //--- DELETE the content of the SMS Store ---
  _sendDeleteSmsStore( smsStorePos );
  
  // Signal the SMS received
  _setHasSms( true );
}   

/* Try to decipher the grpsMsg message containing the response to the request.
   Every sub-routine must call the clearLastReq() once the message handled! */
   
void GoGprs::_processReqResp( byte lastReqID, String gprsMsg ){
  GG_DEBUG_PRINT( "_processReqResp for ReqID " );
  GG_DEBUG_PRINTLN2( lastReqID, DEC );
  
  if( lastReqID == REQ_IMEI ){
    _reqRes1 = gprsMsg;
    _clearLastReq();
  }
  else
  if( lastReqID == REQ_CREG ) { // network registration status
    _reqRes1 = gprsMsg;
    _clearLastReq();
  }
  else 
  if( lastReqID == REQ_READ_SMS_STORE ){ // Read a SMS Store ( +CMGR: "REC UNREAD","+32777889900","","12/09/16,21:57:24+08" )
    _reqRes1 = gprsMsg;
    _clearLastReq();
    
    // Get Prepared to receive the Text of the message
    // Text will certainly be readed directly after the +CMGR in the same execute() loop
    _setRequest( REQ_READ_SMS_TEXT );
  }
  else
  if( lastReqID == REQ_READ_SMS_TEXT ){ // Second part of +CMGR message containing the text :-)
    _reqRes2 = gprsMsg;
    _clearLastReq();
  }
  else 
  if( lastReqID == REQ_CPAS ) { // Call status response
    _reqRes1 = gprsMsg;
    if( _reqRes1.indexOf( "+CPAS: " ) < 0 ){
      // We do not have received the expected response... perhaps because 
      //   spontaneous message (like RING) did comes.
      // So, do not change the _cpasStatus... next attempt will certainly
      //   get a valuable response.
      _clearLastReq();
     return; 
    }
   
    // OK, we get a "+CPAS: x" response... try to figure out the code
    if( _reqRes1.indexOf( ": 0" )>0 )
      _cpasStatus = CPAS_READY;         
    else
      if( _reqRes1.indexOf( ": 2" )>0 )
        _cpasStatus = CPAS_UNKNOWN;
      else
        if( _reqRes1.indexOf( ": 3" )>0 )
          _cpasStatus = CPAS_RINGING;
        else
          if( _reqRes1.indexOf( ": 4" )>0 )
            _cpasStatus = CPAS_CALLING;      
          // otherwise... do not change the last known cpas Status
   
    _clearLastReq();
  } // lastReqID == REQ_CPAS
};

/* ------------------------------
       PUBLIC MEMBERS 
   ------------------------------ */

//Description:
//  Begin start the operation of the GoGprs class
//
//Parameters:
//  sPinCode - SIM Pin Code. Will be automatically send to the GPRS shield upon request
void GoGprs::begin( String sPinCode ){
  GG_DEBUG_PRINTLN("GoGprs begin");
  
  _SIM_PIN_CODE = sPinCode;
  gprsSerial->begin(19200);
  
  // If GPRS already running, we just send the echo off immediately
  _sendEchoOff(); 
}

//Description:
//  Background processing loop. Call it a every execution of the loop.
//  
//Remarks:
//  Read the messages comming from GPRS and process them
void GoGprs::execute(){
  char SerialInByte;
  
  while( gprsSerial->available() )  {
    // The class is listening the messages coming from GPRS Shield
    SerialInByte = (unsigned char)gprsSerial->read();
  
    // When GPRS shield is not plug read will return -1 (all bits are ON).
    /* should not occur when using gprsSerial.available()
    if( SerialInByte == -1 ){
       //Only activate this in case of doubt
       // GG_DEBUG_GPRS_PRINT( "(skip for)" );
       // GG_DEBUG_GPRS_PRINTLN2( int(SerialInByte), BIN );
       return; 
    }
    */
                    
    //If the message ends with <CR> then process the GPRS message
    if( SerialInByte == 13 ){
      if( _gprsMsg.length() == 0 ){
          // GPRS also sends extra Empty Line + Carriage Return
          // We will not process those.
          _clearGprsMsg();
      }
      else {
          // Ensure the Serial Monitor to receive a Carriage Return
          GG_DEBUG_GPRS_PRINTLN("");       
          // Process the message buffer
          _processGprsMsg();
      }
    }
    else
    if( SerialInByte == 10 ){
       // EN: Skip Line feed
    }
    else {
      GG_DEBUG_GPRS_PRINT( SerialInByte );
      // store the current character in the message string buffer
      _gprsMsg += String(SerialInByte);
    }
  }  
}

void GoGprs::_reenterExecute(int waitTime){
  // Clear Current Buffer
  _clearGprsMsg();
  // wait time
  delay( waitTime );
  // Start a new execution loop
  execute();
}

//Description:
//  Indicates if the SIM900 module is ready (up and runnin)
boolean GoGprs::isReady() {
  // Check if the module is already known as "up and running"
  if( _isReady )
    return _isReady;
  
  // Additional check (in case of ProcessGprsMsg did miss the "RDY" message)
  gprsSerial->println( "AT" );  // should respond OK is up and running
  delay( RES_TIME ); // give the time for response
  
  // Process the GPRS message
  // if the GPRS respond to the request, the _isReady will be set "true"
  execute();    
  
  // If not ready... must wait the GPRS to been available
  if( ! _isReady )
    delay( 2000 );
    
  // Return Results
  return _isReady;
}

//Description:
//  Indicates if the module is ready to process Call and sending SMS.
//  This is possible only when the Module is registered on the Mobile Network
boolean GoGprs::isCallReady() {
  // If already known as running 
  //  --> return true
  if( _isCallReady )
    return _isCallReady;
    
  // If GPRS is not responding to normal request 
  //  --> not running --> return false 
  if( ! isReady() )
    return false; 
  
  // GPRS Responding --> request Status registration
  _sendReq_NetRegStatus();
  
  // The execute loop 
  _reenterExecute( RES_TIME ); // process incoming responses (may be OK)
  if( _isReqPending() )
    _reenterExecute( RES_TIME ); 
  if( _isReqPending() )
    return false;

  // Result of the request is placed into _reqRes1
  //
  // Registered on network is true when ",1" is present in the response +CREG 0,1 
  if( (_reqRes1.indexOf( "+CREG:" )==0) && (_reqRes1.indexOf( ",1" )>0) ){
    _setCallReady( true );
    _sendInitCmds(); // Send AT Commands to Init the communication between Arduino & GPRS
  }
 
  // Impose a delay before next request!
  if( !_isCallReady )
    delay(1000); 
  
  return _isCallReady;
}

String GoGprs::IMEI() {
  _sendReq_Imei();
  delay( RES_TIME );
  _reenterExecute( RES_TIME ); // process incoming responses
  if( _isReqPending() )
    _reenterExecute( RES_TIME );
  if( _isReqPending() )
    return "[ERROR]";
  
  return _reqRes1; // IMEI response is stored there
}

//Description:
//  Send a simple SMS (Message) to a target phone number
//  Phone Number must have the structure +32499778866
boolean GoGprs::sendSms( String sPhoneNr, String sMsg ) {
  boolean _Result = false;
  
  if( !_isReady ){
    GG_DEBUG_PRINTLN( "Shield is not ready" );
    return false;
  }
    
  if( !_isCallReady ){
    GG_DEBUG_PRINTLN( "Not registered on Mobile Network" );
    return false; 
  }
  
  gprsSerial->println("AT+CMGS=\""+sPhoneNr+"\""); // Target Cell Phone Number
  _reenterExecute( RES_TIME ); // reads the ">"
  
  //if( !_isOk )
  //  return false;
    
  gprsSerial->print(sMsg+"\r");// Body of the message
  _reenterExecute( RES_TIME ); // reads the ">"
  
  //Indicates that the message has been sent
  //   GG_DEBUG_PRINTLN( sMsg );
  
  delay(1000); 
  gprsSerial->write(26);  // Send Ctrl+Z (end of SMS message) - write a byte
  gprsSerial->write(13);
  // process response of GPRS
  _reenterExecute(5000); 
  return _isOk; 
}

//Description:
//  Check if the GPRS did receive a SMS.
//  A SMS is received and stored when the library intercepts the spontaneous 
//    +CMTI message issued by the GPRS
//
boolean GoGprs::hasSms() {
  return _hasSms;
}

//Description:
//  read the last received SMS message
//
String GoGprs::smsText() {
  return _smsText;
}

//Description:
//  read the Phone number associated to the last received SMS message
//
String GoGprs::smsCallee() {
  return _smsCallee;
}

//Description:
//  Clear all information and flags about receipt SMS
void GoGprs::clearSms() {
  _setHasSms( false );
  _setSmsText( "" );
  _setSmsCallee( "" );
}

//Description:
//  Initiate a Phone call and returns immediately.
//
//Returns:
//  True when the SIM900 did accept the phone number and start to compose it.
//  
boolean GoGprs::call( String sPhoneNr ) {
  gprsSerial->println("ATD+"+sPhoneNr+";"); // Target Cell Phone Number
  _reenterExecute( RES_TIME ); // reads the "OK"
  return _isOk;    
} 

//Description:
//  Send the Hangup signal to terminate voice call
//Returns:
//  true if the hangup order has been accepted by SIM900
//
boolean GoGprs::hangUp() {
  gprsSerial->println("ATH"); // Target Cell Phone Number
  _reenterExecute( RES_TIME ); // reads the "OK"
  return _isOk;
}

//Description:
//  Accept an incoming call (when GPRS is ringing)
//
boolean GoGprs::acceptCall() {
  gprsSerial->println("ATA"); // Accept incoming call
  _reenterExecute( RES_TIME ); // reads the "OK"
  return _isOk;
}

//Description:
//  Check if the phone is ringing (for incoming call)
//
boolean GoGprs::isRinging() {
  _sendReq_Cpas();
  _reenterExecute( RES_TIME ); // reads the Response
  return _cpasStatus == CPAS_RINGING; 
}

//Description:
//  Check if a phone call is establishing or established with target phone
//
boolean GoGprs::isCalling() {
  _sendReq_Cpas();
  _reenterExecute( RES_TIME ); // reads the Response
  return _cpasStatus == CPAS_CALLING; 
}

//Description:
//  Check if a phone is ready to phone call.
//  This function will return false when a call is currently 
//  initiating or currently performed.
//
boolean GoGprs::isReadyForCall() {
  _sendReq_Cpas();
  _reenterExecute( RES_TIME ); // reads the Response
  return _cpasStatus == CPAS_READY; 
}

