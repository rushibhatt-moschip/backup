project('gst-template', 'c', version : '1.19.0.1', license : 'LGPL')

plugins_install_dir = join_paths(get_option('libdir'), 'gstreamer-1.0')

cc = meson.get_compiler('c')

gst_version = meson.project_version()

api_version = '1.0'

gst_dep = dependency('gstreamer-1.0', version : '>=1.19',
    required : true, fallback : ['gstreamer', 'gst_dep'])
gstvideo_dep = dependency('gstreamer-video-1.0', version : '>=1.19',
    required : true, fallback : ['gstreamer-video', 'gst_video_dep'])    
gstbase_dep = dependency('gstreamer-base-1.0', version : '>=1.19',
  fallback : ['gstreamer', 'gst_base_dep'])
cairo_dep = dependency('cairo', required : true)
gstmath_dep = declare_dependency(
    include_directories : include_directories('.'),
    dependencies : cc.find_library('m', required : true)
)

subdir('gst-app')
subdir('gst-plugin')
