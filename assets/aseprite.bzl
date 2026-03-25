def _aseprite_impl(repository_ctx):
    # type: (repository_ctx) -> None

    aseprite_path = repository_ctx.which("aseprite")
    if aseprite_path == None:
        fail("'aseprite' not found in PATH")

    repository_ctx.symlink(aseprite_path, "aseprite")
    repository_ctx.file("BUILD.bazel", 'exports_files(["aseprite"])')

aseprite = repository_rule(
    implementation = _aseprite_impl,
)
