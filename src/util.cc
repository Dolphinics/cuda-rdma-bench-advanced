#include <cstdio>
#include <cstdarg>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <ctime>
#include "util.h"


/* Known SISCI error codes */
static unsigned error_codes[] = {
    SCI_ERR_OK,
    SCI_ERR_BUSY,
    SCI_ERR_FLAG_NOT_IMPLEMENTED,
    SCI_ERR_ILLEGAL_FLAG,
    SCI_ERR_NOSPC,
    SCI_ERR_API_NOSPC,
    SCI_ERR_HW_NOSPC,
    SCI_ERR_NOT_IMPLEMENTED,
    SCI_ERR_ILLEGAL_ADAPTERNO,
    SCI_ERR_NO_SUCH_ADAPTERNO,
    SCI_ERR_TIMEOUT,
    SCI_ERR_OUT_OF_RANGE,
    SCI_ERR_NO_SUCH_SEGMENT,
    SCI_ERR_ILLEGAL_NODEID,
    SCI_ERR_CONNECTION_REFUSED,
    SCI_ERR_SEGMENT_NOT_CONNECTED,
    SCI_ERR_SIZE_ALIGNMENT,
    SCI_ERR_OFFSET_ALIGNMENT,
    SCI_ERR_ILLEGAL_PARAMETER,
    SCI_ERR_MAX_ENTRIES,
    SCI_ERR_SEGMENT_NOT_PREPARED,
    SCI_ERR_ILLEGAL_ADDRESS,
    SCI_ERR_ILLEGAL_OPERATION,
    SCI_ERR_ILLEGAL_QUERY,
    SCI_ERR_SEGMENTID_USED,
    SCI_ERR_SYSTEM,
    SCI_ERR_CANCELLED,
    SCI_ERR_NOT_CONNECTED,
    SCI_ERR_NOT_AVAILABLE,
    SCI_ERR_INCONSISTENT_VERSIONS,
    SCI_ERR_COND_INT_RACE_PROBLEM,
    SCI_ERR_OVERFLOW,
    SCI_ERR_NOT_INITIALIZED,
    SCI_ERR_ACCESS,
    SCI_ERR_NOT_SUPPORTED,
    SCI_ERR_DEPRECATED,
    SCI_ERR_NO_SUCH_NODEID,
    SCI_ERR_NODE_NOT_RESPONDING,
    SCI_ERR_NO_REMOTE_LINK_ACCESS,
    SCI_ERR_NO_LINK_ACCESS,
    SCI_ERR_TRANSFER_FAILED,
    SCI_ERR_EWOULD_BLOCK,
    SCI_ERR_SEMAPHORE_COUNT_EXCEEDED,
    SCI_ERR_IRQL_ILLEGAL,
    SCI_ERR_REMOTE_BUSY,
    SCI_ERR_LOCAL_BUSY,
    SCI_ERR_ALL_BUSY
};


/* Corresponding error strings */
static const char* error_strings[] = {
    "OK",
    "Resource busy",
    "Flag option is not implemented",
    "Illegal flag option",
    "Out of local resources",
    "Out of local API resources",
    "Out of hardware resources",
    "Not implemented",
    "Illegal adapter number",
    "Adapter not found",
    "Operation timed out",
    "Out of range",
    "Segment ID not found",
    "Illegal node ID",
    "Connection to remote node is refused",
    "No connection to segment",
    "Size is not aligned",
    "Offset is not aligned",
    "Illegal function parameter",
    "Maximum possible physical mapping is exceeded",
    "Segment is not prepared",
    "Illegal address",
    "Illegal operation",
    "Illegal query operation",
    "Segment ID already used",
    "Could not get requested resource from the system",
    "Operation cancelled",
    "Host is not connected to remote host",
    "Operation not available",
    "Inconsistent driver version",
    "Out of local resources",
    "Host not initialized",
    "No local or remote access for requested operation",
    "Request not supported",
    "Function deprecated",
    "Node ID not found",
    "Node does not respond",
    "Remote link is not operational",
    "Local link is not operational",
    "Transfer failed",
    "Illegal interrupt line",
    "Remote host is busy",
    "Local host is busy",
    "System is busy"
};


/* Default log file */
static FILE* logFile = stderr;


/* Default log level */
static uint logLevel = 0;


/* Initial time of first log entry */
static uint64_t logStart = 0;


static inline size_t scierridx(sci_error_t code)
{
    const size_t num = sizeof(error_codes) / sizeof(error_codes[0]);

    size_t idx;

    for (idx = 0; idx < num; ++idx)
    {
        if (error_codes[idx] == code)
        {
            return idx;
        }
    }

    return idx;
}


const char* scierrstr(sci_error_t code)
{
    const size_t idx = scierridx(code);

    if (idx < sizeof(error_codes) / sizeof(error_codes[0]))
    {
        return error_strings[idx];
    }

    return "Undocumented error";
}


uint64_t current_usecs()
{
    timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) < 0)
    {
        throw std::runtime_error("Failed to get realtime clock");
    }

    return ts.tv_sec * 1e6 + ts.tv_nsec / 1e3;
}


void initLog(FILE* file, uint level)
{
    logFile = file;
    logLevel = level;
    logStart = current_usecs();
}


static inline void report(FILE* file, const char* prefix, const char* format, va_list arguments)
{
    uint64_t ts = 0;
    if (file != stderr)
    {
        try
        {
            ts = current_usecs() - logStart;
        }
        catch (...)
        {
            ts = 0;
        }
    }

    char buffer[1024];
    size_t length = vsnprintf(buffer, sizeof(buffer), format, arguments);

    if (file != stderr)
    {
        fprintf(file, "[%10lu] <%c> ", ts, *prefix);
    }
    else
    {
        fprintf(file, "%-5s: ", prefix);
    }

    fwrite(buffer, length, 1, file);
    fwrite("\n", 1, 1, file);
    fflush(file);
}


void error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    report(logFile, "ERROR", format, args);
    va_end(args);
}


void warn(const char* format, ...)
{
    if (logLevel >= 1)
    {
        va_list args;
        va_start(args, format);
        report(logFile, "WARN", format, args);
        va_end(args);
    }
}


void info(const char* format, ...)
{
    if (logLevel >= 2)
    {
        va_list args;
        va_start(args, format);
        report(logFile, "INFO", format, args);
        va_end(args);
    }
}


void debug(const char* format, ...)
{
    if (logLevel >= 3)
    {
        va_list args;
        va_start(args, format);
        report(logFile, "DEBUG", format, args);
        va_end(args);
    }
}
