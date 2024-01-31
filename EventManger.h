#ifndef EVENTMANGER_H
#define EVENTMANGER_H

#include <typeinfo>
#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <map>
#include <cassert>
#include "StringHash.h"

class IEventHandler {
public:
    virtual void handle() = 0;

    virtual ~IEventHandler() {}
};

template<class E>
class EventHandler : public IEventHandler {
public:
    EventHandler(const StringHash& eventName) 
        : m_eventName(std::move(eventName)) 
    {

    }

    void handle() {}

    void handle(const E &&event) 
    {
        m_handler(event);
    }
    std::function<void (const E&)> m_handler;
    StringHash m_eventName;
};

class EventHandlerGroup {
public:
    EventHandlerGroup()
    {

    }
    ~EventHandlerGroup()
    {
        for (size_t i = 0; i < m_receivers.size(); ++i)
        {
            delete m_receivers[i];
        }
        
    }
    
    void beginEvent() 
    {
        ++m_send;
    }

    void endEvent()
    {
        assert(m_send > 0);
        --m_send;
        if(m_send == 0 && m_dirty)
        {
            for (size_t i = m_receivers.size()-1; i < m_receivers.size(); --i)
            {
                if(!m_receivers[i])
                {
                    m_receivers.erase(m_receivers.begin() + i);
                }
            }
            m_dirty = false;
        }
    }

    void addEvent(IEventHandler *handler)
    {
        if(handler)
        {
            m_receivers.push_back(handler);
        }
    }

    void delEvent(IEventHandler *handler)
    {
        if(m_send > 0)
        {
            auto it = std::find(m_receivers.begin(), m_receivers.end(), handler);
            if(it != m_receivers.end())
            {
                *it = nullptr;
                m_dirty = false;
            }
            else
            {
                removeHandler(handler);
            }
        }
        delete handler;
        handler = nullptr;
    }

    void removeHandler(IEventHandler* handler) {
        m_receivers.erase(std::remove(m_receivers.begin(), m_receivers.end(), handler), m_receivers.end());
    }

    std::vector<IEventHandler*> m_receivers;
private:
    unsigned int m_send = 0;
    bool m_dirty = false;
};

class EventManager {
public:
    static EventManager& getInstance() {
        static EventManager instance;
        return instance;
    }

    template<typename E, typename Handler>
    IEventHandler* subsrcibeToEvent(Handler handler) 
    {
        StringHash eventName( typeid(E).name() );
        auto h = new EventHandler<E>(eventName);
        h->m_handler = std::move(handler);

        m_eventHandlers.push_back(h);

        EventHandlerGroup *&group = m_eventReceivers[eventName];
        if(!group)
        {
            group = new EventHandlerGroup;
        }

        group->addEvent(h);
        return h;
    }

    template<typename E>
    void unSubscribeToEvent(IEventHandler *handler) 
    {
        if(!handler)
        {
            return ;
        }

        StringHash eventName (typeid(E).name() );

        for (IEventHandler *_handler : m_eventHandlers)
        {
            if(_handler == handler)
            {
                m_eventHandlers.erase(std::remove(m_eventHandlers.begin(), m_eventHandlers.end(), handler), m_eventHandlers.end());
                removeEventHandler(eventName, handler);
            }
        }
    }

    template<typename E>
    void sendEvent(const E& event) {
        StringHash eventName( typeid(E).name() );
        EventHandlerGroup *group = getEventGroup(eventName);
        if(group)
        {
            group->beginEvent();
            for (IEventHandler *receiver : group->m_receivers)
            {
                if( !receiver )
                    continue;
                auto h = dynamic_cast<EventHandler<E> *>(receiver);
                if(h)
                {
                    h->handle(std::move(event));
                }
            }
            group->endEvent();
        }
    }

    template<typename E>
    void postEvent(const E& event) 
    {
        sendEvent<E>( std::move(event) );
    }

private:
    EventManager() {}
    ~EventManager();
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    IEventHandler *findEventHandler(IEventHandler *handler);

    void removeEventHandler(const StringHash &eventType, IEventHandler *handler);

    EventHandlerGroup * getEventGroup(const StringHash &eventType);

private:

    std::map<StringHash, EventHandlerGroup*> m_eventReceivers;
    std::vector<IEventHandler*> m_eventHandlers;
};




#endif // EVENTMANGER_H
