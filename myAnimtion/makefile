shader: main.cpp gizmo.cpp skeleton.cpp node.cpp object3d.cpp common/shader.cpp common/controls.cpp common/objloader.cpp common/texture.cpp common/vboindexer.cpp
	g++ -std=c++11 main.cpp gizmo.cpp object3d.cpp skeleton.cpp node.cpp -o shader common/shader.cpp common/controls.cpp common/objloader.cpp common/texture.cpp common/vboindexer.cpp -lGL -lGLU -lGLEW -lpthread `pkg-config --libs glfw3`

test3: 
	g++ -std=c++11 -o main main.cpp constants.h gizmo.cpp skeleton.cpp node.cpp object3d.cpp common/shader.cpp common/controls.cpp common/objloader.cpp common/texture.cpp common/vboindexer.cpp  -lGL -lGLU -lGLEW  -lglfw3 -lrt -lXrandr -lXinerama -lXi -lXcursor -lGL -lm -ldl -lXrender -ldrm -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lxcb-dri3 -lxcb-present -lxcb-sync -lxshmfence -lXxf86vm -lXfixes -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp -lassimp
