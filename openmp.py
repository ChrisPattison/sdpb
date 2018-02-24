#! /usr/bin/env python
# encoding: utf-8

def configure(conf):
    def get_param(varname,default):
        return getattr(Options.options,varname,'')or default


    openmp_fragment="#include <omp.h>\nint main() { omp_set_num_threads(10);}"

    flags=['-fopenmp','-openmp']
    if conf.options.openmp_flag:
        flags=[conf.options.openmp_flag]
    found_openmp=False
    for flag in flags:
        try:
            conf.check_cxx(msg="Checking OpenMP flag " + flag,
                           fragment=openmp_fragment,
                           cxxflags=flag, linkflags=flag, uselib_store='openmp')
        except conf.errors.ConfigurationError:
            continue
        else:
            found_openmp=True
            break
    if not found_openmp:
        conf.fatal('Could not find OpenMP flag')

def options(opt):
    openmp=opt.add_option_group('OpenMP Options')
    openmp.add_option('--openmp-flag',
                     help='Flag to enable OpenMP')