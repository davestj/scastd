# Contributing

Thank you for your interest in improving scastd. These guidelines help keep contributions consistent and easy to review.

## Coding Style

- Follow the existing C++ style: 4 spaces for indentation, no tabs.
- Place braces on their own line and keep line length under 100 characters.
- Each `.cpp` and `.h` file must start with the GPL header shown in [AGENTS.md](AGENTS.md).
- Ensure the project builds successfully before submitting changes.

## Branch Naming

- Create feature branches from `master`.
- Use a short, lowercase descriptor prefixed by category, e.g.:
  - `feat/add-status-endpoint`
  - `fix/crash-on-empty-config`
  - `docs/update-readme`
- Use hyphens to separate words; avoid spaces and underscores.

## Commit Conventions

- Use [Conventional Commits](https://www.conventionalcommits.org/) for commit messages.
- Start the summary line with a type such as `feat:`, `fix:`, `docs:`, `test:`, or `chore:`.
- Write summaries in the imperative mood and limit them to 72 characters.
- Add additional context in the commit body when necessary and reference related issues.

## Pull Request Process

1. Ensure your branch is up to date with `master` and that the project builds successfully.
2. Open a pull request with a clear title and description of the changes.
3. Link any related issues and request review from a maintainer.
4. Respond to feedback and update your branch as needed.
5. A maintainer will merge the PR once it has approval and passes all checks.

## Release Verification

The release workflow uploads package artifacts to a GitHub Release and then
uses the GitHub API to list the published assets. If any expected package or
checksum file is missing, the job fails. For the exact commands, see the
"Verify uploaded release assets" step in `.github/workflows/release.yml`.

## Release Notes

Release notes are generated with `scripts/generate_release_notes.sh`. The
script summarizes commits, contributors, and line counts between two refs and
outputs Markdown suitable for GitHub releases.

To preview the notes locally for the range between two tags:

```bash
scripts/generate_release_notes.sh v79 v81
```

If no starting ref is provided, the script compares the previous tag to the
current `HEAD`.

Thanks for contributing!
