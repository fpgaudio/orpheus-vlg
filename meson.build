project('orpheus-verilog', 'cpp', default_options: [
  'default_library=static',
  'cpp_std=c++20',
], version: '0.1.0')

cmake = import('cmake')
fs = import('fs')

# Verilator
###############################################################################
verilator_dep = dependency('verilator')
# Verilator needs to fix their shit.
# Fix your fucking pkg-config >:(.
verilated_cpp = [
  verilator_dep.get_pkgconfig_variable('includedir') / 'verilated.cpp',
  verilator_dep.get_pkgconfig_variable('includedir') / 'verilated_vcd_c.cpp'
]

# Orpheus
###############################################################################
orpheus_proj = subproject('orpheus')
orpheus_dep = orpheus_proj.get_variable('liborpheus_dep')

verilator = find_program('verilator')
verilog_models = {
  'mod_sinesource': {},
  'mod_attenuator': {},
  'mod_clock': {},
  'mod_synth': { 'cxxflags': ['-O3'], 'test_timeout': 60 }
}

verilog_includes = [
  '-I' + meson.source_root() / 'src',
  '-I' + meson.source_root() / 'subprojects' / 'cordic' / 'sv'
]

# Anti-pattern. Ideally this would be removed
verilator_global_warn_excludes = [
  '-Wno-LITENDIAN', '-Wno-WIDTH' # Cordic subproject uses these
]

# Testing Fixtures
###############################################################################
catch2_dep = dependency('catch2')
gnuplot = find_program('gnuplot', required: get_option('test-graphs'))
gnuplot_iostream_proj = cmake.subproject('gnuplot-iostream',
                                         required: gnuplot.found())
gnuplot_iostream_dep = gnuplot_iostream_proj.dependency('gnuplot-iostream')

global_test_flags = []
if get_option('test-graphs').enabled()
  global_test_flags += '-DORPHEUS_VLG_TEST_PLOTTING'
endif

foreach model : verilog_models.keys()
  vlg_target = custom_target(
    model,
    output: ['V' + model + '__ALL.a'],
    input: ['src' / model + '.sv'],
    build_by_default: true,
    command: [verilator, '--cc', '--trace', '--build', '@INPUT@',
              verilator_global_warn_excludes,
              verilog_includes]
             + ['-Mdir', '@OUTDIR@']
  )

  test_source = meson.source_root() / 'test' / 'test_' + model + '.cpp'

  test_flags = global_test_flags
  if verilog_models[model].has_key('cxxflags')
    test_flags += verilog_models[model]['cxxflags']
  endif

  if fs.exists(test_source)
    test_name = 'test_' + model
    test_target = executable(
      test_name,
      ['test' / test_name + '.cpp', verilated_cpp, vlg_target],
      cpp_args: test_flags,
      dependencies: [
        catch2_dep,
        verilator_dep,
        orpheus_dep,
        gnuplot_iostream_dep],
    )
  endif

  test(
    test_name,
    test_target,
    args: ['--artifact-directory', meson.build_root() / 'simulation-logs' / test_name],
    timeout: verilog_models[model].has_key('test_timeout')
      ? verilog_models[model]['test_timeout'] : 30
  )
endforeach
