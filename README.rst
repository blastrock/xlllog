==========
 xll::log
==========

What is xll::log?
=================

It is a logging system with the following features:

- header-only
- almost zero allocation
- supports different logging levels
- supports logging into categories
- printf-style formatting (thanks to pnt)
- filtering based on categories
- aims to be efficient

Example
=======

Here is an example of C++ file:

.. code-block:: cpp

    XLL_LOG_CATEGORY("memory-management");

    void* alloc(size_t size)
    {
      if (available_memory < THRESHOLD)
        xWar("Available memory is critical: %d", available_memory);

      return do_alloc(size);
    }

In your main file, you can write:

.. code-block:: cpp

    XLL_LOG_CATEGORY("main");

    int main()
    {
      xll::log::setLevel(xll::log::LEVEL_INFO);

      xInf("Initializing");

      init();

      return 0;
    }

Constraints
===========

There are a few constraints that must be respected for the library to be fast.

On categories
-------------

The strings you use for category names must be literal strings and this
library relies on the linker to merge the same literal strings that may be used
in different translation units. This means that the library may misbehave when
used with dynamically linked libraries.

Note that the same constraint applies when adding category filters through
``addFilter``. The string must not be dynamic as it is stored as a ``const
char*`` and it is *not* copied.

On allocations
--------------

This library does almost 0 allocation. It relies on pnt for formatting which
allocates memory only with floats and with types that need to be casted to
std::string for printing.

The only function that may allocate memory is ``addFilter`` which adds the
filter to a ``std::map``.

License
=======

The code is licensed under the BSD license provided in the COPYING file.

Author
======

Philippe Daouadi <p.daouadi@free.fr>
