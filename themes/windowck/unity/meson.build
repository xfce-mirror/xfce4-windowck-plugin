custom_target(
  'windowck-unity',
  command: [python3, '@INPUT@', '@OUTDIR@'],
  input: 'generator.py',
  output: unity_names,
  install: true,
  install_dir: get_option('prefix') / get_option('datadir') / 'themes' / 'Windowck' / 'unity',
)
