# algnet

# Build, test and run project

Project uses Git submodules to manage dependencies. To initialize submodules run this command:
```bash
git submodule update --init --recursive
```

`CmakeFileLists.txt` have two options: `BUILD_MAIN` and `BUILD_TEST` (both are enabled by default). First one controles building of main executable `simulator`, second one - building tests for it (`test_simulator`).

It is assumed that build files locates in `build` directory. You may do it the way you wish, but using `launch.json`, `tasks.json` and others from `.vscode_template` is recommended. 

# Git hooks

There is `git-hooks-template` directory in project that contains [git hooks](https://git-scm.com/book/ms/v2/Customizing-Git-Git-Hooks) (now they are used for auto-formating code changed in commit). To use them, please, copy all files from mentioned directory to `.git/hooks` or run `git config --global core.hookspath git-hooks-template`.
