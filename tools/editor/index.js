(async function() {
    var container = document.querySelector("#rete");
    var components = [];
    for (var classname in window.editor.component.data) {
        var clz = window.editor.component.data[classname];
        components.push(new clz());
    }

    var editor = new Rete.NodeEditor("editor@0.0.1", container);
    editor.use(ConnectionPlugin.default);
    editor.use(VueRenderPlugin.default);
    editor.use(ContextMenuPlugin.default);
    editor.use(MinimapPlugin.default);
    editor.use(CommentPlugin.default);

    var engine = new Rete.Engine("editor@0.0.1");

    components.map(c => {
        editor.register(c);
        engine.register(c);
    });

    // load autosave
    var autosave = localStorage.getItem("autosave");
    if (autosave) {
        editor.fromJSON(JSON.parse(autosave));
        var editing = localStorage.getItem("editing");
        if (editing) {
            window.editor.editing = editing;
        }
    }

    // autosave
    var timeout = 0;
    editor.bind("reloadrenderer");
    editor.on("process nodecreated noderemoved connectionremoved connectioncreated reloadrenderer", async() => {
        clearTimeout(timeout);
        timeout = setTimeout(function() {
            var json = JSON.stringify(editor.toJSON());
            localStorage.setItem("autosave", json);
            if (window.editor.editing) {
                localStorage.setItem("editing", window.editor.editing);
            } else {
                localStorage.removeItem("editing");
            }
            
            if (editor.nodes[0].name == "cc.Scene") {
                Module.runtimeInitialized && Module.ccall("executeLuaString", 
                "number",
                ["string"],
                ["cc.Director:getInstance():replaceScene(editor.load(json.decode(" + JSON.stringify(json) + ")))"]);
            }
            else if (editor.nodes[0].name == "cc.Layer") {
                Module.runtimeInitialized && Module.ccall("executeLuaString", 
                "number",
                ["string"],
                [`local scene = cc.Scene:create()
                scene:addChild(editor.load(json.decode(${JSON.stringify(json)})))
                cc.Director:getInstance():replaceScene(scene)`]);
            } else {
                console.error("Cannot render " + editor.nodes[0].name)
            }
            
        }, 1000);
    });

    // if a node/action is connected as a child, add a new node/action socket
    editor.on("connectionremoved", async(e) => {
        if (e.input.socket.name == "cc.Node" && e.input.key.startsWith("child.")) {
            var node = e.input.node;
            node.data.$childrenCount--;
        }

        if (e.input.socket.name == "cc.Component" && e.input.key.startsWith("component.")) {
            var node = e.input.node;
            node.data.$componentCount--;
        }

        if (e.input.socket.name == "cc.Action" && e.input.key.startsWith("action.")) {
            var node = e.input.node;
            node.data.$actionCount--;
        }
    });

    editor.on("connectioncreated", async(e) => {
        if (e.input.socket.name == "cc.Node" && e.input.key.startsWith("child.")) {
            var node = e.input.node;
            node.data.$childrenCount++;

            // append a new Node socket when there is no free Node socket left (in order to add more child)
            var key = "child." + node.data.$childrenCount;
            if (!node.inputs.has(key)) {
                e.input.node.addInput(new Rete.Input(key, `Child(${node.data.$childrenCount})`, window.editor.socket("cc.Node")));
            }
        }

        if (e.input.socket.name == "cc.Component" && e.input.key.startsWith("component.")) {
            var node = e.input.node;
            node.data.$componentCount++;

            // append a new Component socket when there is no free Component socket left (in order to add more component)
            var key = "component." + node.data.$componentCount;
            if (!node.inputs.has(key)) {
                e.input.node.addInput(new Rete.Input(key, `Component(${node.data.$componentCount})`, window.editor.socket("cc.Component")));
            }
        }

        if (e.input.socket.name == "cc.Action" && e.input.key.startsWith("action.")) {
            var node = e.input.node;
            node.data.$actionCount++;

            // append a new Action socket when there is no free Action socket left (in order to add more Action)
            var key = "action." + node.data.$actionCount;
            if (!node.inputs.has(key)) {
                e.input.node.addInput(new Rete.Input(key, `Action(${node.data.$actionCount})`, window.editor.socket("cc.Action")));
            }
        }
    });

    editor.view.resize();
    editor.trigger("process");
    window.editor.rete = editor;
})();
