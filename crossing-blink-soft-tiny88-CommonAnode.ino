// Heavily modified from Capreol Museum Rail Road Crossing Controller
// by Kevin McArthur as posted at 
// https://create.arduino.cc/projecthub/KevinMcArthur/model-railroad-grade-crossing-controller-c2c85e
//

// set pin numbers
const int SensorA = A1;         // Pin for Sensor A connection
const int SensorB = A2;         // Pin for Sensor B connection
 
const int ledPin = LED_BUILTIN;           // Pin for LED to indicate OCCUPIED - 13 on most Arduino, 0 on ATTiny
const int RelayC = 14;          // Pin for relay output when crossing gate is running
const int ledL = 9;             // Pin for one of the flasher LEDs - PWM
const int ledR = 10;            // Pin for one of the flasher LEDs - PWM

const long interval = 900;     // On timer for the LED while flashing 
int fadeValue = 0;
int fadeValue2 = 0;
 
                                // Define Variables
int SensorAState = HIGH;
int lastSensorAState = HIGH;

int RUNNING  = LOW;
int OCCUPIED = LOW;

int SensorBState=HIGH;
int lastSensorBState = HIGH;

int ledState = LOW;
int ledState2 = LOW;


// Most of the following are used with millis() timers and need to be long

unsigned long lastDebounceTimeA = 0;
unsigned long lastDebounceTimeB = 0;

unsigned long debounceDelay = 50;   // ms debounce delay
unsigned long looptriggerA;
unsigned long looptriggerB;
unsigned long RUNNINGtime = 3000;   // ms of running time AFTER niether sensor is active
unsigned long RUNNINGtrigger;
unsigned long previousMillis = 0;


void setup() {  

  pinMode(ledPin, OUTPUT);
  pinMode(SensorA, INPUT);
  pinMode(SensorB, INPUT);
  pinMode(RelayC, OUTPUT);
  pinMode(ledL, OUTPUT);
  pinMode(ledR, OUTPUT);

  digitalWrite(ledL, HIGH);       // Make sure crossing LEDs are off
  digitalWrite(ledR, HIGH);  
 
}

void debounceA()                        // Debounce of sensor A 
{
  int readingA = digitalRead(SensorA);  

  if (readingA != lastSensorAState)
  {
    lastDebounceTimeA = millis();
    looptriggerA = millis();
  }
  if ((millis() - lastDebounceTimeA) > debounceDelay)
  {
    if (readingA != SensorAState)
    {
      SensorAState = readingA;
    }
  }
  lastSensorAState = readingA;
  lastDebounceTimeA = 0;
  return;
}

void debounceB()                        // Debounce of sensor B
{
  int readingB = digitalRead(SensorB);

  if (readingB != lastSensorBState)
  {
    lastDebounceTimeB = millis();
    looptriggerB = millis();
  }
  if ((millis() - lastDebounceTimeB) > debounceDelay)
  {
    if (readingB != SensorBState)
    {
      SensorBState = readingB;
    }
  }
  lastSensorBState = readingB;
  lastDebounceTimeB = 0;
  return;
}

void START()                      // One or both sensors have detected
{                                 // Turn on RelayC
  RUNNINGtrigger = millis();
  digitalWrite(RelayC, HIGH);
  RUNNING = HIGH;
}


void STOP()                       // Both sensors are NOT detecting
{                                 // Turn off RelayC
  RUNNINGtrigger = millis();
  digitalWrite(RelayC, LOW);
  RUNNING = LOW;
  digitalWrite(ledL, HIGH);       // Make sure crossing LEDs are off
  digitalWrite(ledR, HIGH);
}


void flasher()                    // Alternate flashing of ledL and ledR
                                  // with fade-in and fade-out effect
{                                 // All non-blocking except for a few milliseconds
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa
    if (ledState == LOW) {
      // fade in from min to max in increments
      for (fadeValue = 0 ; fadeValue <= 255; fadeValue += 10) {
        if (fadeValue >= 244){ fadeValue = 255; }     // Clean up to make sure we reach 0 and 255
        fadeValue2 = 255 - fadeValue;
        analogWrite(ledL, fadeValue);                 // Alternate ledL and ledR
        analogWrite(ledR, fadeValue2);
        
        delay(5);                                     // wait for milliseconds to see the dimming effect
      }  
      ledState = HIGH;
      
    } else {
      // fade out from max to min in increments
      for (fadeValue = 255 ; fadeValue >= 0; fadeValue -= 10) {
        if (fadeValue <= 11){ fadeValue = 0; }        // Clean up to make sure we reach 0 and 255
        fadeValue2 = 255 - fadeValue;
        analogWrite(ledL, fadeValue);                 // Alternate ledL and ledR
        analogWrite(ledR, fadeValue2);
        
        delay(5);                                     // wait for milliseconds to see the dimming effect
      }  //end for
      ledState = LOW;
    }    // end else
  }      // end if (time to flash)
}        // end flasher


void loop()
{
                                                    // Other than a few milliseconds of fade, all runs as non-blocking
  debounceA();                                      // Debounce the sensor inputs
  debounceB();                
  digitalWrite(ledPin, OCCUPIED);                   // Use LED to indicate OCCUPIED (may or may not be RUNNING)
  
  if (SensorAState == HIGH || SensorBState == HIGH) // One or both sensors have detected (set to LOW if your sensors go low on detect)
  {
    OCCUPIED = HIGH;
    RUNNINGtrigger = millis();                      // Mark the start time in case we need to stop RUNNINGtime ms later

  }
  else
  {
    OCCUPIED = LOW;
      
  }

  if (OCCUPIED == HIGH && RUNNING == LOW)           // We are OCCUPIED but have not started flashing
  {
    START();

  }

  if (RUNNING == HIGH)
    flasher();                                      // Start flashing
    if ((millis() - RUNNINGtrigger) > RUNNINGtime)  // If we have reached the end of our RUNNINGtime then stop everything
    {
      STOP();


    }

}
