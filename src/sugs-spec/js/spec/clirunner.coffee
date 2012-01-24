fs = require 'fs'
spec = require 'spec'

# called from native code
$.startup ->
  puts "hello world from the startup!"

isATempFile = (p)->
  result = p.endsWith('~') or (p.startsWith('#') and p.endsWith('#'))
  puts "temp #{result} #{p}"
  result

isASpecsFile = (p) ->
  result = p.endsWith('specs.coffee') or p.endsWith('specs.js') or p.startsWith('specs')
  puts "spec #{result} #{p}"
  result

$.mainLoop 1, ->
  puts "rawPath: #{sugs.spec.rawPath}"
  dirs = sugs.spec.rawPath.split ';'
  testableList = []
  if dirs.length > 0
    for relPath in dirs
      fullPath = relPath
      files = fs.ls fullPath
      if files.length > 0
        for f in files
          if not isATempFile(f) and isASpecsFile(f)
            testableList.push(f)
  puts "testable files..."
  # load the scripts that contain specs
  for f in testableList
    puts "About to run spec script #{f}"
    sugs.spec.runScript f
  # now we have our specs loaded, so let's run them..
  results = spec.runner.run()
  puts "#{results.totalSpecs} Specs (in #{results.totalContexts} Contexts)"
  puts "#{results.successes} Passes, #{results.failures} Failures (#{results.contextFailures} Ctxs), #{results.errors} Errors and #{results.unimpl} Unimpl'd"