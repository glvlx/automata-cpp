#ifndef COMPOUND_STATE_H_
#define COMPOUND_STATE_H_

#include <string>
#include <unordered_set>
#include <functional>

namespace Automata {

/**
 * Represents (compound) states resulting from power set construction
 */
class CompoundState {
    std::unordered_set<std::string> simple_states_;
    std::size_t hash_ = 0;

    void update_hash(const std::string& new_state);

public:
    CompoundState() = default;
    explicit CompoundState(const std::unordered_set<std::string>& simple_states);
    
    void add_state(const std::string& state);

    void add_states(const std::unordered_set<std::string>& states);

    /**
     * Returns a set of states added before the call
     */
    const std::unordered_set<std::string>& get_primary_states() const noexcept;

    /**
     * Adds up hashes of the names of underlying "primary" states
     */
    std::size_t hash() const noexcept;

    bool operator==(const CompoundState& other) const;
};

} // namespace Automata

template <>
struct std::hash<Automata::CompoundState> {
    std::size_t operator()(const Automata::CompoundState& s) const noexcept {
        return s.hash();
    }
};

#endif