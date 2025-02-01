#pragma once

#include <stdint.h>

namespace Engine {

typedef uint64_t UUID;

void UUID_init();
UUID UUID_generate();

}