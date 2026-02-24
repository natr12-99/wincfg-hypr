project('wincfg', 'cpp', version: 'two', default_options: ['cpp_std=c++20'])

gtkmm = dependency('gtkmm-4.0', required: true)
json = dependency('nlohmann_json', required: true)

sources = files(
  'src/firstlaunch.cpp',
  'src/func.cpp',
  'src/loader.cpp',
  'src/main.cpp',
  'src/ruleconfig.cpp',
  'src/rulesparsers.cpp',
  'src/saver.cpp',
  'src/updatehandlers.cpp',
  'src/window.cpp',
)

executable('wincfg', sources, dependencies: [gtkmm, json])
