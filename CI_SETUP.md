# Adding the CI pipeline

The pipeline runs on every push to `dev` or a `feature/*` branch, and on pull requests into `dev`.

| Job | What it does | Fails the pipeline? |
|---|---|---|
| `test` | `make check11` (engine compiles as C++11), `make unit` (mediator and strategy unit tests), then `make test` (integration tests), the last two with AddressSanitizer and UBSan | Yes |
| `docker` | Builds the Dockerfile (compiles FTXUI and the TUI) and checks the `tividy` program is in the image. Runs only if `test` passed | Yes |
| `static-analysis` | `cppcheck` on the engine sources | No, it only reports |

## Steps (PowerShell, in your repo folder)

    git checkout dev
    git pull origin dev
    git checkout -b feature/ci

1. Copy the `.github` folder from this zip into the repo root. It is a hidden-style folder name, so in File Explorer turn on View > Show > Hidden items, or run:

       Copy-Item -Recurse <path-to-zip-contents>\.github .

2. Add this line under the title in `README.md`:

       ![CI](https://github.com/u21450481-art/214FinalProjectTIVIDY/actions/workflows/ci.yml/badge.svg)

3. Commit and push:

       git add .github README.md
       git commit -m "Add CI pipeline: C++11 check, sanitizer tests, Docker build, cppcheck"
       git push -u origin feature/ci

4. On GitHub, open a pull request from `feature/ci` into `dev` (check the base branch is `dev`). The pipeline starts on its own. Open the **Actions** tab to watch it.
5. When it is green, take a screenshot of the Actions run for your bonus evidence, then merge the pull request.

## Optional: make it a gate
GitHub, Settings, Branches, add a rule for `dev`, tick "Require status checks to pass" and choose `test` and `docker`. Then nothing merges into `dev` while the tests fail. You need admin rights on the repo for this.

## If a job goes red
| Job | Likely cause |
|---|---|
| `test` | A teammate's change broke a test or the C++11 build. Open the job log and read the first error |
| `docker` | FTXUI version mismatch or a TUI compile error. The Dockerfile pins FTXUI v6.1.9, which builds the TUI |
| `static-analysis` | Never turns the run red. Read its output as advice |
