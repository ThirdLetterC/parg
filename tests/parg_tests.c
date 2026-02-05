#include <stdio.h>
#include <string.h>

#include "parg/parg.h"

#define ASSERT_EQ_INT(actual, expected)                                         \
  do {                                                                          \
    if ((actual) != (expected)) {                                               \
      fprintf(stderr,                                                            \
              "assert failed: %s == %s (actual=%d expected=%d) at %s:%d\n",     \
              #actual, #expected, (actual), (expected), __FILE__, __LINE__);   \
      return 1;                                                                  \
    }                                                                            \
  } while (0)

#define ASSERT_EQ_STR(actual, expected)                                         \
  do {                                                                          \
    if (strcmp((actual), (expected)) != 0) {                                    \
      fprintf(stderr,                                                            \
              "assert failed: %s == %s (actual=%s expected=%s) at %s:%d\n",     \
              #actual, #expected, (actual), (expected), __FILE__, __LINE__);   \
      return 1;                                                                  \
    }                                                                            \
  } while (0)

static int test_unknown_long_sets_optopt_zero(void) {
  char arg0[] = "prog";
  char arg1[] = "--unknown";
  char *argv[] = {arg0, arg1, NULL};
  struct parg_state ps;
  const struct parg_option longopts[] = {
      {"verbose", PARG_NOARG, NULL, 'v'},
      {NULL, PARG_NOARG, NULL, 0},
  };

  parg_init(&ps);
  ASSERT_EQ_INT(parg_getopt_long(&ps, 2, argv, ":v", longopts, NULL), '?');
  ASSERT_EQ_INT(ps.optopt, 0);
  ASSERT_EQ_INT(ps.optind, 2);
  ASSERT_EQ_INT(parg_getopt_long(&ps, 2, argv, ":v", longopts, NULL), -1);
  return 0;
}

static int test_missing_required_short_returns_colon(void) {
  char arg0[] = "prog";
  char arg1[] = "-o";
  char *argv[] = {arg0, arg1, NULL};
  struct parg_state ps;

  parg_init(&ps);
  ASSERT_EQ_INT(parg_getopt_long(&ps, 2, argv, ":o:", NULL, NULL), ':');
  ASSERT_EQ_INT(ps.optopt, 'o');
  ASSERT_EQ_INT(ps.optind, 2);
  return 0;
}

static int test_missing_required_long_returns_colon(void) {
  char arg0[] = "prog";
  char arg1[] = "--output";
  char *argv[] = {arg0, arg1, NULL};
  struct parg_state ps;
  const struct parg_option longopts[] = {
      {"output", PARG_REQARG, NULL, 'o'},
      {NULL, PARG_NOARG, NULL, 0},
  };

  parg_init(&ps);
  ASSERT_EQ_INT(parg_getopt_long(&ps, 2, argv, ":o:", longopts, NULL), ':');
  ASSERT_EQ_INT(ps.optopt, 'o');
  ASSERT_EQ_INT(ps.optind, 2);
  return 0;
}

static int test_optional_short_argument_only_attached(void) {
  char arg0[] = "prog";
  char arg1[] = "-s";
  char arg2[] = "10";
  char *argv[] = {arg0, arg1, arg2, NULL};
  struct parg_state ps;

  parg_init(&ps);
  ASSERT_EQ_INT(parg_getopt_long(&ps, 3, argv, ":s::", NULL, NULL), 's');
  ASSERT_EQ_INT(ps.optarg == NULL, 1);
  ASSERT_EQ_INT(parg_getopt_long(&ps, 3, argv, ":s::", NULL, NULL), 1);
  ASSERT_EQ_STR(ps.optarg, "10");
  ASSERT_EQ_INT(parg_getopt_long(&ps, 3, argv, ":s::", NULL, NULL), -1);
  return 0;
}

static int test_reorder_moves_options_first(void) {
  char arg0[] = "prog";
  char arg1[] = "input1";
  char arg2[] = "-v";
  char arg3[] = "--output";
  char arg4[] = "out.txt";
  char arg5[] = "input2";
  char *argv[] = {arg0, arg1, arg2, arg3, arg4, arg5, NULL};
  struct parg_state ps;
  int c;
  const struct parg_option longopts[] = {
      {"verbose", PARG_NOARG, NULL, 'v'},
      {"output", PARG_REQARG, NULL, 'o'},
      {NULL, PARG_NOARG, NULL, 0},
  };

  const int optend = parg_reorder(6, argv, ":vo:", longopts);
  ASSERT_EQ_INT(optend, 4);
  ASSERT_EQ_STR(argv[1], "-v");
  ASSERT_EQ_STR(argv[2], "--output");
  ASSERT_EQ_STR(argv[3], "out.txt");
  ASSERT_EQ_STR(argv[4], "input1");
  ASSERT_EQ_STR(argv[5], "input2");

  parg_init(&ps);
  do {
    c = parg_getopt_long(&ps, optend, argv, ":vo:", longopts, NULL);
    ASSERT_EQ_INT(c == 1, 0);
  } while (c != -1);

  return 0;
}

int main(void) {
  if (test_unknown_long_sets_optopt_zero() != 0) {
    return 1;
  }
  if (test_missing_required_short_returns_colon() != 0) {
    return 1;
  }
  if (test_missing_required_long_returns_colon() != 0) {
    return 1;
  }
  if (test_optional_short_argument_only_attached() != 0) {
    return 1;
  }
  if (test_reorder_moves_options_first() != 0) {
    return 1;
  }

  puts("parg tests passed");
  return 0;
}
