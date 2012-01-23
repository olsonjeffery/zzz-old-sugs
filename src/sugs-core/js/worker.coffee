thisModule =
  spawn: (config) ->
    prefix = config.prefix
    componentJson = _.map config.components, (entry) ->
      configObj = entry.config
      if typeof configObj == 'undefined'
        configObj = {}
      { name: entry.name, configJson: JSON.stringify(configObj) }
    dataJson = "{}"
    if typeof(config.data) != 'undefined'
      dataJson = JSON.stringify config.data
    sugs.api.worker.spawn prefix, componentJson, dataJson

return thisModule