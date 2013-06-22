libdogma
========

libdogma is an *ad hoc* fitting engine for the EVE Online multiplayer
game.

Features
--------

* Excellent accuracy (thanks to using the same expressions as the EVE
  client)

* Fast and low memory footprint (thanks to Judy arrays)

* Stable, heavily tested code base

* Customizable skill levels

* Customizable chance-based effects (booster side effects)

* Computes attributes of characters, skills, implants, ships, drones,
  modules and charges

* Supports projected effects

Missing features / Known issues
-------------------------------

* Gang/fleet bonuses are not supported yet

* Area effects (like wormhole effects) are not supported yet

Credits
-------

libdogma is released under the GNU General Public License, version 3
(or later). You can see the full license text in the `COPYING` file.

For the full list of libdogma contributors, see the `CREDITS` file.

libdogma uses Judy, a library released under the GNU Lesser General
Public License, version 2.1 (or later).
<http://judy.sourceforge.net/>

Contact
-------

* <artefact2@gmail.com>

Compiling libdogma
==================

Dependencies
------------

* A C11 compiler or, at least, a C99 compiler that supports anonymous
  unions and `_Static_assert` (`clang >= 3.1` is good);

* Judy arrays;

* A JSON cache dump of the game client (only if you are building from
  git, you can get one with
  [`phobos`](http://jira.evefit.org/browse/PHOBOS));

* The GNU Autotools suite (only if you are building from git).

Compiling recipe
----------------

If you are compiling from a tarball, you can directly jump to the
`./configure` step.

0. Generate the static data from the JSON dump:

   ~~~
   ./tools/bootstrap_from_json <path_to_JSON_dump>
   ~~~

1. If you are bulding from git, generate the `configure` script:

   ~~~
   ./bootstrap
   ~~~

2. Generate the `Makefile` (you can omit the debug flags or the `CC=`
   if you want to use `gcc`):

   ~~~
   CC=clang ./configure --enable-debug
   ~~~

3. Compile the library:

   ~~~
   make
   ~~~

4. Optionally, run the test suite:

   ~~~
   make check
   ~~~

5. Optionally, install the package:

   ~~~
   make install
   ~~~

Examples
========

See the source files in `test/` for examples.

Release checklist
=================

* Update version in `configure.ac`

* Update exported symbols in `src/libdogma.sym`

* Update `version-info` in `Makefile.am`
  ([doc](http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info))

* `make distclean && ./bootstrap && ./configure && make check`

* Commit, tag, push

* `make dist`
