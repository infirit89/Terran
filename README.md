# Terran

Terran is a Windows only (currently) 2D game engine. It also provides an editor, so that users can focus on making games.

---
## Features

* Scene System (based on EnTT)
* 2D Batch Renderer (based on OpenGL)
* 2D Physics System (based on Box2D)
* C# Scripting System (based on .Net 8)

There's more to come! You can see the engine's progress and planned features on our [Trello][trelloDef].

---
## How to build

> [!IMPORTANT]
> Only Visual Studio 2022 and 2019 are currently supported

First off get the engine by doing a:
```
git clone https://github.com/infirit89/Terran
```
in a directory you see fit

after that go into the ```Terran/scripts``` folder and run the ```Setup.bat``` script

if the script finished without errors, run the ```Win-VS-GenProject.bat``` script in the same folder, and choose the Visual Studio version which you have

a Visual Studio solution file should be generated at the root of the project. Open it and choose what you want to build:

* Terran Editor (which is the game editor)
* Sandbox (which is currently used to test out features, but is inteded to be the engine's runtime in the future)


[trelloDef]: https://trello.com/b/A212TiBI/terran
