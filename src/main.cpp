#include <Arduino.h>
#include "state_machine.h"
#include "keypad.h"
#include "rotary_encoder.h"
#include "pin_defs.h"
#include "lcd.h"

void led_setup();
void button_setup();
void check_buttons();
void check_states();

void check_pin();
void printEnteredPin();


void setup()
{
  led_setup();
  button_setup();
  //setup_encoder(RE_PIN_A, RE_PIN_B, RE_SW); // Wegen Pinmangel auskommentiert
  keypad_setup_pins();
  Serial.begin(115200); //115200
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Servus Kollege");
  delay(2000);
  lcd.clear();
}

int32_t last_encoder_state = 0;

// Pin für das Keypad
const char correct_pin[] = "2580";
char entered_pin[5];
int pin_index = 0;

void loop()
{
  check_buttons();
  check_states();
  /*int32_t encoder_state = get_encoder_state();
  if (encoder_state != last_encoder_state) {
    Serial.printf("Encoder state: %d \n", encoder_state);
    last_encoder_state = encoder_state;
  }*/
}

void led_setup()
{
  // Define all leds as output
  pinMode(LED_RED_LOCKED_PIN, OUTPUT);
  pinMode(LED_GREEN_LOCK_1_PIN, OUTPUT);
  pinMode(LED_GREEN_LOCK_2_PIN, OUTPUT);
  pinMode(LED_GREEN_LOCK_3_PIN, OUTPUT);
}

void button_setup()
{
  // Define all buttons as input. PULLUPs are used external
  pinMode(BTN_1_PIN, INPUT);
  pinMode(BTN_2_PIN, INPUT);
  pinMode(BTN_3_PIN, INPUT);
  pinMode(BTN_4_PIN, INPUT);
}


void check_buttons()
{
  if ('A' == get_key())
  {
    state_machine(INPUT_1_ACCEPTED);
    Serial.println("Bitte PIN eingeben: ");
    lcd.clear();
    lcd.print("Bitte PIN eingeben: ");
  }
  /*if (LOW == digitalRead(BTN_2_PIN))      // Wird durch PIN-Eingabe geregelt
  {
    state_machine(INPUT_2_ACCEPTED);
  }*/
  if ('C' == get_key())
  {
    state_machine(INPUT_3_ACCEPTED);
    Serial.println("Tresor offen");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tresor offen");
  }
  if ('D' == get_key())
  {
    state_machine(INPUT_REFUSED);
    state_machine(OPEN_DOOR);
    state_machine(CLOSE_DOOR);
    Serial.println("Tresor gesperrt!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tresor gesperrt!");
    delay(3000);
    lcd.clear();
  }
}

void check_states()
{
  switch (state)
  {
  case SAFE_LOCKED:
    digitalWrite(LED_RED_LOCKED_PIN, HIGH);
    digitalWrite(LED_GREEN_LOCK_1_PIN, LOW);
    digitalWrite(LED_GREEN_LOCK_2_PIN, LOW);
    digitalWrite(LED_GREEN_LOCK_3_PIN, LOW);
    break;
  case LEVEL_1_UNLOCKED:
    digitalWrite(LED_RED_LOCKED_PIN, LOW);
    digitalWrite(LED_GREEN_LOCK_1_PIN, HIGH);
    digitalWrite(LED_GREEN_LOCK_2_PIN, LOW);
    digitalWrite(LED_GREEN_LOCK_3_PIN, LOW);
    check_pin();
    /*if (char key = get_key())
      {
        Serial.printf("Keypad Key: %c \n", key);
      };*/
    break;
  case LEVEL_2_UNLOCKED:
    digitalWrite(LED_RED_LOCKED_PIN, LOW);
    digitalWrite(LED_GREEN_LOCK_1_PIN, HIGH);
    digitalWrite(LED_GREEN_LOCK_2_PIN, HIGH);
    digitalWrite(LED_GREEN_LOCK_3_PIN, LOW);
    break;
  case LEVEL_3_UNLOCKED:
    digitalWrite(LED_RED_LOCKED_PIN, LOW);
    digitalWrite(LED_GREEN_LOCK_1_PIN, HIGH);
    digitalWrite(LED_GREEN_LOCK_2_PIN, HIGH);
    digitalWrite(LED_GREEN_LOCK_3_PIN, HIGH);
    break;
  default:
    break;
  }
}

void check_pin()
{

  char key = get_key();

  if (key == '#'){      // Bestätigung des eingegebenen PIN
    entered_pin[pin_index] = '\0';
    if (pin_index < sizeof(entered_pin)){
      entered_pin[pin_index] = '\0';
    }
    else{
      entered_pin[sizeof(entered_pin) - 1] = '\0';
    }
    if (strcmp(entered_pin, correct_pin) == 0){     // Wenn PIN korrekt eingegeben wurde...
      Serial.print("PIN korrekt!");
      Serial.print(" Zugang gewaehrt. \n");
      lcd.clear();
      lcd.print("PIN Korrekt");
      lcd.setCursor(0, 1);
      lcd.print("Zugang gewaehrt.");
      delay(3000);
      lcd.clear();
      state_machine(INPUT_2_ACCEPTED);
    }
    else{ 	    // Wenn der PIN nicht korrektt eingegeben wurde...
      Serial.print("Falscher PIN!");
      Serial.print(" Zugang verweigert. \n");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Falscher PIN!");
      lcd.setCursor(0, 1);
      lcd.print("Zugang verweigert.");
      delay(3000);
      lcd.clear();
      state_machine(INPUT_REFUSED);
    }
    pin_index = 0;
    }
  else if (pin_index < 4 && key >= '0' && key <= '9'){      // PIN Eingabe und Ausgabe des aktuell eingegebenen PIN
    entered_pin[pin_index] = key;
    pin_index++;
    printEnteredPin();

    /*Serial.print("Eingegebener PIN: ");
    for (int i = 0; i < pin_index; i++){
      Serial.print(entered_pin[i]);
    }
    Serial.println();*/
  }
  else if (key == '*'){     // Löschen der aktuellen Eingabe
    pin_index = 0;
    Serial.println("Eingabe geloescht");
    Serial.print(" Bitte PIN eingeben: ");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Eingabe geloescht");
    lcd.setCursor(0, 1);
    lcd.print("Bitte PIN eingeben: ");
  }
}

void printEnteredPin(){
  // Serielle Ausgabe des PIN
  Serial.print("Eingegebener PIN: ");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Eingeg. PIN:");
  lcd.setCursor(0, 1);
    for (int i = 0; i < pin_index; i++){
      Serial.print(entered_pin[i]);
      lcd.print(entered_pin[i]);
    }
    Serial.println();
}
