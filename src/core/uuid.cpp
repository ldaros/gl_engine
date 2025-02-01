#include "uuid.h"

#include <atomic>
#include <random>
#include <chrono>
#include "assert.h"

namespace Engine {

static std::atomic<UUID> counter;

void UUID_init() 
{
    std::random_device rd;
    std::mt19937_64 rng(rd());
    auto time_seed = static_cast<UUID>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
    );
    rng.seed(rd() ^ time_seed);
    counter.store(rng(), std::memory_order_relaxed);
}

UUID UUID_generate() 
{
    ASSERT(counter.load(std::memory_order_relaxed) != 0, "UUID counter not initialized");
    return counter.fetch_add(1, std::memory_order_relaxed);
}

}