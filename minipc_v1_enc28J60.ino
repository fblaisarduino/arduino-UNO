
//   ****************************************************************************
//   *                                                                          *
//   *        projet SWID_enc28j60 ( Switch ID )    28 sept 2020                *
//   *                                                                          *
//   ****************************************************************************

 
//  ====================================================
//  ==========   debugg   =====
//  ====================================================

int nbrdbg;
void debug( String L0,String L1,String L2,String L3,String L4,String L5,String L6,String L7,String L8,String L9,String L10,String L11,String L12,String L13,String L14){
  nbrdbg++;

 Serial.print(L0);   Serial.print("," + L1);   Serial.print("," + L2);      Serial.print("," + L3);      Serial.print("," + L4); 
  Serial.print("," + L5);   Serial.print("," + L6);      Serial.print("," + L7);     Serial.print("," + L8);  
  Serial.print("," + L9);    Serial.print("," + L10);   Serial.print("," + L11);      Serial.print("," + L12); 
  Serial.print("," + L13);   Serial.println("," + L14); 

} 

//  ====================================================
//  ==========   affichage avec un OLED 128 par 64 =====
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


// ========================================================================================================
//  ================ Ethernet  ======================== 
// ========================================================================================================

#include <SPI.h>         
#include <EtherCard.h>

byte Ethernet::buffer[335];
 
int rbuflen ;

byte mac[] = {  0xFB, 0xAA, 0xBB, 0xFB, 0xDE, 0xFB };
int NBR = 0;
int position[12];


// ========================================================================================================
//  ================  predeclaration des fonctions  ======================== 
// ========================================================================================================
 

bool si_lldp();
void lldp();
String decoupe( unsigned int local  );  
int trouve( const byte rech[] );
String decoupevlan( unsigned int local  );
int* trouvetlv();
String decompte;
void matrame();

// ========================================================================================================
// =============== fonctionne au debut du script seulement one shot ================ SETUP  ===============
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


ether.begin(sizeof Ethernet::buffer,  mac, 10);

  if (!ether.dhcpSetup())
  {
    
    Serial.println("DHCP failed.");
  }
 
  delay(1000);
  ENC28J60::enablePromiscuous();

}
// ===================================================================================================== 
// ==================== en boucle continue ============================================================== 
// ===================================================================================================== 
void loop()
{
 NBR++;   
 
// ===================== info sur la switch  =======================


int rbuflen = ether.packetReceive();

//  ===========  test ====================
//  pour faire test
// byte  Ethernet::buffer[] = {0x00,0xad,0x01,0x80,0xc2,0x00,0x00,0x0e,0x10,0xcd,0xae,0xe7,0x18,0x00,0x88,0xcc,0x02,0x07,0x04,0x10,0xcd,0xae,0xe7,0x18,0x01,0x04,0x07,0x03,0x10,0xcd,0xae,0xe7,0x18,0x49,0x06,0x02,0x00,0x78,0x08,0x0d,0x55,0x6e,0x69,0x74,0x20,0x32,0x20,0x50,0x6f,0x72,0x74,0x20,0x38,0x0a,0x17,0x52,0x31,0x30,0x34,0x2d,0x43,0x41,0x52,0x43,0x33,0x31,0x35,0x36,0x2d,0x31,0x31,0x35,0x2e,0x31,0x2e,0x32,0x30,0x30,0x0c,0x4c,0x45,0x74,0x68,0x65,0x72,0x6e,0x65,0x74,0x20,0x52,0x6f,0x75,0x74,0x69,0x6e,0x67,0x20,0x53,0x77,0x69,0x74,0x63,0x68,0x20,0x34,0x38,0x35,0x30,0x47,0x54,0x53,0x2d,0x50,0x57,0x52,0x2b,0x20,0x20,0x48,0x57,0x3a,0x31,0x34,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x46,0x57,0x3a,0x35,0x2e,0x38,0x2e,0x30,0x2e,0x31,0x20,0x20,0x20,0x53,0x57,0x3a,0x76,0x35,0x2e,0x38,0x2e,0x33,0x2e,0x30,0x31,0x37,0x0e,0x04,0x00,0x14,0x00,0x04,0x10,0x15,0x05,0x01,0x0a,0x73,0x01,0xc8,0x01,0x00,0x00,0x00,0x00,0x09,0x2b,0x06,0x01,0x04,0x01,0x2d,0x03,0x4e,0x02,0xfe,0x06,0x00,0x80,0xc2,0x01,0x0b,0xfc,0xfe,0x0c,0x00,0x12,0x0f,0x02,0x07,0x01,0x01,0x13,0x00,0x00,0x00,0x00,0xfe,0x07,0x00,0x12,0xbb,0x01,0x00,0x2f,0x04,0xfe,0x07,0x00,0x12,0xbb,0x04,0x13,0x01,0x40,0xfe,0x0c,0x00,0x12,0xbb,0x05,0x31,0x34,0x20,0x20,0x20,0x20,0x20,0x20,0xfe,0x0d,0x00,0x12,0xbb,0x06,0x35,0x2e,0x38,0x2e,0x30,0x2e,0x31,0x20,0x20,0xfe,0x0e,0x00,0x12,0xbb,0x07,0x76,0x35,0x2e,0x38,0x2e,0x33,0x2e,0x30,0x31,0x37,0xfe,0x10,0x00,0x12,0xbb,0x08,0x31,0x35,0x4a,0x50,0x31,0x39,0x35,0x48,0x37,0x33,0x57,0x44,0xfe,0x09,0x00,0x12,0xbb,0x09,0x41,0x76,0x61,0x79,0x61,0xfe,0x10,0x00,0x12,0xbb,0x0a,0x34,0x38,0x35,0x30,0x47,0x54,0x53,0x2d,0x50,0x57,0x52,0x2b,0xfe,0x10,0x00,0x40,0x0d,0x08,0x36,0xa5,0x10,0xcd,0xae,0xe7,0x18,0x00,0x00,0x00,0x02,0x08,0x00,0x00};
//   rbuflen = 335;
//  ========= test ===================



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

debug("lldp" ,">>","vlan","" ,LCD_data[6] ,"pos:",String(local) ,"premier:",String(Ethernet::buffer[191]),"deux:",String(Ethernet::buffer[192]),"","" ,"","");
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
   
 if(   Ethernet::buffer[2]  == 1 &&  Ethernet::buffer[3]  == 128 &&  Ethernet::buffer[4]  == 194 &&  Ethernet::buffer[7]  == 14 )       {
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
   Serial.print(Ethernet::buffer[x]);   Serial.print(" ");
 } 
 Serial.println("");
 for(int x = 0 ; x< 334 ; x++)
 {  
   Serial.print(Ethernet::buffer[x],HEX);    
 } 
 Serial.println("");
}
//  ========================================================
//
//  EXTRAIT UN MORCEAU EN HEXA ET LE RENVOI EN STRING

 String decoupe( unsigned int local  ) {
 unsigned int i;
 int grandeur = Ethernet::buffer[local+1] ;                                                           
  String texte=" "; 
  for ( i = ( local + 2 )   ; i < (local + grandeur + 2)  ; i++  ) 
        texte +=   (char)Ethernet::buffer[i] ;
  return texte;  
}
//  ========================================================
//
//  EXTRAIT UN MORCEAU EN HEXA ET LE RENVOI EN STRING

 String decoupevlan( unsigned int local  ) {
 unsigned int i;
 int grandeur = Ethernet::buffer[local-1] ;
 grandeur = grandeur - 4 ;
  int res;
  String texte ; 
  for ( i = (local+4)   ; i< ( (local+4) + ( grandeur) )   ; i++  ) 
    texte += String(Ethernet::buffer[i],HEX);
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
    if (Ethernet::buffer[i] == rech[0])
      if( (Ethernet::buffer[i+1] == rech[1]) and (Ethernet::buffer[i+2] == rech[2]) and (Ethernet::buffer[i+3] == rech[3]) )
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
  
    position[nbr] =  ( (pos + 2) + Ethernet::buffer[pos+1]   );
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
