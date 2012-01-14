thisModule =
  spawn: (config) ->
    prefix = config.prefix
    componentJson = _.map config.components, (entry) ->
      configObj = entry.config
      if typeof configObj == 'undefined'
        configObj = {}
      { name: entry.name, configJson: JSON.stringify(configObj) }
    sugs.api.worker.spawn prefix, componentJson

return thisModule