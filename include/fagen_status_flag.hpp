#ifndef FAGEN_STATUS_FLAG_H_
#define FAGEN_STATUS_FLAG_H_

namespace Automata {

enum FAGenStatusFlag {
    OK                  = 0,
    WRONG_NAME          = 0b00001,
    WRONG_FMT           = 0b00010,
    WRONG_STATE_NAME    = 0b00100,
    WRONG_SYMBOL        = 0b01000,
    NO_INIT             = 0b10000,
};

} // namespace Automata

#endif
