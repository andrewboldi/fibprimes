# fibprimes

Generates music from prime numbers and the Fibonacci sequence, output as a MIDI file.

Each chord consists of a Fibonacci bass note and 4 prime-derived notes.

## Build & Run

```
make
make run
```

This produces `fibprimes.mid` and `fibprimes.txt`.

## How It Works

### Primes → Notes

All primes up to 26,212 are found via the Sieve of Eratosthenes. Each prime is mapped to a note in C major by its scale degree (`prime % 7`):

| Residue | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
|---------|---|---|---|---|---|---|---|
| Note    | B | C | D | E | F | G | A |

Primes are grouped into 4-note chords. Within each chord, notes are spread across ascending octaves so every note is higher than the previous.

### Fibonacci Bass

The bottom note of each chord comes from the Fibonacci sequence mod 7 (mapped to MIDI pitches). This sequence is cyclic with period 16, so the bass pattern repeats every 16 chords.

### Output

- **fibprimes.mid** — Standard MIDI file (format 0, 1 track)
- **fibprimes.txt** — Note names for each chord, one chord per line
