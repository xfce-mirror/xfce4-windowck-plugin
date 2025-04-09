custom_target(
  'windowck-dark-xfwm4',
  command: [python3, '@INPUT@', '@OUTDIR@'],
  input: 'generator.py',
  output: xfwm4_names,
  install: true,
  install_dir: get_option('prefix') / get_option('datadir') / 'themes' / 'Windowck-dark' / 'xfwm4',
)

install_data(
  'themerc',
  install_dir: get_option('prefix') / get_option('datadir') / 'themes' / 'Windowck-dark' / 'xfwm4',
)
