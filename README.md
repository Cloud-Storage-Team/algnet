# algnet

# Build, test and run project

Project uses Git submodules to manage dependencies. To initialize submodules run this command:
```bash
git submodule update --init --recursive
```

`CmakeFileLists.txt` have two options: `BUILD_MAIN` and `BUILD_TEST` (both are enabled by default). First one controles building of main executable `simulator`, second one - building tests for it (`test_simulator`).

Build project with tests:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_PROJECT=ON -DBUILD_TESTS=ON
cmake --build build
```

It is assumed that build files locates in `build` directory. You may do it the way you wish, but using `launch.json`, `tasks.json` and others from `.vscode_template` is recommended.

Run tests:

```bash
./build/test_simulator
```

Run simulator:

```bash
./build/simulator --config configuration_examples/simulation_examples/basic_simulation.yml \
    [--output-dir output-dir-name] \
    [--no-plots] \
    [--export-metrics] \
    [--no-logs]
```

Options:

```bash
./simulator [OPTION...]

  -c, --config arg      Path to the simulation configuration file
      --output-dir arg  Output directory for metrics and plots (default: metrics)
      --no-logs         Disables logging
      --no-plots        Disables plots generation
      --export-metrics  Export metric values into output-dir
  -h, --help            Print usage
```

## Profiling

To profile simulator run `CMake` with option `-DPROFILING=ON`:

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DPROFILING=ON ..
cmake --build .

# Run simulator as usual
./simulator ../configuration_examples/simulation_examples/basic_simulation.yml metrics-dir

# Run profiler gprof
gprof simulator > profile.txt
```

# Smart pointers

Some of classes (`Sender`, `Receiver`, `Switch`, `Flow` etc) are heir from `std::enable_shared_from_this`. That means if you try to create this objects on stack and after that call `shared_from_this`, you will get an error (double free same memory). So please, be carefully with this moment.

# Git hooks

There is `git-hooks-template` directory in project that contains [git hooks](https://git-scm.com/book/ms/v2/Customizing-Git-Git-Hooks) (now they are used for auto-formating code changed in commit). To use them, please, copy all files from mentioned directory to `.git/hooks` or run `git config --global core.hookspath git-hooks-template`.
