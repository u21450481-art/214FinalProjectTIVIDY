# Continuous integration

GitHub runs `.github/workflows/ci.yml` automatically on every push to `dev` or a `feature/**` branch, and on every pull request into `dev`.

| Job | What it does | Fails the run? |
|---|---|---|
| `test` | `make check11` (the engine compiles as plain C++11), `make unit` (mediator and strategy unit tests), then `make test` (integration tests). Both test programs are built with AddressSanitizer and UBSan | Yes |
| `docker` | Builds the Dockerfile (compiles FTXUI and the TUI) and checks the `tividy` program is in the image. Runs only if `test` passed | Yes |
| `static-analysis` | `cppcheck` over `src/`. Reports problems as advice | No |

## Run the same checks locally

From the repo root, in Linux or WSL:

    make check11
    make unit
    make test

If all three pass, the `test` job will pass too.

## Reading a failed run

Open the **Actions** tab, click the red run, click the failed job, and read the first error in the log.

| Failed job | Usual cause |
|---|---|
| `test` | A change broke a test or the C++11 build |
| `docker` | FTXUI version mismatch or a TUI compile error. The Dockerfile pins FTXUI v6.1.9 |
| `static-analysis` | Never turns the run red; read its output as advice |

## Optional: make it a gate

GitHub, Settings, Branches: add a rule for `dev`, tick "Require status checks to pass", and choose `test` and `docker`. Nothing can then be merged into `dev` while the tests fail. This needs admin rights on the repository.
