// Copyright Yahoo. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "my_shared_library.h"
#include <vespa/vespalib/util/signalhandler.h>

// This tiny library exists solely as a way to ensure we get visible function names in our backtrace,
// as that is not necessarily the case for statically linked functions.

// Could have used a single std::barrier<no op functor> here, but when using explicit
// phase latches it sort of feels like the semantics are more immediately obvious.
void my_cool_function(std::latch& arrival_latch, std::latch& departure_latch) {
    arrival_latch.arrive_and_wait();
    // Twiddle thumbs in departure latch until main test thread has dumped our stack
    departure_latch.arrive_and_wait();
}

vespalib::string my_totally_tubular_and_groovy_function() {
    return vespalib::SignalHandler::get_cross_thread_stack_trace(pthread_self());
}
