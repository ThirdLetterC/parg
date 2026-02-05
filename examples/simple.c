#include <stdio.h>
#include <stdlib.h>

#include "parg/parg.h"

static void print_usage(const char *exe) {
  printf("Usage: %s [options] [args...]\n", exe);
  printf("\n");
  printf("Options:\n");
  printf("  -h, --help            Show this help message\n");
  printf("  -v, --verbose         Increase verbosity (repeatable)\n");
  printf("  -o, --output FILE     Output file\n");
  printf("  -s, --size[=N]        Optional size (default 1 if set)\n");
}

int main(int argc, char **argv) {
  struct parg_state ps;
  parg_init(&ps);

  const struct parg_option longopts[] = {
      {"help", PARG_NOARG, NULL, 'h'},
      {"verbose", PARG_NOARG, NULL, 'v'},
      {"output", PARG_REQARG, NULL, 'o'},
      {"size", PARG_OPTARG, NULL, 's'},
      {NULL, PARG_NOARG, NULL, 0},
  };

  int verbose = 0;
  const char *output = NULL;
  int size = 0;

  int opt;
  while ((opt = parg_getopt_long(&ps, argc, argv, "hvo:s::", longopts, NULL)) != -1) {
    if (opt == 1) {
      printf("arg: %s\n", ps.optarg);
      continue;
    }

    switch (opt) {
    case 'h':
      print_usage(argv[0]);
      return 0;
    case 'v':
      ++verbose;
      break;
    case 'o':
      output = ps.optarg;
      break;
    case 's':
      size = ps.optarg ? atoi(ps.optarg) : 1;
      break;
    case '?':
      fprintf(stderr, "Unknown option: %c\n", ps.optopt);
      return 1;
    case ':':
      fprintf(stderr, "Missing value for option: %c\n", ps.optopt);
      return 1;
    default:
      fprintf(stderr, "Unexpected parse result: %d\n", opt);
      return 1;
    }
  }

  printf("verbose=%d\n", verbose);
  printf("output=%s\n", output ? output : "(none)");
  printf("size=%d\n", size);

  return 0;
}
