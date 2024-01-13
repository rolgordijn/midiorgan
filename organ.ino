#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <frequencyToNote.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>


#pragma GCC optimize ("Ofast")


#define OCTAVE_SELECT_0 5
#define OCTAVE_SELECT_1 4
#define OCTAVE_SELECT_2 3


#define INHIBIT 7
#define SHIFT_nLOAD 8


#define DATA_IN 12
#define CLOCK 13

#define LED 10


int dataInput[10][10];

uint8_t shiftIn2(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {

  uint8_t value = 0;

  uint8_t i;

  for (i = 0; i < 8; ++i) {
    value |= digitalRead(dataPin) << (7 - i);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }


  return value;
}






void setup() {

  pinMode(OCTAVE_SELECT_0, OUTPUT);
  pinMode(OCTAVE_SELECT_1, OUTPUT);
  pinMode(OCTAVE_SELECT_2, OUTPUT);

  digitalWrite(OCTAVE_SELECT_0, LOW);
  digitalWrite(OCTAVE_SELECT_1, LOW);
  digitalWrite(OCTAVE_SELECT_2, LOW);



  pinMode(INHIBIT, OUTPUT);

  digitalWrite(INHIBIT, LOW);

  pinMode(SHIFT_nLOAD, OUTPUT);

  pinMode(CLOCK, OUTPUT);

  pinMode(LED, OUTPUT);


  pinMode(DATA_IN, INPUT);


  //Serial.begin(115200);
  //delay(1000);
  //Serial.println("Midi organ decoder In debug mode");


  // put your setup code here, to run once:
}



 inline void noteOn(byte channel, byte pitch, byte velocity) {

  midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };

  MidiUSB.sendMIDI(noteOn);
}

 inline void noteOff(byte channel, byte pitch, byte velocity) {

  midiEventPacket_t noteOff = { 0x08, 0x80 | channel, pitch, velocity };

  MidiUSB.sendMIDI(noteOff);
}


void loop() {

  for (int octave = 0; octave < 8; octave++) {
    digitalWrite(LED, HIGH);
    digitalWrite(LED, LOW);
    digitalWrite(SHIFT_nLOAD, LOW);
    digitalWrite(OCTAVE_SELECT_0,7-octave & 0b001);
    digitalWrite(OCTAVE_SELECT_1, 7-octave & 0b010);
    digitalWrite(OCTAVE_SELECT_2, 7-octave & 0b100);
    digitalWrite(SHIFT_nLOAD, HIGH);

    for (int keyboard = 0; keyboard < 6; keyboard++) {


      int tempDataIn = shiftIn2(DATA_IN, CLOCK, MSBFIRST);
      if (tempDataIn != dataInput[keyboard][octave]) {
        dataInput[keyboard][octave] = tempDataIn;
        for (int j = 0; j < 8; j++) {
          if (0 == bitRead(tempDataIn, j)) {
            noteOn(keyboard, 8 * octave + j + 33, 64);
          } else {
            noteOff(keyboard, 8 * octave + j + 33, 64);
          }
          MidiUSB.flush();
        }
      }
    }
  }
}
