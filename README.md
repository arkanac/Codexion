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

- **FIFO**: requests are served in arrival order.
- **EDF** (Earliest Deadline First): the coder whose burnout deadline
  (`last_compile_start + time_to_burnout`) is closest is served first.

A deterministic tie-breaker rule (detailed below) guarantees a fully
deterministic policy even in the rare case of exactly equal keys.

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
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
400 1 is refactoring
```

## Scheduling: a priority queue (binary heap) per dongle

Fair arbitration is built on a **priority queue implemented as a binary
heap**.

Each dongle owns its own heap. Because a dongle in the ring is shared by
exactly two coders (its two neighbours), that heap has a capacity of two: it
holds the ids of the coders that are **currently requesting** this dongle. The
heap is maintained with the usual operations, written generically over array
indices (parent `(i-1)/2`, children `2i+1` / `2i+2`):

- `heap_push` inserts a requesting coder and restores the heap property with
  `sift_up`;
- `heap_pop` extracts the root (the highest-priority coder) and restores the
  heap property with `sift_down`;
- `heap_remove` removes a specific coder (used when a coder gives up because
  the simulation has stopped);
- `heap_peek` returns the root without modifying the heap.

The ordering is decided by a **comparator** that is evaluated against the
coders' live values, so priority is never frozen:

- **FIFO**: earlier `request_time` wins; on an exact tie, the smaller id wins.
- **EDF**: the earlier burnout deadline wins; on equal deadlines, the coder
  with fewer compilations so far wins; on a further tie, the smaller id wins.

The final tie-break on id makes the policy **fully deterministic**, as the
subject requires: two coders can never be left in an unresolved tie. A coder
is allowed to take a dongle only when it is at the **root** of that dongle's
heap — i.e. it is the highest-priority requester.

Requesting is expressed purely by heap membership: a coder that begins its
acquisition phase pushes itself onto both of its dongles' heaps, and is popped
(on success) or removed (on give-up). This is what lets a coder ignore a
neighbour that is not actually competing (absent from the heap), so
non-adjacent coders can compile in parallel.

## Acquisition model

A coder never holds one dongle while blocking on the other. It polls: on each
attempt it locks both of the dongles it needs, checks — atomically, under those
locks — whether it is the root of **both** heaps and whether both dongles are
free and out of cooldown, and only then takes them both at once, popping itself
from both heaps. If any condition fails it takes nothing, releases the locks,
sleeps briefly, and retries.

This "all-or-nothing" acquisition is the backbone of the design: because a
coder never keeps a dongle it cannot yet use, the hold-and-wait condition
simply does not arise, and neither does the busy hand-off (livelock) that a
naive take-one-then-release scheme produces.

## Blocking cases handled

Concurrency is the heart of this project. The following issues are explicitly
addressed.

### Deadlock prevention (Coffman's conditions)

A deadlock requires all four Coffman conditions to hold simultaneously:
mutual exclusion, hold-and-wait, no preemption, and circular wait. Codexion
breaks two of them.

**Hold-and-wait** is removed by the atomic acquisition described above: a coder
acquires its two dongles together or not at all, so it never sits on one
resource while waiting for another.

**Circular wait** is broken at the mutex level. To perform its atomic check a
coder must briefly lock both of its dongles' mutexes. These are always locked
in a globally consistent order — the dongle with the **smaller index first** —
regardless of which is the coder's left or right dongle. Because no two coders
can ever lock their shared mutexes in a mutually reversed order, no cycle can
form.

### The single-coder degenerate case

With a single coder the ring collapses: the coder's left and right dongle are
the same one, so it can never assemble the two distinct dongles a compilation
requires. This case is detected up front — the coder never completes an
acquisition and is reported as burned out — so the simulation terminates
cleanly instead of self-deadlocking on a doubly-locked mutex.

### Starvation prevention (liveness)

The subject requires liveness: no coder may be starved and burn out under EDF
when the parameters are feasible. Two things guarantee it.

First, because a coder re-enters the heaps on every acquisition cycle and the
comparator reads **live** deadlines, priority is never frozen: a coder that
keeps waiting grows more urgent as its deadline approaches and rises toward the
root before it can burn out.

Second, the heap only ever contains coders that are **genuinely requesting**
the dongle. A coder that is compiling, or that has finished its work, is not in
the heap and therefore blocks no one. This is what prevents a "phantom
priority" where a non-competing neighbour would reserve a dongle it cannot use.

### Synchronized start

All coder threads wait on a start barrier (a condition variable guarded by a
dedicated mutex) until the main thread has created every thread and releases
them together. Starting from a common instant, with a small deterministic
stagger between coders, avoids the artificial start-up collisions that occur
when every coder wakes and grabs for the same dongles at once. The simulation
clock (`start_time`) is taken at the moment the barrier is released, so every
timestamp is measured from the true start.

### Cooldown handling

When a dongle is released, it becomes unavailable for `dongle_cooldown`
milliseconds. This is stored as a future timestamp (`available_at`) rather than
as a blocking sleep, and applies to the dongle for everyone, as the subject
requires. A coder considers a dongle takeable only once the current time has
passed its `available_at`; the polling loop naturally re-checks this on each
attempt, so no thread needs to be woken when a cooldown simply expires.

### Precise burnout detection

A dedicated monitor thread loops continuously (with a short sleep between
passes) and, for each coder, compares the elapsed time since its last
compilation against `time_to_burnout`. Because the loop period is far below
the 10 ms tolerance required by the subject, a burnout is detected and logged
well within that window. A coder that has already reached the required number
of compilations is excluded from burnout checks: having fulfilled its quota, it
can no longer burn out while waiting for the others to finish.

### Log serialization and "burned out" ordering

All log output passes through a single `print_mutex`, so log lines never
interleave or get corrupted across threads. The subject also requires the
"burned out" line to be the last output. The internal wait loops and each timed
step re-check the running state, so once the monitor detects a burnout and
clears the running flag, no coder emits any further log line.

## Thread synchronization mechanisms

Codexion coordinates its threads exclusively through POSIX threading
primitives.

### Threads

One thread runs per coder, plus one dedicated monitor thread. The main thread
creates all coder threads and the monitor, releases the start barrier, and
joins them all once the simulation ends.

### `pthread_mutex_t`

Several classes of mutex protect distinct shared state, kept as fine-grained as
possible to allow real parallelism between non-adjacent coders:

- **One mutex per dongle** — guards that dongle's `owner`, its `available_at`
  timestamp, and its **heap**. A single mutex covers all of a dongle's mutable
  state, which keeps the locking rules simple and removes any lock-order issue
  between two locks of the same dongle. Locking the two needed dongles in index
  order is what makes the two-dongle acquisition both atomic and deadlock-free.
- **One mutex per coder** — guards that coder's `last_compile_start`,
  `compile_count` and `request_time`, which the coder writes and which the
  monitor reads.
- **A print mutex** — serializes all log output.
- **A state mutex** — guards the global `is_running` flag.
- **A start mutex** — paired with the start-barrier condition variable.

The governing rule is that any shared, mutable field is accessed — for both
reading and writing — under its dedicated mutex.

### `pthread_cond_t`

A single condition variable is used, for the **start barrier**: coder threads
wait on it until the main thread sets the ready flag and broadcasts, so the
simulation begins from a common instant. Dongle acquisition itself does not use
condition variables — it is a bounded polling loop — which sidesteps the
lost-wakeup and spurious-wakeup pitfalls that a condition-variable-per-dongle
scheme carries around cooldown expiry.

### Preventing race conditions

The most important cross-thread interaction is between each coder and the
monitor: the monitor reads a coder's `last_compile_start` and `compile_count`
to detect burnout and completion, while the coder writes those same fields
during its cycle. Both sides take the same per-coder mutex around these
accesses. Every access to a dongle's heap (push, pop, peek, remove) is likewise
performed under that dongle's mutex, so the two coders that share a dongle can
never modify its heap concurrently. The final implementation reports no data
races and no lock-order violations under a thread analyzer.

### Thread-safe coder / monitor communication

Coordination of the simulation's end relies on the `is_running` flag. The
monitor sets it to false (under the state mutex) when it detects a burnout or
when all coders have completed. Coders read the same flag (always under the
state mutex) in their main loop and inside their polling loops, so the whole
simulation stops promptly and every thread can be joined without blocking.

## Resources

- The POSIX threads manual pages: `pthread_create`, `pthread_join`,
  `pthread_mutex_lock`, `pthread_cond_wait`, `pthread_cond_broadcast`.
- The manual page for `gettimeofday` for time handling.
- General material on the Dining Philosophers problem and on Coffman's four
  conditions for deadlock.
- The essential Code Vault playlist on threads, which taught me everything i needed on the subject :
    https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2
- The excellent Codexion Vizualer who helped me see the wrongness of my code :
    https://codexion-visualizer.sacha-dev.me/
- My left and right neighbors

### Use of AI

AI was used to:

- explain the underlying concurrency concepts (threads, mutexes, the start
  barrier, atomic multi-resource acquisition, the binary-heap priority queue,
  and the reasoning behind polling versus condition variables);
- review code 
