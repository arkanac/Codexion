*This project has been created as part of the 42 curriculum by repichan.*

# Codexion

## Description

Codexion is a concurrency simulation in which several **coders** compete for shared resources called **dongles** while racing against a burnout deadline. It is a variant of the classic Dining Philosophers problem, adapted with cooldowns, a burnout timer, and a configurable scheduling policy.

The coders and the dongles are arranged in a ring: each coder shares one dongle with its left neighbour and one with its right neighbour. A coder needs **two** dongles at once to compile. Its life cycle is:

1. acquire its two dongles,
2. compile,
3. release both dongles (which then enter a cooldown period),
4. debug,
5. refactor,
6. repeat.

Each coder must reach a required number of compilations. If a coder does not start compiling again within `time_to_burnout` milliseconds of its last compilation (or of the start of the simulation), it burns out.

The simulation ends when **either**:

- every coder has reached the required number of compilations (normal completion), **or**
- a coder burns out.

When several coders request the same dongle, access is granted according to the chosen scheduler:

- **FIFO**: the coder that requested first is served first.
- **EDF** (Earliest Deadline First): the coder whose burnout deadline (`last_compile_start + time_to_burnout`) is closest is served first. Ties on the deadline are broken first in favour of the coder that has compiled the fewest times (the one most at risk of never getting a turn), and only then by the smallest coder id.

## Instructions

Compile the project with:

```bash
make
```

The Makefile provides the standard rules `all`, `clean`, `fclean`, `re` and compiles with `-Wall -Wextra -Werror -pthread`.

Run the program with eight arguments:

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
    time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All durations are expressed in **milliseconds**. `scheduler` must be either `fifo` or `edf`.

Example:

```bash
./codexion 4 800 200 100 100 5 50 fifo
```

This runs 4 coders that must each compile 5 times, with a burnout deadline of 800 ms, a compile/debug/refactor cycle of 200/100/100 ms, a dongle cooldown of 50 ms, and FIFO arbitration.

The program prints one timestamped log line per event, where the timestamp is the number of milliseconds elapsed since the start of the simulation:

```text
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
301 1 is refactoring
```

## Blocking cases handled

Concurrency is the heart of this project. The following issues are explicitly addressed.

### Deadlock prevention (Coffman's conditions)

A deadlock requires all four Coffman conditions to hold simultaneously: mutual exclusion, hold-and-wait, no preemption, and circular wait. Codexion breaks the **circular wait** condition through a consistent acquisition order: every coder always acquires the dongle with the **smaller index first**, regardless of whether that dongle is its left or right one. Because no two coders can ever hold their dongles in a mutually reversed order, no cycle of "A waits for B waits for A" can form.

Additionally, in the edge case of a single coder (`number_of_coders == 1`), left and right dongles point to the same resource. Codexion explicitly checks if `left == right` prior to locking to prevent self-deadlocks (attempting to lock a non-recursive mutex twice).

### Starvation prevention

Fair arbitration guarantees that no coder is indefinitely overtaken. When multiple coders contend for the same dongle, a per-dongle priority queue decides who is served next. Under FIFO the queue preserves arrival order; under EDF it keeps the coder with the nearest burnout deadline at the front.

A deadline, however, is not static: a coder that keeps waiting grows more urgent relative to those that have recently compiled. If a waiting coder's position in the queue were frozen at the moment it first registered, a coder could keep being overtaken by neighbours that compile and re-register with fresher deadlines, and eventually burn out despite the parameters being feasible. This exact starvation was observed under long cooldowns and high contention. To prevent it, a waiting coder re-registers itself with its current deadline on every wakeup, so its queue position always reflects its real, up-to-date urgency.

### Cooldown handling

When a dongle is released, it becomes unavailable for `dongle_cooldown` milliseconds. This is stored as a future timestamp (`available_at`) rather than as a blocking sleep. The end of a cooldown is not an action performed by any thread — it is simply the passage of time — so a coder waiting on a free-but-cooling-down dongle cannot rely on a broadcast to be woken.

For this reason, waiting loops utilize bounded timed mechanisms or periodic checks so that a coder waiting on a cooling-down dongle wakes up precisely when the cooldown expires without busy-spinning.

### Precise burnout detection

A dedicated monitor thread loops continuously (with a short sleep between passes) and, for each coder, compares the elapsed time since its last compilation against `time_to_burnout`. Because the loop period is far below the tolerance required by the subject, a burnout is detected and logged well within the window. A coder that has already reached the required number of compilations is excluded from burnout checks: having fulfilled its quota, it can no longer burn out while waiting for the others to finish.

### Log serialization and "burned out" ordering

All log output passes through a single `print_mutex`, so log lines never interleave or get corrupted across threads. The subject also requires the "burned out" line to be the last output. This is guaranteed by checking the global running state before printing or sleeping, ensuring that once a burnout is detected and the running flag is cleared, no coder emits any further log lines.

## Thread synchronization mechanisms

Codexion coordinates its threads exclusively through POSIX threading primitives.

### Threads

One thread runs per coder, plus one dedicated monitor thread. The main thread creates all coder threads and then the monitor, and joins them all once the simulation ends safely using structured thread failsafes.

### `pthread_mutex_t`

Several mutexes protect distinct classes of shared state, kept as fine-grained as possible to reduce contention:

- **One mutex per dongle** — guards that dongle's `owner`, its `available_at` timestamp, and its internal state.
- **One mutex per coder** — guards that coder's `last_compile_start` and `compile_count`, written by the coder itself and read by both the monitor and other coders (for EDF tie-breaks).
- **A print mutex** — serializes all log output.
- **A state and start mutex** — guards the global `is_running` flag and thread synchronization gates (`is_ready`).

### `pthread_cond_t`

Condition variables are used alongside mutexes to coordinate thread startup (`start_cond`) and resource availability states efficiently without burning CPU cycles on busy-waiting.

### Preventing race conditions

The most important cross-thread interaction is between each coder and the monitor. The monitor reads a coder's `last_compile_start` and `compile_count` to detect burnout and completion; the coder writes those same fields during its cycle. Both sides take the **same per-coder mutex** around these accesses to prevent torn reads and data races.

### Thread-safe coder / monitor communication

Coordination of the simulation's end relies on the `is_running` flag. The monitor sets it to false (under the state mutex) when it detects a burnout or when all coders have completed. Coders read the same flag (always under the state mutex) in their main loop, which lets the simulation stop promptly and ensures every thread joins cleanly.

## Resources

Classic references consulted for this project:

- The POSIX threads manual pages: `pthread_create`, `pthread_join`, `pthread_mutex_lock`, `pthread_cond_wait`, `pthread_cond_timedwait`, `pthread_cond_broadcast`.
- The manual pages for `gettimeofday` and `clock_gettime` for high-precision time handling.
- General material on the Dining Philosophers problem and on Coffman's four conditions for deadlock.
- Documentation and articles on condition variables, predicate re-check loops, and priority scheduling policies (FIFO and Earliest Deadline First).

### Use of AI

AI was used to:

- explain underlying concurrency concepts from scratch (threads, mutexes, condition variables, timed waits, and predicate re-check loops);
- review code for potential data races and deadlocks (such as the single-coder mutex lock edge case);
- discuss design trade-offs for arbitration structures (FIFO versus EDF, tie-breaking) and robust error-handling strategies.