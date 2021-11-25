editor.default = function(typename) {
  return this.default.data[typename];
};

editor.default.data = {
  "cc.Vec2": { x: 0, y: 0 },
  "cc.Size": { width: 0, height: 0 },
  "float": 0,
  "string": "",
  "cc.Color3B": "#ffffff",
  "bool": false
};
