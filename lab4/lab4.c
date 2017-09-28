// Demo of heavily simplified sprite engine
// by Ingemar Ragnemalm 2009
// used as base for lab 4 in TSBK03.
// OpenGL 3 conversion 2013.

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// uses framework Cocoa
#else
	#include <GL/gl.h>
	#include "MicroGlut.h"
#endif

#include <stdlib.h>
#include <limits.h>
#include "LoadTGA.h"
#include "SpriteLight.h"
#include "GL_utilities.h"
#include "math.h"
#include <time.h>

// Lägg till egna globaler här efter behov.


float returnEuclidDistance(float p1x, float p1y, float p2x, float p2y)
{
	float deltaX = abs(p1x - p2x);
	float deltaY = abs(p1y - p2y);
	float result = sqrt((deltaX*deltaX) + (deltaY*deltaY));
	return result;
}

FPoint calcAvoidance(FPoint p1, FPoint p2, float dist)
{
	FPoint avoid;
	if(dist != 0.0f)
	{
	      avoid.h = (p1.h - p2.h)*(10/dist);
	      avoid.v = (p1.v - p2.v)*(10/dist);
	}
	else
	{
	      avoid.h = 0;
	      avoid.v = 0;
	}
	return avoid;
}

void SpriteBehavior() // Din kod!
{
// Lägg till din labbkod här. Det går bra att ändra var som helst i
// koden i övrigt, men mycket kan samlas här. Du kan utgå från den
// globala listroten, gSpriteRoot, för att kontrollera alla sprites
// hastigheter och positioner, eller arbeta från egna globaler.
	/** COHESION **/
	//printf("Testing Behaviour...\n");
	SpritePtr sp = gSpriteRoot;
	float savedXPos[6] = {0};
	float savedYPos[6] = {0};
	float savedXAvoid[6] = {0};
	float savedYAvoid[6] = {0};
	float savedXSpeed[6] = {0};
	float savedYSpeed[6] = {0};
	float kCohesionWeight = 0.003f;
	float kSeparationWeight = 0.01f;
	float kAlignmentWeight = 0.005f;
	int it = 0;
	do
	{
	      int posNum = 0;
	      int avoidNum = 0;
	      int spdNum = 0;
	      FPoint avgPos;
	      FPoint avoid;
	      FPoint avgSpd;
	      avgPos.h = 0.0f;
	      avgPos.v = 0.0f;
	      avoid.h = 0.0f;
	      avoid.v = 0.0f;
	      avgSpd.h = 0.0f;
	      avgSpd.v = 0.0f;
	      float maxDist = 150.0f;
	      float maxDistAvoid = 50.0f;
	      SpritePtr otherSP = gSpriteRoot;
	      do
	      {
		     if(sp != otherSP)
		     {
			    float boidDistance = returnEuclidDistance(sp->position.h, sp->position.v, 
								      otherSP->position.h, otherSP->position.v);
			   
			    if(boidDistance < maxDist)
			    {
				    avgPos.h += otherSP->position.h;
				    avgPos.v += otherSP->position.v;
				    posNum = posNum + 1;
				    avgSpd.h += abs(otherSP->speed.h - sp->speed.h);
				    avgSpd.v += abs(otherSP->speed.v - sp->speed.v);
				    spdNum += 1;
			    }
			    if(boidDistance < maxDistAvoid)
			    {
				    avoid = calcAvoidance(sp->position, otherSP->position, boidDistance);
				    avoidNum = avoidNum + 1;
			    }
		     }
		     otherSP = otherSP->next;
	      }while(otherSP != NULL);
	      
	      if(posNum != 0)
	      {
		     avgPos.h /= posNum;
		     avgPos.v /= posNum;
		     avgSpd.h /= spdNum;
		     avgSpd.v /= spdNum;
	      }
	      if(avoidNum != 0)
	      {
		     avoid.h /= avoidNum;
		     avoid.v /= avoidNum;
		     /*printf("DIR,SP to avgPos:\n(");
		     printf("%.2f", avgPos.h - sp->position.h);
		     printf(", ");
		     printf("%.2f", avgPos.v - sp->position.v);
		     printf(")\n");
		     printf("AVOID:\n(");
		     printf("%.2f", avoid.h);
		     printf(", ");
		     printf("%.2f", avoid.v);
		     printf(")\n");*/
	      }
	      
	      savedXPos[it] = avgPos.h;
	      savedYPos[it] = avgPos.v;
	      savedXAvoid[it] = avoid.h;
	      savedYAvoid[it] = avoid.v;
	      savedXSpeed[it] = avgSpd.h;
	      savedYSpeed[it] = avgSpd.v;
	      it += 1;
	      sp = sp->next;
	}while(sp != NULL);
	
	/*printf("AVGSPEED:\n(");
	printf("%.2f", savedXSpeed[0]);
	printf(", ");
	printf("%.2f", savedYSpeed[0]);
	printf(")\n");
	printf("%.2f", savedXSpeed[1]);
	printf(", ");
	printf("%.2f", savedYSpeed[1]);
	printf(")\n");
	printf("%.2f", savedXSpeed[2]);
	printf(", ");
	printf("%.2f", savedYSpeed[2]);
	printf(")\n");
	printf("%.2f", savedXSpeed[3]);
	printf(", ");
	printf("%.2f", savedYSpeed[3]);
	printf(")\n");
	printf("%.2f", savedXSpeed[4]);
	printf(", ");
	printf("%.2f", savedYSpeed[4]);
	printf(")\n");
	printf("%.2f", savedXSpeed[5]);
	printf(", ");
	printf("%.2f", savedYSpeed[5]);
	printf(")\n");*/
	
	sp = gSpriteRoot;
	it = 0;
	
	do
	{
	      float accuPosX = 0.0f, accuPosY = 0.0f;
	      float accuAvoidanceX = savedXAvoid[it], accuAvoidanceY = savedYAvoid[it];
	      float accuSpeedX = savedXSpeed[it], accuSpeedY = savedYSpeed[it];
	      if(savedXPos[it] != 0)
	      {
		    accuPosX = savedXPos[it] - sp->position.h;
	      }
	      if(savedYPos[it] != 0)
	      {
		    accuPosY = savedYPos[it] - sp->position.v;
	      }
	      sp->speed.h += accuPosX*kCohesionWeight + 
			     accuAvoidanceX*kSeparationWeight + 
			     accuSpeedX*kAlignmentWeight;
	      sp->speed.v += accuPosY*kCohesionWeight + 
			     accuAvoidanceY*kSeparationWeight + 
			     accuSpeedY*kAlignmentWeight;
	      if(it == 1)
	      {
		    FPoint randDir;
		    
		    int randomX = rand(), randomY = rand();
		    //printf("%i\n", randomX);
		    
		    randDir.h = ((float)randomX)/(RAND_MAX);
		    randDir.h /= 10;
		    randDir.v = ((float)randomY)/(RAND_MAX);
		    randDir.v /= 10;
		    //printf("%.2f\n", ((float)randomX)/RAND_MAX);
		    sp->speed.h += randDir.h;
		    sp->speed.v += randDir.v;
	      }
	      /**COHESION TESTING
	      sp->speed.h += accuPosX*kCohesionWeight;
	      sp->speed.v += accuPosY*kCohesionWeight;
	      **/
	      
	      /**AVOIDANCE TESTING
	      sp->speed.h += accuAvoidanceX*kSeparationWeight;
	      sp->speed.v += accuAvoidanceY*kSeparationWeight;
	      **/
	      
	      sp = sp->next;
	      it += 1;
	}while(sp != NULL);
	
}

// Drawing routine
void Display()
{
	SpritePtr sp;
	
	glClearColor(0, 0, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT+GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	DrawBackground();
	
	SpriteBehavior(); // Din kod!
	
// Loop though all sprites. (Several loops in real engine.)
	sp = gSpriteRoot;
	do
	{
		HandleSprite(sp); // Callback in a real engine
		DrawSprite(sp);
		sp = sp->next;
	} while (sp != NULL);
	
	glutSwapBuffers();
}

void Reshape(int h, int v)
{
	glViewport(0, 0, h, v);
	gWidth = h;
	gHeight = v;
}

void Timer(int value)
{
	glutTimerFunc(20, Timer, 0);
	glutPostRedisplay();
}

// Example of user controllable parameter
float someValue = 0.0;

void Key(unsigned char key,
         __attribute__((unused)) int x,
         __attribute__((unused)) int y)
{
  switch (key)
  {
    case '+':
    	someValue += 0.1;
    	printf("someValue = %f\n", someValue);
    	break;
    case '-':
    	someValue -= 0.1;
    	printf("someValue = %f\n", someValue);
    	break;
    case 0x1b:
      exit(0);
  }
}

void Init()
{
	TextureData *sheepFace, *blackFace, *dogFace, *foodFace;
	
	LoadTGATextureSimple("bilder/leaves.tga", &backgroundTexID); // Bakgrund
	
	sheepFace = GetFace("bilder/sheep.tga"); // Ett får
	blackFace = GetFace("bilder/blackie.tga"); // Ett svart får
	dogFace = GetFace("bilder/dog.tga"); // En hund
	foodFace = GetFace("bilder/mat.tga"); // Mat
	
	NewSprite(sheepFace, 100, 200, 1, 1);
	NewSprite(sheepFace, 200, 100, 1.5, -1);
	NewSprite(sheepFace, 250, 200, -1, 1.5);
	NewSprite(dogFace, 300, 150, 2, 1.5);
	NewSprite(blackFace, 150, 300, 1, 2);
	NewSprite(dogFace, 500, 500, -2, -1);
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(3, 2);
	glutCreateWindow("SpriteLight demo / Flocking");
	
	glutDisplayFunc(Display);
	glutTimerFunc(20, Timer, 0); // Should match the screen synch
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Key);
	
	InitSpriteLight();
	Init();
	
	glutMainLoop();
	return 0;
}
