# parg

A small C library for parsing command-line arguments. It supports short options, long options, optional arguments, and GNU-style handling of non-option arguments.

**Build**
- Build the static library and example with Zig: `zig build`
- Run parser regression tests: `zig build test`
- Run the example: `zig build run -- --help`
- Artifacts are placed under `zig-out/`

**Usage**
- Add `include/parg/parg.h` and `src/parg.c` to your build.
- Compile `src/parg.c` as C (the Zig build uses `-std=c23`).

**Example**
```c
#include <stdio.h>
#include <stdlib.h>

#include "parg/parg.h"

int main(int argc, char **argv) {
  struct parg_state ps;
  parg_init(&ps);

  const struct parg_option longopts[] = {
      {"help", PARG_NOARG, nullptr, 'h'},
      {"output", PARG_REQARG, nullptr, 'o'},
      {"size", PARG_OPTARG, nullptr, 's'},
      {nullptr, PARG_NOARG, nullptr, 0},
  };

  int opt;
  while ((opt = parg_getopt_long(&ps, argc, argv, ":ho:s::", longopts, nullptr)) != -1) {
    if (opt == 1) {
      printf("arg: %s\n", ps.optarg);
      continue;
    }

    switch (opt) {
    case 'h':
      printf("help\n");
      return 0;
    case 'o':
      printf("output=%s\n", ps.optarg);
      break;
    case 's':
      printf("size=%d\n", ps.optarg ? atoi(ps.optarg) : 1);
      break;
    case '?':
      if (ps.optopt) {
        fprintf(stderr, "Unknown option: -%c\n", ps.optopt);
      } else {
        fprintf(stderr, "Unknown or ambiguous option: %s\n", argv[ps.optind - 1]);
      }
      return 1;
    case ':':
      fprintf(stderr, "Missing value for option: -%c\n", ps.optopt);
      return 1;
    }
  }

  return 0;
}
```

**API**

- `parg_init(struct parg_state *ps)` initializes parser state.
- `parg_getopt(...)` parses short options.
- `parg_getopt_long(...)` parses short and long options.
- `parg_reorder(...)` reorders argv so options come first.

**License**

- MIT-0. See `include/parg/parg.h` or `src/parg.c` for details.
