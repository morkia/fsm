#ifndef FSM_H
#define FSM_H

#include <map>
#include <vector>
#include <iostream>
#include <thread>

namespace fsm
{

// forward declearations
class EventDispatcher;
class StateMachine;
class State;

///
/// \brief The Event class
/// The events will cause a state machine to transition.
/// It containes references to current and next states.
/// The state machine will check the current state of the Event class
/// with itself and if it's in the same state it transition to the
/// next state indicated by the event.
///
class Event
{
public:
    State* last;
    State* next;
    std::string name;
    Event(const char name[], State &p, State &n) : last(&p), next(&n), name(name) {}
};

///
/// \brief The State class
/// containes the actions the state machine should take
/// upon entering and exiting this state.
/// The actions may include dispatching events to trigger
/// transitions of other machines listenning to that event.
///
class State
{
protected:
    EventDispatcher* ed;
public:
    State(EventDispatcher& ed) : ed(&ed) { }
    virtual ~State() {}
    virtual void enter(StateMachine* m) = 0;
    virtual void exit(StateMachine* m) = 0;
};

///
/// \brief The StateMachine class
/// contains the event handling and other functions
/// related to state machines operations.
///
class StateMachine
{
private:
    std::string name;
    State* current_state;
public:
    StateMachine(const char name[]) : name(name) {}
    virtual ~StateMachine() {}
    virtual void init(State& s) {
        current_state = &s;
        current_state->enter(this);
    }
    virtual void start() {}
    virtual void stop() {}
    virtual void handleEvent(Event& e) {
        if(current_state == e.last) {
            current_state->exit(this);
            current_state = e.next;
            current_state->enter(this);
        }
    }

    std::string getName() {
        return name;
    }
};

///
/// \brief The EventDispatcher class
/// The state machines all contain a reference to the event
/// dispatcher and use it to dispatch events with themselves
/// or other machines which they control.
/// This class is also used by state machines to subscribe
/// or register for events of interest.
///
class EventDispatcher
{
private:
    std::map< Event*, std::vector<StateMachine*> > m;
public:

    template<typename ...S>
    void subscribe(Event& e, S&... s) {
        int a[] = {0, (m[&e].push_back(&s), 0)...};
        static_cast<void>(a);  // unused
    }

    void unsubscribe(Event& e, StateMachine& s) {
        if(m.find(&e) != m.end()) {
            for(auto event : m) {
                for(auto it = event.second.begin() ; it != event.second.end() ;) {
                    *it == &s ? it = event.second.erase(it) : ++it;
                }
            }
        }
    }

    // sending public event to all
    void dispatch(Event& e) {
        for(auto item : m) {
            if(item.first == &e) {
                for(StateMachine* s : item.second) {
                    s->handleEvent(e);
                }
            }
        }
    }

    // machine sending private event
    void dispatch(Event& e, StateMachine* state_machine) {
        state_machine->handleEvent(e);
    }


    Event* getEvent(const char name[]) {
        for(auto event : m) {
            if(event.first->name == name)
                return event.first;
        }
        return nullptr;
    }
};

} // namespace fsm


#endif // FSM_H
