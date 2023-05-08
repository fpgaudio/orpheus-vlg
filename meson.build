project('orpheus-verilog', 'cpp', default_options: [
  'default_library=static',
  'cpp_std=c++20',
], version: '0.1.0')

fs = import('fs')

# Verilator
###############################################################################
verilator_dep = dependency('verilator')
# Verilator needs to fix their shit.
# Fix your fucking pkg-config >:(.
verilated_cpp = verilator_dep.get_pkgconfig_variable('includedir') / 'verilated.cpp'

# Orpheus
###############################################################################
orpheus_proj = subproject('orpheus')
orpheus_dep = orpheus_proj.get_variable('liborpheus_dep')

verilator = find_program('verilator')
verilog_models = [
  'mod_sine16',
  'mod_sinesource',
  'mod_attenuator',
  'mod_clock',
  'mod_synth'
]

test_includes = [
  'subprojects/Catch2/single_include',
  'subprojects/asciichart/include/ascii',
]

foreach model : verilog_models
  vlg_target = custom_target(
    model,
    output: ['V' + model + '__ALL.a'],
    input: ['src' / model + '.sv'],
    build_by_default: true,
    build_always: true,
    command: [verilator, '--cc', '--build', '--assert', '@INPUT@',
              '-I' + meson.source_root() / 'src', '-Mdir', '@OUTDIR@']
  )

  test_source = meson.source_root() / 'test' / 'test_' + model + '.cpp'

  if fs.exists(test_source)
    test_target = executable(
      'test_' + model,
      ['test' / 'test_' + model + '.cpp', verilated_cpp, vlg_target],
      dependencies: [verilator_dep, orpheus_dep],
      include_directories: [test_includes]
    )
  endif

  test('test_' + model, test_target)
endforeach
