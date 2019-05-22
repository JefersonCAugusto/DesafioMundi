//codigo pronto 

  /**********************************************************************************************************
  *       Projeto de RFID desenvolvido por Jeferson Carvalho Augusto em 29/11/16                            *
  *     Aluno do curso de cencia da computação na Anhanguera de Nteroi                                      *
  *      Desenvoldido para Micro contrtolador PIC 18f45520 em 8Mhz de frequencia Compilador Mickro C pro    *
  *         biblioteca utilizada foi com auxilio do blogspot                                                *
  *          http://microcontrolandos.blogspot.com/2014/02/pic-rfid-mfrc522.html                            *                                                                   *
  *         sem esses caras não seria possivel    :-D  														*
  *			ese código foi adaptado para apresentar o desafio da mundi pagg									*
  *			com a implementação de uma totina para rerial emulada de 19200 baud.							* 
  **********************************************************************************************************/






 // pinagem do módulo rfid confguração
sbit MFRC522_CS at RA5_Bit;    // fio amarelo será ligado sda --> RB0     .
sbit MFRC522_Rst at RC7_Bit;    // fio ROXO será ligado RST -->RB1        .
sbit SoftSPI_SDO at RC5_Bit;   // fio amarelo será ligado MISO --> RB4           .
sbit SoftSPI_CLK at RC3_Bit;   // fio VERDE será ligado  SCK-->RB3        .
sbit SoftSPI_SDI at RC4_Bit;  // fio amarelo será ligado MOSI -->RB2         .

sbit MFRC522_CS_Direction at TRISA5_Bit;
sbit MFRC522_Rst_Direction at TRISC7_Bit;
sbit SoftSPI_SDO_Direction at TRISC5_Bit;
sbit SoftSPI_CLK_Direction at TRISC3_Bit;
sbit SoftSPI_SDI_Direction at TRISC4_Bit;

// pinagem modulo lcd
sbit LCD_RS at RE2_bit;
sbit LCD_EN at RE1_bit;
sbit LCD_D4 at RD4_bit;
sbit LCD_D5 at RD5_bit;
sbit LCD_D6 at RD6_bit;
sbit LCD_D7 at RD7_bit;

sbit LCD_RS_Direction at TRISE2_bit;
sbit LCD_EN_Direction at TRISE1_bit;
sbit LCD_D4_Direction at TRISD4_bit;
sbit LCD_D5_Direction at TRISD5_bit;
sbit LCD_D6_Direction at TRISD6_bit;
sbit LCD_D7_Direction at TRISD7_bit;
// End LCD module connections





#include <Built_in.h>

extern sfr sbit MFRC522_CS;
extern sfr sbit MFRC522_Rst;
extern sfr sbit MFRC522_CS_Direction;
extern sfr sbit MFRC522_Rst_Direction;


//MF522 Command word
#define PCD_IDLE              0x00               //NO action; Cancel the current command
#define PCD_AUTHENT           0x0E               //Authentication Key
#define PCD_RECEIVE           0x08               //Receive Data
#define PCD_TRANSMIT          0x04               //Transmit data
#define PCD_TRANSCEIVE        0x0C               //Transmit and receive data,
#define PCD_RESETPHASE        0x0F               //Reset
#define PCD_CALCCRC           0x03               //CRC Calculate
// Mifare_One card command word
#define PICC_REQIDL          0x26               // find the antenna area does not enter hibernation
#define PICC_REQALL          0x52               // find all the cards antenna area
#define PICC_ANTICOLL        0x93               // anti-collision
#define PICC_SElECTTAG       0x93               // election card
#define PICC_AUTHENT1A       0x60               // authentication key A
#define PICC_AUTHENT1B       0x61               // authentication key B
#define PICC_READ            0x30               // Read Block
#define PICC_WRITE           0xA0               // write block
#define PICC_DECREMENT       0xC0               // debit
#define PICC_INCREMENT       0xC1               // recharge
#define PICC_RESTORE         0xC2               // transfer block data to the buffer
#define PICC_TRANSFER        0xB0               // save the data in the buffer
#define PICC_HALT            0x50               // Sleep
#define MI_OK                 0
#define MI_NOTAGERR           1
#define MI_ERR                2
//------------------MFRC522 Register---------------
//Page 0:Command and Status
#define     RESERVED00            0x00
#define     COMMANDREG            0x01
#define     COMMIENREG            0x02
#define     DIVLENREG             0x03
#define     COMMIRQREG            0x04
#define     DIVIRQREG             0x05
#define     ERRORREG              0x06
#define     STATUS1REG            0x07
#define     STATUS2REG            0x08
#define     FIFODATAREG           0x09
#define     FIFOLEVELREG          0x0A
#define     WATERLEVELREG         0x0B
#define     CONTROLREG            0x0C
#define     BITFRAMINGREG         0x0D
#define     COLLREG               0x0E
#define     RESERVED01            0x0F
//PAGE 1:Command
#define     RESERVED10            0x10
#define     MODEREG               0x11
#define     TXMODEREG             0x12
#define     RXMODEREG             0x13
#define     TXCONTROLREG          0x14
#define     TXAUTOREG             0x15
#define     TXSELREG              0x16
#define     RXSELREG              0x17
#define     RXTHRESHOLDREG        0x18
#define     DEMODREG              0x19
#define     RESERVED11            0x1A
#define     RESERVED12            0x1B
#define     MIFAREREG             0x1C
#define     RESERVED13            0x1D
#define     RESERVED14            0x1E
#define     SERIALSPEEDREG        0x1F
//PAGE 2:CFG
#define     RESERVED20            0x20
#define     CRCRESULTREGM         0x21
#define     CRCRESULTREGL         0x22
#define     RESERVED21            0x23
#define     MODWIDTHREG           0x24
#define     RESERVED22            0x25
#define     RFCFGREG              0x26
#define     GSNREG                0x27
#define     CWGSPREG              0x28
#define     MODGSPREG             0x29
#define     TMODEREG              0x2A
#define     TPRESCALERREG         0x2B
#define     TRELOADREGH           0x2C
#define     TRELOADREGL           0x2D
#define     TCOUNTERVALUEREGH     0x2E
#define     TCOUNTERVALUEREGL     0x2F
//PAGE 3:TEST REGISTER
#define     RESERVED30            0x30

//Definição para serial emulada com baud rate de 9600
#define     tx                    Portb.rb7
#define     TIME_BAUD             52



    //Essa rotina serve para pular uma linha apos o envio da string hex de UID
void pula()
{
    tx=0;
    delay_us(52);    //start        //0000 1101 13 cr
    tx=1;            //0
    delay_us(52);
    tx=0;            //1
    delay_us(52);
    tx=1;
    delay_us(52);    //2
    tx=1;
    delay_us(52);    //3
    tx=0;
    delay_us(52);    //4
    tx=0;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    
    
    tx=0;
    delay_us(52);    //start        0000 1010 10 lf
    tx=0;            //0
    delay_us(52);
    tx=1;            //1
    delay_us(52);
    tx=0;
    delay_us(52);    //2
    tx=1;
    delay_us(52);    //3
    tx=0;
    delay_us(52);    //4
    tx=0;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
}


    // a rotina abaixo envia caracter por caracter na saída serial de forma
    //emulada sem a necessidade de um hardware específico para isso
    //não é muito elaborada, mas infelizmente com o clock de 2Mhz
    //executando uma instrução por 1uS foi o melhor que consegui
    //lembrando que o baud é de 9600.
void converte (char dado)
{
switch(dado)
{
case 0 :
    tx=0;
    delay_us(52);    //start        0011 0000
    tx=0;            //0
    delay_us(52);
    tx=0;            //1
    delay_us(52);
    tx=0;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=1;
    delay_us(52);    //4
    tx=1;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;
case 1:       // espaço é 0010 0000
    tx=0;
    delay_us(52);    //start        0011 0001
    tx=1;            //0
    delay_us(52);
    tx=0;            //1
    delay_us(52);
    tx=0;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=1;
    delay_us(52);    //4
    tx=1;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;
case 2:
    tx=0;
    delay_us(52);    //start        0011 0010
    tx=0;            //0
    delay_us(52);
    tx=1;            //1
    delay_us(52);
    tx=0;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=1;
    delay_us(52);    //4
    tx=1;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;
case 3:
    tx=0;
    delay_us(52);    //start        0011 0011
    tx=1;            //0
    delay_us(52);
    tx=1;            //1
    delay_us(52);
    tx=0;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=1;
    delay_us(52);    //4
    tx=1;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;
case 4:
    tx=0;
    delay_us(52);    //start        0011 0100
    tx=0;            //0
    delay_us(52);
    tx=0;            //1
    delay_us(52);
    tx=1;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=1;
    delay_us(52);    //4
    tx=1;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;
case 5:
    tx=0;
    delay_us(52);    //start        0011 0101
    tx=1;            //0
    delay_us(52);
    tx=0;            //1
    delay_us(52);
    tx=1;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=1;
    delay_us(52);    //4
    tx=1;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;
case 6:
    tx=0;
    delay_us(52);    //start        0011 0110
    tx=0;            //0
    delay_us(52);
    tx=1;            //1
    delay_us(52);
    tx=1;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=1;
    delay_us(52);    //4
    tx=1;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;
case 7:
    tx=0;
    delay_us(52);    //start        0011 0111
    tx=1;            //0
    delay_us(52);
    tx=1;            //1
    delay_us(52);
    tx=1;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=1;
    delay_us(52);    //4
    tx=1;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;
case 8:
    tx=0;
    delay_us(52);    //start        0011 1000
    tx=0;            //0
    delay_us(52);
    tx=0;            //1
    delay_us(52);
    tx=0;
    delay_us(52);    //2
    tx=1;
    delay_us(52);    //3
    tx=1;
    delay_us(52);    //4
    tx=1;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;
case 9:
    tx=0;
    delay_us(52);    //start        0011 1001
    tx=1;            //0
    delay_us(52);
    tx=0;            //1
    delay_us(52);
    tx=0;
    delay_us(52);    //2
    tx=1;
    delay_us(52);    //3
    tx=1;
    delay_us(52);    //4
    tx=1;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;
    
    case 11: //b em hex
    tx=0;
    delay_us(52);    //start        0100 0010
    tx=0;            //0
    delay_us(52);
    tx=1;            //1
    delay_us(52);
    tx=0;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=0;
    delay_us(52);    //4
    tx=0;
    delay_us(52);    //5
    tx=1;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;
    
    case 15:     //f em hex
    tx=0;
    delay_us(52);    //start        0100 0110
    tx=0;            //0
    delay_us(52);
    tx=1;            //1
    delay_us(52);
    tx=1;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=0;
    delay_us(52);    //4
    tx=0;
    delay_us(52);    //5
    tx=1;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;

    case 12:     //c em hex
    tx=0;
    delay_us(52);    //start        0100 0011
    tx=1;            //0
    delay_us(52);
    tx=1;            //1
    delay_us(52);
    tx=0;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=0;
    delay_us(52);    //4
    tx=0;
    delay_us(52);    //5
    tx=1;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;

       case 14:   //e em hex
    tx=0;
    delay_us(52);    //start        0100 0101
    tx=1;            //0
    delay_us(52);
    tx=0;            //1
    delay_us(52);
    tx=1;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=0;
    delay_us(52);    //4
    tx=0;
    delay_us(52);    //5
    tx=1;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;

       case 10:   //a em hex
    tx=0;
    delay_us(52);    //start       0100 0001
    tx=1;            //0
    delay_us(52);
    tx=0;            //1
    delay_us(52);
    tx=0;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=0;
    delay_us(52);    //4
    tx=0;
    delay_us(52);    //5
    tx=1;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;

           case 13:   //d em hex
    tx=0;
    delay_us(52);    //start     0100 0100
    tx=0;            //0
    delay_us(52);
    tx=0;            //1
    delay_us(52);
    tx=1;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=0;
    delay_us(52);    //4
    tx=0;
    delay_us(52);    //5
    tx=1;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
    break;

default:
    tx=0;
    delay_us(52);    //start        0100 1101
    tx=1;            //0
    delay_us(52);
    tx=0;            //1
    delay_us(52);
    tx=1;
    delay_us(52);    //2
    tx=1;
    delay_us(52);    //3
    tx=0;
    delay_us(52);    //4
    tx=0;
    delay_us(52);    //5
    tx=1;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit
  break;
    }
     }

    //Essa rotina abaixo peguei na internet para poder criar a serial emulada
    //por motivos de velocidade do processador não pode ser implantada, mas
    //serviu de inspiração para elaborar as outras rotinas.

void escreve_char (char dados) {
    int contagem = 0;
    tx = 0;
    delay_us(52);
    while (dados) //envia dados + stop bit
    {
       //delay_us(TIME_BAUD);   //start bit

        if ((dados & 0x01) == 0X01) //testa bit menos significativo (LSB)
        {
            tx = 1;
        } else {
            tx = 0;
        }
        dados=dados >>1;
        delay_us(TIME_BAUD);
    }
  delay_us(TIME_BAUD); //tempo do ultimo bit
    tx = 1; //volta pra nivel alto (fim de transmissão de caracter)
  delay_us(TIME_BAUD); //stop bit

}

    //Segunda tentativa de fazer a serial emulada nesse momento
    //resolvi debugar o código e ai percebi cada instrução estava demorando
    //muito para ser executada e isso afetava na sincronização da informação
    //que estava sendo enviada na serial


void escrevendo (char valor)
  {
     int contador = 8;
    int xx1 = 1;
      tx=0;
delay_us(52);    //start
    while(contador)
  {
  if (valor & xx1== 1)
  {
     tx=1;
     delay_us(52);    //lsb
  }
  else
  {
      tx=0;
      delay_us(52);    //0
  }

   valor = valor >> 1;
   if (valor) contador=0;
  }
  delay_us(52);    //final
  tx=1;
  delay_us(52);    //lsb
   }

     // a rotina abaixo com algumas pequenas modificações serviu para escrever
    //string já que não existe esse tipo de dado para
    //um processador de 35 instruções assembly.
void escreve_frase(char *frase) {
    int tam_frase = strlen(frase);
    int contagem = 0;
    while (contagem < tam_frase)
    {
        converte(frase[contagem]);
        contagem++;
    }

}



    //as funções abaixo referem-se a biblioteca do módulo 5266
    //
static void MFRC522_Wr( char addr, char value )
{
        MFRC522_CS = 0;
        SPI_Write( ( addr << 1 ) & 0x7E );
        SPI_Write( value );
        MFRC522_CS = 1;
}
static char MFRC522_Rd( char addr )
{
char value;
        MFRC522_CS = 0;
        SPI_Write( (( addr << 1 ) & 0x7E) | 0x80 );
        value = SPI_Read( 0x00 );
        MFRC522_CS = 1;
        return value;
}
static void MFRC522_Clear_Bit( char addr, char mask )
{
     MFRC522_Wr( addr, MFRC522_Rd( addr ) & (~mask) );
}
static void MFRC522_Set_Bit( char addr, char mask )
{
     MFRC522_Wr( addr, MFRC522_Rd( addr ) | mask );
}
void MFRC522_Reset()
{
        MFRC522_Wr( COMMANDREG, PCD_RESETPHASE );
}
void MFRC522_AntennaOn()
{
 MFRC522_Set_Bit( TXCONTROLREG, 0x03 );
}
void MFRC522_AntennaOff()
{
 MFRC522_Clear_Bit( TXCONTROLREG, 0x03 );
}
void MFRC522_Init()
{
     MFRC522_CS_Direction = 0;
     MFRC522_Rst_Direction = 0;
     MFRC522_CS = 1;
     MFRC522_Rst = 1;

     MFRC522_Reset();

     MFRC522_Wr( TMODEREG, 0x8D );      //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
     MFRC522_Wr( TPRESCALERREG, 0x3E ); //TModeReg[3..0] + TPrescalerReg
     MFRC522_Wr( TRELOADREGL, 30 );
     MFRC522_Wr( TRELOADREGH, 0 );

     MFRC522_Wr( TXAUTOREG, 0x40 );    //100%ASK
     MFRC522_Wr( MODEREG, 0x3D );      // CRC valor inicial de 0x6363

     //MFRC522_Clear_Bit( STATUS2REG, 0x08 );//MFCrypto1On=0
     //MFRC522_Wr( RXSELREG, 0x86 );      //RxWait = RxSelReg[5..0]
     //MFRC522_Wr( RFCFGREG, 0x7F );     //RxGain = 48dB
     MFRC522_AntennaOn();
}
char MFRC522_ToCard( char command, char *sendData, char sendLen, char *backData, unsigned *backLen )
{
  char _status = MI_ERR;
  char irqEn = 0x00;
  char waitIRq = 0x00;
  char lastBits;
  char n;
  unsigned i;

  switch (command)
  {
    case PCD_AUTHENT:       //Certification cards close
    {
      irqEn = 0x12;
      waitIRq = 0x10;
      break;
    }
    case PCD_TRANSCEIVE:    //Transmit FIFO data
    {
      irqEn = 0x77;
      waitIRq = 0x30;
      break;
    }
    default:
      break;
  }
  MFRC522_Wr( COMMIENREG, irqEn | 0x80 );  //Interrupt request
  MFRC522_Clear_Bit( COMMIRQREG, 0x80 );   //Clear all interrupt request bit
  MFRC522_Set_Bit( FIFOLEVELREG, 0x80 );   //FlushBuffer=1, FIFO Initialization
  MFRC522_Wr( COMMANDREG, PCD_IDLE );      //NO action; Cancel the current command???



  //Writing data to the FIFO
  for ( i=0; i < sendLen; i++ )
  {
    MFRC522_Wr( FIFODATAREG, sendData[i] );
  }
  //Execute the command
  MFRC522_Wr( COMMANDREG, command );
  if (command == PCD_TRANSCEIVE )
  {
    MFRC522_Set_Bit( BITFRAMINGREG, 0x80 ); //StartSend=1,transmission of data starts
  }
  //Waiting to receive data to complete
  //i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
  i = 0xFFFF;
  do
  {
    //CommIrqReg[7..0]
    //Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
    n = MFRC522_Rd( COMMIRQREG );
    i--;
  }
  while ( i && !(n & 0x01) && !( n & waitIRq ) );
  MFRC522_Clear_Bit( BITFRAMINGREG, 0x80 );    //StartSend=0
  if (i != 0)
  {
    if( !( MFRC522_Rd( ERRORREG ) & 0x1B ) ) //BufferOvfl Collerr CRCErr ProtecolErr
    {
      _status = MI_OK;
      if ( n & irqEn & 0x01 )
      {
        _status = MI_NOTAGERR;       //??
      }
      if ( command == PCD_TRANSCEIVE )
      {
        n = MFRC522_Rd( FIFOLEVELREG );
        lastBits = MFRC522_Rd( CONTROLREG ) & 0x07;
        if (lastBits)
        {
          *backLen = (n-1) * 8 + lastBits;
        }
        else
        {
          *backLen = n * 8;
        }
        if (n == 0)
        {
          n = 1;
        }
        if (n > 16)
        {
        }
        //Reading the received data in FIFO
        for (i=0; i < n; i++)
        {
          backData[i] = MFRC522_Rd( FIFODATAREG );
        }

  backData[i] = 0;
      }
    }
    else
    {
      _status = MI_ERR;
    }
  }
  //MFRC522_Set_Bit( CONTROLREG, 0x80 );
  //MFRC522_Wr( COMMANDREG, PCD_IDLE );
  return _status;
}
char MFRC522_Request( char reqMode, char *TagType )
{
  char _status;
  unsigned backBits;
  MFRC522_Wr( BITFRAMINGREG, 0x07 );
  TagType[0] = reqMode;
  _status = MFRC522_ToCard( PCD_TRANSCEIVE, TagType, 1, TagType, &backBits );
  if ( (_status != MI_OK) || (backBits != 0x10) )
  {
    _status = MI_ERR;
  }
  return _status;
}
void MFRC522_CRC( char *dataIn, char length, char *dataOut )
{
char i, n;
    MFRC522_Clear_Bit( DIVIRQREG, 0x04 );
    MFRC522_Set_Bit( FIFOLEVELREG, 0x80 );


    for ( i = 0; i < length; i++ )
    {
        MFRC522_Wr( FIFODATAREG, *dataIn++ );
    }

    MFRC522_Wr( COMMANDREG, PCD_CALCCRC );

    i = 0xFF;
    //Espera a finalização do Calculo do CRC
    do
    {
        n = MFRC522_Rd( DIVIRQREG );
        i--;
    }
    while( i && !(n & 0x04) );        //CRCIrq = 1

    dataOut[0] = MFRC522_Rd( CRCRESULTREGL );
    dataOut[1] = MFRC522_Rd( CRCRESULTREGM );
}
char MFRC522_SelectTag( char *serNum )
{
  char i;
  char _status;
  char size;
  unsigned recvBits;
  char buffer[9];



  buffer[0] = PICC_SElECTTAG;
  buffer[1] = 0x70;

  for ( i=2; i < 7; i++ )
  {
    buffer[i] = *serNum++;
  }

  MFRC522_CRC( buffer, 7, &buffer[7] );

  _status = MFRC522_ToCard( PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits );
  if ( (_status == MI_OK) && (recvBits == 0x18) )
  {
    size = buffer[0];
  }
  else
  {
    size = 0;
  }
  return size;
}

void MFRC522_Halt()
{
  unsigned unLen;
  char buff[4];

  buff[0] = PICC_HALT;
  buff[1] = 0;
  MFRC522_CRC( buff, 2, &buff[2] );
  MFRC522_Clear_Bit( STATUS2REG, 0x80 );
  MFRC522_ToCard( PCD_TRANSCEIVE, buff, 4, buff, &unLen );
  MFRC522_Clear_Bit( STATUS2REG, 0x08 );
}
char MFRC522_Auth( char authMode, char BlockAddr, char *Sectorkey, char *serNum )
{
  char _status;
  unsigned recvBits;
  char i;
  char buff[12];

  //Verify the command block address + sector + password + card serial number
  buff[0] = authMode;
  buff[1] = BlockAddr;

  for ( i = 2; i < 8; i++ )
  {
    buff[i] = Sectorkey[i-2];
  }

  for ( i = 8; i < 12; i++ )
  {
    buff[i] = serNum[i-8];
  }

  _status = MFRC522_ToCard( PCD_AUTHENT, buff, 12, buff, &recvBits );

  if ( ( _status != MI_OK ) || !( MFRC522_Rd( STATUS2REG ) & 0x08 ) )
  {
    _status = MI_ERR;
  }

  return _status;
}
char MFRC522_Write( char blockAddr, char *writeData )
{
  char _status;
  unsigned recvBits;
  char i;
  char buff[18];
  buff[0] = PICC_WRITE;
  buff[1] = blockAddr;

  MFRC522_CRC( buff, 2, &buff[2] );
  _status = MFRC522_ToCard( PCD_TRANSCEIVE, buff, 4, buff, &recvBits );
  if ( (_status != MI_OK) || (recvBits != 4) || ( (buff[0] & 0x0F) != 0x0A) )
  {
    _status = MI_ERR;
  }
  if (_status == MI_OK)
  {
    for ( i = 0; i < 16; i++ )                //Data to the FIFO write 16Byte
    {
      buff[i] = writeData[i];
    }

    MFRC522_CRC( buff, 16, &buff[16] );
    _status = MFRC522_ToCard( PCD_TRANSCEIVE, buff, 18, buff, &recvBits );
    if ( (_status != MI_OK) || (recvBits != 4) || ( (buff[0] & 0x0F) != 0x0A ) )
    {
      _status = MI_ERR;
    }
  }
  return _status;
}
char MFRC522_Read( char blockAddr, char *recvData )
{
  char _status;
  unsigned unLen;
  recvData[0] = PICC_READ;
  recvData[1] = blockAddr;

  MFRC522_CRC( recvData, 2, &recvData[2] );

  _status = MFRC522_ToCard( PCD_TRANSCEIVE, recvData, 4, recvData, &unLen );
  if ( (_status != MI_OK) || (unLen != 0x90) )
  {
    _status = MI_ERR;
  }
  return _status;
}
char MFRC522_AntiColl( char *serNum )
{
//ttipos de cartoes, (eu tenho o tipo 4 as duas tag sao desse mesmo tipo)
//0x0044 = Mifare_UltraLight
//0x0004 = Mifare_One (S50)
//0x0002 = Mifare_One (S70)
//0x0008 = Mifare_Pro (X)
//0x0344 = Mifare_DESFire
  char _status;
  char i;
  char serNumCheck = 0;
  unsigned unLen;
  MFRC522_Wr( BITFRAMINGREG, 0x00 );                //TxLastBists = BitFramingReg[2..0]
  serNum[0] = PICC_ANTICOLL;
  serNum[1] = 0x20;
  MFRC522_Clear_Bit( STATUS2REG, 0x08 );
  _status = MFRC522_ToCard( PCD_TRANSCEIVE, serNum, 2, serNum, &unLen );
  if (_status == MI_OK)
  {
    for ( i=0; i < 4; i++ )
    {
      serNumCheck ^= serNum[i];
    }

    if ( serNumCheck != serNum[4] )
    {
      _status = MI_ERR;
    }
  }
  return _status;
}

char MFRC522_isCard( char *TagType )
{
    if (MFRC522_Request( PICC_REQIDL, TagType ) == MI_OK)

        return 1;
    else

       return 0;


}
char MFRC522_ReadCardSerial( char *str )
{
char _status;
 _status = MFRC522_AntiColl( str );
 str[5] = 0;
 if (_status == MI_OK)
  return 1;
 else
  return 0;
}

/*********************************************************************************************************************
*               Fim da bbiblioteca                                                                                   *
**********************************************************************************************************************/



 /***************************************************************************
                             declaraão das variáveis
 ***************************************************************************/
int msg1[6];   //variavel que vao receber as Tags
int bau= 140;  //teste  com o baud
char se,mm,hh;        //armazenamento de horas
char x[] = "00:00:00";   // 0  1  2  3  4  5  6  7
int k1=40;              // rotin do tempo para ajuste do reloggio
int k2=0;             //  controle do loop para entrar no modo rfid
int k3=0;
int m1 = 0;
int m2 = 0;
int m3 = 0;
int m4 = 0;
int m5 = 0;
int ajus = 0;
unsigned long contador = 0;
 /***************************************************************************
 As funções abaixo usam variaveis globais então necessitam ficar
 abaixo da declaração. Por esse motivo eu não as coloquei junto com as
 funções do RC5266

 ***************************************************************************/
  void amostra_hora()
  {

    Lcd_Out(1,1,"Desafio");
    Lcd_Out(2,1,"Mundi Pagg");   // 4.294.967.295

 }


    //Fiz uso da memoria do cartão para armazenar algumas informações.
    //talvez de para armazenar os dados do cliente nos bancos disponíveis
    //do cartão. OBS.: precisa ser estudado mais a fundo. apenas se sobrar tempo
    
char key[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };     //senha padrão das ttags rfid lembre de nunca altettrar
char writeData[] = "    Jeferson    ";                            //dado a ser escrito na tag 15 caracteres


    //se o botão 3 for apertado entra no modo de leitura
   void lertag()
   {
    if (!portb.rb3)
       {

       
       delay_ms(200);
       k2=~k2;
       }
   }

  void lerajuste()
  {
    if(!portb.rb4)
        {
        delay_ms(1);
          /********************************************************************
           aqui será a area de teste para as comunicações seriais 19200
          ********************************************************************/


    tx=0;
    delay_us(52);    //start        0010 0000
    tx=0;            //0
    delay_us(52);
    tx=0;            //1
    delay_us(52);
    tx=0;
    delay_us(52);    //2
    tx=0;
    delay_us(52);    //3
    tx=0;
    delay_us(52);    //4
    tx=1;
    delay_us(52);    //5
    tx=0;
    delay_us(52);    //6
    tx=0;
    delay_us(52);   //7
    tx=1;
    delay_us(104);    //stop bit

          

        }
   }
              //na rotina de interrupção tem os controles de timers
              //e a rotina de incremente de segundo para o relogio
void interrupt()
{

      if (INTCON.TMR0IF  == 1)
      {
            se++;
            k1++;
            TMR0L=0XF7;
            TMR0H=0XC2;
            INTCON.TMR0IF=0;
            
            if(hh==24)
            {
                     se=0;   mm=0;  hh=0;
            }

            if(mm==60)
            {
                  mm=0;
                  hh++;
            }

            if (se==60)
            {
                  se=0;
                  mm++;
            }
      }
}





void main()
{
     //aqui criei várias variaveis umas vão receber os dados já responsaveis
     //pelos cartões e outras estão vazias.
     //já preparei também em forma de vetor para caso seja realmente implantado
     //o sistema de armazenamento dos dados.
char temp[6]= {0,0,0,0,0};
char val1[6]={0b11110011, 0b11001111, 0b01101000, 0b11000111, 0b10010011}; //chavero azul
char val2[6]={0b00010110, 0b01110011, 0b01001011, 0b10110101, 0b10011011};
char val3[6]={0b00000011, 0b01000110, 0b01001010, 0b11101100, 0b11100011};  //rio card meu
char val4[6]={0b00001000, 0b00110011, 0b00000001, 0b00001100, 0b11111111};
char val5[6]={0b0,0b0,0b0,0b0,0b0};
char msg[12];
char UID[6];    //fica      armazenado o valor da ID
unsigned int TagType;            //rettorna o tipo de tag utilizada no módulo
char size;
char i;
char txt11=0;
 int li=0;
unsigned short limpa=0;
unsigned short grava=0;
int j,y,z;                      // inicializazção da repetção
porta.ra5=1;               //configura saida ra5 do pic18 como saida altta
trisa.ra5=1;               //configura função saida

          trisc.rc1 = 0; //'configura todos os pinos do tris como saída
          trisc.rc0 = 0;
          portb.rb7= 1;       //tx emulada
          trisb.rb7=0;                    //tx emulada
          trisd.rd5 = 0;
          portd.rd6 =1;
          trisd.rd6 = 0;
          portc.rc0 = 0;





/*******************************************************************************
Abaixo confgure o ttmr0 para 1 segundo
lembrando valor de carga é 449911 em hexa fica c2f7

 *******************************************************************************/
   T0CON=0B10000110;    //ATIVEI     16BYTS          TEMPORIZADOR       BODA DESCIDA        ATIVA PRESCALER       CONFIGURA PRESCALER
   TMR0L=0XF7;
   TMR0H=0XC2;
   INTCON.TMR0IF=0;

// configuração da interrupção
   INTCON.TMR0IE = 0;       // Habilita interrupção do timer0.   1 habilita         0 desabilita
   INTCON.TMR0IF = 0;       // Apaga flag de estouro do timer0, pois é fundamental para a sinalização do estouro.
   T0CON.TMR0ON = 0;        // Liga timer0.                   1 habilita         0 desabilita

   INTCON.GIE = 0;          // Habilita as interrupções  1 habilita         0 desabilita
   INTCON.PEIE = 0;         // Habilita as interrupções dos perifericos.     1 habilita         0 desabilita
   ADCON1 = 0x0f;      // Configura Todos Os pppinos A/D Como I/Ooo         par 84520

         SPI1_Init();   //inicializa comunicação spi  lembrr de setar pinos
         Lcd_Init();    //inicializa lcd nible menos significaivo           lembrar de conferir a ordem de mosi miso
         Lcd_Cmd(_LCD_CLEAR);         //limpa tudo
         Lcd_Cmd(_LCD_CURSOR_OFF);   //desliga cursor
         MFRC522_Init();             //inicializa o modulo RFID


  //apresentação isso ajuda a dar tempo par ao hardware interno finalizar as
  //inicializações
delay_ms(500);
Lcd_Out(2,1,"Jeferson V12.8            ");
Lcd_Out(2,1,"foi  ");
delay_ms(600);
escreve_frase("Testes");
hh=0;  mm=0;   se=0;
 converte (5);

while(1)                      //loop infinito baseado em analisar se exise tag dentro do campo de leittura
{

    if(!limpa)  // essa rotina trabalha a ida e a volta do programa para trabalhar o cod rfid
    {
       Lcd_Cmd(_LCD_CLEAR);
       limpa=~limpa;
    }
    amostra_hora();
    lertag();                 //testa bbotão 3
    lerajuste();                //leia se foi apertado bt de ajuste
    while(k2)
    {
        
        if (limpa)
    {
        Lcd_Cmd(_LCD_CLEAR);
        Lcd_Out(1,1,"Leitura");
        limpa=~limpa;
    }
    lertag();
    
    if(!portb.rb5)
    {
        Lcd_Cmd(_LCD_CLEAR);
        
        for(i=0;i<5;i++)
        {
          val5[i]= 0;
        }
        Lcd_Out(1,1,"Tag Excluida");
        delay_ms(700);
        Lcd_Cmd(_LCD_CLEAR);
        Lcd_Out(1,1,"Leitura         ");
    }
    if( MFRC522_isCard( &TagType ) )      //  tetm tag n
     {
        Lcd_Cmd(_LCD_CLEAR);
        if( MFRC522_ReadCardSerial( &UID ) )                 //Faz a leitura do numero de serie condição verdadeira se tiver algum cartão ali.
        {


          Lcd_Out( 1, 1, "" );           // alterei (1,1) para (1,2)    escrever na 2 linha .  obs.: 2 liha segunda linha ja tem informação não fazer novamente
          for( i=0; i < 5; i++)        //loop enquanto i for menor do que 5 ou seja tem que escrever os 6 caracteres do numero de série em hexadecimal ou seja dois em dois
          {
              ByteToHex( UID[i], msg );  //converte o byte contido em uid em hexadecimal
              Lcd_Out_CP( msg );
              
                   //escreve o hexadecimal 6 vezes

              /*
              ##############aqui eu vou associar o endereço do cartão a variavel validx para ter como validar futuramente.############
              não da para fazer isso dentro de um loop, procure outro lugar.
              */

          }
          
          //a rotna abaixo envia os dados pela serial emulada
             for( i=0; i < 5; i++)
             {
             int estado=0;          //inicializa estado
             estado = UID[i];       //informa a estado um byte do numero de serie
             estado = estado >>4;   //separa apenas a parte mais significaiva
             converte (estado);     //apresenta essa parte
             estado = UID[i];       //pega de novo o conteudo total
             estado = estado & 15;  //separa  aparte menos significativa desse byte
             converte (estado);     //apresenta essa parte
             }
             pula();                  //função de pular linha e retorno de carro

          
          size = MFRC522_SelectTag( &UID );
          for( i=0; i < 5; i++)
          {
               temp[i] = UID[i];
          }
          m1=0; m2=0; m3=0; m4=0; m5=0;
          
          for( i=0; i < 5; i++)
          {
              if(temp[i] == val1[i])
              {
                    m1++;
              }
          }

          for( i=0; i < 5; i++)
          {
              if(temp[i] == val2[i])
              {
                   m2++;
              }
          }




          for( i=0; i < 5; i++)
          {
              if(temp[i] == val3[i])
              {
                    m3++;
              }
          }
          for( i=0; i < 5; i++)
          {
              if(temp[i] == val4[i])
              {
              m4++;
              }
          }

          for( i=0; i < 5; i++)
          {
              if(temp[i] == val5[i])
              {
                 m5++;
              }
          }


          if(m1==5 || m2==5 || m3==5 || m4==5 || m5==5)
          {
                Lcd_Out( 2, 1, "Autorizado" );
                m1=0;
                m2=0;
                m3=0;
                m4=0;
                m5=0;

                /*   rotina para fazer som
                portc.rc0=1;    //buzzer
                delay_ms(110);
                portc.rc0=0;    //buzzer
                delay_ms(110);
                portc.rc0=1;    //buzzer
                delay_ms(110);
                portc.rc0=0;    //buzzer
                */

          }
          else
          {
                m1=0;
                m2=0;
                m3=0;
                m4=0;
                m5=0;
                Lcd_Out( 2, 1, "Nao Autorizado" );
                portc.rc0 = 1;
                delay_ms(1000);
                portc.rc0 = 0;
                
                if(!portb.rb4)
                {
                      Lcd_Cmd(_LCD_CLEAR);
                      Lcd_Out( 1, 1, "Cadastro" );
                      Lcd_Out( 1, 1, "Solte botao" );
                      delay_ms(1000);


                      for( i=0; i < 5; i++)
                      {
                            val5[i] = temp[i];
                      }
                      portc.rc0 = 1;
                      delay_ms(50);
                      portc.rc0 = 0;
                      delay_ms(50);
                      portc.rc0 = 1;
                      delay_ms(50);
                      portc.rc0 = 0;
                      delay_ms(50);
                      portc.rc0 = 1;
                      delay_ms(50);
                      portc.rc0 = 0;
                      Lcd_Out( 1, 1, "concluido  " );
                }
          }
          MFRC522_Halt();    //Estado de hibernação
}
    }
    }
    }
}

//      fim