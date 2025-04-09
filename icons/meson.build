names = [
  'wckbuttons-plugin',
  'wckmenu-plugin',
  'wcktitle-plugin',
]

foreach name : names
  install_data(
    '48x48' / '@0@.png'.format(name),
    install_dir: get_option('prefix') / get_option('datadir') / 'icons' / 'hicolor' / '48x48' / 'apps',
  )
endforeach
