local editor = {}

-- forward declaration
local parse

-- policies
local customAsProperties = {} -- assaign custom input values to lua object

local function readNodeProperty(node, data)
  node:setPosition(data.position)
  node:setRotation(data.rotation)
  node:setScale(data.scale.x, data.scale.y)
  node:setAnchorPoint(data.anchorPoint)
  node:setContentSize(data.contentSize)
  node:setColor(data.color)
  node:setOpacity(data.opacity)
end

local function readLabelProperty(label, data)
  local color = {
    r = data.textColor.r,
    g = data.textColor.g,
    b = data.textColor.b,
    a = 255
  }
  label:setTextColor(color)

  if data.outlineEnabled then
    local color = {
      r = data.outlineColor.r,
      g = data.outlineColor.g,
      b = data.outlineColor.b,
      a = data.outlineOpacity
    }
    label:enableOutline(color, data.outlineSize)
  end

  if data.shadowEnabled then
    local color = {
      r = data.shadowColor.r,
      g = data.shadowColor.g,
      b = data.shadowColor.b,
      a = data.shadowOpacity
    }
    label:enableShadow(color, data.shadowOffset)
  end
end

local types = {
  ["cc.Scene"] = {
    constructor = function ()
      return cc.Scene:create()
    end,
    readers = {}
  },
  ["cc.Node"] = {
    constructor = function ()
      return cc.Node:create()
    end,
    readers = {readNodeProperty}
  },
  ["cc.Sprite"] = {
    constructor = function (data)
      return cc.Sprite:create(data.filename)
    end,
    readers = {readNodeProperty}
  },
  ["cc.ParticleSystemQuad"] = {
    constructor = function (data)
      return cc.ParticleSystemQuad:create(data.filename)
    end,
    readers = {readNodeProperty}
  },
  ["cc.Label"] = {
    constructor = function (data)
      return cc.Label:createWithTTF(data.text, data.fontFilePath, data.fontSize)
    end,
    readers = {readNodeProperty, readLabelProperty},
  },
  ["cc.ComponentLua"] = {
    constructor = function (data)
      return cc.ComponentLua:create(data.filename)
    end,
    readers = {},
    policies = {customAsProperties}
  },
  ["cc.MoveTo"] = {
    constructor = function (data)
      return cc.MoveTo:create(data.duration, data.position)
    end,
    readers = {},
  },
  ["singleton"] = {
    constructor = function (data)
      return data.input
    end,
    readers = {},
  },
  ["LuaFunction"] = {
    constructor = function (data)
      return require(data.filepath)(data)
    end,
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
  local node = type.constructor(data)
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

  if type.policies then
    for _, policy in ipairs(type.policies) do
      if policy == customAsProperties then
        for k, _ in pairs(data["$customInputs"]) do
          node[k] = data[k]
        end
      end
    end
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
