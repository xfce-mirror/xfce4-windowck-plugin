plugin_sources = [
  'theme.c',
  'theme.h',
  'wckbuttons-dialogs.c',
  'wckbuttons-dialogs.h',
  'wckbuttons-theme.c',
  'wckbuttons-theme.h',
  'wckbuttons.c',
  'wckbuttons.h',
]

plugin_sources += gnome.compile_resources(
  'wckbuttons-dialogs-resources',
  'wckbuttons-dialogs.gresource.xml',
)

plugin_install_subdir = 'xfce4' / 'panel' / 'plugins'

plugin_lib = shared_module(
  'wckbuttons',
  plugin_sources,
  gnu_symbol_visibility: 'hidden',
  c_args: [
    '-DG_LOG_DOMAIN="@0@"'.format('xfce4-wckbuttons-plugin'),
  ],
  include_directories: [
    include_directories('..' / '..'),
  ],
  dependencies: [
    glib,
    gtk,
    libwnck,
    libxfce4panel,
    libxfce4ui,
    libxfce4util,
    xfconf,
  ],
  link_with: [
    wck_common,
  ],
  install: true,
  install_dir: get_option('prefix') / get_option('libdir') / plugin_install_subdir,
)

i18n.merge_file(
  input: 'wckbuttons.desktop.in',
  output: 'wckbuttons.desktop',
  po_dir: '..' / '..' / 'po',
  type: 'desktop',
  install: true,
  install_dir: get_option('prefix') / get_option('datadir') / plugin_install_subdir,
)
