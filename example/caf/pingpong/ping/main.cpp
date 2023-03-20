/*
* Created by boil on 2023/3/26.
*/
#include <defines.h>
#include <ping.h>
#include "config.h"

void caf_main(caf::actor_system& system, const config& cfg) {
  run_client(system, cfg);
}

CAF_MAIN(caf::id_block::pingpong_example, caf::io::middleman)
