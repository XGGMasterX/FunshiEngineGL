#ifndef APPLICATION_STATE_MACHINE_H
#define APPLICATION_STATE_MACHINE_H

enum class ApplicationState {
    MainMenu,
    Editing,
    Playing,
    Exiting
};

class ApplicationStateMachine {
private:
    ApplicationState state = ApplicationState::MainMenu;

public:
    ApplicationState getState() const noexcept;
    bool is(ApplicationState value) const noexcept;
    void transitionTo(ApplicationState value) noexcept;
};

#endif
