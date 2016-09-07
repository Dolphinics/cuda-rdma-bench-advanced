#include <string>
#include <stdexcept>
#include <signal.h>
#include <vector>
#include "interrupt.h"
#include "segment.h"
#include "benchmark.h"
#include "log.h"


static bool keepRunning = true;


static void stopServer(int)
{
    keepRunning = false;
}


int runBenchmarkServer(SegmentList& segments, Callback interruptHandler)
{
    std::vector<InterruptPtr> interrupts;
    try
    {
        for (SegmentPtr segment: segments)
        {
            // Export segments on all adapters
            for (uint adapter: segment->adapters)
            {
                // Create interrupt
                InterruptPtr interrupt(new Interrupt(segment->id, adapter, interruptHandler));
                interrupts.push_back(interrupt);

                // Set available on adapter
                Log::debug("Exporting segment %u on adapter %u...", segment->id, adapter);
                segment->setAvailable(adapter);
            }
        }
    } 
    catch (const std::string& error)
    {
        Log::error("%s", error.c_str());
    }
    catch (const std::runtime_error& error)
    {
        Log::error("Unexpected error caused server to abort: %s", error.what());
        return 2;
    }

    // Catch ctrl + c from terminal
    signal(SIGTERM, (sig_t) stopServer);
    signal(SIGINT, (sig_t) stopServer);

    // Run server
    Log::info("Running server...");
    while (keepRunning);

    // Stop server
    Log::info("Shutting down server...");
    for (SegmentPtr segment: segments)
    {
        for (uint adapter: segment->adapters)
        {
            segment->setUnavailable(adapter);
        }
    }


    return 0;
}
