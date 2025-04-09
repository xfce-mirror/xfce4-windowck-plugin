wck_common_sources = [
  'ui_style.c',
  'ui_style.h',
  'wck-plugin.c',
  'wck-plugin.h',
  'wck-utils.c',
  'wck-utils.h',
  xfce_revision_h,
]

wck_common = static_library(
  'wck-common',
  wck_common_sources,
  c_args: [
    '-DG_LOG_DOMAIN="@0@"'.format('libwck-common'),
  ],
  include_directories: [
    include_directories('..'),
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
  install: false,
)
