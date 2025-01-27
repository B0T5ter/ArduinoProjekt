//Biblioteki
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <Stepper.h>

//Adresowanie i inizjowanie wyśweitlacza
#define LCD_I2C_ADDRESS 0x3F
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, 16, 2);

//Piny guzikow
const int buttonDol = 5;
const int buttonGora = 4;
const int buttonTak = 3;
const int buttonNie = 2;

//Zmienne do wyświetlacza
String stan = "menu";
int kursor = 0;
int menuPoziom = 0;
String melodia = "";

//Melodie
int meloStart[] = {1, 2, 3, 4, 5, 6, 7};
int meloSwiatlo[] = {2, 3, 4, 5, 6, 7, 8};
int meloBrakSwiatla[] = {6, 5, 4, 3, 2, 1, 0};
int meloNaladowanie[] = {2, 3, 4, 4, 5, 6, 7};

int meloStartCopy[] = {1, 2, 3, 4, 5, 6, 7};
int meloSwiatloCopy[] = {2, 3, 4, 5, 6, 7, 8};
int meloBrakSwiatlaCopy[] = {6, 5, 4, 3, 2, 1, 0};
int meloNaladowanieCopy[] = {2, 3, 4, 4, 5, 6, 7};
int aktualnaMelodia[7];

int beep[] = {5, 0, 0, 0, 0, 0, 0, 0};
//Zmienne do silniczków
#define STEPS_PER_REV 2048
const int delta = 50;
int pion = 0;
const int maxGora = 400;
const int maxDol = 0;
bool blokadaRuchu = true;

// Tworzenie obiektów dla dwóch silników krokowych
Stepper motorPion(STEPS_PER_REV, 10, 12, 11, 13);
Stepper motorPoziom(STEPS_PER_REV, 6, 8, 7, 9);  

//Zmiennie fotorezystorów
const int photoresistor0 = A0;
const int photoresistor1 = A1;
const int photoresistor2 = A2;
const int photoresistor3 = A3;
int photogora, photodol, photoprawo, photolewo;
bool czyJestSwiatlo;
bool czyUspienie;
int timerUspienia = 0;
int glownyDelay = 50;
int czasDoUspienia = 0;
int czasyUspienia[] = {5,10,15,30,45,60};
void setup() 
{
  Serial.begin(9600);
  wyslanieMelodi(meloStart);
  

  motorPion.setSpeed(10); 
  motorPoziom.setSpeed(10);

  pinMode(buttonDol, INPUT_PULLUP);
  pinMode(buttonGora, INPUT_PULLUP);
  pinMode(buttonTak, INPUT_PULLUP);
  pinMode(buttonNie, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  
  if (photogora + photodol + photoprawo + photolewo < 500)
  {
    czyJestSwiatlo = false;
  }
  else
  {
    czyJestSwiatlo = true;
  }
}

void loop() 
{
  
  photogora = analogRead(photoresistor0);
  photodol = analogRead(photoresistor1);
  photoprawo = analogRead(photoresistor2);
  photolewo = analogRead(photoresistor3);
  //Serial.println("photogora: " + String(photogora) + ", photodol: " + String(photodol) + ", photoprawo: " + String(photoprawo) + ", photolewo: " + String(photolewo));
  if (photogora + photodol + photoprawo + photolewo < 1000 && czyJestSwiatlo == true)
  {
    wyslanieMelodi(meloBrakSwiatla);
    czyJestSwiatlo = false;
  }
  if (photogora + photodol + photoprawo + photolewo >= 1000 && czyJestSwiatlo == false)
  {
    wyslanieMelodi(meloSwiatlo);
    czyJestSwiatlo = true;
  }

  if (czyJestSwiatlo == true && blokadaRuchu == false)
  {
    ruch();
  }
  
  wyswietlacz();
  delay(glownyDelay);
}
void ruch()
{
  if (abs(photogora - photodol) > delta)
  {
    if (photogora < photodol  && pion < maxGora)
    {
      motorPion.step(10);
      pion = pion + 11;
    }
    if (photodol < photogora  && pion > maxDol)
    {
      motorPion.step(-10);
      pion = pion - 10;
    }
    
  }

  if (abs(photoprawo - photolewo) > delta)
  {
    if (photoprawo < photolewo)
    {
      motorPoziom.step(10);
      
    }
    if (photoprawo > photolewo)
    {
      motorPoziom.step(-10);
      
    }
    
  }
}
void wyswietlacz()
{
  if (stan == "menu")
  {
      lcd.setCursor(0, kursor);
      lcd.print(">");
      lcd.setCursor(0, 1 - kursor);
      lcd.print(" ");
      lcd.setCursor(1, 0);
      lcd.print("Stan");
      lcd.setCursor(1, 1);
      lcd.print("Muzyka");
      
      if(menuPoziom == 0)
    {
      lcd.setCursor(1, 0);
      lcd.print("Stan");
      lcd.setCursor(1, 1);
      lcd.print("Fotorezystory");
    }
    if(menuPoziom == 1)
    {
      lcd.setCursor(1, 0);
      lcd.print("Muzyka");
      lcd.setCursor(1, 1);
      lcd.print("Ustawienia");
    }
    zmianaKursoraMenu();
    if (kursor == 0 && menuPoziom == 1 && digitalRead(buttonTak) == LOW)
    {
      delay(30);
      stan = "melodie";
      menuPoziom = 0;
      kursor = 0;
    }
    if (kursor == 1 && menuPoziom == 0 && digitalRead(buttonTak) == LOW)
    {
      delay(30);
      stan = "foto";
    }
    if (kursor == 1 && menuPoziom == 1 && digitalRead(buttonTak) == LOW)
    {
      delay(30);
      stan = "ustawienia";
      menuPoziom = 0;
      kursor = 0;
    }
  }
  if (stan == "melodie")
  {
    lcd.setCursor(0, kursor);
    lcd.print(">");
    lcd.setCursor(0, 1 - kursor);
    lcd.print(" ");
    if(menuPoziom == 0)
    {
      lcd.setCursor(1, 0);
      lcd.print("meloStart");
      lcd.setCursor(1, 1);
      lcd.print("meloSwiat");
    }
    if(menuPoziom == 1)
    {
      lcd.setCursor(1, 0);
      lcd.print("meloBrakSwia");
      lcd.setCursor(1, 1);
      lcd.print("meloNala");
    }
    delay(40);
    if (menuPoziom == 0 and kursor == 0 && digitalRead(buttonTak) == LOW)
    {
      lcd.clear();
      delay(30);
      stan = "melodia";
      melodia = "meloStart";
      kursor = 0;
      memcpy(aktualnaMelodia, meloStart, sizeof(meloStart));
    }
    if (menuPoziom == 0 and kursor == 1 && digitalRead(buttonTak) == LOW)
    {
      delay(30);
      lcd.clear();
      stan = "melodia";
      melodia = "meloSwiat";
      kursor = 0;
      memcpy(aktualnaMelodia, meloSwiatlo, sizeof(meloSwiatlo));
    }
    if (menuPoziom == 1 and kursor == 0 && digitalRead(buttonTak) == LOW)
    {
      delay(30);
      lcd.clear();
      stan = "melodia";
      melodia = "meloBrakSwia";
      kursor = 0;
      memcpy(aktualnaMelodia, meloBrakSwiatla, sizeof(meloBrakSwiatla));
    }
    if (menuPoziom == 1 and kursor == 1 && digitalRead(buttonTak) == LOW)
    {
      delay(30);
      lcd.clear();
      stan = "melodia";
      melodia = "meloNala";
      kursor = 0;
      memcpy(aktualnaMelodia, meloNaladowanie, sizeof(meloNaladowanie));
    }
    if (digitalRead(buttonNie) == LOW) 
    {
      delay(30);
      stan = "menu";
      kursor = 0;
      menuPoziom = 1;
    }
    zmianaKursoraMenu();
  }
  if (stan == "foto")
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Gor:");
    lcd.setCursor(4, 0);
    lcd.print(photogora);

    lcd.setCursor(8, 0);
    lcd.print("Pra:");
    lcd.setCursor(12, 0);
    lcd.print(photoprawo); 
    lcd.setCursor(0, 1);
    lcd.print("Dol:");
    lcd.setCursor(4, 1);
    lcd.print(photodol);

    lcd.setCursor(8, 1);
    lcd.print("Lew:");
    lcd.setCursor(12, 1);
    lcd.print(photolewo);

    if (digitalRead(buttonNie) == LOW) 
    {
      stan = "menu";
      kursor = 1;
      menuPoziom = 0;
    }
  }
  if (stan == "melodia")
  {
    delay(30);
    lcd.setCursor(1, 0);
    lcd.print(melodia);
    for (int i = 0; i < sizeof(aktualnaMelodia) / sizeof(aktualnaMelodia[0]); i++) 
    {
    lcd.setCursor(1 + i * 2, 1);
    lcd.print(aktualnaMelodia[i]);
    }
    lcd.setCursor(kursor * 2, 1);
    lcd.print(">");
    lcd.setCursor((kursor * 2) + 2, 1);
    lcd.print('<');

    zmianaKursoraMelodie();

    if (digitalRead(buttonTak) == LOW) 
    {
      delay(30);
      aktualnaMelodia[kursor] = aktualnaMelodia[kursor] + 1;
      if (aktualnaMelodia[kursor] >= 9)
      {
        aktualnaMelodia[kursor] = 0;
      }

    }
    
    
    if (digitalRead(buttonNie) == LOW) 
    {
      delay(30);
      wyslanieMelodi(aktualnaMelodia);
      stan = "melodie";
      if (melodia == "meloStart")
      {
        kursor = 0;
        menuPoziom = 0;
        
        memcpy(meloStart, aktualnaMelodia, sizeof(aktualnaMelodia));
      }
      if (melodia == "meloSwiat")
      {
        kursor = 1;
        menuPoziom = 0;
        memcpy(meloSwiatlo, aktualnaMelodia, sizeof(aktualnaMelodia));
      }
      if (melodia == "meloBrakSwia")
      {
        kursor = 0;
        menuPoziom = 1;
        memcpy(meloBrakSwiatla, aktualnaMelodia, sizeof(aktualnaMelodia));
      }
      if (melodia == "meloNala")
      {
        kursor = 1;
        menuPoziom = 1;
        memcpy(meloNaladowanie, aktualnaMelodia, sizeof(aktualnaMelodia));
      }
    }
  }
  if (stan == "ustawienia")
  {
    lcd.setCursor(0, kursor);
    lcd.print(">");
    lcd.setCursor(0, 1 - kursor);
    lcd.print(" ");
    zmianaKursoraMenu();
    if (menuPoziom == 0)
    {
      lcd.setCursor(1, 0);
      lcd.print("Blokada:");
      lcd.setCursor(10, 0);
      if(blokadaRuchu == true)
      {
        lcd.print("True");
      }
      else
      {
        lcd.print("False");
      }
      
      lcd.setCursor(1, 1);
      lcd.print("Domy. muzyka");
      
      
      if (digitalRead(buttonTak) == LOW && kursor == 0)
      {
        blokadaRuchu = !blokadaRuchu;
      } 
      if (digitalRead(buttonTak) == LOW && kursor == 1)
      {
        memcpy(meloStart, meloStartCopy, sizeof(meloStartCopy));
        memcpy(meloSwiatlo, meloSwiatlo, sizeof(meloSwiatlo));
        memcpy(meloBrakSwiatla, meloBrakSwiatlaCopy, sizeof(meloBrakSwiatlaCopy));
        memcpy(meloNaladowanie, meloNaladowanieCopy, sizeof(meloNaladowanieCopy));
        
      } 
    }
    if (menuPoziom == 1)
    {
      lcd.setCursor(1, 0);
      lcd.print("Uspien:");
      lcd.setCursor(10, 0);
      if(czyUspienie == true)
      {
        lcd.print("True");
        lcd.setCursor(1, 1);
        lcd.print(czasyUspienia[czasDoUspienia]);
        lcd.setCursor(5, 1);
        lcd.print("Sekundy");
        if (kursor == 1 && czasDoUspienia == 0)
        {
          lcd.setCursor(2, 1);
          lcd.print("<");
        }
        if (kursor == 1 && czasDoUspienia != 0)
        {
          lcd.setCursor(3, 1);
          lcd.print("<");
        }
      }
      else
      {
        lcd.setCursor(1, 1);
        lcd.print("---");
        lcd.setCursor(10, 0);
        lcd.print("False");
      }
      if (digitalRead(buttonTak) == LOW && kursor == 0)
      {
       czyUspienie = !czyUspienie;
      }
      if (digitalRead(buttonTak) == LOW && kursor == 1)
      {
       czasDoUspienia = czasDoUspienia + 1;
       if(czasDoUspienia >= 5)
       {
        czasDoUspienia = 0;
       }
      }  
    }
    if (digitalRead(buttonNie) == LOW) 
    {
      stan = "menu";
      kursor = 1;
      menuPoziom = 1;
    }
  }
  czyszczenie();
  uspienie();
  
}
void uspienie()
{
  Serial.print(timerUspienia);
  Serial.println(czasyUspienia[czasDoUspienia] * 1000);
  if (czyUspienie == true)
  {
    if (czasyUspienia[czasDoUspienia] * 1000 >= timerUspienia)
    {
      timerUspienia = timerUspienia + glownyDelay * 2;
    }
    if (czasyUspienia[czasDoUspienia] * 1000 < timerUspienia)
    {
      lcd.noBacklight();
    }
    else
    {
      lcd.backlight();
    }
  }
  else
  {
    lcd.backlight();
  }
}
void zmianaKursoraMenu()
{
  if (digitalRead(buttonDol) == LOW) 
  {
    kursor = kursor - 1;
  }
  if (digitalRead(buttonGora) == LOW) 
  {
    kursor = kursor + 1;
  }
  if (kursor >= 2)
  {
    kursor = 0;
    menuPoziom = menuPoziom + 1;
  }
  if (kursor <= -1)
  {
    kursor = 1;
    menuPoziom = menuPoziom - 1;
  }
  if(menuPoziom <= -1)
  {
    menuPoziom = 1;
    kursor = 1;
  }
   if(menuPoziom >= 2)
  {
    menuPoziom = 0;
    kursor = 0;
  }
}
void zmianaKursoraMelodie()
{
  if (digitalRead(buttonDol) == LOW) 
  {
    kursor = kursor - 1;
  }
  if (digitalRead(buttonGora) == LOW) 
  {
    kursor = kursor + 1;
  }
  if (kursor >= 7)
  {
    kursor = 0;
  }
  if (kursor <= -1)
  {
    kursor = 7;
  }
}
void czyszczenie()
{
  if (digitalRead(buttonDol) == LOW || digitalRead(buttonGora) == LOW || digitalRead(buttonNie) == LOW || digitalRead(buttonTak) == LOW) 
  {
    wyslanieMelodi(beep);
    delay(100);
    lcd.clear();
    timerUspienia = 0;
  }
}
void wyslanieMelodi(int* array) 
{
  for (int i = 0; i < 7; i++) 
  {
    Serial.print(array[i]);
    if (i < 6) 
    {
      Serial.print(",");
    }
  }
  Serial.println();
}

