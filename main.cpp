#include <iostream>
#include <thread>
#include "fsm.h"

using namespace std;

///-------------- Traffic Light States, Transitions and State Machine ----------------///

/// Traffic Light Transitions

class Red2Green : public fsm::Event {
public:
    Red2Green(const char name[], fsm::State &p, fsm::State &n) : fsm::Event(name, p, n) {}
};

class Green2Amber : public fsm::Event {
public:
    Green2Amber(const char name[], fsm::State &p, fsm::State &n) : fsm::Event(name, p, n) {}};

class Amber2Red : public fsm::Event {
public:
    Amber2Red(const char name[], fsm::State &p, fsm::State &n) : fsm::Event(name, p, n) {}

};

/// Traffic Light States

class Red : public fsm::State
{
public:
    Red(fsm::EventDispatcher &ed) : fsm::State(ed) {}
    void enter(fsm::StateMachine* m) {
        cout << m->getName() << "Red. " << endl;
    }
    void exit(fsm::StateMachine* m) {
        static_cast<void>(m);
    }
};


class Amber : public fsm::State
{
private:
    thread *t;
public:
    Amber(fsm::EventDispatcher& ed) : fsm::State(ed), t(nullptr) {}
    void enter(fsm::StateMachine* m) {
        cout << m->getName() << "Amber. " << endl;
        t = new thread([=]()
        {
            this_thread::sleep_for(std::chrono::seconds(2));
            fsm::Event *e = ed->getEvent("A2R");
            if(e)
                ed->dispatch(*e, m);
        });

        t->detach();
    }
    void exit(fsm::StateMachine* m) {
        static_cast<void>(m);
    }
};

class Green : public fsm::State
{
private:
    thread *t;
public:
    Green(fsm::EventDispatcher& ed) : fsm::State(ed), t(nullptr) {}
    void enter(fsm::StateMachine* m) {
        cout << m->getName() << "Green. " << endl;
        t = new thread([=]()
        {
            this_thread::sleep_for(std::chrono::seconds(15));
            fsm::Event *e = ed->getEvent("G2A");
            if(e)
                ed->dispatch(*e, m);
        });

        t->detach();
    }

    void exit(fsm::StateMachine* m) {
        static_cast<void>(m);
    }
};

/// Traffic Light StateMachine

class TrafficLight : public fsm::StateMachine
{
public:
    TrafficLight(const char name[]) : fsm::StateMachine(name) {}
};


///-------------- Intersection Controller States, Transitions and State Machine ----------------///

/// Intersection Controller Transitions

class AllStop2NorthSouthGo : public fsm::Event {
public:
    AllStop2NorthSouthGo(const char name[], fsm::State &p, fsm::State &n) : fsm::Event(name, p, n) {}
};

class NorthSouthGo2AllStop : public fsm::Event {
public:
    NorthSouthGo2AllStop(const char name[], fsm::State &p, fsm::State &n) : fsm::Event(name, p, n) {}
};

class AllStop2EastWestGo : public fsm::Event {
public:
    AllStop2EastWestGo(const char name[], fsm::State &p, fsm::State &n) : fsm::Event(name, p, n) {}
};

class EastWestGo2AllStop : public fsm::Event {
public:
    EastWestGo2AllStop(const char name[], fsm::State &p, fsm::State &n) : fsm::Event(name, p, n) {}
};

/// Intersection Controller States

class AllStop : public fsm::State
{
    thread *t;
public:
    AllStop(fsm::EventDispatcher &ed) : fsm::State(ed), t(nullptr) {}
    void enter(fsm::StateMachine* m) {
        cout << m->getName() << " AllStop." << endl;
        t = new thread([=]()
        {
            this_thread::sleep_for(std::chrono::seconds(2));
        });

        t->join();
    }
    void exit(fsm::StateMachine* m) {
        static_cast<void>(m);
    }
};

class NorthSouthGo : public fsm::State
{
    thread *t;
public:
    NorthSouthGo(fsm::EventDispatcher &ed) : fsm::State(ed), t(nullptr) {}
    void enter(fsm::StateMachine* m) {
        cout << m->getName() << " North and South Go." << endl;
        fsm::Event *e = ed->getEvent("AS2NSGO_");
        if(e)
            ed->dispatch(*e);
        e = ed->getEvent("AS2NSGO__");
        if(e)
            ed->dispatch(*e);
        t = new thread([=]()
        {
            this_thread::sleep_for(std::chrono::seconds(18));
        });

        t->join();
    }
    void exit(fsm::StateMachine* m) {
        static_cast<void>(m);
    }
};

class EastWestGo : public fsm::State
{
    thread *t;
public:
    EastWestGo(fsm::EventDispatcher &ed) : fsm::State(ed), t(nullptr) {}
    void enter(fsm::StateMachine* m) {
        cout << m->getName() << " East and West Go." << endl;;
        fsm::Event *e = ed->getEvent("AS2ESGO_");
        if(e)
            ed->dispatch(*e);
        e = ed->getEvent("AS2ESGO__");
        if(e)
            ed->dispatch(*e);
        t = new thread([=]()
        {
            this_thread::sleep_for(std::chrono::seconds(18));
        });

        t->join();
    }
    void exit(fsm::StateMachine* m) {
        static_cast<void>(m);
    }
};

/// Intersection Controller State Machine

class IntersectionControiller : public fsm::StateMachine
{
    std::string m_name;
public:
    IntersectionControiller(const char name[]) : fsm::StateMachine(name), m_name(name) {}
};



int main()
{
    // Initialize the event dispatcher
    fsm::EventDispatcher ed;

    // Initialize traffic lights
    Red r(ed);
    Amber a(ed);
    Green g(ed);
    Red2Green r2g("R2G", r, g);
    Green2Amber g2a("G2A", g, a);
    Amber2Red a2r("A2R", a, r);
    TrafficLight l("TL1");
    TrafficLight l2("TL2");
    TrafficLight l3("TL3");
    TrafficLight l4("TL4");
    l.init(r);
    l2.init(r);
    l3.init(r);
    l4.init(r);
    ed.subscribe(r2g, l, l2, l3, l4);
    ed.subscribe(g2a, l, l2, l3, l4);
    ed.subscribe(a2r, l, l2, l3, l4);

    // Initialize intersection controller
    AllStop as(ed);
    NorthSouthGo nsgo(ed);
    EastWestGo esgo(ed);
    AllStop2NorthSouthGo as2nsgo("AS2NSGO", as, nsgo);
    AllStop2NorthSouthGo as2nsgo_("AS2NSGO_", r, g);
    AllStop2NorthSouthGo as2nsgo__("AS2NSGO__", r, r);
    NorthSouthGo2AllStop nsgo2as("NSGO2AS", nsgo, as);
    AllStop2EastWestGo as2esgo("AS2ESGO", as, esgo);
    AllStop2EastWestGo as2esgo_("AS2ESGO_", r, g);
    AllStop2EastWestGo as2esgo__("AS2ESGO__", r, r);
    EastWestGo2AllStop esgo2as("ESGO2AS", esgo, as);
    IntersectionControiller ic("Intersection ");
    ic.init(as);
    ed.subscribe(as2nsgo, ic);
    ed.subscribe(nsgo2as, ic);
    ed.subscribe(as2esgo, ic);
    ed.subscribe(esgo2as, ic);

    ed.subscribe(as2nsgo_, l, l3);
    ed.subscribe(as2nsgo__, l2, l4);
    ed.subscribe(as2esgo_, l2, l4);
    ed.subscribe(as2esgo__, l, l3);

    // endless loop
    while (true) {
        ed.dispatch(as2nsgo);
        ed.dispatch(nsgo2as);
        ed.dispatch(as2esgo);
        ed.dispatch(esgo2as);
    }


    return 0;
}
