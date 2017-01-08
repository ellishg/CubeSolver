attribute vec4 position;
attribute vec4 color;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

varying vec4 destinationColor;

void main(void)
{    
    destinationColor = color;
    //destinationColor = vec4(1, 0, 0, 1);
    
    gl_Position = projectionMatrix * modelViewMatrix * position;
}