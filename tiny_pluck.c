/* Author: Peter Sovietov */

#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define SR 22050.
#define DELAY_SIZE 256

#define PI 3.141592653589793

uint8_t Delay_buffer[DELAY_SIZE];
uint8_t Delay_index;
uint16_t Delay_phase;

uint16_t Attack;

uint8_t Noise_buffer[DELAY_SIZE];
uint8_t Sine_buffer[DELAY_SIZE];
uint8_t Saw_buffer[DELAY_SIZE];

uint8_t* Current_buffer = Noise_buffer;

uint8_t process(uint16_t period) {
  uint8_t x;
  if (Attack) {
    Delay_buffer[Delay_index] = Current_buffer[Delay_index];
    Attack -= 1;
  }
  x = Delay_buffer[Delay_index];
  Delay_phase += period;
  Delay_index += Delay_phase >> 14;
  Delay_phase &= 16383;
  Delay_buffer[Delay_index] = (x + Delay_buffer[Delay_index]) >> 1;
  return x;
}

void write_note(FILE* f, int attack, int freq, int duration) {
  int i;
  uint8_t sample;
  Attack = attack;
  for (i = 0; i < SR / duration; i += 1) {
    sample = process(16383 * DELAY_SIZE * (freq / SR));
    sample = (sample >> 4) + 127;
    fwrite(&sample, 1, 1, f);
  }
}

void write_rest(FILE* f, int duration) {
  int i;
  uint8_t sample;
  for (i = 0; i < SR / duration; i += 1) {
    sample = Delay_buffer[Delay_index];
    sample = (sample >> 4) + 127;
    fwrite(&sample, 1, 1, f);
  }
}

void write_chameleon_riff(FILE* f) {
  write_note(f, 400, 48, 4);
  write_note(f, 200, 51, 4);
  write_note(f, 100, 55, 4);
  write_note(f, 200, 58, 4);
  write_rest(f, 8);
  write_note(f, 500, 103, 8);
  write_rest(f, 4);
  write_note(f, 300, 116, 4);
  write_rest(f, 4);
  write_note(f, 400, 65, 4);
  write_note(f, 200, 69, 4);
  write_note(f, 100, 73, 4);
  write_note(f, 200, 77, 4);
  write_rest(f, 8);
  write_note(f, 500, 116, 8);
  write_rest(f, 4);
  write_note(f, 300, 138, 4);
  write_rest(f, 4);
}

int main(void) {
  int i;
  float phase;
  FILE* f;
  f = fopen("result.pcm", "wb");
  for (i = 0; i < DELAY_SIZE; i += 1) {
    phase = 2 * PI * i / DELAY_SIZE;
    Sine_buffer[i] = ((int) (sin(phase) * 127) + 127);
    Noise_buffer[i] = rand() & 255;
    Saw_buffer[i] = i;
  }
  write_chameleon_riff(f);
  write_chameleon_riff(f);
  write_chameleon_riff(f);
  write_chameleon_riff(f);
  fclose(f);
  return 0;
}
