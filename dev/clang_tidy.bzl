load("@bazel_skylib//lib:paths.bzl", "paths")
load("@bazel_skylib//lib:shell.bzl", "shell")

def _clang_tidy_impl(ctx):
    out_file = ctx.actions.declare_file(ctx.label.name)

    ctx.actions.write(
        output = out_file,
        content = """\
#!/bin/sh -eu

"$BUILD_EXECROOT"/{run_clang_tidy} -p "$BUILD_EXECROOT"/{db_dir} -fix
""".format(
            run_clang_tidy = shell.quote(ctx.executable._run_clang_tidy.path),
            db_dir = shell.quote(paths.dirname(ctx.file.db.path)),
        ),
    )

    return DefaultInfo(
        executable = out_file,
        runfiles = ctx.runfiles([
            ctx.executable._run_clang_tidy,
            ctx.file.db,
        ]),
    )

clang_tidy = rule(
    implementation = _clang_tidy_impl,
    executable = True,
    attrs = {
        "_run_clang_tidy": attr.label(
            default = "@llvm//:bin/run-clang-tidy",
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "db": attr.label(
            doc = "compile_commands.json file",
            mandatory = True,
            allow_single_file = True,
        ),
    },
)
