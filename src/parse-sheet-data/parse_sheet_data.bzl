def _parse_sheet_data_impl(ctx):
    ctx.actions.run(
        outputs = [ctx.outputs.out],
        inputs = [ctx.file.src],
        executable = ctx.executable._parse_sheet_data,
        arguments = [
            "--input={}".format(ctx.file.src.path),
            "--output={}".format(ctx.outputs.out.path),
        ],
    )

parse_sheet_data = rule(
    implementation = _parse_sheet_data_impl,
    attrs = {
        "_parse_sheet_data": attr.label(
            default = ":parse-sheet-data",
            executable = True,
            cfg = "exec",
        ),
        "src": attr.label(
            doc = "input JSON from Aseprite",
            mandatory = True,
            allow_single_file = True,
        ),
        "out": attr.output(
            doc = "output C++ header to generate",
            mandatory = True,
        ),
    },
)
