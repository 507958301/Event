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


class IEventHandler {
public:
    virtual void handle(const void* event) = 0;
    virtual ~IEventHandler() {}
};

template<class T>
class EventHandler : public IEventHandler {
public:
    EventHandler(std::function<void(const T&)> _handler) : m_handler(std::move(_handler)) {}

    void handle(const void* event) override {
        m_handler(*static_cast<const T*>(event));
    }

private:
    std::function<void(const T&)> m_handler;
};

class HandlerGroup {
public:
    void addHandler(IEventHandler* handler) {
        handlers.push_back(handler);
    }

    void removeHandler(IEventHandler* handler) {
        handlers.erase(std::remove(handlers.begin(), handlers.end(), handler), handlers.end());
    }

    void notifyHandlers(const void* event) {
        for (auto handler : handlers) {
            handler->handle(event);
        }
    }

private:
    std::vector<IEventHandler*> handlers;
};

class EventManager {
public:
    static EventManager& getInstance() {
        static EventManager instance;
        return instance;
    }
    template<typename E, typename Handler>
    void registerHandler(Handler handler) {
        nameStr name = typeid(E).name();
        if (handlers.find(name) == handlers.end()) {
            handlers[name] = HandlerGroup();
        }
        auto newHandler = new EventHandler<E>(std::move(handler));
        handlers[name].addHandler(newHandler);
    }

    template<typename E>
    void unregisterHandler() {
        nameStr name = typeid(E).name();
        if (handlers.find(name) != handlers.end()) {
            handlers.erase(name);
        }
    }

    template<typename E>
    void post(const E& event) {
        nameStr name = typeid(E).name();
        if (handlers.find(name) != handlers.end()) {
            handlers[name].notifyHandlers(&event);
        }
    }

private:
    EventManager() {}
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    std::map<nameStr, HandlerGroup> handlers;
};




#endif // OBSERVER_H
