# Smart Pointer System (C++)

A **high-performance, production-grade reimplementation** of C++ smart pointers (`unique_ptr`, `shared_ptr`, `weak_ptr`) with a strong focus on:

* Memory management internals
* Ownership semantics
* Thread safety & atomics
* Cache-aware performance design
* Allocator-aware abstractions

---

# Overview

This project recreates the core functionality of:

* `std::unique_ptr`
* `std::shared_ptr`
* `std::weak_ptr`

and extends them with:

* Custom control block design
* `make_shared` (single-allocation optimization)
* `allocate_shared` (allocator-aware construction)
* Benchmarking suite for performance analysis

---

# Features

## `unique_ptr`

* Exclusive ownership (move-only)
* Zero-overhead abstraction (compiles close to raw pointers)
* Custom deleter support (EBO optimization)

---

## `shared_ptr`

* Reference-counted ownership
* Thread-safe via `std::atomic`
* Custom control block (separate + in-place variants)
* Efficient destruction and memory reclamation

---

## `weak_ptr`

* Non-owning reference
* Safe promotion via `lock()`
* Handles object expiration correctly
* Lock-free CAS-based promotion

---

## Advanced Features

* `make_shared` (single allocation)
* `allocate_shared` (allocator-aware)
* Custom allocator integration
* Multithreaded correctness
* Benchmarking suite

---

# Design

## Control Block Architecture

```
shared_ptr ─────┐
                ├── control_block ─── object
weak_ptr  ──────┘
```

### Control Block Stores:

* `strong_count` (atomic)
* `weak_count` (atomic)
* deleter
* allocator (for allocate_shared)

---

## Allocation Strategies

### Standard (`shared_ptr(new T)`)

```
[ control block ]    [ object ]
```

* 2 allocations
* worse cache locality

---

### Optimized (`make_shared`)

```
[ control block | object ]
```

* 1 allocation
* better cache locality
* fewer malloc calls

---

# Thread Safety

Reference counting uses:

```cpp
fetch_add(1, std::memory_order_relaxed)
fetch_sub(1, std::memory_order_acq_rel)
```

### Why?

* `relaxed` → fast increments
* `acq_rel` → safe destruction ordering

---

# Benchmark Results

## Setup

* N = 1,000,000 operations
* Compiled in Release mode
* Apple Silicon (Clang)

---

# shared_ptr Benchmarks

## Allocation (Key Performance Metric)

| Method                  | Time (ns/op) |
| ----------------------- | ------------ |
| std::shared_ptr (new)   | 100.37       |
| std::make_shared        | 96.86        |
| custom shared_ptr (new) | 85.51        |
| **custom make_shared**  | **55.95** |
| custom allocate_shared  | 79.35        |

### Insight

* `make_shared` reduces allocation overhead by ~35–40%
* Single-allocation design significantly improves cache locality

---

## Copy (Atomic Overhead)

| Method            | Time (ns/op) |
| ----------------- | ------------ |
| std::shared_ptr   | 11.68        |
| custom shared_ptr | 14.71        |

### Insight

* Slight overhead due to less aggressive stdlib optimizations
* Within acceptable range for atomic operations

---

## Move (Ownership Transfer)

| Method                | Time (ns/op) |
| --------------------- | ------------ |
| std::shared_ptr       | 11.96        |
| **custom shared_ptr** | **8.38**  |

### Insight

* Move is essentially pointer transfer → minimal overhead
* Custom implementation is leaner

---

## Reset (Destroy + Allocate)

| Method            | Time (ns/op) |
| ----------------- | ------------ |
| std::shared_ptr   | 74.29        |
| custom shared_ptr | 78.70        |

---

## Multithreaded Contention

| Method            | Time (ns/op) |
| ----------------- | ------------ |
| std::shared_ptr   | 92.39        |
| custom shared_ptr | 111.79       |

### Insight

* Slower due to cache line contention
* Can be improved with cache-line padding

---

# unique_ptr Benchmarks

## Allocation

| Method                | Time (μs)    |
| --------------------- | ------------ |
| raw new/delete        | 42668        |
| std::unique_ptr       | 33795        |
| **custom unique_ptr** | **30201** |

---

## Move

| Method            | Time (μs) |
| ----------------- | --------- |
| raw pointer       | 1421      |
| std::unique_ptr   | 8921      |
| custom unique_ptr | 8973      |

---

## Reset

| Method            | Time (μs) |
| ----------------- | --------- |
| raw pointer       | 22615     |
| std::unique_ptr   | 23019     |
| custom unique_ptr | 25020     |

---

# weak_ptr Benchmarks

## lock() (Success)

| Method          | Time (ns/op) |
| --------------- | ------------ |
| std::weak_ptr   | 30.31        |
| custom weak_ptr | 29.97        |

---

## lock() (Failure)

| Method          | Time (ns/op) |
| --------------- | ------------ |
| std::weak_ptr   | 9.97         |
| custom weak_ptr | 11.86        |

---

## Construction

| Method          | Time (ns/op) |
| --------------- | ------------ |
| std::weak_ptr   | 12.07        |
| custom weak_ptr | 22.51        |

---

## Multithreaded

| Method          | Time (ns/op) |
| --------------- | ------------ |
| std::weak_ptr   | 244.7        |
| custom weak_ptr | 509.9        |

### Insight

* Higher overhead due to atomic contention + CAS loop
* Correct but not yet fully optimized

---

# Key Learnings

### Performance Wins

* `make_shared` significantly reduces allocation cost
* Move operations are highly efficient
* Memory layout strongly impacts performance

---

### Tradeoffs

* Atomic operations introduce unavoidable overhead
* Multithreaded contention affects scalability
* Allocator abstraction adds flexibility but slight cost

---

# Engineering Insights

This project demonstrates:

* Separation of allocation vs construction
* Lock-free reference counting
* Cache-aware data structure design
* Memory ordering in concurrent systems
* Allocator-aware programming

---

# Project Structure

```
include/
  unique_ptr.hpp
  shared_ptr.hpp
  weak_ptr.hpp
  control_block.hpp
  make_shared.hpp
  allocate_shared.hpp

benchmarks/
tests/
```

---

# Testing

* Unit tests for all pointer types
* Stress tests (including multithreaded scenarios)
* Verified with sanitizers (ASAN/UBSAN)

---

# Future Improvements

* Cache-line padding for control block (reduce contention)
* Custom memory pool allocator
* Intrusive reference counting variant
* Polymorphic allocators (`std::pmr`)
* Debug instrumentation mode

---

# Conclusion

This project explores the **core mechanics behind C++ smart pointers**, going beyond usage into:

* design
* correctness
* performance engineering

It reflects the kind of low-level reasoning used in:

* libc++
* libstdc++

---

# Author

Arjun Varma
Computer Science @ University of Leeds

