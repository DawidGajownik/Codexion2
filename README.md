*This project has been created as part of the 42 curriculum by dgajowni.*

# Codexion

## Description

Codexion is a C concurrency simulation built with POSIX threads. Coder threads
share a circular set of USB dongles and compete to compile before their burnout
deadlines. Each coder repeatedly compiles, debugs, and refactors. The program
supports FIFO and EDF arbitration for dongle requests, dongle cooldown periods,
and a monitor thread that stops the simulation when a coder burns out or every
coder has completed the required number of compiles.

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
- **Starvation prevention:** Requests are ordered by the selected policy.
  FIFO uses request arrival order. EDF uses
  `last_compile_start + time_to_burnout`, with coder ID as a deterministic
  tie-breaker.
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

- POSIX threads and synchronization: [pthreads(7)](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- POSIX mutex functions: [pthread_mutex_lock(3)](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- POSIX condition variables: [pthread_cond_wait(3)](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html)
- Valgrind user manual: [Valgrind documentation](https://valgrind.org/docs/manual/manual.html)
- The project specification: [`en.subject.pdf`](./en.subject.pdf)

### AI usage

AI assistance was used to review the implementation against the project
specification, investigate concurrency diagnostics from Helgrind and memory
diagnostics from Memcheck, and prepare this documentation. The implementation
and test results should be verified by the project author.
