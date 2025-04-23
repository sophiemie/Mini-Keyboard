#include <BleKeyboard.h>
#include <ezButton.h> 

BleKeyboard bleKeyboard("ESP32_Keyboard", "ESP32", 100);

const int discordButton = 4;  // Knopf für Discord Mute
const int spotifyButton = 16;  // Knopf für Spotify Next
const int spotifyPrevButton = 23;    // Button für vorheriges Lied
const int spotifyPlayPauseButton = 17; // Button für Play/Pause

// NVidia Aufnahmen
const int recordButton = 22;
const int lastMinutesButton = 1;

const int discordLed = 13;
bool discordMuteOn = false;

// Poti:
#define CLK_PIN 27 //25 // ESP32 pin GPIO25 connected to the rotary encoder's CLK pin
#define DT_PIN  14 //26 // ESP32 pin GPIO26 connected to the rotary encoder's DT pin
#define SW_PIN  12 //27 // ESP32 pin GPIO27 connected to the rotary encoder's SW pin

#define DIRECTION_CW  0   // clockwise direction
#define DIRECTION_CCW 1  // counter-clockwise direction

int counter = 0;
int direction = DIRECTION_CW;
int CLK_state;
int prev_CLK_state;
ezButton button(SW_PIN);  // create ezButton object that attach to pin 7;


void setup() {
  pinMode(discordButton, INPUT_PULLUP);
  pinMode(spotifyButton, INPUT_PULLUP);
  pinMode(spotifyPrevButton, INPUT_PULLUP);
  pinMode(spotifyPlayPauseButton, INPUT_PULLUP);
  pinMode(recordButton, INPUT_PULLUP);
  pinMode(lastMinutesButton, INPUT_PULLUP);

  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  prev_CLK_state = digitalRead(CLK_PIN);

  bleKeyboard.begin();
}

void loop() {
  if (bleKeyboard.isConnected()) {
    // 🎤 Discord Mute/Unmute (Strg + Shift + k)
    // Setandardmaessig mit 'm', allerdings ging das nur in der Anwendung selbst
    if (digitalRead(discordButton) == LOW) {
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.press(','); // Vielleicht anderen Buchstaben nehmen, da dies sonst alle Reiter bei Arc schließt
      delay(100);
      bleKeyboard.releaseAll();
      delay(500);
      discordMuteOn = !discordMuteOn;

      if (discordMuteOn) digitalWrite(discordLed, 0);
      else digitalWrite(discordLed, 1);

    }

    // 🎵 Spotify Next Track
    if (digitalRead(spotifyButton) == LOW) {
      bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
      delay(500);
    }

    // ⏪ Spotify Previous Track
    if (digitalRead(spotifyPrevButton) == LOW) {
      bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
      delay(500);
    }


    // ⏯️ Spotify Play/Pause
    if (digitalRead(spotifyPlayPauseButton) == LOW) {
      bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
      delay(500);
    }    
    
    // 🔊 Lautstärke per Poti
    readPoti();

    // Nvidia Shadowplay
    recordMonitor();

  }
}


void recordMonitor() {
  // Spielaufnahme
  if (digitalRead(recordButton) == LOW)
  {
    bleKeyboard.press(KEY_LEFT_ALT);
    bleKeyboard.press(KEY_F9);
    delay(300);
    bleKeyboard.releaseAll();
  }

  // Letzten 5 Minuten
  if (digitalRead(lastMinutesButton) == LOW)
  {
    bleKeyboard.press(KEY_LEFT_ALT);
    bleKeyboard.press(KEY_F10);
    delay(300);
    bleKeyboard.releaseAll();
  }  
}


void readPoti() {
  CLK_state = digitalRead(CLK_PIN);

  if (CLK_state != prev_CLK_state && CLK_state == HIGH) {
    if (digitalRead(DT_PIN) == HIGH) {
      counter--;
      direction = DIRECTION_CCW;
    } else {
      // the encoder is rotating in clockwise direction => increase the counter
      counter++;
      direction = DIRECTION_CW;
    }

    if (direction == DIRECTION_CW) {
      bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
    }
    else
    {
      bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
    }
  }

  // save last CLK state
  prev_CLK_state = CLK_state;

  //if (button.isPressed()) {
    //Serial.println("The button is pressed");
  //} 
}


