/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-7-22
    Notes:
        An alternative interface for communication.
*/

#include "Messagehandler.h"
#include <LibUV\uv.h>
#include <algorithm>
#include <memory>
#include <thread>

// Platform dependent pipe-name.
#ifdef _WIN32
constexpr const char *Pipename = "\\\\.\\pipe\\ayria_client";
#else
constexpr const char *Pipename = "ayria_client";
#endif

// Windows.h annoyance.
#undef min
#undef max

namespace Messagehandler
{
    uv_pipe_t Pipe;
    uv_loop_t Eventloop;

    // UV callbacks.
    void Bufferallocator(uv_handle_t *Handle, size_t Size, uv_buf_t *Buffer)
    {
        *Buffer = uv_buf_init(std::make_unique<char []>(std::min(Size, size_t(2048))).get(), std::min(Size, size_t(2048)));
    }
    void onRead(uv_stream_t *Stream, ssize_t Readcount, const uv_buf_t *Buffer)
    {
        /*
            TODO(Convery):
            Implement a steam buffer and read into it.
        */
    }
    void onConnection(uv_stream_t *Handle, int Status)
    {
        if (Status)
        {
            /*
                TODO(Convery):
                A non-null status is an error and should be handled.
            */
        }

        // Create a new pipe handle for this connection.
        uv_pipe_t Localpipe;
        uv_pipe_init(&Eventloop, &Localpipe, 0);

        // Accept the connection and start reading.
        int Errorcode = 0;
        if ((Errorcode = uv_accept(Handle, (uv_stream_t *)&Localpipe)))
        {
            DebugPrint(va("%s error: %s", __func__, uv_err_name(Errorcode)));
            uv_close((uv_handle_t *)&Localpipe, NULL);
            return;
        }
        if ((Errorcode = uv_read_start((uv_stream_t *)&Pipe, Bufferallocator, onRead)))
        {
            DebugPrint(va("%s error: %s", __func__, uv_err_name(Errorcode)));
            uv_close((uv_handle_t *)&Localpipe, NULL);
            return;
        }
    }

    // Create a pipe for alternative communication.
    void Createpipeinterface()
    {
        // Initialize the UV structs.
        uv_loop_init(&Eventloop);
        uv_pipe_init(&Eventloop, &Pipe, 0);

        // Process data in UV every 30 ms.
        auto Lambda = [](uv_loop_t *pEventloop) { while (0 == uv_run(pEventloop, UV_RUN_DEFAULT)) std::this_thread::sleep_for(std::chrono::milliseconds(30)); };
        std::thread(Lambda, &Eventloop).detach();

        // Bind the pipe and listen on it.
        int Errorcode = 0;
        if ((Errorcode = uv_pipe_bind(&Pipe, Pipename)))
        {
            DebugPrint(va("%s error: %s", __func__, uv_err_name(Errorcode)));
            return;
        }
        if ((Errorcode = uv_listen((uv_stream_t*)&Pipe, 128, onConnection)))
        {
            DebugPrint(va("%s error: %s", __func__, uv_err_name(Errorcode)));
            return;
        }
    }
    void Senddataonpipe(std::string &Databuffer) {};
    void Senddataonpipe(uint32_t Length, const void *Databuffer) {};
};
