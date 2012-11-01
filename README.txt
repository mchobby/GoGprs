=== INTRODUCTION (french) ===

Ceci est la libraire GoGprs de MCHobby prévu pour fonctionner avec un SIM900

Testé et fonctionnel pour the GPRS Shield de GeekOnFire utilisant le SIM900 préconfiguré
    ------> http://mchobby.be/PrestaShop/product.php?id_product=63

Achetez en un aujourd'hui sur le WebShop de MCHobby et aidez le mouvement open-source et open-translate! -Dominique

MCHobby investi du temps et des ressources pour fournir du code open-source ouvert et de la documentation.
Supportez MCHobby et nos efforts de traductions/documentations en achetant vos produits chez MCHobby!

Ecrit par Dominique Meurisse pour MCHobby. 
BSD license, vérifez license.txt pour plus d'informations

=== INTRODUCTION (english) ===

This the MCHobby GoGprs library developped for the SIM900 Chip.

Tested and working for the GPRS Shield of GeekOnFire wich is using a preconfigured SIM900 chipset.
    ------> http://mchobby.be/PrestaShop/product.php?id_product=63

Buy your shield on the MCHobby webshop and support the open-source and open translate movement! 

Written by Dominique Meurisse for MCHobby. 
BSD license, check the license.txt for more information

=== Notes importantes ===

--- Lecture des SMS --- TRES IMPORTANT!!! ---
Pour lire les SMS, il faut modifier la valeur de la taille du buffer de NewSoftSerial sur 128 bytes.
Voir la ligne _NewSS_MAX_RX_BUFF dans NewSoftSerial.h/SoftwareSerial.h 

--- Installer la librairie ---

Pour télécharger, cliquez sur le bouton DOWNLOADS dans le coin à haut à droite, renommer le répertoire décompressé comme GoGprs. 
Vérifiez que la répertoire GoGprs contienne bien GoGprs.cpp et GoGprs.h

Placez le répertoire GoGprs contenant la librairie dans votre répertoire <arduinosketchfolder>/libraries/ folder. 
Vous pourriez avoir besoin de créer le répertoire "libraries" s'il s'agit de votre première libraire.
Redémarrer Arduino IDE.

Truc & astuce: Sur Ubuntu le répertoire libraries se trouve dans /usr/share/arduino-1.0-rc2/libraries/

https://github.com/mchobby/GoGprs-Library
