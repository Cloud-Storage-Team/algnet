# algnet

# Build, test and run project

`CmakeFileLists.txt` have two options: `BUILD_MAIN` and `BUILD_TEST` (both are enabled by default). First one controles building of main executable `simulator`, second one - building tests for it (`test_simulator`).

It is assumed that build files locates in `build` directory. You may do it the way you wish, but using `launch.json`, `tasks.json` and others from `.vscode_template` is recomended. 