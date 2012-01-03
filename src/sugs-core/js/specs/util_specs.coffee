{When, Before, It, After, assert, trap_error} = require 'spec'

When "getting a random uuid id", ->
  result = ''
  Before ->
    result = __native_random_uuid()

  It "should return one", ->
    assert result != ''

When "embedding an object within a namespace two levels deep, neither of which exist", ->
  outer = { bar : 'baz' }
  addedObj = { foo : 'bar' }
  namespaceRaw = [ 'hello', 'world' ]
  nsStr = namespaceRaw.join('.')
  Before ->
    embedObjectInNamespace outer, nsStr, addedObj

  It "will place the addedObject in the correct location", ->
    assert outer.hello.world.foo == 'bar'
  It "should not remove or reset any props on the outerObj", ->
    assert outer.bar == 'baz'

When "embedding an object within a namespace two levels deep, and the first level already exists", ->
  outer = { bar : 'baz' }
  prevObj = { world : 'goodbye' }
  addedObj = { foo : 'bar' }
  prevNamespaceStr = 'hello.cruel'
  namespaceRaw = [ 'hello', 'world' ]
  nsStr = namespaceRaw.join('.')
  Before ->
    embedObjectInNamespace outer, prevNamespaceStr, prevObj
    embedObjectInNamespace outer, nsStr, addedObj

  It "will place the addedObject in the correct location", ->
    assert outer.hello.world.foo == 'bar'

  It "should not overwrite the previous addition", ->
    assert outer.hello.cruel.world == 'goodbye'

When "embedding an object within a namespace and providing an empty namespace string", ->
  outer = { bar : 'baz' }
  addedObj = { foo : 'bar' }
  nsStr = ''
  error = null
  Before ->
    error = trap_error ->
      embedObjectInNamespace outer, nsStr, addedObj

  It "should cause an error", ->
    assert error != null

When "embedding an object within a namspace and providing a namespace that is only one level deep", ->
  outer = { bar : 'baz' }
  addedObj = { foo : 'bar' }
  namespaceRaw = [ 'hello' ]
  nsStr = namespaceRaw.join('.')
  Before ->
    embedObjectInNamespace outer, nsStr, addedObj

  It "should place the addedObject as a property of the outerObject", ->
    assert outer.hello.foo == 'bar'