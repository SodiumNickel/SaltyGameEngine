#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <functional> // TODO: make sure this is needed
#include <list>
#include <map>
#include <memory>
#include <typeindex>

#include "Game/EventBus/Event.h"


// Base class
class IEventCallback {
private:
    virtual void Call(Event& e) = 0;

public:
    virtual ~IEventCallback() = default;

    void Execute(Event& e)
    {
        Call(e);
    }
};

template <typename TOwner, typename TEvent>
class EventCallback: public IEventCallback {
private:
    typedef void (TOwner::* CallbackFunction)(TEvent&);

    TOwner* ownerInstance;
    CallbackFunction callbackFunction;

    virtual void Call(Event& e) override
    {
        std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
    }
public:
    EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction)
    {
        this->ownerInstance = ownerInstance;
        this->callbackFunction = callbackFunction;
    }

    virtual ~EventCallback() override = default;
};

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus {
private:
    std::map<std::type_index, std::unique_ptr<HandlerList>> listeners;

public:
    EventBus() { }

    ~EventBus() { }

    void Reset()
    {
        listeners.clear();
    }

    // TODO: decide naming for these two, honestly dont mind subscribe and emit
    template <typename TEvent, typename TOwner>
    void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::*callbackFunction)(TEvent&))
    {
        // If theres no listeners of type TEvent yet, create new pointer
        if(!listeners[typeid(TEvent)].get())
        {
            listeners[typeid(TEvent)] = std::make_unique<HandlerList>();
        }
        auto listener = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
        listeners[typeid(TEvent)]->push_back(std::move(listener));
    }

    // TODO: maybe BroadcastEvents
    template <typename TEvent, typename ...TArgs>
    void EmitEvent(TArgs&& ...args)
    {
        auto handlers = listeners[typeid(TEvent)].get();
        // null check
        if(handlers)
        {
            for(auto it = handlers->begin(); it != handlers->end(); it++)
            {
                auto handler = it->get();
                TEvent event(std::forward<TArgs>(args)...);
                handler->Execute(event);
            }
        }
    }
};

#endif // EVENTBUS_H