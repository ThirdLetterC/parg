# Security Model

`parg` is a small C23 command-line argument parser. It parses caller-supplied
`argc`/`argv` data, matches short and long options, and can reorder `argv`
entries in place. It does not perform file I/O, network I/O, dynamic
allocation, privilege separation, sandboxing, or secret management.

Everything passed into the public API should be treated as untrusted input.
That includes option strings, long-option tables, `argv` contents, and any
storage reachable through caller-provided pointers.

## Trust Boundaries

- `argc`, `argv`, `optstring`, and `struct parg_option` arrays are
  caller-controlled.
- Every argument string is parsed as raw NUL-terminated bytes. The library does
  not validate encoding, normalize Unicode, or apply shell-style escaping.
- `parg_reorder()` mutates the caller's `argv` array in place.
- `parg_getopt_long()` writes through `longopts[i].flag` when that pointer is
  non-null. Those destinations are part of the caller's trusted state.
- `struct parg_state` is caller-owned mutable state. Sharing one parser state
  across threads without external synchronization is outside the supported
  model.

## Protected Properties

- Memory safety inside the parser when the caller honors the documented pointer
  and lifetime requirements.
- Deterministic option parsing for short options, long options, optional
  arguments, and GNU-style non-option handling.
- Rejection of unknown or ambiguous long options by returning `'?'`.
- Reporting of missing required arguments via `'?'` or `':'`, depending on the
  leading character of `optstring`.

## Defensive Posture In This Codebase

- The implementation is small and self-contained in
  `include/parg/parg.h` and `src/parg.c`.
- No heap allocation occurs in the library, which removes allocator misuse and
  ownership-transfer bugs from the core parser.
- No global mutable state is used. Parser progress lives in `struct parg_state`
  provided by the caller.
- Parsing uses bounded string primitives such as `strchr()`, `strcspn()`, and
  `strncmp()` rather than ad hoc pointer walks.
- Public entry points assert that required pointers such as `ps`, `argv`, and
  `optstring` are not null.
- Public parsing entry points reject negative `argc` values and caller-mutated
  parser states whose `optind` falls outside `[1, argc]`, avoiding out-of-bounds
  `argv` indexing even when a caller corrupts `struct parg_state`.
- The build is configured for `-std=c23 -Wall -Wextra -Wpedantic -Werror`,
  `-fstack-protector-strong`, and `-fno-omit-frame-pointer` in both
  `build.zig` and `compile_flags.txt`.
- Zig debug builds enable `sanitize_c = .full`, which provides C sanitizer
  coverage during debug test runs.
- Regression tests cover ambiguous and unknown long options, missing required
  arguments, extraneous long-option arguments, `flag` writes, option reordering,
  and invalid parser-state rejection.

## Security-Relevant Limits And Caller Responsibilities

- This library only parses command-line argument vectors. It does not sanitize
  application-specific values such as paths, numbers, URLs, or shell fragments
  carried inside those arguments.
- All input strings must remain valid and NUL-terminated for the duration of
  parsing. Passing dangling pointers, unterminated byte sequences, or invalid
  `argv` layouts is undefined at the C level.
- `struct parg_state` must be initialized with `parg_init()` before first use.
- `parg_reorder()` requires a writable `argv` array. Do not call it on
  immutable storage or shared vectors that other code expects to remain in the
  original order.
- `longopts` must be terminated by an entry whose `name` is `nullptr`.
- Any non-null `longopts[i].flag` must point to writable storage of type `int`.
- The parser is byte-oriented and locale-agnostic. If your security model cares
  about canonicalization, homoglyphs, or Unicode normalization, enforce that in
  the application before acting on parsed values.
- The library is not a policy engine. Authorization, command whitelisting,
  filesystem safety checks, and environment hardening belong in the caller.

## Verification

Validation paths present in this checkout:

- `zig build` builds the static library and example program.
- `zig build test` builds and runs the parser regression tests in
  `tests/parg_tests.c`.
- `just test` delegates to `zig build test`.
- Debug builds use Zig's C sanitizer support; release-style builds keep the
  warning policy but disable sanitizers by default.
- The README and `examples/simple.c` intentionally use bounded integer parsing
  via `strtol()` rather than `atoi()`.

## Supported Versions

This repository does not currently publish a separate maintained-branches
matrix. Report security issues against the current `main` branch unless the
maintainer states otherwise.

## Reporting

This repository does not currently publish a dedicated private security contact
or coordinated disclosure workflow in-tree. If you need to report a security
issue, use an existing maintainer contact channel if you already have one and
avoid posting exploit details publicly before the maintainer has had a chance
to assess the report.
