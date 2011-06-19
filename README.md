# sugs
`sugs` is a JavaScript *platform* (ala [node.js](http://nodejs.org)) that targets multiuser, rich media applications. It supports [CoffeeScript](http://coffee-script.org) as a first class language (in fact, its what all of the non-native code in `sugs` in written with).

# OS platform support
Currently, 64bit linux is supported. 32bit linux should be trivial to support. 32bit windows support is planned (via mingw32). Initial OSX support will come via wineskin wrappers of a windows version.

### setup on a fresh ubuntu 64bit vm
1. `sudo apt-get build-dep libsfml`
2. `sudo apt-get install libnspr4-dev mercurial libasound2-dev libcurl4-openssl-dev libnotify-dev libxt-dev libiw-dev mesa-common-dev autoconf2.13 yasm libmozjs-dev`
3. You need to have `node.js` and `npm` installed in order to pack your app for release. Once done, install `uglify.js` with `npm install -g uglify-js`
