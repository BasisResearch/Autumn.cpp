# Autumn.cpp (a.k.a. Autumn.Wasm) A CPP Implementation of Autumn that compiles to WASM (and more)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

This repository hosts the Autumn interpreter, built with C++. It compiles to libraries in WebAssembly (`WASM`), `C++`, `Python` and `Julia`.

![Example Programs: Particles, Magnets, and Space Invader](assets/examples_small.gif)

## Setting Up the Environment
Create a Conda environment using the following command:
```sh
conda env create --file environment.yml
```

## Using the pre-built package
To avoid the hassle in re-building, please try visiting the [Release](https://github.com/BasisResearch/Autumn.cpp/releases) page. Download the corresponding version and use it according to the [Wiki](https://github.com/BasisResearch/Autumn.cpp/wiki)

## Building the C++ Interpreter
### Install Julia Package
Run julia and install CxxWrap
```sh
using Pkg
Pkg.add("CxxWrap")
```


Compile the C++ interpreter by running:
```sh
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$(julia -e 'using CxxWrap;print(joinpath(CxxWrap.prefix_path()), "/lib/cmake")')
make  -j12
cd ..
```

If you’re using macOS with Apple Silicon, use `cmake .. -DCMAKE_OSX_ARCHITECTURES=arm64` in place of `cmake ..` to ensures that all files are compiled and linked for arm64 architecture.



## Running Autumn programs
### Python
Then, in the same folder, execute the following commands:

```sh
cp build/*.so .
python python_test_mpl.py tests/grow.sexp 2> stderr_python.txt 
```

If you want to visualize preset of animation, first, install `ffmpeq`
```sh
# Mac
brew install ffmpeg
# Linux
apt install ffmpeg libffmpeg
```


### Julia Build
In your julia project:
```sh
using Pkg;
Pkg.develop(path=joinpath(first(DEPOT_PATH), "packages", "AutumnInterpreter"))
Pkg.add("Plots") # This is for visualization
Pkg.add("PlotlyJS") # This is for visualization
Pkg.add("JSON3") # This is for visualization
Pkg.add("WebIO")
Pkg.add("Observables")
Pkg.add("JSExpr")
using AutumnInterpreter
```

Finally, try out these tests:
```shell
julia test.jl
julia test2.jl
```
The main purpose is to allow building Autumn agent in Julia. For interactive purpose, please use Python package.


### WASM Build
```sh
sh install_scripts/setup_emscripten.sh
source ./emsdk/emsdk_env.sh
make web
cp interpreter_web.* ./flask_server/static/
```

# What's working?
[BBQ](tests/bbq.sexp) [Egg](tests/egg.sexp) [Gravity 3](tests/gravity_3.sexp) [Grow](tests/grow.sexp) [Magnets](tests/magnets.sexp) [Paint](tests/paint.sexp) [Particles](tests/particles.sexp) [Sokoban II](tests/sokoban_ii.sexp) [Waterplug](tests/waterplug.sexp) [Game of Life](tests/gameOfLife.sexp) [Gravity 4](tests/gravity_4.sexp) [Ice](tests/ice.sexp) [Mario](tests/mario.sexp) [Particle 1](tests/particle_1.sexp) [Sand](tests/sand.sexp) [Sokoban](tests/sokoban.sexp) [Wind](tests/wind.sexp)


# Bleeding Edge
We welcome and hope to receive contributions in the following modules:
- `python_test_mpl_ci.py` This is used for headless CI testing.
- `python_test` is a faster interface under development, but is not working properly, use `python_test_mpl.py` instead.


# Citations
```bibtex
@article{das2023autumn,
author = {Das, Ria and Tenenbaum, Joshua B. and Solar-Lezama, Armando and Tavares, Zenna},
title = {Combining Functional and Automata Synthesis to Discover Causal Reactive Programs},
year = {2023},
issue_date = {January 2023},
publisher = {Association for Computing Machinery},
address = {New York, NY, USA},
volume = {7},
number = {POPL},
url = {https://doi.org/10.1145/3571249},
doi = {10.1145/3571249},
journal = {Proc. ACM Program. Lang.},
month = jan,
articleno = {56},
numpages = {31},
keywords = {synthesis, reactive, causal, automata}
}
```

# License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

# Acknowledgement
We acknowledge the open-source projects and communities that made this interpreter possible:
- The parser is made possible thanks to [Sexpresso - an S-Expression parser](https://github.com/BitPuffin/sexpresso)

# Contributors
Dat Nguyen, Archana Warrier, Yichao Liang, Cambridge Yang, Michelangelo Naim, Zenna Tarvares
