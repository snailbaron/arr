def _pack_aseprite_impl(ctx):
    # type: (ctx) -> None

    ctx.actions.run(
        outputs = [ctx.outputs.out_data, ctx.outputs.out_sheet],
        inputs = ctx.files.srcs,
        executable = ctx.executable._aseprite,
        arguments = [
            "aseprite",
            "--batch",
            "--data={}".format(ctx.outputs.out_data.path),
            "--sheet={}".format(ctx.outputs.out_sheet.path),
            "--format=json-array",
            "--sheet-type=packed",
            "--filename-format={title}:{tag}:{tagframe}",
        ] + [f.path for f in ctx.files.srcs],
    )

pack_aseprite = rule(
    implementation = _pack_aseprite_impl,
    attrs = {
        "_aseprite": attr.label(
            default = "@aseprite",
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "srcs": attr.label_list(
            doc = ".aseprite files to pack into a sheet",
            mandatory = True,
            allow_files = True,
        ),
        "out_data": attr.output(
            doc = "output JSON file",
            mandatory = True,
        ),
        "out_sheet": attr.output(
            doc = "output PNG file",
            mandatory = True,
        ),
    },
)
