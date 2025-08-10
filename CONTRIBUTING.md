# Contributing

Thank you for your interest in improving scastd. These guidelines help keep contributions consistent and easy to review.

## Coding Style

- Follow the existing C++ style: 4 spaces for indentation, no tabs.
- Place braces on their own line and keep line length under 100 characters.
- Each `.cpp` and `.h` file must start with the GPL header shown in [AGENTS.md](AGENTS.md).
- Run `make check` before submitting changes to ensure the build and tests pass.

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

1. Ensure your branch is up to date with `master` and that `make check` completes successfully.
2. Open a pull request with a clear title and description of the changes.
3. Link any related issues and request review from a maintainer.
4. Respond to feedback and update your branch as needed.
5. A maintainer will merge the PR once it has approval and passes all checks.

Thanks for contributing!
