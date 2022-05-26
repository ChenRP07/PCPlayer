#version 330 core
layout(location = 0) in vec3 aPos;  //顶点着色器 索引为0入口  位置
layout(location = 1) in vec3 aColor;//索引为1 aColor需要送出去
out vec3 ourColor;            //定义一个vec3对象把aColor传到ourColor
uniform mat4 trans;
void main(){
    gl_Position = trans  * vec4(aPos, 1.0);
    ourColor = aColor;
}

