
int melodia[7];
int poprzednia = -1;

const int speakerPin = 8;

int przerwa = 100;

void setup() 
{
  Serial.begin(9600); 
  pinMode(speakerPin, OUTPUT);  
  Serial.println("Odbieranie melodii...");
}

void loop() 
{
  if (Serial.available() > 0) 
  {
   
    for (int i = 0; i < 7; i++) 
    {
      melodia[i] = Serial.parseInt();
      Serial.print(melodia[i]); 
      if (i < 6) 
      {
        Serial.print(", ");
      } else 
      {
        Serial.println();
      }
    }

    granie(melodia);
  }

  delay(50);  
}

void granie(int melodia[]) 
{
  for (int i = 0; i < 7; i++) 
  {
    int note = melodia[i];

    if (i != 0) 
    {
      poprzednia = melodia[i - 1];
    }

    if (poprzednia == 0 && note == 0) 
    {
      break; 
    }

    if (note == 0) 
    {
    
      delay(przerwa); 
    } 
    else 
    {
      int frequency = nuty(note);
      
      if (frequency > 0) 
      {
        tone(speakerPin, frequency, przerwa);
        delay(przerwa);
      }
    }
  }
}


int nuty(int note) {
  switch(note) {
    case 1: return 262;
    case 2: return 294;
    case 3: return 330;
    case 4: return 349;
    case 5: return 392;
    case 6: return 440;
    case 7: return 494;
    case 8: return 523;
    default: return 0; 
  }
}
