*This project has been created as part of the 42 curriculum by dgajowni.*

# Codexion

## Description

Codexion is a project designed to help users understand how multithreading and thread synchronization work.
Coders represent threads that must take turns performing tasks such as compiling, debugging, and refactoring. Mutexes are represented by dongles, which the coders require in order to compile. The program employs FIFO (First-In, First-Out) and EDF (Earliest Deadline First) scheduling. The simulation concludes once all coders have completed the required number of compilations, or if any one of them experiences burnout.

## Instructions

Build the program from the repository root:

```sh
make
```

To rebuild from scratch:

```sh
make re
```

Run the program with all eight required arguments:

```sh
./codexion number_of_coders time_to_burnout time_to_compile \
    time_to_debug time_to_refactor number_of_compiles_required \
    dongle_cooldown fifo
```

The final argument must be `fifo` or `edf`. All time values are in
milliseconds. For example:

```sh
./codexion 5 10000 10 1 1 2 0 fifo
./codexion 5 10000 10 1 1 2 0 edf
```

The program prints timestamped events for dongle acquisition, compiling,
debugging, refactoring, and burnout. `make clean` removes object files;
`make fclean` also removes the executable.

## Blocking cases handled

- **Competing dongle requests:** The arbiter reserves both dongles for one
  coder at a time, preventing two coders from being granted the same dongle.
- **Deadlock prevention:** Dongle mutexes are acquired in ascending dongle ID
  order. The arbiter is released before a coder locks dongle mutexes, and
  dongle mutexes are released before the arbiter is reacquired. This avoids
  circular wait, one of the conditions required for deadlock.
- **Scheduling:** The scheduler chooses by policy among requests whose two
  dongles are both available and out of cooldown. FIFO uses request arrival
  order. EDF uses `last_compile_start + time_to_burnout`, with coder ID as a
  deterministic tie-breaker. A request waiting for an unavailable dongle
  does not reserve its other dongle and block a ready request.
- **Dongle cooldown:** Released dongles remain unavailable until their
  cooldown interval has elapsed. Timed condition-variable waits allow the
  scheduler to reconsider requests when a cooldown expires.
- **Burnout detection:** A dedicated monitor checks coder deadlines and stops
  the simulation when a coder burns out.
- **Serialized logging:** A shared output mutex protects event messages so
  concurrent threads do not interleave writes to the log.
- **Single-coder case:** The left and right dongle references may identify the
  same dongle. The implementation avoids acquiring the same dongle mutex
  twice.

## Thread synchronization mechanisms

- `pthread_mutex_t` locks protect the arbiter state, each dongle while it is
  being used, and output logging. Request-queue state, dongle availability,
  cooldown timestamps, coder compile counts, and the monitor's stop state are
  accessed while holding the arbiter mutex.
- `pthread_cond_t` lets coder threads wait for a dongle grant or for a
  cooldown deadline. The monitor broadcasts on shutdown so waiting coders can
  wake and exit.
- No separate custom event mechanism is used; condition variables and
  mutex-protected state communicate grants, cooldown expiry, and shutdown.
- A binary heap stores pending requests and applies FIFO or EDF ordering. The
  scheduler examines ready requests while holding the arbiter mutex and
  reserves both dongles before granting a request.
- The monitor reads coder progress under the arbiter mutex. Coder threads
  update their last compile start and completed-compile count under the same
  mutex, providing synchronized communication between the monitor and coders.
- The output mutex is separate from the arbiter mutex. Each event is printed
  while holding the output mutex, which prevents interleaved log messages.

## Resources

- "Talking" with Copilot
- The project specification: [`en.subject.pdf`](./en.subject.pdf)

### AI usage

AI support was used to quickly review the implementation, explain the
project rules, and help validate the concurrency logic against the subject.
It was also used to cross-check scheduler behavior, burnout detection, and
thread-safety assumptions before running the final verification tests.
