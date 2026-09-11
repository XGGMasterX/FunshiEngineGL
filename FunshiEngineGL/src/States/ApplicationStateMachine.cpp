#include "ApplicationStateMachine.h"

ApplicationState ApplicationStateMachine::getState() const noexcept {
    return state;
}

bool ApplicationStateMachine::is(ApplicationState value) const noexcept {
    return state == value;
}

void ApplicationStateMachine::transitionTo(ApplicationState value) noexcept {
    state = value;
}
