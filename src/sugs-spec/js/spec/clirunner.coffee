fs = require 'fs'
spec = require 'spec'
worker = require 'worker'

#############
# UTIL FUNCS
#############
isATempFile = (p)->
  result = p.endsWith('~') or (p.startsWith('#') and p.endsWith('#'))
  puts "temp #{result} #{p}"
  result

isASpecsFile = (p) ->
  result = p.endsWith('specs.coffee') or p.endsWith('specs.js') or p.startsWith('specs')
  puts "spec #{result} #{p}"
  result

########
# SETUP
########
rawPath = worker.current.getData().rawPath
puts "rawPath: #{rawPath}"
dirs = rawPath.split ';'
testableList = []
if dirs.length > 0
  for relPath in dirs
    fullPath = relPath
    files = fs.ls fullPath
    if files.length > 0
      for f in files
        if not isATempFile(f) and isASpecsFile(f)
          testableList.push(f)
# load the scripts that contain specs
runSpecs = true
for f in testableList
  puts "About to run spec script #{f}"
  try
    sugs.api.spec.runScript f
  catch e
    puts "FAILURE TO PARSE SPEC FILE: #{f}"
    puts e
    runSpecs = false
# now we have our specs loaded, so let's run them..

############
# RUN SPECS
############
if runSpecs
  results = spec.runner.run()
  puts "#{results.totalSpecs} Specs (in #{results.totalContexts} Contexts)"
  puts "#{results.successes} Passes, #{results.failures} Failures (#{results.contextFailures} Ctxs), #{results.errors} Errors and #{results.unimpl} Unimpl'd"