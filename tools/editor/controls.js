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
