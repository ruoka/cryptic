# Release Policy

## Current status: v2.0.0

**[`v2.0.0`](https://github.com/ruoka/cryptic/releases/tag/v2.0.0)** (1 August 2026) is the
first supported SemVer release of the modules-era surface (`import cryptic;` on Clang 21+
with libc++ modules). Prior header-only history is treated as an untagged v1 line.

Pin `v2.0.0` (or a later tag / deliberate commit) via submodule. Nested
`deps/tester` must match the parent tree’s tester pin — see
[YarDB versioning](https://github.com/ruoka/YarDB/blob/master/docs/versioning.md).

## What is public

| Surface | Contract |
|---------|----------|
| `import cryptic;` | Exported partitions and algorithms documented in the [README](../README.md) |
| Build / test | `./tools/CB.sh` debug/release build and test entry points used by CI |

Internals, benchmark harness details, and nested `deps/tester` contents (beyond the
gitlink SHA) are not a cryptic compatibility promise.

## Versioning

Semantic versioning (`vMAJOR.MINOR.PATCH`) on the public surface above.

| Bump | Means |
|------|-------|
| MAJOR | Incompatible change to the exported module API, or raising the minimum toolchain |
| MINOR | Additive, backward-compatible public change |
| PATCH | Fix that leaves documented behaviour intact |

**Modules-era major.** `v2.0.0` starts the Clang 21 + modules line.

**Tester pin.** When bumping `deps/tester`, land the same SHA in cryptic, net4cpp,
json4cpp, and YarDB together.

## Release criteria

1. Default-branch CI green on the tagged commit (`--jsonl=failures`, `--jobs=$(nproc)`).
2. Open PRs that affect the release surface are merged or closed.
3. Nested `deps/tester` matches the ecosystem pin rule.
4. README / this policy agree with the tagged surface; minimum toolchain stated.

## Cutting a release

```bash
./tools/CB.sh debug test --jsonl=failures --jobs="$(nproc)"
git tag -a vX.Y.Z -m "vX.Y.Z"
git push origin vX.Y.Z
gh release create vX.Y.Z --title vX.Y.Z --notes-file release-notes.md
```

Release notes: breaking changes, additions, fixes, minimum compiler, tester pin SHA/tag.
