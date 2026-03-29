def _bin_to_header(ctx):
    out_file = ctx.outputs.out or ctx.actions.declare_file(
        ctx.label.name + ".hpp",
    )

    ctx.actions.run(
        outputs = [out_file],
        inputs = [ctx.file.src],
        executable = ctx.executable._bin_to_header,
        arguments = [
            "--input={}".format(ctx.file.src.path),
            "--output={}".format(out_file.path),
        ],
    )

    return DefaultInfo(files = depset([out_file]))

bin_to_header = rule(
    implementation = _bin_to_header,
    attrs = {
        "_bin_to_header": attr.label(
            default = ":bin-to-header",
            executable = True,
            cfg = "exec",
        ),
        "src": attr.label(
            doc = "input binary file to encode",
            mandatory = True,
            allow_single_file = True,
        ),
        "out": attr.output(
            doc = "output header to generate",
        ),
    },
)
