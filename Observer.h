#ifndef OBSERVER_H
#define OBSERVER_H

#include <typeinfo>
#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <map>

struct nameStr {
    std::string name;
    nameStr(const char* _name) :name(_name) {};
    nameStr(std::string _name) :name(_name) {};

    bool operator<(const nameStr& other) const {
        return name < other.name;
    }
};


class Handler {
public:
    virtual void handle(const void* event) = 0;
    virtual ~Handler() {}
};

template<class T>
class weightHandler : public Handler {
public:
    weightHandler(std::function<void(const T&)> _handler) : m_handler(_handler) {}

    void handle(const void* event) override {
        m_handler(*static_cast<const T*>(event));
    }

private:
    std::function<void(const T&)> m_handler;
};

class HandlerGroup {
public:
    void addHandler(Handler* handler) {
        handlers.push_back(handler);
    }

    void removeHandler(Handler* handler) {
        handlers.erase(std::remove(handlers.begin(), handlers.end(), handler), handlers.end());
    }

    void notifyHandlers(const void* event) {
        for (auto handler : handlers) {
            handler->handle(event);
        }
    }

private:
    std::vector<Handler*> handlers;
};

class Person {
public:
    static Person& getInstance() {
        static Person instance;
        return instance;
    }
    template<typename E>
    void registerHandler(std::function<void(const E&)> handler) {
        if (handlers.find(typeid(E).name()) == handlers.end()) {
            handlers[typeid(E).name()] = HandlerGroup();
        }
        auto newHandler = new weightHandler<E>(handler);
        handlers[typeid(E).name()].addHandler(newHandler);
    }

    template<typename E>
    void unregisterHandler() {
        if (handlers.find(typeid(E).name()) != handlers.end()) {
            handlers.erase(typeid(E).name());
        }
    }

    template<typename E>
    void post(const E& event) {
        if (handlers.find(typeid(E).name()) != handlers.end()) {
            handlers[typeid(E).name()].notifyHandlers(&event);
        }
    }

private:
    Person() {}
    Person(const Person&) = delete;
    Person& operator=(const Person&) = delete;

    std::map<nameStr, HandlerGroup> handlers;
};




#endif // OBSERVER_H
