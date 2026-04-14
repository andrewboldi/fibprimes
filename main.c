#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define PRIME_LIMIT 26212
#define VELOCITY 0x32
#define NOTES_PER_CHORD 4
#define FIB_CYCLE_LEN 16

/* Fibonacci sequence mod 7 mapped to MIDI note numbers (cyclic period of 16) */
static const unsigned char FIB_BASS[FIB_CYCLE_LEN] = {
    36, 36, 38, 40, 43, 36, 45, 47,
    45, 45, 43, 41, 38, 45, 36, 47
};

static const unsigned char MIDI_HEADER[] = {
    0x4D, 0x54, 0x68, 0x64,  /* MThd */
    0x00, 0x00, 0x00, 0x06,  /* chunk length */
    0x00, 0x00,              /* format 0 */
    0x00, 0x01,              /* 1 track */
    0x00, 0x01               /* 1 tick per quarter note */
};

static const unsigned char TRACK_HEADER[] = {
    0x4D, 0x54, 0x72, 0x6B,  /* MTrk */
    0x00, 0x00, 0xFF, 0xFE   /* track length placeholder */
};

static const unsigned char END_OF_TRACK[] = {
    0x02, 0xFF, 0x2F, 0x00
};

static bool is_prime[PRIME_LIMIT];

static unsigned char note_to_midi(char note) {
    switch (note) {
        case 'A': return 57;
        case 'B': return 59;
        case 'C': return 48;
        case 'D': return 50;
        case 'E': return 52;
        case 'F': return 53;
        case 'G': return 55;
        default:  return 0;
    }
}

static char degree_to_note(int degree) {
    static const char notes[] = "BCDEFGA";
    return notes[degree];
}

static void write_bytes(FILE *f, const unsigned char *data, size_t len) {
    fwrite(data, 1, len, f);
}

static void write_event(FILE *f, unsigned char delta, unsigned char status,
                        unsigned char note, unsigned char vel) {
    unsigned char ev[] = {delta, status, note, vel};
    write_bytes(f, ev, 4);
}

static void sieve(void) {
    for (int i = 0; i < PRIME_LIMIT; i++)
        is_prime[i] = true;
    is_prime[0] = is_prime[1] = false;

    for (int i = 2; i <= (int)sqrt(PRIME_LIMIT); i++) {
        if (!is_prime[i]) continue;
        for (int j = i * i; j < PRIME_LIMIT; j += i)
            is_prime[j] = false;
    }
}

static void write_chord(FILE *midi, const char *chord, int chord_idx) {
    unsigned char notes[NOTES_PER_CHORD];
    for (int i = 0; i < NOTES_PER_CHORD; i++)
        notes[i] = note_to_midi(chord[i]);

    /* Spread across octaves so each note is higher than the previous */
    for (int i = 0; i < NOTES_PER_CHORD - 1; i++) {
        if (notes[i] >= notes[i + 1])
            notes[i + 1] += 12;
    }

    /* Note-on: Fibonacci bass */
    unsigned char bass = FIB_BASS[chord_idx % FIB_CYCLE_LEN];
    write_event(midi, 0x00, 0x90, bass, VELOCITY);

    /* Note-on: prime-derived chord tones */
    for (int i = 0; i < NOTES_PER_CHORD; i++)
        write_event(midi, 0x00, 0x90, notes[i], VELOCITY);

    /* Note-off: bass then chord tones */
    write_event(midi, 0x04, 0x80, bass, VELOCITY);
    for (int i = 0; i < NOTES_PER_CHORD; i++)
        write_event(midi, 0x00, 0x80, notes[i], VELOCITY);
}

int main(void) {
    sieve();

    FILE *txt = fopen("fibprimes.txt", "w");
    FILE *mid = fopen("fibprimes.mid", "wb");
    if (!txt || !mid) {
        fprintf(stderr, "Error: could not open output files\n");
        return EXIT_FAILURE;
    }

    write_bytes(mid, MIDI_HEADER, sizeof(MIDI_HEADER));
    write_bytes(mid, TRACK_HEADER, sizeof(TRACK_HEADER));

    char buf[NOTES_PER_CHORD];
    int note_count = 0;
    int chord_count = 0;

    for (int p = 0; p < PRIME_LIMIT; p++) {
        if (!is_prime[p]) continue;

        int pos = note_count % NOTES_PER_CHORD;
        buf[pos] = degree_to_note(p % 7);

        if (pos == NOTES_PER_CHORD - 1) {
            fprintf(txt, "%c %c %c %c\n", buf[0], buf[1], buf[2], buf[3]);
            write_chord(mid, buf, chord_count);
            chord_count++;
        }

        note_count++;
    }

    write_bytes(mid, END_OF_TRACK, sizeof(END_OF_TRACK));
    fclose(txt);
    fclose(mid);

    printf("Generated %d chords from %d prime-derived notes\n",
           chord_count, note_count);
    return EXIT_SUCCESS;
}
