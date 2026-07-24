plugin_sources = [
  'wckspacer-dialogs.c',
  'wckspacer-dialogs.h',
  'wckspacer-spacer.c',
  'wckspacer-spacer.h',
  'wckspacer.c',
  'wckspacer.h',
]

plugin_sources += gnome.compile_resources(
  'wckspacer-dialogs-resources',
  'wckspacer-dialogs.gresource.xml',
)

plugin_install_subdir = 'xfce4' / 'panel' / 'plugins'

plugin_lib = shared_module(
  'wckspacer',
  plugin_sources,
  gnu_symbol_visibility: 'hidden',
  c_args: [
    '-DG_LOG_DOMAIN="@0@"'.format('xfce4-wckspacer-plugin'),
  ],
  include_directories: [
    include_directories('..' / '..'),
  ],
  dependencies: [
    glib,
    gtk,
    libxfce4panel,
    libxfce4ui,
    libxfce4util,
    libxfce4windowing,
    libxfce4windowingui,
    xfconf,
  ],
  link_with: [
    wck_common,
  ],
  install: true,
  install_dir: get_option('prefix') / get_option('libdir') / plugin_install_subdir,
)

i18n.merge_file(
  input: 'wckspacer.desktop.in',
  output: 'wckspacer.desktop',
  po_dir: '..' / '..' / 'po',
  type: 'desktop',
  install: true,
  install_dir: get_option('prefix') / get_option('datadir') / plugin_install_subdir,
)
