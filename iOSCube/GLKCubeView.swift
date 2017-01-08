//
//  GLKCubeView.swift
//  Cube
//
//  Created by Ellis Sparky Hoag on 7/7/16.
//  Copyright © 2016 Ellis Sparky Hoag. All rights reserved.
//

import Foundation
import UIKit
import QuartzCore
import OpenGLES
import GLKit

class GLKCubeView: GLKView, GLKViewControllerDelegate {
    
    var eaglLayer: CAEAGLLayer!
    var colorRenderBuffer: GLuint = GLuint()
    var depthRenderBuffer: GLuint = GLuint()
    var positionSlot: GLuint = GLuint()
    var colorSlot: GLuint = GLuint()
    var vertexBuffer: GLuint = GLuint()
    var colorBuffer: GLuint = GLuint()
    var VAO: GLuint = GLuint()
    
    var modelViewMatrix = GLKMatrix4()
    var modelViewMatrixSlot = GLuint()
    
    var rot_x: GLfloat = 0
    
    var face_rotation_angle: GLfloat = 0
    var rotation_face: Int = 1
    
    override class var layerClass : AnyClass {
        return CAEAGLLayer.self
    }
    
    required init(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)!
        
        eaglLayer = layer as! CAEAGLLayer
        eaglLayer.isOpaque = true
        
        let api: EAGLRenderingAPI = EAGLRenderingAPI.openGLES2
        context = EAGLContext(api: api)
        
        if (!EAGLContext.setCurrent(context)) {
            print("Failed to set current OpenGL context!")
            exit(1)
        }
        
        glEnable(GLenum(GL_DEPTH_TEST))
        glEnable(GLenum(GL_CULL_FACE))
        
        glGenRenderbuffers(1, &depthRenderBuffer)
        glBindRenderbuffer(GLenum(GL_RENDERBUFFER), depthRenderBuffer)
        //context.renderbufferStorage(Int(GL_RENDERBUFFER), fromDrawable: eaglLayer)
        glRenderbufferStorage(GLenum(GL_RENDERBUFFER), GLenum(GL_DEPTH_COMPONENT16), 2 * GLsizei(frame.size.width), 2 * GLsizei(frame.size.height))
        
        glGenRenderbuffers(1, &colorRenderBuffer)
        glBindRenderbuffer(GLenum(GL_RENDERBUFFER), colorRenderBuffer)
        context.renderbufferStorage(Int(GL_RENDERBUFFER), from: eaglLayer)
        
        var frameBuffer: GLuint = GLuint()
        glGenFramebuffers(1, &frameBuffer)
        glBindFramebuffer(GLenum(GL_FRAMEBUFFER), frameBuffer)
        glFramebufferRenderbuffer(GLenum(GL_FRAMEBUFFER), GLenum(GL_DEPTH_ATTACHMENT), GLenum(GL_RENDERBUFFER), depthRenderBuffer)
        glFramebufferRenderbuffer(GLenum(GL_FRAMEBUFFER), GLenum(GL_COLOR_ATTACHMENT0), GLenum(GL_RENDERBUFFER), colorRenderBuffer)
        
        // Compile our vertex and fragment shaders.
        let vertexShader: GLuint = compileShader("SimpleVertex", shaderType: GLenum(GL_VERTEX_SHADER))
        let fragmentShader: GLuint = compileShader("SimpleFragment", shaderType: GLenum(GL_FRAGMENT_SHADER))
        
        // Call glCreateProgram, glAttachShader, and glLinkProgram to link the vertex and fragment shaders into a complete program.
        let programHandle: GLuint = glCreateProgram()
        glAttachShader(programHandle, vertexShader)
        glAttachShader(programHandle, fragmentShader)
        glLinkProgram(programHandle)
        
        // Check for any errors.
        var linkSuccess: GLint = GLint()
        glGetProgramiv(programHandle, GLenum(GL_LINK_STATUS), &linkSuccess)
        if (linkSuccess == GL_FALSE) {
            print("Failed to create shader program!")
            // TODO: Actually output the error that we can get from the glGetProgramInfoLog function.
            exit(1);
        }
        
        // Call glUseProgram to tell OpenGL to actually use this program when given vertex info.
        glUseProgram(programHandle)
        
        // Finally, call glGetAttribLocation to get a pointer to the input values for the vertex shader, so we
        //  can set them in code. Also call glEnableVertexAttribArray to enable use of these arrays (they are disabled by default).
        positionSlot = GLuint(glGetAttribLocation(programHandle, "position"))
        colorSlot = GLuint(glGetAttribLocation(programHandle, "color"))
        modelViewMatrixSlot = GLuint(glGetUniformLocation(programHandle, "modelViewMatrix"))
        glEnableVertexAttribArray(positionSlot)

        glGenVertexArraysOES(1, &VAO);
        glBindVertexArrayOES(VAO);
        
        glGenBuffers(1, &vertexBuffer)
        glBindBuffer(GLenum(GL_ARRAY_BUFFER), vertexBuffer)
        glBufferData(GLenum(GL_ARRAY_BUFFER), cubieVerticies.count * MemoryLayout<GLfloat>.size, cubieVerticies, GLenum(GL_STATIC_DRAW))
        
        glEnableVertexAttribArray(positionSlot)
        glVertexAttribPointer(positionSlot, 3, GLenum(GL_FLOAT), GLboolean(UInt8(GL_FALSE)), 3 * GLsizei(MemoryLayout<GLfloat>.size), nil)
        
        glGenBuffers(1, &colorBuffer)
        glBindBuffer(GLenum(GL_ARRAY_BUFFER), colorBuffer)
        glBufferData(GLenum(GL_ARRAY_BUFFER), cubieColors.count * MemoryLayout<GLfloat>.size, cubieColors, GLenum(GL_STATIC_DRAW))
        
        glEnableVertexAttribArray(colorSlot)
        glVertexAttribPointer(colorSlot, 3, GLenum(GL_FLOAT), GLboolean(UInt8(GL_FALSE)), 3 * GLsizei(MemoryLayout<GLfloat>.size), nil)
        
        glBindBuffer(GLenum(GL_ARRAY_BUFFER), 0)
        glBindVertexArrayOES(0)
        
        
        glViewport(0, 0, GLint(2 * frame.width), GLint(2 * frame.height))
        
        let aspectRatio = Float(self.frame.width / self.frame.height)
        
        var projectionMatrix = GLKMatrix4MakePerspective(Float(M_PI) / 2, aspectRatio, 0.1, 30)
        var buffer = [GLfloat](repeating: 0, count: 16)
        memcpy(&buffer, &projectionMatrix.m, 16 * MemoryLayout<GLfloat>.size)
        
        let projectionMatrixSlot = GLint(glGetUniformLocation(programHandle, "projectionMatrix"))
        glUniformMatrix4fv(projectionMatrixSlot, 1, 0, buffer)
    }
    
    func compileShader(_ shaderName: String, shaderType: GLenum) -> GLuint {
        
        // Get NSString with contents of our shader file.
        let shaderPath: String! = Bundle.main.path(forResource: shaderName, ofType: "glsl")
        var error: NSError? = nil
        var shaderString: NSString?
        do {
            shaderString = try NSString(contentsOfFile:shaderPath, encoding: String.Encoding.utf8.rawValue)
        } catch let error1 as NSError {
            error = error1
            shaderString = nil
        }
        if (shaderString == nil) {
            print("Failed to set contents shader of shader file!")
            print(error)
        }
        
        // Tell OpenGL to create an OpenGL object to represent the shader, indicating if it's a vertex or a fragment shader.
        let shaderHandle: GLuint = glCreateShader(shaderType)
        
        if shaderHandle == 0 {
            NSLog("Couldn't create shader")
        }
        // Conver shader string to CString and call glShaderSource to give OpenGL the source for the shader.
        var shaderStringUTF8 = shaderString!.utf8String
        var shaderStringLength: GLint = GLint(Int32(shaderString!.length))
        glShaderSource(shaderHandle, 1, &shaderStringUTF8, &shaderStringLength)
        
        // Tell OpenGL to compile the shader.
        glCompileShader(shaderHandle)
        
        // But compiling can fail! If we have errors in our GLSL code, we can here and output any errors.
        var compileSuccess: GLint = GLint()
        glGetShaderiv(shaderHandle, GLenum(GL_COMPILE_STATUS), &compileSuccess)
        if (compileSuccess == GL_FALSE) {
            print("Failed to compile shader!")
            // TODO: Actually output the error that we can get from the glGetShaderInfoLog function.
            exit(1)
        }
        
        return shaderHandle
    }

    func glkViewControllerUpdate(_ controller: GLKViewController) {
        
        face_rotation_angle += 0.05
        
        rot_x += 0.03
        
        modelViewMatrix = GLKMatrix4Identity
        
        modelViewMatrix = GLKMatrix4Translate(modelViewMatrix, 0, 0, -10)
                
        //modelViewMatrix = GLKMatrix4RotateX(modelViewMatrix, Float(M_PI) / 4)
        //modelViewMatrix = GLKMatrix4RotateY(modelViewMatrix, Float(M_PI) / 4)
        
    }
    
    override func display() {
        
        glClear(GLbitfield(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT))

        glBindVertexArrayOES(VAO);
        
        let cubieSeparation: Float = 1.1
        
        for x in -1...1
        {
            for y in -1...1
            {
                for z in -1...1
                {
                    if (x == 0 && y == 0 && z == 0) {continue}
                    
                    var cubieModelViewMatrix = modelViewMatrix
                    
                    switch rotation_face {
                    case 0:
                        if (z == 1)
                        {
                            cubieModelViewMatrix = GLKMatrix4RotateZ(cubieModelViewMatrix, -face_rotation_angle)
                        }
                    case 1:
                        if (x == 1)
                        {
                            cubieModelViewMatrix = GLKMatrix4RotateX(cubieModelViewMatrix, -face_rotation_angle)
                        }
                    case 2:
                        if (z == -1)
                        {
                            cubieModelViewMatrix = GLKMatrix4RotateZ(cubieModelViewMatrix, face_rotation_angle)
                        }
                    case 3:
                        if (x == -1)
                        {
                            cubieModelViewMatrix = GLKMatrix4RotateX(cubieModelViewMatrix, face_rotation_angle)
                        }
                    case 4:
                        if (y == 1)
                        {
                            cubieModelViewMatrix = GLKMatrix4RotateY(cubieModelViewMatrix, -face_rotation_angle)
                        }
                    case 5:
                        if (y == -1)
                        {
                            cubieModelViewMatrix = GLKMatrix4RotateY(cubieModelViewMatrix, face_rotation_angle)
                        }
                    default: break
                    }
                    
                    cubieModelViewMatrix = GLKMatrix4Translate(cubieModelViewMatrix, Float(x) * cubieSeparation, Float(y) * cubieSeparation, Float(z) * cubieSeparation)

                    
                    var matrixBuffer = [GLfloat](repeating: 0, count: 16)
                    
                    memcpy(&matrixBuffer, &cubieModelViewMatrix.m, MemoryLayout<GLfloat>.size * 16)
                    
                    glUniformMatrix4fv(GLint(modelViewMatrixSlot), 1, 0, matrixBuffer)
                    
                    glDrawArrays(GLenum(GL_TRIANGLES), 0, GLsizei(cubieVerticies.count))
                }
            }
        }
        
        context.presentRenderbuffer(Int(GL_RENDERBUFFER))
        
        glBindVertexArrayOES(0)
    }
}
