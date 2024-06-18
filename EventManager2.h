#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <functional>
#include <utility> // for std::move
#include <cassert> // 包含 assert 宏
#include <list>
#include <algorithm>
#include <map>
#include <iostream>
#include <future> // for std::async
#include <string>
#include <string.h>
template <typename T>
int indexOf(const std::vector<T>& lst, const T& value) {
    auto it = std::find(lst.begin(), lst.end(), value);
    if (it != lst.end()) {
        return std::distance(lst.begin(), it);
    } else {
        return -1; // 如果未找到，返回 -1
    }
}
using StringHash = std::string;
class IEventHandler
{
public:
    virtual ~IEventHandler()
    {

    }
    virtual void handle() = 0;
};

template <typename E>
class EventHandler : public IEventHandler
{
public:
    EventHandler(const StringHash &eventName)
        : m_eventName(eventName)
    {

    }

    void handle() {}

    //void handle(const E &&e)
    //{
    //    m_handler(e);
    //}

    void handle(const E &e)
    {
        m_handler(std::forward<const E &>(e));
    }


    std::function<void (const E &)> m_handler;
    StringHash m_eventName;
};

template <typename E, typename S>
class EventHandlerM : public IEventHandler
{
public:
    EventHandlerM(const StringHash &eventName)
        : m_eventName(eventName)
    {

    }

    void handle() {}

    void handle(const E &&e, const S &&s)
    {
        m_handler(e, s);
    }

    std::function<void (const E &, const S &)> m_handler;
    StringHash m_eventName;
};

class EventHandlerGroup
{
public:
    EventHandlerGroup()
    {

    }

    ~EventHandlerGroup()
    {
        for (int i = 0; i < m_receivers.size(); ++i)
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
        if (m_send == 0 && m_dirty)
        {
            // NOTE::增加i为-1时的判断，会导致索引越界，触发断言 2021年3月25日16:41:27 @xtualong
            for (int i = m_receivers.size() - 1; i < m_receivers.size() && i >= 0; --i)
            {
                if (!m_receivers[i])
                {
                    m_receivers.erase(m_receivers.begin()+i);
                }
            }
            m_dirty = false;
        }
    }

    void addEvent(IEventHandler *handler)
    {
        if (handler)
            m_receivers.emplace_back(handler);
    }

    void delEvent(IEventHandler *handler)
    {
        int i = indexOf(m_receivers, handler);
        if (m_send > 0)
        {

            if (i != -1)
            {
                m_receivers[i] = nullptr;
                m_dirty = true;
            }
        }
        else
        {
            if (i != -1)
            {
                m_receivers.erase(m_receivers.begin()+i);
            }
        }
        delete handler;
        // edit by xzl::取消注册事件后删除Handler指针
        handler = nullptr;
    }
    std::vector<IEventHandler *> m_receivers;
private:
    unsigned int m_send = 0;
    bool m_dirty = false;
};


class EventManager
{
public:
    static EventManager *instance();

private:
    EventManager();
    ~EventManager();

public:
    /**
     * @brief 订阅事件-方式1
     * @param handler
     * @note 如果一个对象中订阅了事件，则需要在析构时取消订阅，否则会导致下次事件发送失败
     * @return
     */
    template <typename E, typename Handler,typename ...Args>
    IEventHandler *subscribeToEvent(Handler handler, Args&&... args)
    {
        StringHash eventName(typeid(E).name());
        auto h = new EventHandler<E>(eventName);
        h->m_handler = std::move(handler);
        //往事件处理函数池中添加一个事件处理函数
        m_eventHandlers.emplace_back(h);
        //查找事件处理函数组是否存在
        EventHandlerGroup *&group = m_eventReceivers[eventName];
        if (!group)
        {
            group = new EventHandlerGroup;
        }
        //往组里添加一个事件处理函数
        group->addEvent(h);
        return h;
    }

    //订阅事件-方式2
    template <typename E, typename S, typename Handler >
    IEventHandler *subscribeToEvent(Handler handler)
    {
        StringHash eventName(typeid(E).name());
        auto h = new EventHandlerM<E, S>(eventName);
        h->m_handler = std::move(handler);

        //往事件处理函数池中添加一个事件处理函数
        m_eventHandlers.emplace_back(h);
        //查找事件处理函数组是否存在
        EventHandlerGroup *&group = m_eventReceivers[eventName];
        if (!group)
        {
            group = new EventHandlerGroup;
        }
        //往组里添加一个事件处理函数
        group->addEvent(h);
        return h;
    }

    //取消订阅事件
    template <typename E>
    void unSubscribeToEvent(IEventHandler *handler)
    {
        if (!handler)
        {
            return;
        }
        StringHash eventName(typeid(E).name());
        //判断事件处理函数是否存在事件处理函数池中
        std::cout<<"m_eventHandlers.size():   " <<m_eventHandlers.size()<<std::endl;
        int index = indexOf(m_eventHandlers, handler);
        if(index != -1)
        {
            m_eventHandlers.erase(m_eventHandlers.begin()+index);
            removeEventHandler(eventName, handler);
            std::cout<<"m_eventHandlers.size():   " <<m_eventHandlers.size()<<std::endl;
        }
    }

    //发送事件-方式1，同步方式
    template <typename E>
    void sendEvent(const E &event)
    {
        StringHash eventName(typeid(E).name());
        //查找事件处理函数组
        EventHandlerGroup *group = getEventGroup(eventName);
        if (group)
        {
            group->beginEvent();
            for (IEventHandler *receiver : group->m_receivers)
            {
                if (!receiver)
                    continue;
                auto h = dynamic_cast<EventHandler<E> *>(receiver);
                if (h)
                {
                    h->handle(std::move(event));
                }
            }
            group->endEvent();
        }
    }

    //发送事件-方式2，同步方式
    template <typename E, typename S>
    void sendEvent(const E &event, const S &sender)
    {
        StringHash eventName(typeid(E).name());
        //查找事件处理函数组
        EventHandlerGroup *group = getEventGroup(eventName);
        if (group)
        {
            group->beginEvent();
            for (IEventHandler *receiver : group->m_receivers)
            {
                if (!receiver)
                    continue;
                auto h = dynamic_cast<EventHandlerM<E, S> *>(receiver);
                if (h)
                {
                    if (sender)
                    {
                        h->handle(std::move(event), std::move(sender));
                    }
                    else
                    {
                        //qDebug() << std("EventManager Filtered null sender Message %1 %2!").arg(typeid(E).name()).arg(typeid(S).name());
                    }
                }
            }
            group->endEvent();
        }
    }

    //发送事件-方式1，异步方式
    template <typename E>
    void postEvent(const E &event)
    {
        //qDebug() << QString("EventManager,postEvent1.");
        //QTimer::singleShot(0, this, [ = ]()
        {
            //sendEvent<E>(std::move(event));
        }//);
        {
            // 异步发送操作
              auto future = std::async(std::launch::async, [=]() {
                  // 模拟发送操作
                  //std::this_thread::sleep_for(std::chrono::nanoseconds(2)); // 模拟发送耗时
                  sendEvent<E>(std::move(event));

                  //MyEventType name("1213123");
                  //name.name = "12322222222222222";
                  //return name;
              });
             // auto temp = future.get();
              //std::cout << "future data."<<temp.name<< std::endl;
        }
    }

    //发送事件-方式2，异步方式
    template <typename E, typename S>
    void postEvent(const E &event, const S &sender)
    {
        //qDebug() << QString("EventManager,postEvent2.");
        //QTimer::singleShot(0, this, [ = ]()
        {
            sendEvent<E, S>(std::move(event, sender));
        }//);
    }

private:
    IEventHandler *findEventHandler(IEventHandler *handler);

    void removeEventHandler(const StringHash &eventType, IEventHandler *handler);

    EventHandlerGroup *getEventGroup(const StringHash &eventType);

private:
    std::map<StringHash, EventHandlerGroup *> m_eventReceivers;
    std::vector<IEventHandler *> m_eventHandlers;
};

static EventManager *m_instance = nullptr;

EventManager *EventManager::instance()
{
    if (m_instance == nullptr)
    {
        m_instance = new EventManager;
    }
    return m_instance;
}

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
    for (auto& pair  : m_eventReceivers)
    {
        EventHandlerGroup*& group = pair.second;
        delete group;
    }
}

IEventHandler *EventManager::findEventHandler(IEventHandler *handler)
{
    for (IEventHandler *&_handler : m_eventHandlers)
    {
        if (_handler == handler)
        {
            return handler;
        }
    }
    return nullptr;
}

void EventManager::removeEventHandler(const StringHash &eventType, IEventHandler *handler)
{
    EventHandlerGroup *&group = m_eventReceivers[eventType];
    if (group)
    {
        group->delEvent(handler);
    }
}

EventHandlerGroup *EventManager::getEventGroup(const StringHash &eventType)
{
    std::map<StringHash, EventHandlerGroup *>::iterator it = m_eventReceivers.find(eventType);
    if (it != m_eventReceivers.end())
    {
        return it->second;
    }
    else
        return nullptr;
}
#endif // EVENTMANAGER_H

