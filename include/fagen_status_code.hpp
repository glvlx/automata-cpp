#ifndef FAGEN_STATUS_CODE_H_
#define FAGEN_STATUS_CODE_H_

namespace Automata {

enum FAGenStatusCode {
    OK = 0,
    WRONG_NAME,
    WRONG_FMT,
    WRONG_STATE_NAME,
    WRONG_SYMBOL,
    NO_INIT
};

} // namespace Automata

#endif
