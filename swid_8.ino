//  ====================================================
//
//   *******************************************
//   *        projet SWID ( Switch ID )        *
//   *******************************************
//
//           version 8.0
//
//   date debut : 15 mars 2020 
//   MODIF      : 21,22, 28 ,30 MARS ET 1 , 2 , 3 , 13 AVRIL ,   3 ,4 ,6 , 9 , 11 , 13 , 14 , 16 mai 2020
//
//   programmation: Francois Blais Montreal QC
// 
//   capture de trame pour la decouverte de commutateur (lldp et cdp)
//
//   materiels
//   ---------
//   arduino  uno r3  32k prog   2k ram 
//   shield ethernet HanRun hr911105a
//   ecran affichage avec un OLED 128 par 64
//    
//  bibliotheques
//  -------------
//  chemin de bibli shield w5100 lecture en mode raw   C:\Users\fblai\OneDrive\Documents\Arduino\libraries\Ethernet\src\utility
//
//  C:\Users\fblai\OneDrive\Documents\Arduino\libraries
//  ===============  problem  ================================ 
// conflit i2c  et spi   ou bibliotheque  ( adafruit 1306 ??? )   jai pris ssd1306ascii
// 
//  i2c
//  ===
//  5v et gnd
//  A4 , A5  ( 2 dernier)
//  ou au dessus analog et (ref)
//
//  spi
//  ---
//  Numerique D10 ,D12,D13,D14  et  gnd et 3.3v
//
//  ====================================================
//===============================================
// outil de debugage arduino
//===============================================
// exemple: ("pos:" ,String( ),"" + String(  ,DEC ),"" + String(  ,HEX ),"" + (char) abc );
// debug("" ,"","" ,"");
 
//  debug("" ,"","","" ,"","","" ,"","","" ,"","","" ,"","");
int nbrdbg;
void debug( String L0,String L1,String L2,String L3,String L4,String L5,String L6,String L7,String L8,String L9,String L10,String L11,String L12,String L13,String L14){
  nbrdbg++;

 Serial.print(L0);   Serial.print("," + L1);   Serial.print("," + L2);      Serial.print("," + L3);      Serial.print("," + L4); 
  Serial.print("," + L5);   Serial.print("," + L6);      Serial.print("," + L7);     Serial.print("," + L8);  
  Serial.print("," + L9);    Serial.print("," + L10);   Serial.print("," + L11);      Serial.print("," + L12); 
  Serial.print("," + L13);   Serial.println("," + L14); 

} 
//=============================================================================
  //  port description   valeur tvl (4) 1 octets  et avec  grandeur 1 octets  donc 2 octets
  //  system name        valeur tvl (5)  et avec  grandeur
  //  vlan identifier    valeur tvl (127) grandeur 6 ==>>  3 octet ieee 00:80:ce , 1 octet subtype 0x01  donc   2 derniers octets
  //     335 bytes  
  //  destination broadcast lldp   01:80:c2:00:00:0e        0x88cc ?
  
 //  string   https://www.arduino.cc/reference/tr/language/variables/data-types/stringobject/
 // operateur  https://www.arduino.cc/en/Tutorial/StringAppendOperator
 //  conversion     String thisString = String(13, HEX);     String thisString = String(13, BIN);
 //  myString.indexOf(val, from)      myString.substring(from, to)
   
//    ====================== apprentissage ===============================  
//    1- la bonne bibli mon shield est un w5100 Ethernet version 1.1.2 maximum sinon pas fonctions #include <utility/w5100.h> mettre include ethernet sinon marche pas  
//    2- les bibli pour modules  28j60        ethercard et ethershield  ----pas compatible  dans document/arduino
//    3- ecran oled 0.96  #include "SSD1306Ascii.h"  plus léger au adafruit
//    4-  calcul ordi  en binaire qui represente de lheaxdecimal de a base 16 0-9 et a-f
//    5- les include et define ne marche pas dans une fonction
//    6-  la carte a un buffer et doit etre rempli en un certain temp minimum 60 sec ou   60 000 miliseconde 
//    7- certaine erreur de compilation disparaisse si compile a nouveau
//    8- exemple  https://github.com/net-flow/Arduino-LLDP-Viewer/blob/master/lldp.ino
//    9- inString += (char)inChar;   convertir int en char et ajoute a un string
//    10- inString.toInt()    char a int  
//    11- char a[] = {0x052,0x052};  valeur en hexa vers tableau de car >> converti hexa en ascii donne  RR
//    12- pour acceder place le a dans serial.println(a)
//    13-  int grandeur = trame[local-1] ;  converti hex a int
//    14- ethernet attente de 30 seconde puis passe et reboot le system ( donc une tentative )
//    15- socket init dans setup
//    16- String lcd_date  definit global , lcd_date="" initialiser localement ==== Gele le deroulement ?
//    17- attend creation de la class    while (!Serial) 
//    18- global  seulement des declaration/assignation mais pas assignation seul
//    19- Serial.println (x2i ("0bfc"), DEC);  et  String( ,HEX)
//    20-  &&  and    || or        ne pas faire socket dans le vide (test) sinon garbagge
//    21-   i++ ajout apres eval(usuel)       ajoute avant eval ++i  sinon decale de 1
//    22- par defaut byte en decimal  et en comparaison seul le String a peut comparer avec autre string
//    23- meme si on met de lhexa comme valeur mais a la fin il sagit  de décimal en lecture  a=0x01 et if a=1
//    24-  dans fonction declere tab byte et utilisation valeur change ?
//    25-   switch envoi lldp au 30 000 ( 30 sec )
//    26- type ethernet  position  14-15     valeur  0x88cc  dec  136 , 204
//    27- ne pas mettre de delay dans loop sauf si reponse trouver sinon manque le bateau

//    28-  debut  pos 0 = 0  et pos 1 = 173    
//    29- grandeur  6 octets destination  2-8
//    30- grandeur  6 octets source  9-15
//    31- grandeur  2 octets ethernet type  14-15  valeur  0x88cc  dec  136 , 204
//    32- premier tlv  type et grandeur
//    33- signature de reconnaissance de la trame  0,173,1,128,194,0,0,14,60,217,43,193,147,107,136,204

//            0 173 1 128 194 0 0 14 60 217 43 193 147 107 136 204 2 7 4 60 217 43 193 147 0 4 3 7 50 49 6 2 0 120 8 2 50 49 10 8 76 84 83 87 48 49 49 53 12 90 80 114 111 67 117 114 118 101 32 74 57 48 56 56 65 32 83 119 105 116 99 104 32 50 54 49 48 45 52 56 44 32 114 101 118 105 115 105 111 110 32 82 46 49 49 46 55 48 44 32 82 79 77 32 82 46 49 48 46 48 54 32 40 47 115 119 47 99 111 100 101 47 98 117 105 108 100 47 110 101 109 111 40 82 95 110 100 120 41 41 14 4 0 20 0 4 16 12 5 1 10 140 224 10 2 0 0 0 0 0 254 9 0 18 15 1 3 108 0 0 16 0 0 71 69 78 84 58 32 71 111 111 103 108 101 32 67 104 114 111 109 101 47 56 49 46 48 46 52 48 52 52 46 49 51 56 32 87 105 110 100 111 119 115 13 10 13 10 69 73 10 115 71 255 0 137 0 137 0 58 11 94 173 88 1 16 0 1 0 0 0 0 0 0 32 70 65 69 77 69 79 69 74 69 66 69 70 69 78 69 66 69 71 69 80 69 66 69 70 69 79 69 76 69 68 65 65 0 0 32 0 1 83 99 53 1 8 61 7 1 108 75 144 165 237 33 12 8 67 80 67 48 52 52 54 49 60 8 77 83 70 84 32 53 46 48 55 13 1 15 3 6 44 46 47 31 33 121 249 43 252 255 0 0 
//            0AD180C200E3CD92BC1936B88CC2743CD92BC1930437323162078823231A84C54535730313135C5A50726F4375727665204A39303838412053776974636820323631302D34382C207265766973696F6E20522E31312E37302C20524F4D20522E31302E303620282F73772F636F64652F6275696C642F6E656D6F28525F6E64782929E40140410C51A8CE0A200000FE9012F136C00100047454E543A20476F6F676C65204368726F6D652F38312E302E343034342E3133382057696E646F7773DADA4549A7347FF08908903AB5EAD5811001000000204641454D454F454A45424546454E45424547455045424546454F454C45444141002001536335183D716C4B90A5ED21C843504330343436313C84D53465420352E3037D1F362C2E2F1F2179F92BFCFF00
//  wiresharq 0180c200000e3cd92bc1936f88cc0207043cd92bc19300040307313706020078080231370a084c545357303131350c5a50726f4375727665204a39303838412053776974636820323631302d34382c207265766973696f6e20522e31312e37302c20524f4d20522e31302e303620282f73772f636f64652f6275696c642f6e656d6f28525f6e647829290e0400140004100c05010a8ce00a020000000000fe0900120f01036c000010fe070012bb01000f040000
 

// donc id  et grandeur du contenu ( utiliser wireshark pour avoir position ) VALEUR NON STATIC
// (1)  byte 14-15  chassis  grandeur 7 + 2 (id et grandeur en HEXA ) si grandeur 7
// (2)  byte subtype mac 23-24   si grandeur 7
// (3)  time to live  32-33      si grandeur 2
// (4)  description  36-37       si grandeur 13
// (5)  system name  51-52       si grandeur 23
//                   78
//                   156
//                   162
//  avant info ethernet , offset  14 - 23 - 32 -36 -51

//  String ""     char ''
//  declaration  String thisString = String(13,HEX);  DEC , BIN , HEX   ou rien     = "Hello String";
//  .length()   .trim();     .indexOf(':',offsetdepart)    .toInt()   .replace("cible", "par")     .setCharAt( Pos , '=');
//   .substring(debut,fin)  


//  ====================================================
//     affichage avec un OLED 128 par 64
//  ====================================================
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
#define RST_PIN -1
SSD1306AsciiAvrI2c oled;

String LCD_data[7];

void presentation(String L1,String L2,String L3,String L4,String L5,String L6,String L7,String L8){
     oled.clear();
     oled.println(L1);       oled.println(L2);       oled.println(L3);       oled.println(L4); 
     oled.println(L5);       oled.println(L6);       oled.println(L7);       oled.println(L8); 
 } 

//  ================ Ethernet  ======================== 
//  335 octets
//  fin de la trame ethernet qui englobe  08-00-00
//  debut section lldp  \0x01\0x80\0xc2\0x00\0x00\0x0e
//  pout trouver localisation et ip switch   R104- EQUVALENT HEX 52-31-30-34-2d
//  vlan - preceder par hex 00-80-c2-01    2 prochains octets
//  port - unit  equivalent hex 55-6e-69-74
//  2 octets precedent 1-type  2-longueur
// byte rech[]={0x52,0x31,0x30,0x34}; ==pas de retour de ligne ==virgule == pas text""  == hexa vers octet
// char myArray[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; traduit  hex en ascii
// ethernet.begin    reboot arduino si pas de connection apres 30 seconde

#include <SPI.h>         
#include <Ethernet.h>
#include <utility/w5100.h>

SOCKET s; // the socket that will be openend in RAW mode
 
int rbuflen ;

//  ===========  test ====================
//  pour faire test
// byte  trame[] = {0x00,0xad,0x01,0x80,0xc2,0x00,0x00,0x0e,0x10,0xcd,0xae,0xe7,0x18,0x00,0x88,0xcc,0x02,0x07,0x04,0x10,0xcd,0xae,0xe7,0x18,0x01,0x04,0x07,0x03,0x10,0xcd,0xae,0xe7,0x18,0x49,0x06,0x02,0x00,0x78,0x08,0x0d,0x55,0x6e,0x69,0x74,0x20,0x32,0x20,0x50,0x6f,0x72,0x74,0x20,0x38,0x0a,0x17,0x52,0x31,0x30,0x34,0x2d,0x43,0x41,0x52,0x43,0x33,0x31,0x35,0x36,0x2d,0x31,0x31,0x35,0x2e,0x31,0x2e,0x32,0x30,0x30,0x0c,0x4c,0x45,0x74,0x68,0x65,0x72,0x6e,0x65,0x74,0x20,0x52,0x6f,0x75,0x74,0x69,0x6e,0x67,0x20,0x53,0x77,0x69,0x74,0x63,0x68,0x20,0x34,0x38,0x35,0x30,0x47,0x54,0x53,0x2d,0x50,0x57,0x52,0x2b,0x20,0x20,0x48,0x57,0x3a,0x31,0x34,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x46,0x57,0x3a,0x35,0x2e,0x38,0x2e,0x30,0x2e,0x31,0x20,0x20,0x20,0x53,0x57,0x3a,0x76,0x35,0x2e,0x38,0x2e,0x33,0x2e,0x30,0x31,0x37,0x0e,0x04,0x00,0x14,0x00,0x04,0x10,0x15,0x05,0x01,0x0a,0x73,0x01,0xc8,0x01,0x00,0x00,0x00,0x00,0x09,0x2b,0x06,0x01,0x04,0x01,0x2d,0x03,0x4e,0x02,0xfe,0x06,0x00,0x80,0xc2,0x01,0x0b,0xfc,0xfe,0x0c,0x00,0x12,0x0f,0x02,0x07,0x01,0x01,0x13,0x00,0x00,0x00,0x00,0xfe,0x07,0x00,0x12,0xbb,0x01,0x00,0x2f,0x04,0xfe,0x07,0x00,0x12,0xbb,0x04,0x13,0x01,0x40,0xfe,0x0c,0x00,0x12,0xbb,0x05,0x31,0x34,0x20,0x20,0x20,0x20,0x20,0x20,0xfe,0x0d,0x00,0x12,0xbb,0x06,0x35,0x2e,0x38,0x2e,0x30,0x2e,0x31,0x20,0x20,0xfe,0x0e,0x00,0x12,0xbb,0x07,0x76,0x35,0x2e,0x38,0x2e,0x33,0x2e,0x30,0x31,0x37,0xfe,0x10,0x00,0x12,0xbb,0x08,0x31,0x35,0x4a,0x50,0x31,0x39,0x35,0x48,0x37,0x33,0x57,0x44,0xfe,0x09,0x00,0x12,0xbb,0x09,0x41,0x76,0x61,0x79,0x61,0xfe,0x10,0x00,0x12,0xbb,0x0a,0x34,0x38,0x35,0x30,0x47,0x54,0x53,0x2d,0x50,0x57,0x52,0x2b,0xfe,0x10,0x00,0x40,0x0d,0x08,0x36,0xa5,0x10,0xcd,0xae,0xe7,0x18,0x00,0x00,0x00,0x02,0x08,0x00,0x00};
  byte  trame[335]; 
// ============== test ====================  
 
byte mac[] = {  0xFB, 0xAA, 0xBB, 0xFB, 0xDE, 0xFB };
int NBR = 0;
int position[12];

bool si_lldp();
void lldp();
String decoupe( unsigned int local  );  
int trouve( const byte rech[] );
String decoupevlan( unsigned int local  );
int* trouvetlv();
String decompte;
void matrame();

// ========================================================================================================
// =============== fonctionne au debut du script seulement one shot ============================================
// ========================================================================================================
 
void setup()
{
  Serial.begin(9600);

 
 // ============ initialise lecran oled ===================
 #if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
  oled.setFont(Adafruit5x7);
   
   presentation( "SWID version 8.0 " ,    " "  ,"Capteur Ethernet", "" ,"Trame LLDP " ," "," ","F.Blais MTL 2020" );
  delay(5000); 
  
 // ============ initialise ethernet ===================  
 
  W5100.init();
  W5100.writeSnMR(s, SnMR::MACRAW);
  W5100.execCmdSn(s, Sock_OPEN);
}
// ===================================================================================================== 
// ==================== en boucle continue ============================================================== 
// ===================================================================================================== 
void loop()
{
 NBR++;   
 
// ===================== info sur la switch  =======================
  
//  =========== test avec les donnees  ===========
   rbuflen = W5100.getRXReceivedSize(s);
// rbuflen = 335;
// =========== test ===========


// =========== test ===========
//  doit etre en commentaire pour eviter les vidanges lors de test
//
    if(rbuflen>0) {
    if(rbuflen > sizeof(trame))
    rbuflen = sizeof(trame);
    W5100.recv_data_processing(s, trame, rbuflen);
    W5100.execCmdSn(s, Sock_RECV);
 }
//    ========= test ===================


 decompte = "  b:" + String(NBR) + "   g:" +  String(rbuflen) ;

if ( rbuflen > 0 ) {
    if (si_lldp()) {
       lldp();
       presentation( "" , LCD_data[0],LCD_data[1] ,LCD_data[2] ,LCD_data[3] , LCD_data[4] ,LCD_data[5], LCD_data[6]);
      delay(30000);
    }
 }
}
//  ===============================================================================================
//  ==================================  mes fonctions    ====================================
//  ===============================================================================================

void lldp(){
 
   int local=0;
   String partie;
   trouvetlv();     

 //   description 
  partie = decoupe(   position[5] ) ;
  LCD_data[1] =  partie.substring(1,14);
  LCD_data[2] =  partie.substring(15,partie.length() );
  debug("lldp" ,">>","descr",partie ,   LCD_data[1],    LCD_data[2]  ,"" ,"","","" ,"","","" ,"","");
  
// =========================

 // equivalent Unit/port
LCD_data[4] = decoupe(position[4] ) ;
LCD_data[4].trim();
 debug("lldp" ,">>","u/p",LCD_data[4] ,"","","" ,"","","" ,"","","" ,"","");

// ============================
   // equivalent 00-80-c2-01 signature de la ligne vlan   pos:187   0x0bfc = 3068
  byte vlan[]={0x00,0x80,0xc2,0x01}; 
  local = trouve(vlan);

  if (  local < 330 )
    LCD_data[6] = "Vlan:"  + decoupevlan(local) ;
  else
   LCD_data[6] = "   pas Vlan"  ;

debug("lldp" ,">>","vlan","" ,LCD_data[6] ,"pos:",String(local) ,"premier:",String(trame[191]),"deux:",String(trame[192]),"","" ,"","");
}
 // =======================================================================================
 //
 // VERIFI SIL Y A UNE PARTIE DE LA TRAME QUI CORRESPOND AU ID LLDP
 //
 //   lldp_mac[] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e};  // dst mac for lldp traffic, we look for this
 //    vrai valeur    1   128    194   0     0    14
 //   en realite trame[0]=0   trame[1]=173   trame[2]=1  trame[3]=128  trame[4]=194   trame[5]=0   trame[6]=0    trame[7]=14
 //   trame[4]=194  semble fiable
 //  
 //  0,173,1,128,194,0,0,14,60,217,43,193,147,107,136
bool si_lldp() {
   
 if(   trame[2]  == 1 &&  trame[3]  == 128 &&  trame[4]  == 194 &&  trame[7]  == 14 )       {
      matrame();
       return true;       
      } 
    else   
       return false;    
}
//===========================================================
void matrame()
{
  for(int x = 0 ; x< 334 ; x++)
 {  
   Serial.print(trame[x]);   Serial.print(" ");
 } 
 Serial.println("");
 for(int x = 0 ; x< 334 ; x++)
 {  
   Serial.print(trame[x],HEX);    
 } 
 Serial.println("");
}
//  ========================================================
//
//  EXTRAIT UN MORCEAU EN HEXA ET LE RENVOI EN STRING

 String decoupe( unsigned int local  ) {
 unsigned int i;
 int grandeur = trame[local+1] ;                                                           
  String texte=" "; 
  for ( i = ( local + 2 )   ; i < (local + grandeur + 2)  ; i++  ) 
        texte +=   (char)trame[i] ;
  return texte;  
}
//  ========================================================
//
//  EXTRAIT UN MORCEAU EN HEXA ET LE RENVOI EN STRING

 String decoupevlan( unsigned int local  ) {
 unsigned int i;
 int grandeur = trame[local-1] ;
 grandeur = grandeur - 4 ;
  int res;
  String texte ; 
  for ( i = (local+4)   ; i< ( (local+4) + ( grandeur) )   ; i++  ) 
    texte += String(trame[i],HEX);
  texte = x2i(texte);
  texte.trim();
  return texte;  
}
//  ========================================================
//
//  RECHERCHE UN MORCEAU SPECIFIQUE DANS LA TRAME
//
// =======================================================
 int trouve(const byte rech[] ) {
  unsigned int i = 0;
  for ( i = 0 ; i < 330  ; i++ ) {
    if (trame[i] == rech[0])
      if( (trame[i+1] == rech[1]) and (trame[i+2] == rech[2]) and (trame[i+3] == rech[3]) )
      return i;
       }         
  }  
  // =================================================================
  // trouve les deplacements 
  // 
 int* trouvetlv(){
       int nbr;
      int pos = 16;       position[0]=0;   position[1]=16;    
   
  for ( nbr = 2 ; nbr < 9  ; nbr++ ) {
  
    position[nbr] =  ( (pos + 2) + trame[pos+1]   );
    pos  = position[nbr];
debug("trouvetlv" ,">>","pos","" ,String(pos) ,"","" ,"","","" ,"","","" ,"","");
   }
    return position ;
 }
// ======================================================================
   String print_ip(const byte a[], unsigned int offset, unsigned int length) {
  String ip;
  for (unsigned int i = offset; i < offset + length; i++) {
    //    if(i>offset) Serial.print('.');
    //   Serial.print(a[i], DEC);
    if (i > offset) ip = ip + '.';
    ip = ip + String (a[i]);
  }
  int iplentgh;
  return ip;
}
// ======================================================================
String print_mac(const byte a[], unsigned int offset, unsigned int length) {
  String Mac;
  char temp [40];
  LCD_data[1] = "";
  for (unsigned int i = offset; i < offset + length; ++i) {

    if (i > offset) {
      //  LCD_data[1] = LCD_data[1] + Mac + ':';
      Mac = Mac + ':';
    }
    if (a[i] < 0x10) {
      Mac = Mac + '0';
      //    LCD_data[1] = LCD_data[1] + Mac + '0';
    }
    Mac = Mac + String (a[i], HEX);
  }
  LCD_data[1] = LCD_data[1]  + Mac;
  return Mac;
}
//  ====================================================
//
//  converti hexa vers dec en type string
//
//  ====================================================
String x2i(String monhexa)
{
  int x = 0; int nbr;
  for(nbr=0 ; nbr < monhexa.length() ; nbr++) {
    
    if (monhexa[nbr] >= '0' && monhexa[nbr] <= '9') {
      x *= 16;
      x += monhexa[nbr] - '0';
    }
    else if (monhexa[nbr] >= 'A' && monhexa[nbr] <= 'F') {
      x *= 16;
      x += (monhexa[nbr] - 'A') + 10;
    }
    else if (monhexa[nbr] >= 'a' && monhexa[nbr] <= 'f') {
      x *= 16;
      x += (monhexa[nbr] - 'a') + 10;
    }
    else break;
  }
  return String(x,DEC);
}
 // ================= fin script =====================================================================================================  
 // ================= fin script =====================================================================================================  
 // ================= fin script =====================================================================================================  
