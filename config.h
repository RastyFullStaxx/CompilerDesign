#ifndef CONFIG_H
#define CONFIG_H

// Reserved Words
#define RESERVED_WORDS_COUNT 4
extern const char *RESERVED_WORDS[RESERVED_WORDS_COUNT];

// Noise Words
#define NOISE_WORDS_COUNT 3
extern const char *NOISE_WORDS[NOISE_WORDS_COUNT];

// Delimiters
#define DELIMITERS ",.;:(){}[]\"'"

// File Extension
#define VALID_FILE_EXTENSION ".prsm"

#endif // CONFIG_H
