#include "EventManger.h"
EventManager::~EventManager()
{
    for (auto group : m_eventReceivers)
    {
        delete group.second;
    }
}

IEventHandler *EventManager::findEventHandler(IEventHandler *handler)
{
    for (IEventHandler *_handler : m_eventHandlers)
    {
        if(_handler == handler)
        {
            return handler;
        }
    }
}

void EventManager::removeEventHandler(const StringHash &eventType, IEventHandler *handler)
{
    EventHandlerGroup *group = m_eventReceivers[eventType];
    if (group)
    {
        group->delEvent(handler);
    }
}

EventHandlerGroup * EventManager::getEventGroup(const StringHash &eventType)
{
    auto it = m_eventReceivers.find(eventType);
    if (it != m_eventReceivers.end()) {
        return it->second;
    }
    return nullptr;  // 或者返回适当的错误处理
}