import os
from setuptools import setup, Extension

module = Extension('regs', sources=['regs.cpp','register.cpp','bit.cpp','bitfieldparser.cpp','hamming.cpp','registercontainer.cpp'], language='c++',
	include_dirs=['C:\\Qt\\MSVC4.8.6\\include','C:\\Qt\\MSVC4.8.6\\include\\QtCore','C:\\Qt\\MSVC4.8.6\\include\\QtNetwork'],
	library_dirs=['C:\\Qt\\MSVC4.8.6\\lib','C:\\Qt\\MSVC4.8.6\\bin'],
	libraries=['QtCore4','QtNetwork4']
	)

setup(name='regs', ext_modules = [module])