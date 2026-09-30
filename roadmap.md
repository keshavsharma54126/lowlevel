
# Systems / Low-Level Engineering Project Roadmap --- Resource Edition

I kept the original 12-project progression and added a focused resource
stack for each project. The idea is **learn → implement → inspect a real
implementation → extend your project**.

------------------------------------------------------------------------

## 🖥️ Category 1: Systems Programming & Operating Systems

### =\> Project 1: A Custom Command-Line Shell (Mini-Bash)

**The Project:** Build a terminal application that prints a prompt,
accepts user command inputs, executes the corresponding system programs,
and handles background processes.

**Concepts You Learn:** - Process lifecycle management: `fork()`,
`exec*()`, `waitpid()` - File descriptors and I/O redirection: `dup2()`,
`open()`, `close()` - Piping: `pipe()` - Foreground/background jobs and
signals - Process groups and terminal control as an advanced extension

**Real-Life Application:** These are the same process and
file-descriptor primitives used underneath Unix shells and many
process-oriented server tools.

**Best Resources:** 1. **OSTEP --- Operating Systems: Three Easy
Pieces**\
Read the CPU virtualization/processes, concurrency, and persistence
sections first.\
https://pages.cs.wisc.edu/\~remzi/OSTEP/ 2. **Harvard CS61 --- Shell
Exercises**\
Extremely relevant for implementing `fork`, `exec`, `pipe`, `dup2`,
`waitpid`, and pipe hygiene.\
https://cs61.seas.harvard.edu/site/2020/ShellEx/ 3. **MIT xv6**\
Read the process, file descriptor, system call, and scheduling
implementations after your first shell works.\
https://pdos.csail.mit.edu/6.828/ 4. **Linux man-pages**\
Use `fork(2)`, `execve(2)`, `pipe(2)`, `dup2(2)`, `waitpid(2)`,
`open(2)`, `sigaction(2)`, and `setpgid(2)` as your API reference.\
https://man7.org/linux/man-pages/ 5. **K&R --- The C Programming
Language**\
Particularly useful for pointers, arrays, strings, structs, and
Unix-oriented C.

**Implementation milestones:**
`command execution → arguments → redirection → pipelines → sequential commands → background jobs → signals → process groups/job control`

------------------------------------------------------------------------

### =\> Project 2: A User-Space Thread Scheduler (Fibers/Green Threads)

**The Project:** Write a runtime library where lightweight tasks
explicitly yield execution control back and forth without relying on the
OS scheduler.

**Concepts You Learn:** - Context switching - Stack creation and
management - Saving/restoring registers - Cooperative multitasking -
Calling conventions / ABI - Scheduler data structures - Later extension:
preemption using timers/signals

**Best Resources:** 1. **MIT 6.828/6.1810 --- Multithreading Lab**\
This is one of the closest educational exercises to the project. It
explicitly asks you to implement user-level thread switching and
save/restore registers.\
https://pdos.csail.mit.edu/6.828/2021/labs/thread.html 2. **OSTEP ---
Concurrency**\
Read threads, locks, scheduling, and condition variables.\
https://pages.cs.wisc.edu/\~remzi/OSTEP/ 3. **MIT xv6 --- Scheduling +
threads source**\
Excellent reference for understanding the difference between user-level
and kernel-level scheduling.\
https://pdos.csail.mit.edu/6.828/ 4. **x86-64 calling
convention/register guide**\
Learn which registers are caller/callee saved and how function calls use
the stack.\
https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf 5.
**Advanced:** read the source of Go's runtime scheduler after your
implementation works. Don't copy it; use it to compare design decisions.

**Implementation milestones:**
`thread struct → individual stacks → context struct → switch assembly → yield() → scheduler loop → sleep/wakeup → optional preemption`

------------------------------------------------------------------------

## 🌐 Category 2: Networking & Distributed Systems

### =\> Project 3: An HTTP/1.1 Web Server Built from Raw Sockets

**The Project:** Open a raw network socket, wait for incoming traffic,
manually parse HTTP, locate a file on disk, and write a formatted
response.

**Concepts You Learn:** - `socket()`, `bind()`, `listen()`, `accept()` -
TCP byte streams - Partial reads/writes - HTTP request parsing - HTTP
response serialization - Keep-alive connections - Concurrency with
threads/processes/event loops

**Best Resources:** 1. **Beej's Guide to Network Programming**\
Probably the best hands-on starting point for C socket programming.\
https://beej.us/guide/bgnet/ 2. **Beej's Guide to Networking Concepts**\
Use this before or alongside Beej's socket guide to understand TCP/IP
and protocols.\
https://beej.us/guide/bgnet0/ 3. **RFC 9112 --- HTTP/1.1**\
Use the RFC as the authoritative protocol reference once your basic
server works.\
https://www.rfc-editor.org/rfc/rfc9112.html 4. **OSTEP --- Persistence /
concurrency sections**\
Useful when you turn the single-client server into a concurrent server.\
https://pages.cs.wisc.edu/\~remzi/OSTEP/ 5. **Nginx source code ---
advanced comparison**\
After your server works, inspect how a production server structures
event handling and connection state.\
https://github.com/nginx/nginx

**Implementation milestones:**
`TCP server → request line → headers → GET → static files → correct status codes → keep-alive → concurrent clients → HTTP edge cases`

------------------------------------------------------------------------

### =\> Project 4: A Distributed Key-Value Store with Data Replication

**The Project:** Create a simple in-memory database running across three
nodes that replicate changes and maintain a consistent state.

**Important scope correction:** Start with **leader-based replication /
Raft**, rather than trying to invent general distributed consensus from
scratch.

**Concepts You Learn:** - Replicated state machines - Leader election -
Log replication - Terms/epochs - Quorum - Network partitions - Crash
recovery - State-machine application

**Best Resources:** 1. **Raft --- official site**\
https://raft.github.io/ 2. **Raft paper --- In Search of an
Understandable Consensus Algorithm**\
Read this before implementing consensus.\
https://raft.github.io/raft.pdf 3. **MIT 6.5840 Distributed Systems**\
Especially the Raft lab and later replicated-service work.\
https://pdos.csail.mit.edu/6.824/ 4. **Designing Data-Intensive
Applications --- Martin Kleppmann**\
Best companion book for understanding replication, partitioning,
consistency, logs, and distributed-system tradeoffs. 5. **Jepsen**\
Use it later to learn how distributed systems are actually tested
against faults and consistency problems.\
https://jepsen.io/

**Implementation milestones:**
`3-node transport → leader election → replicated log → commit index → state machine → persistence → restart recovery → partitions → client retries`

------------------------------------------------------------------------

## 🗄️ Category 3: Database Internals

### =\> Project 5: A Disk-Backed Log-Structured Merge (LSM) Storage Engine

**The Project:** Build a database engine that persists key-value records
to disk using a structured format and efficient indexing.

**Concepts You Learn:** - Binary serialization - File offsets - WAL -
Memtables - SSTables - Sorted indexes - Compaction - Checksums - Crash
recovery - Bloom filters - Read/write/space amplification

**Best Resources:** 1. **RocksDB Overview and Architecture**\
Study the relationship between memtables, WALs, SST files, and
compaction.\
https://github.com/facebook/rocksdb/wiki/RocksDB-Overview 2. **RocksDB
MemTable documentation**\
https://github.com/facebook/rocksdb/wiki/MemTable 3. **RocksDB SST/LSM
documentation**\
https://github.com/facebook/rocksdb/wiki/ 4. **Database Internals ---
Alex Petrov**\
One of the best books for storage engines, B-trees, LSM trees, WALs,
replication, and database architecture. 5. **SQLite File Format**\
Even though SQLite is B-tree based rather than an LSM engine, its
file-format documentation is excellent for learning real-world page
layouts, offsets, journals, and crash recovery.\
https://sqlite.org/fileformat.html 6. **SQLite Architecture**\
https://sqlite.org/arch.html

**Implementation milestones:**
`append-only log → binary record format → recovery scan → memtable → SSTable → sparse index → WAL → compaction → Bloom filter → checksums → crash testing`

------------------------------------------------------------------------

## 🛠️ Category 4: Software Engineering & Tooling Architecture

### =\> Project 6: A Mini-Git Version Control CLI Tool

**The Project:** Build a basic CLI that tracks project directories,
hashes content, stores snapshots, and maintains history.

**Concepts You Learn:** - Content-addressable storage - SHA-1/SHA-256
hashing - Blobs, trees, commits - Directed acyclic graphs - References -
Object storage - Diffs - Three-way merge

**Best Resources:** 1. **Pro Git --- Git Internals**\
Start with Git objects, references, packfiles, and Git's
content-addressable model.\
https://git-scm.com/book/en/v2/Git-Internals-Git-Objects.html 2. **Pro
Git --- entire book**\
https://git-scm.com/book/en/v2 3. **Git source code**\
Read it only after you have your simplified implementation working.\
https://github.com/git/git 4. **Git User Manual / Technical
Documentation**\
https://git-scm.com/docs 5. **Git from the Bottom Up**\
Useful for developing a mental model of the plumbing commands and object
database.\
https://jwiegley.github.io/git-from-the-bottom-up/

**Implementation milestones:**
`hash-object → blob storage → tree objects → commits → refs/HEAD → log → checkout → diff → branches → merge → packfiles`

**Important:** Don't begin by implementing Git's entire feature set.
Build a tiny content-addressed object database first.

------------------------------------------------------------------------

### =\> Project 7: A Recursive-Descent JSON Parser

**The Project:** Parse raw JSON into native data structures while
validating syntax.

**Concepts You Learn:** - Lexing/tokenization - Recursive descent -
Grammar design - AST/data representation - Error reporting - String
escaping - Numeric parsing

**Best Resources:** 1. **RFC 8259 --- JSON**\
Use the specification as the source of truth.\
https://www.rfc-editor.org/rfc/rfc8259.html 2. **Crafting Interpreters
--- scanning + parsing chapters**\
https://craftinginterpreters.com/contents.html 3. **JSONTestSuite**\
Use this to test valid/invalid JSON and edge cases.\
https://github.com/nst/JSONTestSuite 4. **Crafting a Compiler ---
recursive descent concepts**\
Use the parsing material in Crafting Interpreters before moving to a
full language. 5. **Your own parser tests**\
Build tests for nesting, escapes, Unicode, numbers, malformed input, and
whitespace.

**Implementation milestones:**
`cursor → whitespace → strings → numbers → literals → arrays → objects → recursive nesting → errors → test suite`

------------------------------------------------------------------------

## 🤖 Category 5: Compilers, Interpreters & Simulation

### =\> Project 8: A Tree-Walking Programming Language Interpreter

**The Project:** Invent a small programming language and implement
scanning, parsing, evaluation, variables, control flow, functions, and
closures.

**Concepts You Learn:** - Formal grammars - Lexing - Parsing - ASTs -
Evaluation - Environments - Lexical scope - Closures - Runtime errors

**Best Resources:** 1. **Crafting Interpreters --- Robert Nystrom**\
This should be your primary resource. The tree-walk interpreter section
goes through scanning, parsing, evaluation, state, control flow,
functions, binding, and classes.\
https://craftinginterpreters.com/ 2. **Crafting Interpreters ---
Tree-Walk Interpreter**\
https://craftinginterpreters.com/a-tree-walk-interpreter.html 3.
**Essentials of Programming Languages --- Friedman & Wand**\
Use later for deeper language-semantics understanding. 4. **Programming
Language Pragmatics --- Scott**\
Useful once you want to understand how language implementation choices
affect semantics and runtime design. 5. **LLVM Kaleidoscope tutorial ---
advanced next step**\
https://llvm.org/docs/tutorial/

**Implementation milestones:**
`lexer → parser → AST → evaluator → variables → blocks → if/while → functions → closures → classes → resolver`

------------------------------------------------------------------------

### =\> Project 9: A CHIP-8 CPU Emulator (Virtual Machine)

**The Project:** Build a software simulation of a small virtual CPU with
memory, registers, instruction decoding, timers, input, and display.

**Concepts You Learn:** - Fetch/decode/execute - Virtual memory -
Registers - Instruction encoding - Bitmasks/shifts - Timers -
Input/output - Deterministic emulation

**Best Resources:** 1. **CHIP-8 technical references**\
https://github.com/mattmikolay/chip-8/wiki/CHIP-8-Technical-Reference 2.
**Cowgod's CHIP-8 Technical Reference**\
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM 3. **Timendus CHIP-8
Test Suite**\
Use this to validate opcode behavior, flags, quirks, keypad, timers, and
other compatibility issues.\
https://github.com/Timendus/chip8-test-suite 4. **Corax+ opcode test
ROM**\
https://github.com/corax89/chip8-test-rom 5. **Computer Organization and
Design --- Patterson & Hennessy**\
Use for deeper CPU/memory architecture after the emulator works.

**Implementation milestones:**
`memory → registers → fetch/decode → opcode dispatcher → stack → timers → display → keypad → ROM loading → compatibility tests`

------------------------------------------------------------------------

## 🧠 Category 6: Advanced Algorithms & Computer Graphics

### =\> Project 10: A Terminal-Based Roguelike Game (Procedural Maps)

**The Project:** Create a dungeon-crawling game using a grid, procedural
map generation, and pathfinding.

**Concepts You Learn:** - Graph representation - BFS/DFS - Dijkstra -
A\* - Procedural generation - Cellular automata - Binary space
partitioning - Priority queues - Game-state design

**Best Resources:** 1. **Red Blob Games --- A\* Introduction**\
Excellent visual explanation of BFS, Dijkstra, greedy search, and A\*.\
https://www.redblobgames.com/pathfinding/a-star/introduction.html 2.
**Red Blob Games --- A\* Implementation**\
https://www.redblobgames.com/pathfinding/a-star/implementation.html 3.
**Roguelike Dev Tutorial**\
https://rogueliketutorials.com/ 4. **Procedural Dungeon Generation
resources**\
Experiment with BSP, cellular automata, drunkard's walk, and
room/corridor generation. 5. **Game Programming Patterns**\
https://gameprogrammingpatterns.com/

**Implementation milestones:**
`grid → player movement → random rooms → corridors → connectivity validation → enemies → BFS/Dijkstra → A* → procedural difficulty`

------------------------------------------------------------------------

### =\> Project 11: A Custom 3D Raycasting Engine (Wolfenstein Style)

**The Project:** Map a 2D grid into a pseudo-3D first-person view by
casting rays from the player's camera.

**Concepts You Learn:** - Vectors and trigonometry - DDA grid
traversal - Camera plane - Perspective correction - Texture mapping -
Frame buffers - Input/game loops - Performance optimization

**Best Resources:** 1. **Lode's Computer Graphics Tutorial ---
Raycasting**\
One of the best implementation-oriented explanations, including DDA,
camera vectors, wall height, and source code.\
https://lodev.org/cgtutor/raycasting.html 2. **Permadi --- Ray-Casting
Tutorial**\
A classic explanation of the mathematics and rendering pipeline.\
https://permadi.com/1996/05/ray-casting-tutorial-table-of-contents/ 3.
**Permadi Raycasting Source/Demos**\
https://permadi.com/2019/03/ray-casting-source-code-and-demos/ 4.
**LearnOpenGL --- Coordinate Systems**\
Useful when you later move from a software raycaster toward actual 3D
graphics.\
https://learnopengl.com/Getting-Started/Coordinate-Systems 5. **Computer
Graphics: Principles and Practice**\
Use this for deeper graphics mathematics rather than as your first
implementation guide.

**Implementation milestones:**
`2D map → player/camera → one ray → DDA → wall distance → vertical wall slice → full screen → movement → collision → textures → floor/ceiling → sprites`

------------------------------------------------------------------------

## 🔒 Category 7: Cybersecurity & Cryptography

### =\> Project 12: A Password Manager with Single-Key Master Encryption

**The Project:** Build a secure console password vault whose encrypted
contents can only be opened with a master passphrase.

**Important security rule:** Treat this as a **learning project**, not a
vault for real passwords until it has undergone serious security review.
Do not invent cryptographic primitives or protocols.

**Concepts You Learn:** - Password-based key derivation - Argon2id -
Salts - AEAD encryption - Nonces - Authentication tags - Secure random
generation - Key separation - Memory handling - File-format/version
design

**Best Resources:** 1. **OWASP Password Storage Cheat Sheet**\
Use it for modern password hashing/KDF guidance. OWASP currently
recommends Argon2id for password storage and explains why fast hashes
such as SHA-256 are unsuitable for password hashing.\
https://cheatsheetseries.owasp.org/cheatsheets/Password_Storage_Cheat_Sheet.html
2. **Libsodium Password Hashing**\
Excellent practical reference for Argon2id and password-derived keys.\
https://doc.libsodium.org/password_hashing/default_phf 3. **Libsodium
AEAD documentation**\
Learn authenticated encryption rather than implementing AES/ChaCha20
yourself.\
https://libsodium.gitbook.io/doc/secret-key_cryptography/aead 4.
**Libsodium XChaCha20-Poly1305**\
Particularly useful for a learning vault because it gives authenticated
encryption with a large nonce and a high-level API.\
https://libsodium.gitbook.io/doc/secret-key_cryptography/aead/chacha20-poly1305/xchacha20-poly1305_construction
5. **Cryptopals Crypto Challenges**\
Excellent for learning cryptography by attacking broken constructions.\
https://cryptopals.com/ 6. **Serious Cryptography --- Jean-Philippe
Aumasson**\
Good conceptual book for understanding modern cryptographic primitives.

**Implementation milestones:**
`randomness → password KDF → key derivation → AEAD file format → encryption/decryption → authentication failure handling → atomic writes → file permissions → memory hygiene → versioned format`

**Do NOT do this:** - Don't use SHA-256 directly as the password-to-key
conversion. - Don't invent your own encryption algorithm. - Don't reuse
nonces incorrectly. - Don't store the master password. - Don't silently
accept authentication failures. - Don't use the project to store real
credentials until the design has been professionally reviewed.

------------------------------------------------------------------------

# Recommended Order

If the goal is to become genuinely strong at low-level/backend/system
design rather than simply collect projects, I would build them in this
order:

1.  **Mini Shell**
2.  **User-Space Threads**
3.  **Raw-Socket HTTP Server**
4.  **CHIP-8 Emulator**
5.  **JSON Parser**
6.  **Mini-Git**
7.  **LSM Storage Engine**
8.  **Tree-Walking Interpreter**
9.  **Roguelike + A**\*
10. **Raycasting Engine**
11. **Distributed KV Store + Raft**
12. **Password Manager**

The ordering deliberately moves from **OS primitives → concurrency →
networking → binary/data representation → storage → language runtimes →
algorithms/graphics → distributed systems → security**.

# How to Use the Resources

For each project:

1.  **Read only enough theory to understand the next implementation
    step.**
2.  **Build the smallest working version.**
3.  **Write tests before adding major features.**
4.  **Read a real implementation after your own version works.**
5.  **Use the real implementation to discover better designs, not to
    copy code.**
6.  **Add one serious failure-mode test.**
7.  **Document the architecture and tradeoffs in the repository.**

The most valuable outcome is not the final code. It is being able to
explain, from first principles, **why every major component exists and
what breaks when you remove it.**

