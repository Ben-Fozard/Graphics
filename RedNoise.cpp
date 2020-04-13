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

void readImage();
void draw();
void update();
void handleEvent(SDL_Event event);
std::vector<float> interpolate(float from, float to, int numberOfValues);
std::vector<vec3> newInterpolate(vec3 from, vec3 to, int numberOfValues);
void drawLine(CanvasPoint point1, CanvasPoint point2, Colour colour);
void stroked(CanvasTriangle points, Colour colour);
void filledTriangle(CanvasTriangle points, Colour colour);
void fillBottomFlatTriangle(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour);
void fillTopFlatTriangle(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour);

//For textures
void drawLineTex(CanvasPoint point1, CanvasPoint point2, Colour colour);
void filledTriangleTex(CanvasTriangle points, Colour colour);
void fillBottomFlatTriangleTex(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour);
void fillTopFlatTriangleTex(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour);


Colour texture[480*395]; //THIS SHOULD BE DONE PROGRAMMATICALLY
int width = 0;
int height = 0;

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

int main(int argc, char* argv[])
{
  readImage();

  SDL_Event event;
  // window = DrawingWindow(WIDTH, HEIGHT, false);
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

void readImage() {
  std::ifstream ifs ("texture.ppm", std::ifstream::in);
  string line;

  //FILE TYPE
  getline(ifs, line);
  puts(line.c_str());

  //MOVE PAST WHITESPACE
  getline(ifs, line);

  //GET DIMENSIONS
  getline(ifs, line);
  string gap = " ";
  size_t found = line.find(gap);

  string widthS = line.substr(0, found);
  puts(widthS.c_str());

  string heightS = line.substr(found+1);
  puts(heightS.c_str());

  width = stoi(widthS);
  height = stoi(heightS);

  printf("WIDTH: %d   ", width);
  printf("HEIGHT: %d\n", height);


  //OTHER HEADER INFO
  getline(ifs, line);
  string maxVal = line;
  puts(maxVal.c_str());

  //USE THIS TO DRAW THE WINDOW
  // window = DrawingWindow(width, height, false);

  for (int i = 0; i < height; i++) { //FOR EACH ROW
    for (int j = 0; j < width; j++) { //FOR EACH PIXEL
      //READ IN THE COLOURS FOR THE PIXEL
      char red;
      ifs.read((char*)&red, 1);

      char green;
      ifs.read((char*)&green, 1);

      char blue;
      ifs.read((char*)&blue, 1);

      Colour colour = Colour(red, green, blue);

      //SET THE PIXEL COLOUR
      // uint32_t colour32 = (255<<24) + (colour.red<<16) + (colour.green<<8) + colour.blue;
      // window.setPixelColour(j, i, colour32);

      texture[i * height + j] = colour;

    }
  }

  printf("Finished reading in file\n");


}

void draw()
{
  window.clearPixels();

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

  //Lab 2 DRAW LINE
  /*
  Colour drawColour = Colour(255, 0, 0);
  CanvasPoint start = CanvasPoint(10.f, 10.f);
  CanvasPoint end = CanvasPoint(100.f, 80.f);
  drawLine(start, end, drawColour);
  */

  //Lab 2 DRAW TRIANGLE
  /*
  Colour drawColour = Colour(255, 0, 0);
  CanvasPoint cp1 = CanvasPoint(50.f, 10.f);
  CanvasPoint cp2 = CanvasPoint(20.f, 120.f);
  CanvasPoint cp3 = CanvasPoint(80.f, 70.f);
  CanvasTriangle points = CanvasTriangle(cp1, cp2, cp3);
  stroked(points, drawColour);
  */

  //Lab 2 DRAW FILLED TRIANGLE
  /*
  Colour drawColour = Colour(255, 0, 0);
  CanvasPoint cp1 = CanvasPoint(50.f, 10.f);
  CanvasPoint cp2 = CanvasPoint(20.f, 120.f);
  CanvasPoint cp3 = CanvasPoint(80.f, 70.f);
  CanvasTriangle points = CanvasTriangle(cp1, cp2, cp3);
  filledTriangle(points, drawColour);
  */

  //Lab 2 TASK 5
  //Draw stroked triangle
  Colour drawColour = Colour(255, 0, 0);
  CanvasPoint cp1 = CanvasPoint(160.f, 10.f);
  CanvasPoint cp2 = CanvasPoint(300.f, 230.f);
  CanvasPoint cp3 = CanvasPoint(10.f, 150.f);
  CanvasTriangle points = CanvasTriangle(cp1, cp2, cp3);
  stroked(points, drawColour);
  //Associate each point with a texture point
  cp1.texturePoint = TexturePoint(195, 5);
  cp2.texturePoint = TexturePoint(395, 380);
  cp3.texturePoint = TexturePoint(65, 330);
  //Now fill the triangle
  filledTriangleTex(points, drawColour);

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

void filledTriangle(CanvasTriangle points, Colour colour) {
  //First sort the veritces according to y value ASCENDING
  if (points.vertices[0].y > points.vertices[1].y) {
    swap(points.vertices[0], points.vertices[1]);
  }
  if (points.vertices[1].y > points.vertices[2].y) {
    swap(points.vertices[1], points.vertices[2]);
  }
  if (points.vertices[0].y > points.vertices[1].y) {
    swap(points.vertices[0], points.vertices[1]);
  }

  CanvasPoint top = points.vertices[0];
  CanvasPoint mid = points.vertices[1];
  CanvasPoint btm = points.vertices[2];

  //Split the triangle into two with flat bases
  CanvasPoint midpoint = CanvasPoint(top.x + (((mid.y - top.y) / (btm.y - top.y)) * (btm.x - top.x)), mid.y);

  //Then fill these two triangles that are produced
  fillBottomFlatTriangle(top, mid, midpoint, colour);
  fillTopFlatTriangle(mid, midpoint, btm, colour);

}

void fillBottomFlatTriangle(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour) {
  //TOP, MID, MIDPOINT
  //Draw from top to each point along the base
  for (int curX = point2.x; curX <= point3.x; curX++) {
    CanvasPoint end = CanvasPoint(curX, point2.y);
    drawLine(point1, end, colour);
  }


}

void fillTopFlatTriangle(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour) {
  //MID, MIDPOINT, BTM
  //Draw from bottom to each point along the flat top
  for (int curX = point1.x; curX <= point2.x; curX++) {
    CanvasPoint end = CanvasPoint(curX, point2.y);
    drawLine(point3, end, colour);
  }
}

void filledTriangleTex(CanvasTriangle points, Colour colour) {
  //First sort the veritces according to y value ASCENDING
  if (points.vertices[0].y > points.vertices[1].y) {
    swap(points.vertices[0], points.vertices[1]);
  }
  if (points.vertices[1].y > points.vertices[2].y) {
    swap(points.vertices[1], points.vertices[2]);
  }
  if (points.vertices[0].y > points.vertices[1].y) {
    swap(points.vertices[0], points.vertices[1]);
  }

  CanvasPoint top = points.vertices[0];
  CanvasPoint mid = points.vertices[1];
  CanvasPoint btm = points.vertices[2];

  //Split the triangle into two with flat bases
  CanvasPoint midpoint = CanvasPoint(top.x + (((mid.y - top.y) / (btm.y - top.y)) * (btm.x - top.x)), mid.y);
  //Determine the Texture point of this
  midpoint.texturePoint = TexturePoint(top.x + (((mid.texturePoint.y - top.texturePoint.y) / (btm.texturePoint.y - top.texturePoint.y)) * (btm.texturePoint.x - top.texturePoint.x)), mid.texturePoint.y);

  //Then fill these two triangles that are produced
  // fillBottomFlatTriangle(top, mid, midpoint, colour);
  // fillTopFlatTriangle(mid, midpoint, btm, colour);

}

void fillBottomFlatTriangleTex(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour) {
  //FOR POINTS
  float invslope1 = (point2.x - point1.x) / (point2.y - point1.y);
  float invslope2 = (point3.x - point1.x) / (point3.y - point1.y);
  float curx1 = point1.x;
  float curx2 = point1.x;

  //FOR TEXTURES
  float invslopeT1 = (point2.texturePoint.x - point1.texturePoint.x) / (point2.texturePoint.y - point1.texturePoint.y);
  float invslopeT2 = (point3.texturePoint.x - point1.texturePoint.x) / (point3.texturePoint.y - point1.texturePoint.y);
  float curTx1 = point1.texturePoint.x;
  float curTx2 = point2.texturePoint.x;

  for (int scanlineY = point1.y; scanlineY <= point2.y; scanlineY++) {
    CanvasPoint start = CanvasPoint(curx1, scanlineY);
    CanvasPoint end = CanvasPoint(curx2, scanlineY);
    start.texturePoint = TexturePoint(curTx1, point1.texturePoint.y + scanlineY);
    end.texturePoint = TexturePoint(curTx2, point1.texturePoint.y + scanlineY);
    drawLineTex(start, end, colour);
    curx1 += invslope1;
    curx2 += invslope2;

    curTx1 += invslopeT1;
    curTx2 += invslopeT2;
  }
}

void drawLineTex(CanvasPoint point1, CanvasPoint point2, Colour colour) {
  int xlen = point1.x - point2.x;
  int ylen = point1.y - point2.y;
  int len = sqrt((xlen * xlen) + (ylen * ylen));

  std::vector<float> xsteps = interpolate(point1.x, point2.x, len);
  std::vector<float> ysteps = interpolate(point1.y, point2.y, len);

  std::vector<float> xTsteps = interpolate(point1.texturePoint.x, point2.texturePoint.x, len);
  std::vector<float> yTsteps = interpolate(point1.texturePoint.y, point2.texturePoint.y, len);

  for (int i = 0; i < len; i++) {
    int x = xsteps.back();
    xsteps.pop_back();
    int y = ysteps.back();
    ysteps.pop_back();

    Colour tex = texture[x*height + y];

    uint32_t colour32 = (255<<24) + (tex.red<<16) + (tex.green<<8) + tex.blue;
    window.setPixelColour(x, y, colour32);
  }
}
