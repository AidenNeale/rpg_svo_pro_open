#!/usr/bin/env python

from distutils.core import setup

package_name = 'vikit_py'

setup(
    name=package_name,
    version='0.0.0',
    install_requires=['yaml'],
    zip_safe=True,
    packages=[package_name],
    package_dir={'': 'src'},
    maintainer='',
    maintainer_email='',
    description='Vikit Python Package',
    license='BSD',
)