project('orpheus-verilog', 'cpp', default_options: [
  'default_library=static',
  'cpp_std=c++20',
  'warning_level=1',
], version: '0.1.0')

verilator_dep = dependency('verilator')
# Verilator needs to fix their shit.
# Fix your fucking pkg-config kids.
verilated_cpp = verilator_dep.get_pkgconfig_variable('includedir') / 'verilated.cpp'

# Orpheus
###############################################################################
orpheus_proj = subproject('orpheus')
orpheus_dep = orpheus_proj.get_variable('liborpheus_dep')

# Testing Things
###############################################################################
test_includes = [
  'subprojects/Catch2/single_include',
  'subprojects/asciichart/include/ascii',
]

# Verilog crap
###############################################################################
verilator = find_program('verilator')
verilog_modules = [
  ['orpheus_sine16',
    'src/orpheus_sine16.sv',
    'Vorpheus_sine16__ALL.a',
    ['test_sine16', 'test/test_sine16.cpp']
  ],
  ['orpheus_sine_source',
   'src/orpheus_sine_source.sv',
   'Vorpheus_sine_source__ALL.a',
   []
  ],
]

foreach verilog_module : verilog_modules
  message(verilog_module)
  module_target = custom_target(verilog_module[0],
    output: verilog_module[2],
    input: verilog_module[1],
    command: [verilator, '--cc', '--build', '--assert', '@INPUT@',
              '-Mdir', '@OUTDIR@'],
  )
  test_info = verilog_module[3]
  if (test_info.length() > 0)
    test_target = executable(test_info[0],
      [test_info[1], module_target, verilated_cpp],
      dependencies: [verilator_dep, orpheus_dep],
      include_directories: [test_includes])
    test(test_info[0], test_target)
  endif
endforeach
