#include "file_selector.h"
#include "token.h"
#include "state_machine.h"
#include "keywords.h"
#include "utils.h"
#include "comment_handler.h"
#include "config.h"

// Reserved Words
const char *RESERVED_WORDS[RESERVED_WORDS_COUNT] = {

    "true", "false", "null", "const"

}; // end of reserved words functions




// Noise Words
const char *NOISE_WORDS[NOISE_WORDS_COUNT] = {

    "by", "from", "until"

}; // end of noise words functions
