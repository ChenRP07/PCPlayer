#version 330 core
out vec4 FragColor;
in vec3 ourColor;//流水线下游收到颜色
void main(){
    FragColor = vec4(ourColor, 1.0f);//接收到的颜色放到FragColor
}

