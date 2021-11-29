editor.control = function(typename) {
    return this.control.data[typename];
};

editor.control.data = {
    "cc.Vec2": class extends Rete.Control {
        constructor(emitter, key, readonly) {
            super(key);
            this.component = {
                props: ["readonly", "emitter", "ikey", "getData", "putData"],
                template: `<div>
                ${key}
                <input type="number" class="grid1x1" data-field="y" :readonly="readonly" :value="value.y" @input="change($event)" @dblclick.stop=""/>
                <input type="number" class="grid1x1" data-field="x" :readonly="readonly" :value="value.x" @input="change($event)" @dblclick.stop=""/>
                </div>`,
    
                data() {
                    return {
                        value: { x: 0, y: 0 },
                    }
                },
                methods: {
                    change(e) {
                        this.value[e.target.dataset.field] = +e.target.value;
                        this.update();
                    },
                    update() {
                        if (this.ikey)
                            this.putData(this.ikey, this.value)
                        this.emitter.trigger("process");
                    }
                },
                mounted() {
                    this.value = this.getData(this.ikey);
                }
            };
            this.props = { emitter, ikey: key, readonly };
        }
    
        setValue(val) {
            this.vueContext.value = val;
        }
    },
    "cc.Size": class extends Rete.Control {
        constructor(emitter, key, readonly) {
            super(key);
            this.component = {
                props: ["readonly", "emitter", "ikey", "getData", "putData"],
                template: `<div>
                ${key}
                <input type="number" class="grid1x1" data-field="height" :readonly="readonly" :value="value.height" @input="change($event)" @dblclick.stop=""/>
                <input type="number" class="grid1x1" data-field="width" :readonly="readonly" :value="value.width" @input="change($event)" @dblclick.stop=""/>
                </div>`,
    
                data() {
                    return {
                        value: { width: 0, height: 0 },
                    }
                },
                methods: {
                    change(e) {
                        this.value[e.target.dataset.field] = +e.target.value;
                        this.update();
                    },
                    update() {
                        if (this.ikey)
                            this.putData(this.ikey, this.value)
                        this.emitter.trigger("process");
                    }
                },
                mounted() {
                    this.value = this.getData(this.ikey);
                }
            };
            this.props = { emitter, ikey: key, readonly };
        }
    
        setValue(val) {
            this.vueContext.value = val;
        }
    },
    "float": class extends Rete.Control {
        constructor(emitter, key, readonly) {
            super(key);
            this.component = {
                props: ["readonly", "emitter", "ikey", "getData", "putData"],
                template: `<div>
                ${key}
                <input type="number" class="grid2x1" :readonly="readonly" :value="value" @input="change($event)" @dblclick.stop=""/>
                </div>`,
    
                data() {
                    return {
                        value: 0,
                    }
                },
                methods: {
                    change(e) {
                        this.value = +e.target.value;
                        this.update();
                    },
                    update() {
                        if (this.ikey)
                            this.putData(this.ikey, this.value)
                        this.emitter.trigger("process");
                    }
                },
                mounted() {
                    this.value = this.getData(this.ikey);
                }
            };
            this.props = { emitter, ikey: key, readonly };
        }
    
        setValue(val) {
            this.vueContext.value = val;
        }
    },
    "string": class extends Rete.Control {
        constructor(emitter, key, readonly) {
            super(key);
            this.component = {
                props: ["readonly", "emitter", "ikey", "getData", "putData"],
                template: `<div>
                ${key}
                <input type="text" class="grid2x1" :readonly="readonly" :value="value" @input="change($event)" @dblclick.stop=""/>
                </div>`,
    
                data() {
                    return {
                        value: "",
                    }
                },
                methods: {
                    change(e) {
                        this.value = e.target.value;
                        this.update();
                    },
                    update() {
                        if (this.ikey)
                            this.putData(this.ikey, this.value)
                        this.emitter.trigger("process");
                    }
                },
                mounted() {
                    this.value = this.getData(this.ikey);
                }
            };
            this.props = { emitter, ikey: key, readonly };
        }
    
        setValue(val) {
            this.vueContext.value = val;
        }
    },
    "cc.Color3B": class extends Rete.Control {
        constructor(emitter, key, readonly) {
            super(key);
            this.component = {
                props: ["readonly", "emitter", "ikey", "getData", "putData"],
                template: `<div>
                ${key}
                <input type="color" class="grid2x1" :readonly="readonly" :value="value" @input="change($event)" @dblclick.stop=""/>
                </div>`,
    
                data() {
                    return {
                        value: "#ffffff",
                    }
                },
                methods: {
                    change(e) {
                        this.value = e.target.value;
                        this.update();
                    },
                    update() {
                        if (this.ikey) {
                            var rgb = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(this.value);
                            rgb = rgb ? 
                            {
                                r: parseInt(rgb[1], 16),
                                g: parseInt(rgb[2], 16),
                                b: parseInt(rgb[3], 16)
                            } : 
                            {
                                r: 255,
                                g: 255,
                                b: 255
                            };
                            this.putData(this.ikey, rgb);
                        }
                            
                        this.emitter.trigger("process");
                    }
                },
                mounted() {
                    var rgb = this.getData(this.ikey);
                    var hex = "#" + 
                    (rgb.r + 256).toString(16).substring(1) + 
                    (rgb.g + 256).toString(16).substring(1) + 
                    (rgb.b + 256).toString(16).substring(1);
                    this.value = hex;
                }
            };
            this.props = { emitter, ikey: key, readonly };
        }
    
        setValue(val) {
            this.vueContext.value = val;
        }
    },
    "bool": class extends Rete.Control {
        constructor(emitter, key, readonly) {
            super(key);
            this.component = {
                props: ["readonly", "emitter", "ikey", "getData", "putData"],
                template: `<div>
                ${key}
                <input type="checkbox" :readonly="readonly" :checked="value" @input="change($event)" @dblclick.stop=""/>
                </div>`,
    
                data() {
                    return {
                        value: false,
                    }
                },
                methods: {
                    change(e) {
                        this.value = e.target.checked ? true : false;
                        this.update();
                    },
                    update() {
                        if (this.ikey)
                            this.putData(this.ikey, this.value)
                        this.emitter.trigger("process");
                    }
                },
                mounted() {
                    this.value = this.getData(this.ikey);
                }
            };
            this.props = { emitter, ikey: key, readonly };
        }
    
        setValue(val) {
            this.vueContext.value = val;
        }
    }
};

var customInputDropDown = document.getElementById("customInputDropDown")
for (let typename in editor.control.data) {
    var a = document.createElement("a");
    a.innerText = "Add Custom Input(" + typename + ")";
    a.href = "#"
    a.onclick = function() {
        var selected = editor.rete.selected.list[0]
        if (selected) {
            var inputName = window.prompt("Please enter input name", "Untitled");
            if (!inputName) {
                return;
            }
            
            // add new input, this can error by duplicated name so data left intact
            var input = new Rete.Input(inputName, typename, editor.socket(typename));
            var controlClazz = editor.control(typename);
            input.addControl(new controlClazz(editor.rete, inputName, false));
            selected.addInput(input);
            selected.data[inputName] = editor.default(typename)

            // update data 
            if (!selected.data.$customInputs) {
                selected.data.$customInputs = {}
            }
            selected.data.$customInputs[inputName] = typename;

            // auto save
            var json = JSON.stringify(editor.rete.toJSON());
            localStorage.setItem("autosave", json);
        }
        return false;
    };
    customInputDropDown.appendChild(a);
}


var customOutputDropDown = document.getElementById("customOutputDropDown")
for (let typename in editor.control.data) {
    var a = document.createElement("a");
    a.innerText = "Add Custom Output(" + typename + ")";
    a.href = "#"
    a.onclick = function() {
        var selected = editor.rete.selected.list[0]
        if (selected) {
            var outputName = window.prompt("Please enter output name", "output");
            if (!outputName) {
                return;
            }
            
            selected.addOutput(new Rete.Output(outputName, outputName, editor.socket(typename), false));

            // update data 
            if (!selected.data.$customOutputs) {
                selected.data.$customOutputs = {}
            }
            selected.data.$customOutputs[outputName] = typename;

            // auto save
            var json = JSON.stringify(editor.rete.toJSON());
            localStorage.setItem("autosave", json);
        }
        return false;
    };
    customOutputDropDown.appendChild(a);
}
