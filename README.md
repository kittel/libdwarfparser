Libdwarfparser
==============

Libdwarfparser is a tool to read and use symbol and debug information for VMI purposes. This library currently reads symbol information from dwarf debug information and Linux kernel symbol information.

Together with libvmi (and libvmiwrapper which is also contained within this library) it is possible to transparently use the supplied symbol information and navigate through the memory of a virtual machine.


Installation
------------

### Dependencies
 * `libdwarf  https://sourceforge.net/p/libdwarf/`
 * `libvmi    https://github.com/libvmi/libvmi/`

If you intend to use this library from python the following dependencies are also required:

 * `python`
 * `cython    https://github.com/cython/cython/`

### Building

This library can either be built from scratch or as a debian package. Building with autoconf:

 * clone this repository
 * `./autogen.sh`
 * `./configure`
 * `make`

A debian package can be built with: `dpkg-buildpackage -uc -us -tc -rfakeroot`. After this process four _*.deb_ have been created in the parent directory and can be installed with: `dpkg -i *.deb`

Usage
-----

This libary comes with a rudimentary doxygen documentation. Run make `doxygen-run` to generate it.

### SymbolManager

To access symbol information you have to load and instance of class Symbolmanager. This can be accomplished, for example, by loading a binary that was compiled with debug information.

Example:

	try {
	    DwarfParser::parseDwarfFromFD(this->getFD());
	} catch(DwarfException &e) {
	    std::cout << e.what() << std::endl;
	}

Combine with our elf parser to automatically determine and load dependencies.

### Accessing Symbols

Every Symbol is represented of an instance of class Symbol. These symbols either represent a Variable or a certain BaseType. Both of which allow to create an Instance of the Symbol (while for BaseTypes an address has to be specified.

The specialized versions of BaseType are:

 * Enum
 * Function
 * Structured (Struct, Union)
 * RefBaseType

while the RefBaseType is further devided into:

 * Pointer (and Array)
 * FuncPointer
 * Typedef
 * Consttype

libvmiwrapper
-------------
To enable introspection on a live vm, libdwarfparser uses libvmi to communicate with the guest vm. To leverage this feature, use the libvmiwrapper singleton and connect it to the target virtual machine.

 * `VMIInstance vmi(vmPath, [VMI_KVM|VMI_XEN] | VMI_INIT_COMPLETE);`

python-interface
----------------
To simplify virtual machine introspection from python libdwarfparser also comes with python bindings.These bindings are built by default and can be found in _@top_srcdir@/libdwarfparser/python/.


