# EmGL

OpenGL sample for crossplatform with GLUT and GLM.

## How to Build

### Linux(Debian)

```sh
sudo aptitude install cmake freeglut3-dev libglm-dev
cmake .
make
```

You can get a build/bin/emgl executable.

### Mac

```sh
brew install cmake freeglut glm
cmake .
make
```

You can get a build/bin/emgl executable.

### Windows

I don't have Windows machine now.

### Emscripten

After local build, you can build with emscripten.

```sh
make emscripten
```

You can get a emgl.html and can execute it via web server. Direct file access from web browsers don't work well.
