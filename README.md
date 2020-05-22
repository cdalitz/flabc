flabc
=====

flabc is an editor for abc text files that is based on the
cross platform C++ GUI toolkit FLTK. For converting abc
files to postscript/pdf or midi it relies on the additional 
external software abctab2ps and abc2midi.


Installation from source
------------------------

Prerequisites:

  - C++98 compiler with STL support. Compilation has been tested
    with clang on MacOS X, gcc on Linux, and MingW on Windows
  - GNU make
  - FLTK 1.3.3 or later (on OSX: 1.3.4 or later),
    see https://www.fltk.org/

To compile *flabc* from the source code package, change to the subdirectory
*src* and type *make*. To install it on Linux, type ($ is the shell prompt):

	$ sudo make install

If you ever need to uninstall *flabc*, type *sudo make uninstall*.
Alternatively, you can use the ESP Package Manager *epm* (see
https://www.msweet.org/epm/), to create a RPM or DEB package and
install it (replace "platform" with your processor type and "x.y.z"
with the current version of flabc):

    $ # on rpm based systems like Red Hat or Suse:
    $ make rpm
	$ sudo rpm -i linux-platform/flabc-x.y.z.rpm

    $ # on deb based systems like Debian or Ubuntu:
    $ sudo make deb
	$ sudo dpkg -i linux-platform/flabc-x.y.z.rpm

To create an OSX application bundle *flabc.app*, you can use the script
*makeosxapp* in the root directory of this package. To create a Windows
binary installer with InnoSetup (see https://jrsoftware.org/isinfo.php),
you can use the script *makewindist*.


Binary packages
---------------

File releases and precompiled binary installers for different platforms are
available from http://www.lautengesellschaft.de/cdmm/.


Usage and configuration hints
-----------------------------

The main menu is accessible with the "Alt" key on Linux/Win32 and
with the "Ctrl + Alt" key on OSX. Hence the keyboard shortcut for
listening to a tune is "Ctrl + Alt + A, L" on OSX and
"Alt + A, L" on Linux.

On OSX you can drag & drop files on flabc. Alternatively you 
can open an abc file 'bla.abc' from the command line with 
"open -a flabc 'bla.abc'" when you have moced the application
bundle to some standard application directory (eg. $HOME/Applications).

When previewing or listening does not work check whether something
is wrong with the settings under "Options/System Environment".
On OSX the folder Contents/Resources/bin in the application bundle
is added to the search path by flabc automatically. It contains
a helper script "psview" that calls ghostscript for PDF conversion
and opens the PDF with Preview. This requires ghostscript to be installed.


Copyright and author
--------------------

Copyright (c) 2006-2020 by Christoph Dalitz

Official home page of flabc and abctab2ps:
http://www.lautengesellschaft.de/cdmm/

For licensing information, see the file LICENSE for details.
