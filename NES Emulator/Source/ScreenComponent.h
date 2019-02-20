//
//  ScreenComponent.h
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 27/07/2018.
//
//

#ifndef __NES_Emulator_Go_Port__ScreenComponent__
#define __NES_Emulator_Go_Port__ScreenComponent__

#include "../JuceLibraryCode/JuceHeader.h"
#include <OpenGL/gl.h> //OS x libs
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "ppu.h"


struct Color
{
	float r;
	float g;
	float b;
  //  float a;
};

const int bufferSize = pixelWidth * pixelHeight;

class ScreenComponent   : public OpenGLAppComponent
{
public:
    
    
    
    ScreenComponent()
    {
        ppu = nullptr;
    }
    
    ~ScreenComponent()
    {
        shutdownOpenGL();
    }
    
    void initialise() override
    {
        
    }
    
    void shutdown() override
    {
    }
    
    void render() override
    {
        OpenGLHelpers::clear (Colours::black);
       // pl.Render(myBuffer);
        //        ;
        
//        glViewport(0,0, getWidth(), getHeight());

       // glWindowPos2d(getWidth() / 2, getHeight() / 2);
        
        //const float desktopScale = (float) openGLContext.getRenderingScale();
        //glViewport (0, 0, roundToInt (desktopScale * getWidth()), roundToInt (desktopScale * getHeight()));

        glMatrixMode(GL_MODELVIEW);
        if (ppu != nullptr) {
//            for (int i = 0; i < pixelWidth; i++) {
//                for (int j = 0; j < pixelHeight; j++) {
//                    myBuffer[i][j].r = ppu->front[i][j].r / 255.0;
//                    myBuffer[i][j].g = ppu->front[i][j].g / 255.0;
//                    myBuffer[i][j].b = ppu->front[i][j].b / 255.0;
////                    myBuffer[i][j].a = ppu->front[i][j].b / 255.0;
//                }
//            }
            
            int counter = 0;
                for (int y = 0; y < pixelHeight; y++) {
                    for (int x = 0; x < pixelWidth; x++) {
                        const int yMap = (pixelHeight-1) - y;
                        bufferToRender[counter].r = ppu->front[x][yMap].r;
                        bufferToRender[counter].g = ppu->front[x][yMap].g;
                        bufferToRender[counter].b = ppu->front[x][yMap].b;
                        bufferToRender[counter].a = ppu->front[x][yMap].a;
                        counter++;
                    
                }
            }
            
            
            glPixelZoom (4.0, 4.0);
            

//            glDrawPixels(pixelHeight, pixelWidth, GL_RGBA, GL_UNSIGNED_BYTE, ppu->front);
            glDrawPixels(pixelWidth, pixelHeight, GL_RGBA, GL_UNSIGNED_BYTE, bufferToRender);
        }
//        glScalef(12.0, 12.0, 12.0);
        
        
       // glDrawPixels(pixelWidth, pixelHeight, GL_RGB, GL_FLOAT, myBuffer);
        
        //   glutSwapBuffers();
        
        
        //glGenTextures (1, &texID);
       // glBindTextures (GL_TEXTURE_RECTANGLE_EXT, texID);
#if 0
        GLuint testThing;
        glGenTextures (1, &testThing);
        glBindTexture (GL_TEXTURE_RECTANGLE_EXT, testThing);
        glTexImage2D (GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, pixelWidth, pixelHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, ppu->front);
        glBegin (GL_QUADS);
        glTexCoord2f (0, 0);
        glVertex2f (0, 0);
        glTexCoord2f (pixelWidth, 0);
        glVertex2f (getWidth(), 0);
        glTexCoord2f (pixelWidth, pixelHeight);
        glVertex2f (getWidth(), getHeight());
        glTexCoord2f (0, getHeight());
        glVertex2f (0, getHeight());
        glEnd();
#endif
        
    }
    
    void paint (Graphics& g) override
    {
        // You can add your component specific drawing code here!
        // This will draw over the top of the openGL background.
    }
    
    void resized() override
    {

        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
        
    }
    void setPPU (PPU * ppu)
    {
        this->ppu = ppu;
    }
    
    
private:
    //==============================================================================
    
    // private member variables
    NesColour   bufferToRender[bufferSize];
    Color myBuffer[pixelWidth][pixelHeight];
    PPU * ppu;
    //Player pl;
};

#endif /* defined(__NES_Emulator_Go_Port__ScreenComponent__) */
