*This project has been created as part of the 42 curriculum by repichan.*

# Codexion

## Description

Codexion is a concurrency simulation in which several **coders** compete for
shared resources called **dongles** while racing against a burnout deadline.
It is a variant of the classic Dining Philosophers problem, adapted with
cooldowns, a burnout timer, and a configurable scheduling policy.

The coders and the dongles are arranged in a ring: each coder shares one
dongle with its left neighbour and one with its right neighbour. A coder
needs **two** dongles at once to compile. Its life cycle is:

1. acquire its two dongles,
2. compile,
3. release both dongles (which then enter a cooldown period),
4. debug,
5. refactor,
6. repeat.

Each coder must reach a required number of compilations. If a coder does not
start compiling again within `time_to_burnout` milliseconds of its last
compilation (or of the start of the simulation), it burns out.

The simulation ends when **either**:

- every coder has reached the required number of compilations (normal
  completion), **or**
- a coder burns out.

When several coders request the same dongle, access is granted according to
the chosen scheduler:

- **FIFO**: the coder that requested first is served first.
- **EDF** (Earliest Deadline First): the coder whose burnout deadline
  (`last_compile_start + time_to_burnout`) is closest is served first, with
  ties broken by the smallest coder id.

## Instructions

Compile the project with:

```
make
```

The Makefile provides the standard rules `all`, `clean`, `fclean`, `re` and
compiles with `-Wall -Wextra -Werror -pthread`.

Run the program with eight arguments:

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
    time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All durations are expressed in **milliseconds**. `scheduler` must be either
`fifo` or `edf`.

Example:

```
./codexion 4 800 200 100 100 5 50 fifo
```

This runs 4 coders that must each compile 5 times, with a burnout deadline of
800 ms, a compile/debug/refactor cycle of 200/100/100 ms, a dongle cooldown of
50 ms, and FIFO arbitration.

The program prints one timestamped log line per event, where the timestamp is
the number of milliseconds elapsed since the start of the simulation:

```
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
301 1 is refactoring
```

## Blocking cases handled

Concurrency is the heart of this project. The following issues are explicitly
addressed.

### Deadlock prevention (Coffman's conditions)

A deadlock requires all four Coffman conditions to hold simultaneously:
mutual exclusion, hold-and-wait, no preemption, and circular wait. Codexion
breaks the **circular wait** condition through a consistent acquisition
order: every coder always acquires the dongle with the **smaller index
first**, regardless of whether that dongle is its left or right one. Because
no two coders can ever hold their dongles in a mutually reversed order, no
cycle of "A waits for B waits for A" can form.

A second, subtler deadlock is also avoided. Because a coder needs two
dongles, it registers in a dongle's waiting queue **only just before
attempting to take that specific dongle**, not both queues in advance. If a
coder registered for its second dongle while still waiting for its first, it
could reserve (by priority) a dongle it is not yet actively requesting,
producing a mixed possession/priority cycle. Registering one dongle at a time
eliminates this case.

### Starvation prevention

Fair arbitration guarantees that no coder is indefinitely overtaken. When
multiple coders contend for the same dongle, a per-dongle priority queue
decides who is served next. Under FIFO the queue preserves arrival order;
under EDF it keeps the coder with the nearest burnout deadline at the front,
breaking ties by the smallest id. With feasible parameters, every coder
eventually acquires its dongles and reaches the required number of
compilations, as verified through testing (including odd coder counts and
high-contention scenarios).

### Cooldown handling

When a dongle is released, it becomes unavailable for `dongle_cooldown`
milliseconds. This is stored as a future timestamp (`available_at`) rather
than as a blocking sleep. A coder waiting on a dongle that is free but still
cooling down cannot be woken by any other thread, because the end of a
cooldown is not an action performed by any thread — it is simply the passage
of time. This case is therefore handled with `pthread_cond_timedwait`, which
wakes the waiting coder automatically at the cooldown's expiry, without
depending on a broadcast. A coder waiting on a dongle that is *held* by
someone else uses a plain `pthread_cond_wait`, since it is guaranteed to be
woken by the broadcast issued when that dongle is dropped.

### Precise burnout detection

A dedicated monitor thread loops continuously (with a short sleep between
passes) and, for each coder, compares the elapsed time since its last
compilation against `time_to_burnout`. Because the loop period is far below
the 10 ms tolerance required by the subject, a burnout is detected and
logged well within that window. A coder that has already reached the required
number of compilations is excluded from burnout checks: having fulfilled its
quota, it can no longer burn out while waiting for the others to finish.

### Log serialization and "burned out" ordering

All log output passes through a single `print_mutex`, so log lines never
interleave or get corrupted across threads. The subject also requires the
"burned out" line to be the last output. This is guaranteed in two ways:
the internal wait loops re-check the running state so a coder does not sleep
uselessly after the simulation has stopped, and after each timed step a coder
verifies the running state before logging the next step. As a result, once
the monitor logs a burnout and clears the running flag, no coder emits any
further log line.

## Thread synchronization mechanisms

Codexion coordinates its threads exclusively through POSIX threading
primitives.

### Threads

One thread runs per coder, plus one dedicated monitor thread. The main thread
creates all coder threads and then the monitor, and joins them all once the
simulation ends.

### `pthread_mutex_t`

Several mutexes protect distinct classes of shared state, kept as
fine-grained as possible to reduce contention:

- **One mutex per dongle** — guards that dongle's `owner`, its `available_at`
  timestamp, and its waiting queue.
- **One mutex per coder** — guards that coder's `last_compile_start` and
  `compile_count`, which are written by the coder itself and read by the
  monitor.
- **A print mutex** — serializes all log output.
- **A state mutex** — guards the global `is_running` flag.

The governing rule is that any shared, mutable field is always accessed —
for both reading and writing — while holding its dedicated mutex.

### `pthread_cond_t`

One condition variable per dongle lets waiting coders sleep instead of
busy-waiting. A coder blocked on a dongle waits on that dongle's condition
variable; `drop_dongle` broadcasts on it to wake all waiters, which then
re-evaluate their condition. As explained above, the two waiting reasons use
different primitives: `pthread_cond_wait` when a held dongle will be released
by a future broadcast, and `pthread_cond_timedwait` when only the passage of
time (the cooldown) can unblock the coder.

### Preventing race conditions

The most important cross-thread interaction is between each coder and the
monitor. The monitor reads a coder's `last_compile_start` and `compile_count`
to detect burnout and completion; the coder writes those same fields during
its cycle. Both sides take the **same per-coder mutex** around these
accesses. Without this discipline, the monitor could read a half-written
value (a torn read of a multi-byte field) and trigger a spurious burnout or
miss a real one.

### Thread-safe coder / monitor communication

Coordination of the simulation's end relies on the `is_running` flag. The
monitor sets it to false (under the state mutex) when it detects a burnout or
when all coders have completed, then broadcasts on every dongle's condition
variable so that any sleeping coder wakes up, re-checks the flag, and exits
its waiting loop cleanly. Coders read the same flag (always under the state
mutex) in their main loop and inside their waiting loops, which lets the
whole simulation stop promptly and lets every thread be joined without
blocking.

## Resources

Classic references consulted for this project:

- The POSIX threads manual pages: `pthread_create`, `pthread_join`,
  `pthread_mutex_lock`, `pthread_cond_wait`, `pthread_cond_timedwait`,
  `pthread_cond_broadcast`.
- The manual pages for `gettimeofday` and `clock_gettime` for time handling.
- General material on the Dining Philosophers problem and on Coffman's four
  conditions for deadlock.
- Documentation and articles on condition variables, the necessity of the
  predicate re-check loop (spurious wakeups and lost-wakeup avoidance), and
  priority scheduling policies (FIFO and Earliest Deadline First).

### Use of AI

An AI assistant was used throughout this project as a tutor and code
reviewer. Specifically, it was used to:

- explain the underlying concurrency concepts from scratch (threads,
  mutexes, condition variables, timed waits, and the reasoning behind the
  predicate re-check loop);
- review code that had already been written by hand, and point out bugs
  rather than provide implementations — notably a lost-wakeup on the cooldown
  path, and a self-deadlock caused by locking an already-held mutex
- discuss design trade-offs for the arbitration structure (per-dongle waiting
  queue, FIFO versus EDF, tie-breaking) and for the error-handling strategy

