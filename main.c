#include <16F877A.h>
#define ADC = 10
#fuses XT,NOWDT,NOPROTECT,NOBROWNOUT,NOLVP,NOPUT,NOWRT,NODEBUG,NOCPD
#use delay(clock=4M)
#use fast_io(A)


#define use_portb_lcd TRUE   // LCD B portuna baðlý
//***DS1302 ÝÇÝN PIN TANIMLAMALARI***//
#define RTC_SCLK PIN_C3    
#define RTC_IO PIN_D6
#define RTC_RST PIN_D4


#include "lcd420.c"
#include "stdlib.h"
#include "dht-11.c" //dht-11 dosyasý tanýtýlýyor.
#include "ds1302.c"  //DS1302 AÝT FONKSÝYONLAR PROGRAMA EKLENÝR





#define input1 PIN_C0
#define input2 PIN_C1
#define input3 PIN_C4
#define input4 PIN_C5
#define enable_a PIN_C6
#define enable_b PIN_C7

#define normal PIN_D0
#define tehlike PIN_D1
#define uyari PIN_D2
#define buzzer PIN_D3
#define gaz_seviyesi PIN_E2


int temperature;
int humidity;
int saniye,dakika,saat,gun,ay,yil,haftanin_gunu;   //DS1302 DEÐÝÞKENLERÝ
int sayici=0,zamanlayici=0;
//unsigned int8 kesme=0;


#int_timer0 
void timer0_kesme() //SAYICI
{
set_timer0(255);
sayici++;
}

#int_timer1
void timer1_kesme() //ZAMANLAYICI
{

set_timer1(63036);

if(sayici==1 && zamanlayici<500)
{

output_high(PIN_E0);
zamanlayici++;

if(zamanlayici==500)
{

output_low(PIN_E0);
zamanlayici=0;
sayici=0;

}
}
}

/*
#int_timer2
void timer2_kesme(){
 set_timer2(5);
 kesme++;
if(input(PIN_E0)){

output_b(kesme);
lcd_init();
printf(lcd_putc,"\f kesme=%u",kesme);
}
}
*/



void main()

{

   lcd_init();
   
   lcd_gotoxy(1,1); 
   lcd_putc("KOU - MIKRO");
   
   lcd_gotoxy(1,2); 
   lcd_putc("   PROJE");
   delay_ms(100);

   lcd_gotoxy(1,1);
   lcd_putc("\fUmutcan GOK");
   
   lcd_gotoxy(1,2); 
   lcd_putc("180223047");
   delay_ms(100);

       kesme:
       setup_psp(PSP_DISABLED);        // PSP birimi devre dýþý
     //setup_timer_1(T1_DISABLED);     // T1 zamanlayýcýsý devre dýþý
       setup_timer_2(T2_DISABLED,0,1); // T2 zamanlayýcýsý devre dýþý
       setup_adc_ports(NO_ANALOGS);    // ANALOG giriþ yok
       setup_adc(ADC_OFF);             // ADC birimi devre dýþý
    
       setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
       set_timer1(63036);
  
       setup_timer_0(RTCC_EXT_H_TO_L | RTCC_DIV_1);
       set_timer0(255);  
       
       enable_interrupts(INT_timer0);
       enable_interrupts(INT_timer1);
       enable_interrupts(GLOBAL);
       while(1)
       break;
       
       output_high(enable_a);
       output_high(enable_b);
  
   set_tris_a(0x18);   // RA0, RA1, RA2, RA3 pinleri giriþ
  
   rtc_init();
  

   

       
      
       
   while(TRUE)
   {   

       rtc_get_time(saat,dakika,saniye);  //DS1302'den saat,dakika,saniye bilgileri alýnýyor
       rtc_get_date(gun,ay,yil,haftanin_gunu);//DS1302'den gün,ay,yýl ve günün isim bilgisi alýnýyor
       
       read_dht();
       temperature=dht_dat[2];
       humidity=dht_dat[0];
 
      switch(haftanin_gunu)   //DS1302'den sayýsal olarak alýnan haftanýn günü bilgisine
     {               //metin olarak karþýlýk atanýyor.
     
     case 1:  lcd_gotoxy(3,2);printf(lcd_putc,"PAZ");break;  // Eðer "1" ise LCD'ye PAZ yaz
     case 2:  lcd_gotoxy(3,2);printf(lcd_putc,"PZT");break;  // Eðer "2" ise LCD'ye PZT yaz
     case 3:  lcd_gotoxy(3,2);printf(lcd_putc,"SAL");break;  // Eðer "3" ise LCD'ye SAL yaz
     case 4:  lcd_gotoxy(3,2);printf(lcd_putc,"CRS");break;  // Eðer "4" ise LCD'ye CRS yaz
     case 5:  lcd_gotoxy(3,2);printf(lcd_putc,"PER");break;  // Eðer "5" ise LCD'ye PER yaz
     case 6:  lcd_gotoxy(3,2);printf(lcd_putc,"CUM");break;  // Eðer "6" ise LCD'ye CUM yaz
     case 7:  lcd_gotoxy(3,2);printf(lcd_putc,"CTS");break;  // Eðer "7" ise LCD'ye CTS yaz
     
     
     }
     
      if(input(PIN_A2))  
      {
         printf(lcd_putc,"\f");
         restart_wdt();
         write_eeprom(0,temperature);  // 0 (0x00) adresine saniye deðerini yaz
         write_eeprom(1,humidity);   // 1 (0x01) adresine milisaniye deðerini yazr
         write_eeprom(2,saniye);
         write_eeprom(3,dakika);
         write_eeprom(4,saat);
         write_eeprom(5,gun);
         write_eeprom(6,ay);
         write_eeprom(7,yil);
         write_eeprom(8,gaz_seviyesi);

         
         while(input(PIN_A2));
         restart_wdt();
         
      }

      if(input(PIN_A3))  
      {   
      
          printf(lcd_putc,"\f");
          restart_wdt();
     
         while(input(PIN_A3))
         {

            lcd_gotoxy(1,1);
            printf(lcd_putc,"\fSicaklik:%dC,Nem:%d",read_eeprom(0),read_eeprom(1));
            lcd_gotoxy(1,2);
            printf(lcd_putc,"Gaz(1/0):%d",read_eeprom(8));
            lcd_gotoxy(1,3);
            printf(lcd_putc," %d:%d:%d",read_eeprom(4),read_eeprom(3), read_eeprom(2));
            lcd_gotoxy(1,4);
            printf(lcd_putc," %d.%d.%d",read_eeprom(5),read_eeprom(6), read_eeprom(7));
            while(input(PIN_A3));
            restart_wdt();
            
         }
      }
      
      if(input(PIN_E1))  
      {
         printf(lcd_putc,"\f");
         restart_wdt();

         while(input(PIN_E1))
         {
            rtc_get_time(saat,dakika,saniye);  //DS1302'den saat,dakika,saniye bilgileri alýnýyor
            rtc_get_date(gun,ay,yil,haftanin_gunu);//DS1302'den gün,ay,yýl ve günün isim bilgisi alýnýyor
          
            read_dht();
            temperature=dht_dat[2];
            humidity=dht_dat[0];
            lcd_gotoxy(1,1);
            printf(lcd_putc,"\fSicaklik:%dC, Nem:%d\n",temperature,humidity);
            printf(lcd_putc,"     %02d/%02d/%02d\n",gun,ay,yil); //LCD'ye sýcaklýk,gün,ay,yýl bilgileri yazdýrýlýyor
            printf(lcd_putc,"     %02d:%02d:%02d",saat,dakika,saniye);   //LCD'ye saat,dakika ve saniye deðerlerini yaz
            delay_ms(250);
         }
         restart_wdt();
         
      }
      
            if(input(PIN_E2))
      {
          printf(lcd_putc,"\f");
          restart_wdt();
          
         while(input(PIN_E2)){
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,1);
         printf(lcd_putc,"\fYuksek Gaz Seviyesi\n");
         printf(lcd_putc,"!  !  !  !  !  !  !");
         output_high(PIN_D7);
         delay_ms(100);
         output_low(PIN_D7);
         delay_ms(100);
         write_eeprom(0,temperature);  // 0 (0x00) adresine saniye deðerini yaz
         write_eeprom(1,humidity);   // 1 (0x01) adresine milisaniye deðerini yazr
         write_eeprom(2,saniye);
         write_eeprom(3,dakika);
         write_eeprom(4,saat);
         write_eeprom(5,gun);
         write_eeprom(6,ay);
         write_eeprom(7,yil);         
         write_eeprom(8,gaz_seviyesi);
              }
            while(input(PIN_E2));
            restart_wdt();         
      }
  
 setup_WDT(WDT_2304MS); 
 normal_isi:
      while(temperature<30)
      {

       
         lcd_gotoxy(7,1);
         printf(lcd_putc,"\fSicaklik(%d C)",temperature);
         lcd_gotoxy(5,2);
         printf(lcd_putc,"Normal");
         delay_ms(250);
      
      output_high(normal);
      output_low(input1);
      output_low(input2);
      output_low(buzzer);
      output_low(tehlike);
      output_low(uyari);
      
      
      if(30<=temperature && temperature<=35)
      {
      goto tehlikeli_isi;
      }
            if(input(PIN_A4) && sayici==0)
            {
             goto kesme;
             break;
            }       
            
      restart_wdt();
      break;
      }
      
      tehlikeli_isi:
      while(30<=temperature && temperature<=35)
            {

       
                lcd_gotoxy(8,1);
                printf(lcd_putc,"\fSicaklik(%d C)",temperature);
                lcd_gotoxy(4,2);
                printf(lcd_putc,"Yukseliyor");
                delay_ms(250);

                
                output_high(tehlike);
                output_high(buzzer);
                delay_ms(250);
                output_low(tehlike);
                output_low(buzzer);

                
                if(temperature>35)
                {
                    goto asiri_isi;
                }
               else if(temperature<30)
               {
                    goto normal_isi;
               }
            if(input(PIN_A4) && sayici==0)
            {
             goto kesme;
             break;
            } 
               restart_wdt();
               
               break;
            }
            
        asiri_isi: 
        while(temperature>35 && temperature<=40)
   {
   
         output_low(normal); 
         output_high(input1);
         output_low(input2);
         

         lcd_gotoxy(4,1);
         printf(lcd_putc,"\fSicaklikYukseldi %dC",temperature);
         lcd_gotoxy(5,2);
         printf(lcd_putc,"Fan Aktif");
         output_high(buzzer);
         output_high(uyari);
         delay_ms(100);
         output_low(uyari);
         output_low(buzzer);
         
           if(30<=temperature && temperature<=35)
            {
             goto tehlikeli_isi;
            }
            if(input(PIN_A4) && sayici==0)
            {
             goto kesme;
             break;
            } 
            restart_wdt();
            break;
  }
      
      while(temperature>40)
            {
          
            output_high(buzzer);
            output_high(uyari);
            output_high(input1);
            output_low(input2);
            output_low(normal);
            output_low(tehlike);
            

            lcd_gotoxy(7,1);
            printf(lcd_putc,"\fAsiri   (%d C)",temperature);
            lcd_gotoxy(8,2);
            printf(lcd_putc,"Sicaklik");         
            delay_ms(250);
            
            if (input(PIN_A0)) // Eðer RA0 giriþindeki butona basýlýrsa
      {
            delay_ms(20);  // Buton arkýný önlemek için
            printf(lcd_putc,"\f");    
            output_high(enable_b);
            output_high(input3);
            output_low(input4);
            restart_wdt();         
      }  
      
      else
      {
      output_low(enable_b);
      }
      
               if(35<temperature && temperature<=40)
            {
             goto asiri_isi;
             break;
            }                  
            
            if(input(PIN_A4) && sayici==0)
            {
             goto kesme;
             break;
            } 
            restart_wdt();         
            break;
            }

  }

}
