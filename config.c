#include "state_machine.h"
#include "file_selector.h"
#include "token.h"
#include "keywords.h"
#include "utils.h"
#include "comment_handler.h"
#include "config.h"

// Reserved Words
const char *RESERVED_WORDS[RESERVED_WORDS_COUNT] = {
    "true", "false", "null", "const"
}; // end of reserved words

// Noise Words
const char *NOISE_WORDS[NOISE_WORDS_COUNT] = {
    "by", "from", "until"
}; // end of noise words

// Test function for debugging
void testKeywordState() {
    KeywordState test = R;
    printf("State R: %d\n", test);
}
