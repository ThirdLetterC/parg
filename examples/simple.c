#include <errno.h>
#include <limits.h>
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

static const char *find_long_name(const struct parg_option *longopts, int val) {
  for (int i = 0; longopts[i].name != nullptr; ++i) {
    if (longopts[i].flag == nullptr && longopts[i].val == val) {
      return longopts[i].name;
    }
  }

  return nullptr;
}

static int parse_nonnegative_int(const char *text, int *out) {
  char *end = nullptr;
  long value;

  errno = 0;
  value = strtol(text, &end, 10);

  if (errno != 0 || end == text || *end != '\0' || value < 0 || value > INT_MAX) {
    return -1;
  }

  *out = (int)value;
  return 0;
}

int main(int argc, char **argv) {
  struct parg_state ps;
  parg_init(&ps);

  const struct parg_option longopts[] = {
      {"help", PARG_NOARG, nullptr, 'h'},
      {"verbose", PARG_NOARG, nullptr, 'v'},
      {"output", PARG_REQARG, nullptr, 'o'},
      {"size", PARG_OPTARG, nullptr, 's'},
      {nullptr, PARG_NOARG, nullptr, 0},
  };

  int verbose = 0;
  const char *output = nullptr;
  int size = 0;

  int opt;
  while ((opt = parg_getopt_long(&ps, argc, argv, ":hvo:s::", longopts, nullptr)) != -1) {
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
      if (ps.optarg != nullptr) {
        if (parse_nonnegative_int(ps.optarg, &size) != 0) {
          fprintf(stderr, "Invalid size value: %s\n", ps.optarg);
          return 1;
        }
      } else {
        size = 1;
      }
      break;
    case '?':
      if (ps.optopt != 0) {
        fprintf(stderr, "Unknown option: -%c\n", ps.optopt);
      } else {
        const char *token = ps.optind > 0 ? argv[ps.optind - 1] : "(unknown)";
        fprintf(stderr, "Unknown or ambiguous option: %s\n", token);
      }
      return 1;
    case ':':
      {
        const char *long_name = find_long_name(longopts, ps.optopt);
        if (long_name != nullptr) {
          fprintf(stderr, "Missing value for option: --%s\n", long_name);
        } else {
          fprintf(stderr, "Missing value for option: -%c\n", ps.optopt);
        }
      }
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
