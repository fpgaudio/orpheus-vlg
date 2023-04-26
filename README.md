# orpheus-vlg

This is the SystemVerilog implementation for the orpheus sound engine.

## Getting Started

Make sure you have [verilator](https://www.veripool.org/verilator/) and
[meson](https://mesonbuild.com/). When you clone the repo, proceed as follows:

```bash
git submodule update --init --recursive
meson build
cd build
meson test
```
