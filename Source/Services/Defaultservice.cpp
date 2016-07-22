/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-7-22
    Notes:
        A default service for testing and template.
*/

#include <Configuration\All.h>
#include <Communication\Messagehandler.h>
#include <Cache\Cachemanager.h>
#include <thread>

struct MyTestcache : public Cacheblock
{
    virtual void Update() override
    {
        PrintFunction();
    }
};

struct MyTestservice
{
    // A simple service taking two args and returning nothing.
    static void Core(int arg1, int arg2)
    {
        DebugPrint(va("%s called with arg1 = %i and arg2 = %i", __FUNCTION__, arg1, arg2));
    }

    // Handlers.
    static void Testservice_sync(va_list *Arguments, Bytebuffer *Buffer)
    {
        int arg1, arg2;

        if (Arguments)
        {
            arg1 = va_arg(*Arguments, int);
            arg2 = va_arg(*Arguments, int);
        }
        else
        {
            arg1 = Buffer->Read<uint32_t>();
            arg2 = Buffer->Read<uint32_t>();
        }

        Core(arg1, arg2);
    }
    static void Testservice_async(va_list *Arguments, Bytebuffer *Buffer)
    {
        int arg1, arg2;

        if (Arguments)
        {
            arg1 = va_arg(*Arguments, int);
            arg2 = va_arg(*Arguments, int);
        }
        else
        {
            arg1 = Buffer->Read<uint32_t>();
            arg2 = Buffer->Read<uint32_t>();
        }

        auto Lambda = [](int arg1) { Messagehandler::Addrequest(); Core(arg1, 0); Messagehandler::Removerequest(); };
        std::thread(Lambda, arg1).detach();
    }

    // Register the handlers and data.
    MyTestservice()
    {
        Messagehandler::Registerhandler(FNV1a_Compiletime_32("Testservice_sync"), Testservice_sync);
        Messagehandler::Registerhandler(FNV1a_Compiletime_32("Testservice_async"), Testservice_async);

        Cachemanager::Registerblock(new MyTestcache(), 5000, FNV1a_Compiletime_32("Testcache"));
    }
};

// Call the constructor on startup.
static MyTestservice MyService;
