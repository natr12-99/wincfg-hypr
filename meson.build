project('wincfg', 'cpp', version: '1😎', default_options: ['cpp_std=c++17'])

gtkmm_dep = dependency('gtkmm-4.0')

sources = files(
  'src/firstlaunch.cpp',
  'src/func.cpp',
  'src/loader.cpp',
  'src/main.cpp',
  'src/ruleconfig.cpp',
  'src/saver.cpp',
  'src/window.cpp',
)

executable('wincfg', sources, dependencies: gtkmm_dep)
