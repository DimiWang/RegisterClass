import os
from setuptools import setup, Extension

module = Extension('regs', sources=['regs.cpp','../../source/register.cpp','../../source/bit.cpp','../../source/bitfield.cpp','../../source/bitset.cpp','registercontainer.cpp'], language='c++',
	include_dirs=['/usr/include/x86_64-linux-gnu/qt5/QtCore','/usr/include/x86_64-linux-gnu/qt5/QtNetwork'],
	library_dirs=['/lib/x86_64-linux-gnu/qt5/bin'],
	libraries=['QtCore4','QtNetwork4']
	)

setup(name='regs', ext_modules = [module])