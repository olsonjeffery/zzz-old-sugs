# sugs

...is a JavaScript _platform_ (ala [node.js](http://nodejs.org), [narwhal](http://nawwhaljs.org), the browser, etc) that targets multiuser, rich media applications. It is released under the ["Simplified", 2-clause BSD License](http://en.wikipedia.org/wiki/BSD_licenses#2-clause_license_.28.22Simplified_BSD_License.22_or_.22FreeBSD_License.22.29).

### It is, at its core:

* A hybrid native/JavaScript platform, comprised of one-or-more _Workers_
* Workers are shared-nothing, isolated JavaScript VMs
* [SpiderMonkey](http://developer.mozilla.org/en/SpiderMonkey) (that _other_ JavaScript engine) is the library used for JS execution
* Each Worker runs in its own OS thread, like:
  - One Worker blocking the main thread and managing the overall process lifecycle
  - Zero-or-more additional Workers running on separate threads, as needed
* Communication between Workers via a JSON-based, asynchronous message passing scheme
* All messages originate from a Worker to another known Worker, like:
  - _Worker-A_ _subscribes_ to a message via a string URI
  - _Worker-B_ (who may actually be Worker-A) _publishes_ a message for Worker-A (by it's unique identifier) _and the same URI_
  - Worker-A gets the message and processes it, eventually, in addition to any other work it may be doing
* Consequently, spawning order is important and natural parent/child hierarchies will form
* Each Worker is completely synchronous (and single-threaded) without the context of *its own lifetime*

### You also get:
* [CoffeeScript](http://coffee-script.org) as a first class language (and vanilla [ECMAScript 5.1](http://en.wikipedia.org/wiki/ECMAScript#Versions), as well)
* Bindings for 2D graphics and keyboard/mouse input using the SFML v2 library (`sugs-richclient`)
* 2D, rigid body physics via the [Chipmunk Game Dynamics](http://code.google.com/p/chipmunk-physics/) engine (currently in `sugs-core` as the `ChipmunkPhysicsComponent`, soon to broken into its own lib)

### A tad more on Workers (and Components):

* The Worker abstraction encapsulates:
  - OS threading
  - An isolated SpiderMonkey `JSRuntime*` and all the trimmings
  - An in-process, asynchronuos messaging mechanism (innovatively named the _MessageExchange_)
* Everything else is implemented via _Components_
* Components allow a Worker to take on additional capabilities
* This includes things like:
  - Running a user script (`ScriptRunnerComponent`
  - Bindings for various 3rd part libraries (`ChipmunkPhysicsComponent`, `RichClientComponent`)
  - General-purpose filesystem access (`FilesystemComponent`)
* As hinted at by the examples above, Components serve to both:
  - Embed new capabilities within a Worker besides the defaults (thread asynchronicity, JavaScript-execution and inter-Worker messaging)
  - Provide a way to deliberately compartmentalize/manage those capabilities
* A Worker's Components are specified at spawn-time

### JavaScript libraries out-of-the-box

* [CoffeeScript](http://coffee-script.org) 1.1.1
* [Underscore.js](http://documentcloud.github.com/underscore/) 1.1.7
* [Sugar.js](http://sugarjs.com) 0.9.4

### You got a unit testing framework?

Hells yeah, we do. It's called `sugs-spec` and follows the rather blah but useful _Context/Specification_ format.

### Things that sugs should be all about at some point in the future:

* Transparent, inter-process/network messaging
  - the existing abstraction ought to scale out with the addition of an onConnect type handler so that the 'receiver' of a new connection can be notified of remote Worker(s) trying to make contact with it
* WebGL abstractions that closely track how things work in the browser with 3D-capable canvases (this will be fun)
* Persistance support (google's [leveldb](http://code.google.com/leveldb) and/or [sqlite](http://sqlite.org)
* Windows and OSX support
  - currently all development is happening in a 64bit Ubuntu VM, but the above-named platforms are intended targets
* [libuv](http://github.com/joyent/libuv)-based event loops for all Workers (we truly are converging on [node.js](http://nodejs.org), heh)

### Want to know more?

Too bad.
