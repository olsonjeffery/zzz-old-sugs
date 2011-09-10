# called from native code
$.startup ->
  puts "hello world from the startup!"

$.mainLoop 1, ->
  puts "rawPath: #{sugs.specNative.rawPath}"
  dirs = sugs.specNative.rawPath.split ';'
  if dirs.length > 0
    for relPath in dirs
      fullPath = sugsConfig.moduleDir + relPath
      puts "fullPath: #{fullPath} fs: #{typeof(sugs.fsNative) == 'undefined'}"