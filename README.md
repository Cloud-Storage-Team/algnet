# algnet

<<<<<<< HEAD
Проект 5: Алгоритмы контроля перегрузки PowerTCP [1] на C++

Студент: Артем Выгузов

[1] V. Addanki, O. Michel and S. Schmid, “PowerTCP: Pushing the performance limits of datacenter networks” (2022)

Проект 1: Моделирование перегрузки сети в дата-центре на C++

Студент: Иван Шаныгин

Проект 2: Алгоритм контроля перегрузки на основе кредитов на C++

Студент: Артем Пешков
=======
# Build, test and run project

`CmakeFileLists.txt` have two options: `BUILD_MAIN` and `BUILD_TEST` (both are enabled by default). First one controles building of main executable `simulator`, second one - building tests for it (`test_simulator`).

It is assumed that build files locates in `build` directory. You may do it the way you wish, but using `launch.json`, `tasks.json` and others from `.vscode_template` is recomended. 

# Git hooks

There is `git-hooks-template` directory in project that contains [git hooks](https://git-scm.com/book/ms/v2/Customizing-Git-Git-Hooks) (now they are used for auto-formating code changed in commit). To use them, please, copy all files from mentioned directory to `.git/hooks` or run `git config --global core.hookspath git-hooks-template`.
>>>>>>> 6ed5fd03215664cd785e2ea19cfb827e7c7cee05
