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
        *Buffer = uv_buf_init(std::make_unique<char []>(std::min(Size, size_t(MAX_MESSAGE_SIZE))).get(), std::min(Size, size_t(MAX_MESSAGE_SIZE)));
    }
    void onRead(uv_stream_t *Stream, ssize_t Readcount, const uv_buf_t *Buffer)
    {
        void *pBuffer = Buffer->base;
        uint32_t Length = uint32_t(Readcount);
        bool Recursive;

        // Error checking.
        if (Readcount < 0)
        {
            if(Readcount != UV_EOF)
                DebugPrint(va("%s error: %s", __func__, uv_err_name(Readcount)));
            return;
        }

        // Read the stream.
        static Streambuffer Streamreader;
        Recursive = Streamreader.Read(&Length, pBuffer);

        // Handle the message if available.
        if (Streamreader.Full)
        {
            Bytebuffer Messagebuffer(Streamreader.Internallength, Streamreader.Internalbuffer.get());
            uint32_t RequestID = Messagebuffer.Read<uint32_t>();
            Handlemessage(RequestID, &Messagebuffer);
        }

        // Call this function again if there's still data in the stream.
        if (Recursive)
        {
            uv_buf_t Newbuffer = uv_buf_init((char *)pBuffer, Length);
            onRead(Stream, Length, &Newbuffer);
        }
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

    void Writecleanup(uv_write_t* Request, int Status)
    {
        delete Request;
    }
    void Senddataonpipe(std::string &Databuffer) 
    {
        Senddataonpipe(Databuffer.size(), Databuffer.data());
    };
    void Senddataonpipe(uint32_t Length, const void *Databuffer) 
    {
        // Format the data.
        Streambuffer Streamwriter;
        Streamwriter.Write(Length, Databuffer);

        // Create a new buffer for the message and the request.
        uv_buf_t Buffer = uv_buf_init(std::make_unique<char[]>(Streamwriter.Internallength).get(), Streamwriter.Internallength);
        std::memcpy(Buffer.base, Streamwriter.Internalbuffer.get(), Streamwriter.Internallength);
        uv_write_t *Request = new uv_write_t();
        Request->data = Buffer.base;
        
        // Send the request out on the pipe.
        if (uv_write(Request, (uv_stream_t *)&Pipe, &Buffer, 1, Writecleanup))
        {
            /*
                TODO(Convery):
                Error handling, it could be useful to have.
            */

            delete Request;
        }
    };
};
