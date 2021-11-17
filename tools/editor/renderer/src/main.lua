
cc.FileUtils:getInstance():setPopupNotify(false)

require "config"
require "cocos.init"
cc.exports.editor = require "editor"

local function main()
    local scene = cc.Scene:create()
    cc.Director:getInstance():runWithScene(scene)
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    print(msg)
end
