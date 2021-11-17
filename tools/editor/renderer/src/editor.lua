local editor = {}

-- forward declaration
local parse

local function c3b(color)
  return {
    r = tonumber(string.sub(color, 2, 3), 16),
    g = tonumber(string.sub(color, 4, 5), 16),
    b = tonumber(string.sub(color, 6, 7), 16)
  }
end

local function c4b(color, opacity)
  local result = c3b(color)
  result.a = opacity or 255
  return result
end

local function readNodeProperty(node, data)
  node:setPosition(data.position)
  node:setRotation(data.rotation)
  node:setScale(data.scale.x, data.scale.y)
  node:setAnchorPoint(data.anchorPoint)
  node:setContentSize(data.contentSize)
  node:setColor(c3b(data.color))
  node:setOpacity(data.opacity)
end

local function readLabelProperty(label, data)
  label:setTextColor(c4b(data.textColor))

  if data.outlineEnabled then
    label:enableOutline(c4b(data.outlineColor, data.outlineOpacity), data.outlineSize)
  end

  if data.shadowEnabled then
    label:enableShadow(c4b(data.shadowColor, data.shadowOpacity), data.shadowOffset)
  end
end

local types = {
  ["cc.Scene"] = {
    class = cc.Scene,
    constructor = cc.Scene.create,
    initializers = {},
    readers = {}
  },
  ["cc.Node"] = {
    class = cc.Node,
    constructor = cc.Node.create,
    initializers = {},
    readers = {readNodeProperty}
  },
  ["cc.Sprite"] = {
    class = cc.Sprite,
    constructor = cc.Sprite.create,
    initializers = {"filename"},
    readers = {readNodeProperty}
  },
  ["cc.ParticleSystemQuad"] = {
    class = cc.ParticleSystemQuad,
    constructor = cc.ParticleSystemQuad.create,
    initializers = {"filename"},
    readers = {readNodeProperty}
  },
  ["cc.Label"] = {
    class = cc.Label,
    constructor = cc.Label.createWithTTF,
    initializers = {"text", "fontFilePath", "fontSize"},
    readers = {readNodeProperty, readLabelProperty},
  },
  ["cc.ComponentLua"] = {
    class = cc.ComponentLua,
    constructor = cc.ComponentLua.create,
    initializers = {"filename"},
    readers = {},
  },
  ["cc.MoveTo"] = {
    class = cc.MoveTo,
    constructor = cc.MoveTo.create,
    initializers = {"duration", "position"},
    readers = {},
  },
  ["i18n"] = {
    constructor = i18n,
    initializers = {"path"},
    readers = {},
  },
}

parse = function(reteRoot, key)
  local reteNode = reteRoot.nodes[key]
  local className = reteNode.name

  -- singleton is special component which only executes once and always return the result of that execution
  if className == "singleton" then
    if reteNode["$$"] then
      return reteNode["$$"].output
    end
  end

  local children = {}
  local actions = {}
  local components = {}
  local inputs = {} -- used as cache
  -- shallow copy so leave reteNode.data untouched
  local data = {}
  for k, v in pairs(reteNode.data) do
    data[k] = v
  end

  -- parse inputs before creating node because it might need values here
  for name, input in pairs(reteNode.inputs) do
    local conn = input.connections[1]
    if conn then
      local value = parse(reteRoot, tostring(conn.node))
      inputs[name] = value

      if string.sub(name, 1, 6) == "child." then
        table.insert(children, value)
      elseif string.sub(name, 1, 10) == "component." then
        table.insert(components, value)
      elseif string.sub(name, 1, 7) == "action." then
        table.insert(actions, value)
      else
        data[name] = value
      end
    end
  end

  local type = types[className]
  local paramKeys = type.initializers
  local paramValues = {}
  for i, v in ipairs(paramKeys) do
    paramValues[i] = data[v]
  end

  local class = type.class
  local node
  if class then
    node = type.constructor(class, unpack(paramValues))
  else
    node = type.constructor(unpack(paramValues))
  end

  assert(node ~= nil , "failed to parse node " .. key)

  local readers = type.readers
  for _, reader in ipairs(readers) do
    reader(node, data)
  end

  for _, child in ipairs(children) do
    node:addChild(child)
  end

  for _, action in ipairs(actions) do
    node:runAction(action)
  end

  for _, component in ipairs(components) do
    node:addComponent(component)
  end

  -- cache inputs and output
  reteNode["$$"] = {
    output = node,
    inputs = inputs
  }
  return node
end

editor.load = function (data)
  return parse(data, "1")
end

return editor
