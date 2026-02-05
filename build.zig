const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const lib_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    lib_module.addIncludePath(b.path("include"));
    lib_module.addCSourceFile(.{
        .file = b.path("src/parg.c"),
        .flags = &.{
            "-std=c23",
            "-Wall",
            "-Wextra",
            "-Wpedantic",
        },
    });

    const lib = b.addLibrary(.{
        .name = "parg",
        .root_module = lib_module,
        .linkage = .static,
    });

    b.installArtifact(lib);

    const exe_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    exe_module.addIncludePath(b.path("include"));
    exe_module.addCSourceFile(.{
        .file = b.path("examples/simple.c"),
        .flags = &.{
            "-std=c23",
            "-Wall",
            "-Wextra",
            "-Wpedantic",
        },
    });
    exe_module.linkLibrary(lib);

    const exe = b.addExecutable(.{
        .name = "parg-example",
        .root_module = exe_module,
    });

    b.installArtifact(exe);

    const tests_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    tests_module.addIncludePath(b.path("include"));
    tests_module.addCSourceFile(.{
        .file = b.path("tests/parg_tests.c"),
        .flags = &.{
            "-std=c23",
            "-Wall",
            "-Wextra",
            "-Wpedantic",
        },
    });
    tests_module.linkLibrary(lib);

    const tests_exe = b.addExecutable(.{
        .name = "parg-tests",
        .root_module = tests_module,
    });

    const run_cmd = b.addRunArtifact(exe);
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }
    b.step("run", "Run the example").dependOn(&run_cmd.step);

    const run_tests_cmd = b.addRunArtifact(tests_exe);
    b.step("test", "Run parser regression tests")
        .dependOn(&run_tests_cmd.step);
}
