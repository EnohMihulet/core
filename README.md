# c_core

`c_core` is a small C header-only utility library.

## Contents

- `DataStructures/dynarray.h`: generic dynamic array.
- `DataStructures/queue.h`: generic FIFO queue.
- `DataStructures/stack.h`: generic stack.
- `DataStructures/ringbuffer.h`: generic ring buffer / deque-style container.
- `DataStructures/priorityqueue.h`: generic priority queue.
- `DataStructures/bitset.h`: generic dynamic bitset.
- `DataStructures/hashmap.h`: generic hash map.
- `Memory/arena.h`: linear arena allocator.
- `Memory/mempool.h`: fixed-block allocator / object pool.
- `Math/bitmath.h`: integer bit manipulation helpers.
- `Strings/string_builder.h`: growable string builder.

## Usage

Most data structures are generated with a macro using a prefix, item type, and initial capacity.

```c
#define CORE_DYNARRAY_IMPLEMENTATION
#include "DataStructures/dynarray.h"

DYNARRAY_DEFINE(int, int, 8)
```

Define the corresponding `CORE_*_IMPLEMENTATION` macro in exactly one C or C++ translation unit before including the header. Include the header without that macro everywhere else declarations are needed.

The containers are intended for plain old data / trivially copyable types.

## Tests

Tests for all documented headers live under `Tests`. Run the complete suite or pass one or more test files to run a subset.

```sh
Tests/run.sh
Tests/run.sh Tests/test_dynarray.c
```

## License

MIT License. See `LICENSE`.
