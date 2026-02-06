default:
  @just --list

build:
  zig build

test:
  zig build test

format:
  rg --files -g '*.c' -g '*.h' | xargs -r clang-format-20 -i

run *args:
  zig build run -- {{args}}

clean:
  rm -rf zig-out .zig-cache .zig-global-cache
