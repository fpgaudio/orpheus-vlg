project('orpheus-verilog', 'cpp', default_options: [
  'default_library=static',
  'cpp_std=c++20',
  'warning_level=2',
], version: '0.1.0')

verilator_dep = dependency('verilator')
# Verilator needs to fix their shit.
# Fix your fucking pkg-config kids.
verilated_cpp = verilator_dep.get_pkgconfig_variable('includedir') / 'verilated.cpp'

# Verilog crap
###############################################################################
verilator = find_program('verilator')
verilog_sources = [
  'src/orpheus_sine16.verilog'
]

verilog_sim_objs = custom_target('verilog-sim',
  output: ['Vorpheus_sine16__ALL.a'],
  input: verilog_sources,
  command: [verilator, '--cc', '--build', '--assert', '@INPUT@',
            '-Mdir', '@OUTDIR@'],
)

test_includes = [
  'lib/Catch2/single_include',
  'lib/asciichart/include/ascii',
]

# Testing Things
###############################################################################
test_sources = [
  ['test_sine16', ['test/test_sine16.cpp', verilog_sim_objs, verilated_cpp], []]
]

foreach test_src : test_sources
  t_exe = executable(test_src[0], test_src[1],
                     dependencies: verilator_dep,
                     include_directories: [test_includes, test_src[2]])
  test(test_src[0], t_exe)
endforeach
