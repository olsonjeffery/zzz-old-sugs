return {
  into: (receiver, mixinClass) ->
    mixInst = new mixinClass()
    mixInst.doMix receiver
  MixinBase: class
    doMix: (receiver) ->
}
