#include "compound_state.hpp"

#include <string>
#include <functional>
#include <unordered_set>

namespace Automata {

CompoundState::CompoundState(const std::unordered_set<std::string>& simple_states)
        : simple_states_(simple_states) {
    for (const auto& state : simple_states) {
        update_hash(state);
    }
}

void CompoundState::update_hash(const std::string& new_state) {
    /* needs to be order-independent because unordered_set */
    hash_ += std::hash<std::string>{}(new_state);
}

void CompoundState::add_state(const std::string& state) {
    auto [it, inserted] = simple_states_.insert(state);
    if (inserted) {
        update_hash(state);
    }
}

void CompoundState::add_states(const std::unordered_set<std::string>& states) {
    for (const auto& state : states) {
        add_state(state);
    }
}

const std::unordered_set<std::string>& CompoundState::get_primary_states() const noexcept {
    return simple_states_;
}

/**
 * Adds up hashes of the names of underlying "primary" states
 */
std::size_t CompoundState::hash() const noexcept {
    return hash_;
}

bool CompoundState::operator==(const CompoundState& other) const {
    return simple_states_ == other.simple_states_;
}

} // namespace Automata