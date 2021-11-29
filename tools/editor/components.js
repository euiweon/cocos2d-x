Rete.Component.prototype.bind = function (node) {
    this._node = node;
    return this;
}

Rete.Component.prototype.addProperty = function(param) {
    if (this._node.data[param.key] === undefined) {
        this._node.data[param.key] = param.default;
    }
    var input = new Rete.Input(param.key, param.socket.name, param.socket);
    input.addControl(new param.control(this.editor, param.key, param.readonly || false, param.default));
    this._node.addInput(input);
    return this;
}

Rete.Component.prototype.addNodeInputs = function() {
    // actions
    this._node.data.$actionCount = this._node.data.$actionCount || 0;
    for (var i = 0; i <= this._node.data.$actionCount; i++) {
        this._node.addInput(new Rete.Input(`action.${i}`, `Action(${i})`, editor.socket("cc.Action")));
    }
    this._node.data.$actionCount = 0; // will be restored when action connect
    
    // components
    this._node.data.$componentCount = this._node.data.$componentCount || 0;
    for (var i = 0; i <= this._node.data.$componentCount; i++) {
        this._node.addInput(new Rete.Input(`component.${i}`, `Component(${i})`, editor.socket("cc.Component")));
    }
    this._node.data.$componentCount = 0; // will be restored when component connect

    // nodes
    this._node.data.$childrenCount = this._node.data.$childrenCount || 0;
    for (var i = 0; i <= this._node.data.$childrenCount; i++) {
        this._node.addInput(new Rete.Input(`child.${i}`, `Child(${i})`, editor.socket("cc.Node")));
    }
    this._node.data.$childrenCount = 0; // will be restored when node connect
}

Rete.Component.prototype.addCustomInputs = function() {
    if (!this._node.data.$customInputs) {
        return this;
    }

    for (var key in this._node.data.$customInputs) {
        var typename = this._node.data.$customInputs[key];
        this.addProperty({
            key: key,
            default: editor.default(typename),
            socket: editor.socket(typename),
            control: editor.control(typename)
        })
    }

    return this;
}

Rete.Component.prototype.addCustomOutputs = function() {
    if (!this._node.data.$customOutputs) {
        return this;
    }

    for (var key in this._node.data.$customOutputs) {
        var typename = this._node.data.$customOutputs[key];
        this._node.addOutput(new Rete.Output(key, key, editor.socket(typename), true));
    }

    return this;
}

editor.socket = function(typename) {
    var data = this.socket.data;
    if (!data[typename]) {
        data[typename] = new Rete.Socket(typename);
    }

    return data[typename];
};
editor.socket.data = {};

editor.component = function(typename) {
    return this.component.data[typename];
};

editor.component.data = {
    "cc.Node": class extends Rete.Component {
        constructor() {
            super("cc.Node");
        }
    
        builder(node) {
            this.bind(node)
            .addProperty({
                key: "position",
                default: {x: 0, y: 0},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "rotation",
                default: 0,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
            .addProperty({
                key: "scale",
                default: {x: 1, y: 1},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "anchorPoint",
                default: {x: 0.5, y: 0.5},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "contentSize",
                default: {width: 0, height: 0},
                socket: editor.socket("cc.Size"),
                control: editor.control("cc.Size")
            })
            .addProperty({
                key: "color",
                default: {r: 255, g: 255, b: 255},
                socket: editor.socket("cc.Color3B"),
                control: editor.control("cc.Color3B")
            })
            .addProperty({
                key: "opacity",
                default: 255,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
            node.addInput(new Rete.Input("action", "Action", editor.socket("cc.Action")));
            this.addNodeInputs();
            node.addOutput(new Rete.Output("output", "Node", editor.socket("cc.Node"), true));
        }
    
        worker(node, inputs, outputs) {}
    },
    "cc.ParticleSystemQuad": class extends Rete.Component {
        constructor() {
            super("cc.ParticleSystemQuad");
        }
    
        builder(node) {
            this.bind(node)
            .addProperty({
                key: "position",
                default: {x: 0, y: 0},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "rotation",
                default: 0,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
            .addProperty({
                key: "scale",
                default: {x: 1, y: 1},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "anchorPoint",
                default: {x: 0.5, y: 0.5},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "contentSize",
                default: {width: 0, height: 0},
                socket: editor.socket("cc.Size"),
                control: editor.control("cc.Size")
            })
            .addProperty({
                key: "color",
                default: {r: 255, g: 255, b: 255},
                socket: editor.socket("cc.Color3B"),
                control: editor.control("cc.Color3B")
            })
            .addProperty({
                key: "opacity",
                default: 255,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
            .addProperty({
                key: "filename",
                default: "",
                socket: editor.socket("string"),
                control: editor.control("string")
            })
            node.addInput(new Rete.Input("action", "Action", editor.socket("cc.Action")));
            this.addNodeInputs();
            node.addOutput(new Rete.Output("output", "Node", editor.socket("cc.Node"), true));
        }
    
        worker(node, inputs, outputs) {}
    },
    "cc.Sprite": class extends Rete.Component {
        constructor() {
            super("cc.Sprite");
        }
    
        builder(node) {
            this.bind(node)
            .addProperty({
                key: "position",
                default: {x: 0, y: 0},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "rotation",
                default: 0,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
            .addProperty({
                key: "scale",
                default: {x: 1, y: 1},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "anchorPoint",
                default: {x: 0.5, y: 0.5},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "contentSize",
                default: {width: 0, height: 0},
                socket: editor.socket("cc.Size"),
                control: editor.control("cc.Size")
            })
            .addProperty({
                key: "color",
                default: {r: 255, g: 255, b: 255},
                socket: editor.socket("cc.Color3B"),
                control: editor.control("cc.Color3B")
            })
            .addProperty({
                key: "opacity",
                default: 255,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
            .addProperty({
                key: "filename",
                default: "",
                socket: editor.socket("string"),
                control: editor.control("string")
            })
            node.addInput(new Rete.Input("action", "Action", editor.socket("cc.Action")));
            this.addNodeInputs();
            node.addOutput(new Rete.Output("output", "Node", editor.socket("cc.Node"), true));
        }
    
        worker(node, inputs, outputs) {}
    },
    "cc.Label": class extends Rete.Component {
        constructor() {
            super("cc.Label");
        }
    
        builder(node) {
            this.bind(node)
            .addProperty({
                key: "position",
                default: {x: 0, y: 0},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "rotation",
                default: 0,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
            .addProperty({
                key: "scale",
                default: {x: 1, y: 1},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "anchorPoint",
                default: {x: 0.5, y: 0.5},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "contentSize",
                default: {width: 0, height: 0},
                socket: editor.socket("cc.Size"),
                control: editor.control("cc.Size")
            })
            .addProperty({
                key: "color",
                default: {r: 255, g: 255, b: 255},
                socket: editor.socket("cc.Color3B"),
                control: editor.control("cc.Color3B")
            })
            .addProperty({
                key: "opacity",
                default: 255,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
            .addProperty({
                key: "text",
                default: "",
                socket: editor.socket("string"),
                control: editor.control("string")
            })
            .addProperty({
                key: "fontFilePath",
                default: "",
                socket: editor.socket("string"),
                control: editor.control("string")
            })
            .addProperty({
                key: "fontSize",
                default: 20,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
            .addProperty({
                key: "textColor",
                default: {r: 255, g: 255, b: 255},
                socket: editor.socket("cc.Color3B"),
                control: editor.control("cc.Color3B")
            })
            .addProperty({
                key: "shadowEnabled",
                default: false,
                socket: editor.socket("bool"),
                control: editor.control("bool")
            })
            .addProperty({
                key: "shadowColor",
                default: "#000000",
                socket: editor.socket("cc.Color3B"),
                control: editor.control("cc.Color3B")
            })
            .addProperty({
                key: "shadowOpacity",
                default: 255,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
            .addProperty({
                key: "shadowOffset",
                default: {width: 0, height: 0},
                socket: editor.socket("cc.Size"),
                control: editor.control("cc.Size")
            })
            .addProperty({
                key: "outlineEnabled",
                default: false,
                socket: editor.socket("bool"),
                control: editor.control("bool")
            })
            .addProperty({
                key: "outlineColor",
                default: "#000000",
                socket: editor.socket("cc.Color3B"),
                control: editor.control("cc.Color3B")
            })
            .addProperty({
                key: "outlineOpacity",
                default: 255,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
            .addProperty({
                key: "outlineSize",
                default: 1,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
            
            node.addInput(new Rete.Input("action", "Action", editor.socket("cc.Action")));
            this.addNodeInputs();
            node.addOutput(new Rete.Output("output", "Node", editor.socket("cc.Node"), true));
        }
    
        worker(node, inputs, outputs) {}
    },
    "cc.Scene": class extends Rete.Component {
        constructor() {
            super("cc.Scene");
        }
    
        builder(node) {
            this.bind(node);
            this.addNodeInputs();
        }
    
        worker(node, inputs, outputs) {}
    },
    "cc.ComponentLua": class extends Rete.Component {
        constructor() {
            super("cc.ComponentLua");
        }
    
        builder(node) {
            this.bind(node)
            .addProperty({
                key: "filename",
                default: "",
                socket: editor.socket("string"),
                control: editor.control("string")
            })
            .addCustomInputs();
            
            node.addOutput(new Rete.Output("output", "Component", editor.socket("cc.Component"), true));
        }
    
        worker(node, inputs, outputs) {}
    },
    "cc.MoveTo": class extends Rete.Component {
        constructor() {
            super("cc.MoveTo");
        }
    
        builder(node) {
            this.bind(node)
            .addProperty({
                key: "position",
                default: {x: 0, y: 0},
                socket: editor.socket("cc.Vec2"),
                control: editor.control("cc.Vec2")
            })
            .addProperty({
                key: "duration",
                default: 0,
                socket: editor.socket("float"),
                control: editor.control("float")
            })
    
            node.addOutput(new Rete.Output("output", "Action", editor.socket("cc.Action"), true));
        }
    
        worker(node, inputs, outputs) {}
    },
    // A special component which only executes once and the output will always be the one of that execution.
    "singleton": class extends Rete.Component {
        constructor() {
            super("singleton");
        }
    
        builder(node) {
            node.addInput(new Rete.Input("input", "Node", editor.socket("cc.Node")));
            node.addOutput(new Rete.Output("output", "Node", editor.socket("cc.Node"), true));
        }
    
        worker(node, inputs, outputs) {}
    },
    "LuaFunction": class extends Rete.Component {
        constructor() {
            super("LuaFunction");
        }
    
        builder(node) {
            this.bind(node)
            .addProperty({
                key: "filepath",
                default: "",
                socket: editor.socket("string"),
                control: editor.control("string")
            })
            .addCustomInputs()
            .addCustomOutputs();
        }
    
        worker(node, inputs, outputs) {}
    }
};
