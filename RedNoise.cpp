#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>

using namespace std;
using namespace glm;

#define WIDTH 320
#define HEIGHT 240

void draw();
void update();
void handleEvent(SDL_Event event);
std::vector<float> interpolate(float from, float to, int numberOfValues);
std::vector<vec3> newInterpolate(vec3 from, vec3 to, int numberOfValues);
void drawLine(CanvasPoint point1, CanvasPoint point2, Colour colour);
void stroked(CanvasTriangle points, Colour colour);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

int main(int argc, char* argv[])
{
  SDL_Event event;
  while(true)
  {
    // We MUST poll for events - otherwise the window will freeze !
    if(window.pollForInputEvents(&event)) handleEvent(event);
    update();
    draw();
    // Need to render the frame at the end, or nothing actually gets shown on the screen !
    window.renderFrame();
  }
}

void draw()
{
  window.clearPixels();

  //Lab 2 DRAW LINE
  /*
  Colour drawColour = Colour(255, 0, 0);
  CanvasPoint start = CanvasPoint(10.f, 10.f);
  CanvasPoint end = CanvasPoint(100.f, 80.f);
  drawLine(start, end, drawColour);
  */

  //Lab 2 DRAW TRIANGLE
  Colour drawColour = Colour(255, 0, 0);
  CanvasPoint cp1 = CanvasPoint(50.f, 10.f);
  CanvasPoint cp2 = CanvasPoint(20.f, 120.f);
  CanvasPoint cp3 = CanvasPoint(80.f, 70.f);
  CanvasTriangle points = CanvasTriangle(cp1, cp2, cp3);
  stroked(points, drawColour);

  //Lab 1 COLOUR GRADIENT
  /*
  std::vector<vec3> right = newInterpolate(vec3(0,255,0), vec3(0,0,255), window.height);
  std::vector<vec3> left = newInterpolate(vec3(255,255,0), vec3(255,0,0), window.height);

  for(int y=0; y<window.height ;y++) {
    vec3 lefti = left.back();
    left.pop_back();
    vec3 righti = right.back();
    right.pop_back();
    std::vector<vec3> pixel = newInterpolate(righti, lefti, window.width);
    for(int x=0; x<window.width ;x++) {
      uint32_t colour = (255<<24) + (int(pixel.back().x)<<16) + (int(pixel.back().y)<<8) + int(pixel.back().z);
      pixel.pop_back();
      window.setPixelColour(x, y, colour);
    }
  }
  */
}

void update()
{
  // Function for performing animation (shifting artifacts or moving the camera)
}

void handleEvent(SDL_Event event)
{
  if(event.type == SDL_KEYDOWN) {
    if(event.key.keysym.sym == SDLK_LEFT) cout << "LEFT" << endl;
    else if(event.key.keysym.sym == SDLK_RIGHT) cout << "RIGHT" << endl;
    else if(event.key.keysym.sym == SDLK_UP) cout << "UP" << endl;
    else if(event.key.keysym.sym == SDLK_DOWN) cout << "DOWN" << endl;
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN) cout << "MOUSE CLICKED" << endl;
}

std::vector<float> interpolate(float from, float to, int numberOfValues) {
  std::vector<float> results = {};

  float step = (to - from)/(numberOfValues - 1);

  for (int i = 0; i < numberOfValues; i++) {
    results.push_back(from + (i*step));
  }

  return results;
}

std::vector<vec3> newInterpolate(vec3 from, vec3 to, int numberOfValues) {
  std::vector<vec3> results= {};

  float stepX = (to.x - from.x)/(numberOfValues - 1);
  float stepY = (to.y - from.y)/(numberOfValues - 1);
  float stepZ = (to.z - from.z)/(numberOfValues - 1);

  for (int i = 0; i < numberOfValues; i++){
    vec3 v = vec3(from.x + (i*stepX), from.y + (i*stepY), from.z + (i*stepZ));
    results.push_back(v);
  }

  return results;
}

void drawLine(CanvasPoint point1, CanvasPoint point2, Colour colour) {
  int xlen = point1.x - point2.x;
  int ylen = point1.y - point2.y;
  int len = sqrt((xlen * xlen) + (ylen * ylen));

  std::vector<float> xsteps = interpolate(point1.x, point2.x, len);
  std::vector<float> ysteps = interpolate(point1.y, point2.y, len);

  for (int i = 0; i < len; i++) {
    int x = xsteps.back();
    xsteps.pop_back();
    int y = ysteps.back();
    ysteps.pop_back();

    uint32_t colour32 = (255<<24) + (colour.red<<16) + (colour.green<<8) + colour.blue;
    window.setPixelColour(x, y, colour32);
  }
}

void stroked(CanvasTriangle points, Colour colour) {
  drawLine(points.vertices[0], points.vertices[1], colour);
  drawLine(points.vertices[1], points.vertices[2], colour);
  drawLine(points.vertices[0], points.vertices[2], colour);
}
