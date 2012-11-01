/***********************************
   Ceci est la libraire GoGprs de MCHobby prévu 
   pour fonctionner avec un SIM900

Testé et fonctionnel pour the GPRS Shield de GeekOnFire utilisant 
le SIM900 préconfiguré.
    ------> http://mchobby.be/PrestaShop/product.php?id_product=63

Achetez en un aujourd'hui sur le WebShop de MCHobby et  
aidez le mouvement open-source et open-translate! -Dominique

MCHobby investi du temps et des ressources pour fournir du code open-source ouvert et de la documentation.
Supportez MCHobby et nos efforts de traductions/documentations en achetant vos produits chez MCHobby!

Ecrit par Dominique Meurisse pour MCHobby. 
BSD license, vérifez license.txt pour plus d'informations
----------------------------------------
   This the MCHobby GoGprs library developped for the SIM900 Chip.

Tested and working for the GPRS Shield of GeekOnFire wich is using
a preconfigured SIM900 chipset.
    ------> http://mchobby.be/PrestaShop/product.php?id_product=63

Buy your shield on the MCHobby webshop and support the open-source and open-translate movement! 

Written by Dominique Meurisse for MCHobby. 
BSD license, check the license.txt for more information
****************************************/

// --- IMPORTANT NOTE ---            
//   *** VERY IMPORTANT!!!                                                   ***
//   *** FOR READING SMS, DO NOT FORGET TO CHANGE                            ***
//   *** THE BUFFER SIZE OF NewSoftSerial to 128 bytes                       ***
//   ***   see line _NewSS_MAX_RX_BUFF in NewSoftSerial.h/SoftwareSerial.h   ***
//
// --- DEBUGGING ---
//   define GG_DEBUG to activate debugging message.
//   define GG_DEBUG_GPRS_SERIAL to see bytes send by GPRS module.
//   DEBUGGING REQUIRES THE CALLEE TO initialise the Serial at 19200 bauds.
//        Serial Monitor should have 19200 bauds + Carriage Return,

#ifndef GoGprs_h
#define GoGprs_h

#include "Arduino.h"
#include "SoftwareSerial.h"

#define GOGPRS_LIB_VERSION "0.01" // Tested with Arduino V1.0 

#define PIN_GPRS_RX 7
#define PIN_GPRS_TX 8

// Identification of the various Request send with _sendReq_xxx
#define REQ_NONE 0
#define REQ_IMEI 1
#define REQ_CREG 2 // Network registration status request
#define REQ_READ_SMS_STORE 3 // read a particular sms store
#define REQ_READ_SMS_TEXT  4 // second part of Read_sms_store (the text)
#define REQ_CPAS 5 // read Call status 

#define RES_TIME 500 // time given to GPRS to prepare its response before reading the Serial connection

#define BUFFER_LEN_GPRS_MSG 128 // Len of the library buffer to process the GPRS message
#define SMS_TEXT_LEN        40  // reserve 40 bytes to store the SMS message
#define SMS_CALLEE_LEN      20  // reserve 15 bytes to store the phone number of SMS sender

#define CPAS_READY   0 // Ready to Receive/Send call
#define CPAS_UNKNOWN 2 // Unknow state
#define CPAS_RINGING 3 // Phone is ringing (when calling or receiving call)
#define CPAS_CALLING 4 // Voice call established

// Send debug message @ 19200 Baud, Serial must be initialized 
// by callee
//#define GG_DEBUG
// Forward GPRS incomming serial data to the debug channel.
//#define GG_DEBUG_GPRS_SERIAL

#ifdef GG_DEBUG
  #define GG_DEBUG_PRINTLN(x) Serial.println(x)
  #define GG_DEBUG_PRINTLN2(x,y) Serial.println(x,y)
  #define GG_DEBUG_PRINT(x) Serial.print(x)
  #define GG_DEBUG_PRINT2(x,y) Serial.print(x,y)
#else
  #define GG_DEBUG_PRINTLN(x)
  #define GG_DEBUG_PRINTLN2(x,y)
  #define GG_DEBUG_PRINT(x)
  #define GG_DEBUG_PRINT2(x,y)
#endif 

#ifdef GG_DEBUG_GPRS_SERIAL
  #define GG_DEBUG_GPRS_PRINTLN(x) Serial.println(x)
  #define GG_DEBUG_GPRS_PRINTLN2(x,y) Serial.println(x,y)
  #define GG_DEBUG_GPRS_PRINT(x) Serial.print(x)
  #define GG_DEBUG_GPRS_PRINT2(x,y) Serial.print(x,y)
#else
  #define GG_DEBUG_GPRS_PRINTLN(x)
  #define GG_DEBUG_GPRS_PRINTLN2(x,y)
  #define GG_DEBUG_GPRS_PRINT(x)
  #define GG_DEBUG_GPRS_PRINT2(x,y)
#endif 

class GoGprs {
  private:
    SoftwareSerial *gprsSerial;// SoftwareSerial gprsSerial( PIN_GPRS_TX, PIN_GPRS_RX ); 
    String _gprsMsg;          // INTERNAL: Message Buffer for the incomming GPRS messages
    String _SIM_PIN_CODE;     // INTERNAL: Pin code for the SIM CARD
    boolean _isReady;         // INTERNAL: Indicates if the SIM900 is up and running
    boolean _isCallReady;     // INTERNAL: Module ready to process CALL and SMS messaging
    
    boolean _isOk;            // INTERNAL: Last processGrpsMsg() did receive OK
                              //           By example: send an SMS, perform execute() and 
                              //           finally read the _isOk status ;-)
    boolean _isError;         // INTERNAL: Last processGrpsMsg() did receive ERROR
    boolean _hasSms;          // INTERNAL: Indicates when a SMS has been received
    String  _smsText;         // INTERNAL: contains the text of the last received SMS
    String  _smsCallee;       // INTERNAL: contains the Phone number of the last SMS callee
    byte    _cpasStatus;      // INTERNAL: contains the last AT+CPAS result (call status)
    /* ------------------------------------------------------------
       --- Variables to manage to LOOP of Posting a request to ----
       --- GPRS and waiting the coming response.               ---- */ 
    byte   _lastReqID;        // INTERNAL: Identification of the last requested into
    String _reqRes1;          // INTERNAL: One of the response store, initialized by _processReqResp() 
    String _reqRes2;          // INTERNAL: a second response store, initialized by _processReqResp()

    void _processReqResp( byte lastReqID, String gprsMsg ); // Second step of _processGprsMsg... when message contains a response
    boolean _isReqPending();  // Indicates if a request has been posted... and waits for the repsonse
    void _setRequest( byte aRequest ); // Set the _lastReqID 
    void _clearLastReq();     // Clear the "last request" information 
    /* ------------------------------------------------------------ */
    
    void _processGprsMsg();   // Manage the message stored inside the message buffer
    void _processSmsReceipt();// dedicated _processGprsMsg() to handle reception of SMS (+CMTI message) 
    void _clearGprsMsg();     // clear the gprs message buffer
    void _setReady( boolean newStatus );     // Set the _isReady 
    void _setCallReady( boolean newStatus ); // Set the _isCallReady
    void _setSmsText( String aText );        // Set the _smsText variable
    void _setSmsCallee( String aPhoneNr );   // Set the _smsCallee variable
    void _setHasSms( boolean newValue );     // Set the _hasSms variable
    
    /* GPRS Message Utilities */
    void _sendPinCode();      // Send the PIN code to GPRS
    void _sendInitCmds();     // Send AT Commands to Init the communication between Arduino & GPRS
    void _sendEchoOff();      // Ask SIM900 to stop ECHOING input to the output
    void _sendTextModeSms();  // Ask SIM900 to process SMS as Text
    void _sendDeleteSmsStore( String smsStore ); // Ask the SIM900 to drop the message in a SMS store.
    
    void _sendReq_Imei();         // Request for IMEI
    void _sendReq_NetRegStatus(); // Request the "network registration status" on the fly
    void _sendReq_ReadSmsStore( String smsStore ); // Request the content of a given SMS Store
    void _sendReq_Cpas();         // Request for Call Status
  public:
    GoGprs(SoftwareSerial *ser);       // constructor
    
    void begin( String sPinCode ); // Initializing routine (call once from setup)
    void execute();                // Call at every cycle of loop
    void _reenterExecute( int waitTime );// Re-enter the execute from within the code source
   
    boolean isReady();             // Indicates if the GPRS SIM900 module is up and running (can process commands)
    boolean isCallReady();         // Indicates if the GPRS shield is registered on the Mobile Network
                                   // Can perform call and process SMS when isCallReady is true 
    
    String IMEI();                 // Grab the IMEI number
    
    /* === SMS related functions === */
    boolean sendSms( String sPhoneNr, String sMsg ); // Sending a simple SMS
    
    boolean hasSms();  // did the GPRS shield received a SMS ?
    String  smsText(); // return the last received SMS
    String  smsCallee(); // return the phone number of the SMS sender.
    void    clearSms();  // Clear the hasSms flag (and reset SMS variables)   
    
    /* === CALL related functions === */
    boolean call( String sPhoneNr ); // Make a voice call to a target phone number
    boolean hangUp();                // Terminate a phone call 
    boolean acceptCall();            // Accept an incoming call

    boolean isRinging();      // The Phone is currently ringing (incoming call only)
    boolean isCalling();      // The Voice Call is currently establishing or established
    boolean isReadyForCall(); // No ringing and no calling... so ready for call    
};


#endif


