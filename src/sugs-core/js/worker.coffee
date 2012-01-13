thisModule =
  spawn: (config) ->
    prefix = config.prefix
    componentJson = _.map config.components, (entry) ->
      { name: entry.name, configJson: entry.config }
    dataJson = JSON.stringify config.data
    'sdfsdfsdf'

return thisModule