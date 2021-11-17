A node-based Scene/Action editor based on rete.js inspired by Unreal blueprint.

Run it:
```
cd renderer

cocos compile -p emscripten

copy emscripten-build\bin\renderer\*.* ..

cd ..

:: start any http server you like at this path
http-server
```

Variable in data prefixed by $ are for editor-use only. Parser can ignore these values.