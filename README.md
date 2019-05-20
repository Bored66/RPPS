## RPPS
RPPS (or Rapid Protocol Prototyping Solution) is a set of tools and classes which allow fast, easy and consistent development for device protocols. Just define your protocol using set of XML files in a text editor or using Protocol Editor provided by RPPS and generate supporting infrastructure C++ classes which should provide datagram parsing, dispatching, serialization/diserialization, create basic device protocol emulation facility as well as to be able to check protocol consistency/coherence and test using basic emulation. Also, it provides a set of comprehensive transport classes (Serial/TCP/UDP/HTTP) specifically designed to work with the generated code base.

It's written in C++ using Qt framework (4/5).

### Features
* Supports
  -  regular C++ types as datagram fields
  - custom composite types (POD structures), enumerations and bit fields as datagram fields
  - fixed-size fields (arrays)
  - variable-sized fields including custom types
  - arbitrary number of variable-sized fields
  - arbitrary number of checksum (CRC) fields according to given protocol definition
  - datagram transformations (i.e. byte stuffing/unstuffing)
* Checks protocol consistency/coherence prior to code generation
* Creates basic protocol emulation
* Creates autotests
* Provides flexible and robust diagnostics and logging

### Tools
* Generator - creates C++ classes from XML
* Protocol Editor
  - Supports full development cycle:
  create, edit, build and test your projects in one program
  - Includes smart terminal facility:
  Once even small parts of your protocol defined you can immediately start testing it using different transport with a real device without creating actual C++ code
* Terminal and transport test


