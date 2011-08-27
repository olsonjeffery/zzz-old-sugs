# sugs

`sugs` is a JavaScript *platform* (ala [node.js](http://nodejs.org), [narwhal](http://nawwhaljs.org), the browser, etc) that targets multiuser, rich media applications. It is released under the ["Simplified", 2-clause BSD License](http://en.wikipedia.org/wiki/BSD_licenses#2-clause_license_.28.22Simplified_BSD_License.22_or_.22FreeBSD_License.22.29).

### Sugs is all about

* A shared-nothing, "worker"-based runtime environment where each worker is an
  isolated JavaScript VM, running in its own OS thread.
* Communication between Workers via a JSON-based, asynchronous message passing
  system. Messages can either be broadcast to any receiver who is
  subscribed to a message (by a string-based message identifier), or sent to a specific
  recipient and message identifier.
* Bindings for 2D graphics using the SFML library.
* [CoffeeScript](http://coffee-script.org) as a first class language.
  (JavaScript is supported as well, of course).
* 2D, rigid body physics via the [Chipmunk Game Dynamics](http://code.google.com/p/chipmunk-physics/) engine.

### Techically speaking

A `sugs` application looks like:

* A C++ application that takes a dependency on the various `sugs` shared
  libraries.
* One-or-more work processor(s) (referred to here as a Worker). It encapsulates a
  JavaScript runtime and whatever functionality (graphics, physics,
  network management, etc) that is added to it in a compositional manner.
  Workers use the excellent Mozilla SpiderMonkey JavaScript engine (the JS
  engine for Firefox, amongst other things). A worker typically runs
  one-per-thread (including the main thread, usually reserved for a Worker
  with rich client capabilities to handle UX and user IO).
* The `sugs` shared libraries expose various components that a developer can
  use to compose specific, task-oriented Workers that run their own
  event loop and can iterate over various tasks germaine to their purpose
  (manage world state, do AI tasks, run a graphics frontend and user IO, etc).
* A user's application code in CoffeeScript/JavaScript. The native component
  system gives developers the patterns to push performance intensive
  code paths down into native code (developers can write their own
  components in C++, in addition to using ones from the `sugs` library)
  while exposing it cleanly for use by code running in SpiderMonkey.
* JavaScript code running in a given Worker is sandboxed within that
  Worker. Each Worker runs it's own JavaScript *runtime* and *all*
  information sharing happens via JSON message passing (you can't pass
  functions or native-bound JavaScript objects between Workers, either).
  This design decision scales transparently to network communications,
  as well.

### JavaScript libraries out-of-the-box

* [CoffeeScript](http://coffee-script.org) 1.1.1
* [Underscore.js](http://documentcloud.github.com/underscore/) 1.1.7
* [Sugar.js](http://sugarjs.com) 0.9.4

### Things that sugs should be all about at some point in the future:

* Message passing transparently scaled to support network-based
  communication between multiple `sugs` processes (either local or
remote).
* More graphics bindings (2D/3D particle effects, 3D bindings, etc).
* Multiple approaches to running client code:
  * Use a pre-built executable to orchestrate and run your scripts,
    designating one to run in the main thread, display UX and handle
    user IO.
  * A repl ala node, coffee, etc.
  * A way to custom build client apps with native c/c++ as needed
    (pretty much the current setup).

### OS platform support

Currently, only 64bit linux is supported (as this is where all of the dev is occuring). 32bit linux should be trivial to support. 32bit windows support is planned. Initial OSX support will come via wineskin wrappers of a windows version.

### setup on a fresh ubuntu 64bit vm

1. `sudo apt-get build-dep libsfml libmozjs-dev`
2. `sudo apt-get install libnspr4-dev mercurial libasound2-dev libcurl4-openssl-dev libnotify-dev libxt-dev libiw-dev mesa-common-dev autoconf2.13`
