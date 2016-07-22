/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: MIT
    Started: 2016-7-22
    Notes:
        A simple system for logging strings to files.
*/

#pragma once

// The logfile gets stored in ./Plugins/Logs with your EXTENSIONNAME.
void AppendToLogfile(const char *Message, const char *Prefix);
void DeleteLogfile();
