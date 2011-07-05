# sugs
`sugs` is a JavaScript *platform* (ala [node.js](http://nodejs.org), [narwhal](http://nawwhaljs.org), the browser, etc) that targets multiuser, rich media applications.

It emphasizes:
* [CoffeeScript](http://coffee-script.org) as a first class languae
  (JavaScript is also supported).
* A shared-nothing, "worker"-based runtime environment where each worker is an
  isolated JavaScript VM, running in its own OS thread.
* Workers that communicate via a JSON-based, asynchronous message passing
  system. Messages can either be broadcast to any receiver who is
  subscribed to a message (by a message identifier), or sent to a specific
  recipient and message identifier.
* Bindings for 2D graphics using the SFML library.

Things that should be added to the above list soon:
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

`sugs` is released under the ["Simplified", 2-clause BSD License](http://en.wikipedia.org/wiki/BSD_licenses#2-clause_license_.28.22Simplified_BSD_License.22_or_.22FreeBSD_License.22.29).

# OS platform support
Currently, only 64bit linux is supported (as this is where all of the dev is occuring). 32bit linux should be trivial to support. 32bit windows support is planned. Initial OSX support will come via wineskin wrappers of a windows version.

### setup on a fresh ubuntu 64bit vm
1. `sudo apt-get build-dep libsfml libmozjs-dev`
2. `sudo apt-get install libnspr4-dev mercurial libasound2-dev libcurl4-openssl-dev libnotify-dev libxt-dev libiw-dev mesa-common-dev autoconf2.13`
