/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-7-22
    Notes:
        Routes messages to subsystems.
*/

#include "Messagehandler.h"
#include <unordered_map>
#include <thread>
#include <deque>
#include <mutex>

namespace Messagehandler
{
    // Local map of the handlers.
    std::unordered_map<uint32_t /* Message ID */, mCallback> Handlermap;

    // Register a callback for the subsystem.
    void Registerhandler(uint32_t MessageID, mCallback Handler)
    {
        Handlermap.emplace(MessageID, Handler);
    }

    // Route the message to the subsystem.
    void Handlemessage(uint32_t MessageID, va_list Arguments)
    {
        auto Handler = Handlermap.find(MessageID);

        // Process the messages in parallel and return them in order.
        if (Handler != Handlermap.end())
        {
            auto Lambda = [](auto Handler, va_list Arguments) { Addrequest(); Handler->second(Arguments); };
            std::thread(Lambda, Handler, Arguments).detach();
        }            
    }

    // Request-synchronization.
    std::mutex Requestguard;
    std::deque<std::thread::id> Requestqueue;
    void Addrequestfront()
    {
        Requestguard.lock();
        {
            auto Thread = Requestqueue.front();
            Requestqueue.push_front(Thread);
        }
        Requestguard.unlock();
    }
    void Removerequest()
    {
        Waitrequest();
        Requestguard.lock();
        {
            Requestqueue.pop_front();
        }
        Requestguard.unlock();
    }
    void Waitrequest()
    {
        std::thread::id Currentthread = std::this_thread::get_id();
        std::thread::id Frontthread;

        do
        {
            Requestguard.lock();
            {
                Frontthread = Requestqueue.front();
            }
            Requestguard.unlock();

            if (Currentthread != Frontthread)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            else
                break;
        } while (true);
    }
    void Addrequest()
    {
        Requestguard.lock();
        {
            Requestqueue.push_front(std::this_thread::get_id());
        }
        Requestguard.unlock();
    }
};
