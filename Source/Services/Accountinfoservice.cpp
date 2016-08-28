/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-8-29
    Notes:
        Retrieves the account information from the backend.
*/

#include <Configuration\All.h>
#include <Communication\Messagehandler.h>
#include <Cache\Cachemanager.h>
#include <thread>

// A structure to track the current set of account info available.
struct Internal_Accountinformation
{
    uint32_t ForumID;
    uint32_t IPv4Address;
    std::string Username;
};

// The cache results for this service.
struct ForumIDcache : public Cacheblock
{
    virtual void Update() override
    {
        /*
            TODO(Convery);
            Send a request to the server so we can update this.
        */
        
        if(0 == Buffersize)
        {
            uint32_t Defaultvalue = 0xDEADC0DE;            
            
            Databuffer = std::make_shared<char []>(sizeof(uint32_t));
            std::memcpy(Databuffer.get(), &Defaultvalue, sizeof(uint32_t));
            Buffersize = sizeof(uint32_t);
        }
        
        Debugprint("If you are seeing this, ForumIDcache is not implemented.");
    }
};
struct IPAddresscache : public Cacheblock
{
    virtual void Update() override
    {
        /*
            TODO(Convery);
            Send a request to the server so we can update this.
        */
        
        if(0 == Buffersize)
        {
            uint32_t Defaultvalue = 0;            
            
            Databuffer = std::make_shared<char []>(sizeof(uint32_t));
            std::memcpy(Databuffer.get(), &Defaultvalue, sizeof(uint32_t));
            Buffersize = sizeof(uint32_t);
        }
        
        Debugprint("If you are seeing this, IPAddresscache is not implemented.");
    }
};
struct Usernamecache : public Cacheblock
{
    virtual void Update() override
    {
        /*
            TODO(Convery);
            Send a request to the server so we can update this.
        */
        
        if(0 == Buffersize)
        {
            std::string Defaultvalue = "NotImplemented";

            Databuffer = std::make_shared<char []>(Defaultvalue.size());
            std::memcpy(Databuffer.get(), Defaultvalue.c_str(), Defaultvalue.size());
            Buffersize = Defaultvalue.size();
        }
        
        Debugprint("If you are seeing this, Usernamecache is not implemented.");
    }
};

struct Accountinfoservice
{
    // This service only handles push-updates.
    static void Accountinfo_updateForumID(va_list *Arguments, Bytebuffer *Buffer)
    {
        Cachemanager::Updateblock(FNV1a_Compiletime_32("ForumIDcache"));
    }
    static void Accountinfo_updateIPAddress(va_list *Arguments, Bytebuffer *Buffer)
    {
        Cachemanager::Updateblock(FNV1a_Compiletime_32("IPAddresscache"));
    }
    static void Accountinfo_updateUsername(va_list *Arguments, Bytebuffer *Buffer)
    {
        Cachemanager::Updateblock(FNV1a_Compiletime_32("Usernamecache"));
    }
    
    // Register the handlers and cache.
    Accountinfoservice()
    {
        Messagehandler::Registerhandler(FNV1a_Compiletime_32("Accountinfo_updateForumID"), Accountinfo_updateForumID);
        Messagehandler::Registerhandler(FNV1a_Compiletime_32("Accountinfo_updateIPAddress"), Accountinfo_updateIPAddress);
        Messagehandler::Registerhandler(FNV1a_Compiletime_32("Accountinfo_updateUsername"), Accountinfo_updateUsername);
        
        Cachemanager::Registerblock(new ForumIDcache(), 3600000, FNV1a_Compiletime_32("ForumIDcache"));
        Cachemanager::Registerblock(new IPAddresscache(), 3600000, FNV1a_Compiletime_32("IPAddresscache"));
        Cachemanager::Registerblock(new Usernamecache(), 3600000, FNV1a_Compiletime_32("Usernamecache"));
    }
}

// Call the constructor on startup.
static Accountinfoservice Serviceloader;
