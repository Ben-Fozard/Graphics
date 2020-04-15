#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

using namespace std;
using namespace glm;

#define WIDTH 320
#define HEIGHT 240

struct Camera{
  float focalLength;
  vec3 position;
};

void readImage();
void draw(Camera camera);
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
void drawLineTex(CanvasPoint point1, CanvasPoint point2);
void filledTriangleTex(CanvasTriangle points, Colour colour);
void fillBottomFlatTriangleTex(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, CanvasPoint btm, Colour colour);
void fillTopFlatTriangleTex(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour);

Colour texture[480*395]; //THIS SHOULD BE DONE PROGRAMMATICALLY
int width = 0;
int height = 0;

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

//LOAD THE MATERIALS FOR AN OBJ
std::map<string, Colour> readMtl(string filename) {
  std::map<string, Colour> cMap;

  std::ifstream ifs (filename, std::ifstream::in);
  string line;
  string gap = " ";

  string curName;

  while (getline(ifs, line)) {
    size_t found = line.find(gap);

    string type = line.substr(0, found);
    string values = line.substr(found+1);
    // puts(heightS.c_str());

    if (type == "newmtl") { //NEW COLOUR
      //Get the materials library
      curName = values;
      // puts(curName.c_str());

      //THEN LOAD PALLETES

    }
    else if (type == "Kd") { //HERE THE COLOUR IS SPECIFIED
      stringstream ss(values);
      float r, g, b;
      ss >> r >> g >> b;

      Colour newColour = Colour(255*r, 255*g, 255*b);
      cMap[curName] = newColour;
    }
  }


  return cMap;
}

//OBJ LOADER
std::vector<ModelTriangle> load_obj(string filename) {
  std::ifstream ifs (filename, std::ifstream::in);
  string line;
  string gap = " ";

  vector<vec3> vertices;

  vector<ModelTriangle> triangles;

  vector<Colour> colours;
  string materialLib;
  std::map<string, Colour> cMap;
  string curMtl;


  while (getline(ifs, line)) {
    size_t found = line.find(gap);

    string type = line.substr(0, found);
    string values = line.substr(found+1);
    // puts(heightS.c_str());



    //DEFAULT COLOUR SO WE CAN TEST VERTICES BEFORE DOING MATERIALS
    if (type == "mtllib") {
      //Get the materials library
      materialLib = values;
      // puts(materialLib.c_str());

      //THEN LOAD PALETTES
      cMap = readMtl("cornell-box.mtl");
    }
    else if (type == "usemtl") { //We have changed what material we are using
      curMtl = values;
    }
    else if (type == "v") {
      stringstream ss(values);
      float x, y, z;
      ss >> x >> y >> z;
      vec3 newV = vec3(x, y, z);
      // cout << newV.x;
      vertices.push_back(newV);
    }
    else if (type == "f") {
      stringstream ss1(values);
      int v1;
      int v2 = 0; int v3 = 0;
      ss1 >> v1;

      // FIND THE NEXT VALUE
      found = values.find(gap);
      values = values.substr(found+1);
      stringstream ss2(values);
      ss2 >> v2;

      // FIND THE NEXT VALUE
      found = values.find(gap);
      values = values.substr(found+1);
      stringstream ss3(values);
      ss3 >> v3;

      ModelTriangle triangle = ModelTriangle(vertices[v1-1], vertices[v2-1], vertices[v3-1], cMap[curMtl]);
      triangles.push_back(triangle);
    }
  }
  return triangles;
}

int main(int argc, char* argv[])
{
  //FOR TEXTURES
  // readImage();

  //STILL NEED TO EXPERIMENT WITH THESE VALUES
  Camera camera = {
    3,
    vec3(0, 0, 0)
  };

  vector<ModelTriangle> triangles = load_obj("cornell-box.obj");

  SDL_Event event;
  // window = DrawingWindow(WIDTH, HEIGHT, false);
  while(true)
  {
    // We MUST poll for events - otherwise the window will freeze !
    if(window.pollForInputEvents(&event)) handleEvent(event);
    update();
    draw(camera);
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

void draw(Camera camera)
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
  /*
  //Draw stroked triangle
  Colour drawColour = Colour(255, 0, 0);
  CanvasPoint cp1 = CanvasPoint(160.f, 10.f);
  CanvasPoint cp2 = CanvasPoint(300.f, 230.f);
  CanvasPoint cp3 = CanvasPoint(10.f, 150.f);
  //Associate each point with a texture point
  cp1.texturePoint = TexturePoint(195, 5);
  cp2.texturePoint = TexturePoint(395, 380);
  cp3.texturePoint = TexturePoint(65, 330);
  CanvasTriangle points = CanvasTriangle(cp1, cp2, cp3);
  stroked(points, drawColour);
  //Now fill the triangle
  filledTriangle(points, drawColour);
  */

  //LAB 3 WIREFRAMES
  vector<ModelTriangle> triangles = load_obj("cornell-box.obj");
  // cout << triangles.size() << "\n";
  int len = int(triangles.size()); //GOT TO DECLARE HERE OTHERWISE WE POP THE LIST WHILST IT'S SIZE IS BEING USED AS A LOOP CONDITION
  float xi = 0.f;
  float yi = 0.f;

  for (int i = 0; i < len; i++) {
    ModelTriangle t = triangles.back();
    //For the first canvas point
    xi = camera.focalLength * ((t.vertices[0].x - camera.position.x) / (t.vertices[0].z - camera.position.z));
    //Now we center the value relative to the screen
    xi = xi + WIDTH/2;
    yi = camera.focalLength * ((t.vertices[0].y - camera.position.y) / (t.vertices[0].z - camera.position.z));
    //Now we center the value relative to the screen
    yi = yi + HEIGHT/2;
    CanvasPoint cp1 = CanvasPoint(xi, yi);
    //For the second canvas point
    xi = camera.focalLength * ((t.vertices[1].x - camera.position.x) / (t.vertices[1].z - camera.position.z));
    //Now we center the value relative to the screen
    xi = xi + WIDTH/2;
    yi = camera.focalLength * ((t.vertices[1].y - camera.position.y) / (t.vertices[1].z - camera.position.z));
    //Now we center the value relative to the screen
    yi = yi + HEIGHT/2;
    CanvasPoint cp2 = CanvasPoint(xi, yi);
    //For the third canvas point
    xi = camera.focalLength * ((t.vertices[2].x - camera.position.x) / (t.vertices[2].z - camera.position.z));
    //Now we center the value relative to the screen
    xi = xi + WIDTH/2;
    yi = camera.focalLength * ((t.vertices[2].y - camera.position.y) / (t.vertices[2].z - camera.position.z));
    //Now we center the value relative to the screen
    yi = yi + HEIGHT/2;
    CanvasPoint cp3 = CanvasPoint(xi, yi);

    // cout << i << "\n";

    //THEN DRAW THE TRIANGLE
    CanvasTriangle points = CanvasTriangle(cp1, cp2, cp3);
    // stroked(points, t.colour);
    filledTriangle(points, t.colour);

    triangles.pop_back();
  }

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

  if (numberOfValues == 1) {
    results.push_back(from);
    return results;
  }

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
  std::vector<float> startXs = interpolate(point2.x, point1.x, point2.y - point1.y);
  std::vector<float> endXs = interpolate(point3.x, point1.x, point3.y - point1.y);
  // cout << point3.y - point1.y << " vs " << int(endXs.size()) << "\n";
  int len = int(endXs.size());
  // cout << len << "\n";

  float y = point1.y;
  for (int i = 0; i < len; i ++) {
    float xStart = startXs.back();
    startXs.pop_back();
    float xEnd = endXs.back();
    endXs.pop_back();

    CanvasPoint start = CanvasPoint(xStart, y);
    CanvasPoint end = CanvasPoint(xEnd, y);
    drawLine(start, end, colour);
    y+=1.0f;
    // cout << y << "\n";
  }
}

void fillTopFlatTriangle(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour) {
  //MID, MIDPOINT, BTM
  std::vector<float> startXs = interpolate(point3.x, point1.x, point3.y - point1.y);
  std::vector<float> endXs = interpolate(point3.x, point2.x, point3.y - point1.y);
  int len = int(endXs.size());

  float y = point1.y;
  for (int i = 0; i < len; i++) {
    float xStart = startXs.back();
    startXs.pop_back();
    float xEnd = endXs.back();
    endXs.pop_back();

    CanvasPoint start = CanvasPoint(xStart, y);
    CanvasPoint end = CanvasPoint(xEnd, y);
    drawLine(start, end, colour);
    y+=1.0f;
  }
}

void filledTriangleTex(CanvasTriangle points, Colour colour) {
  // printf("%f\n", points.vertices[0].texturePoint.x);

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
  //IS THIS RIGHT?????????????????
  midpoint.texturePoint = TexturePoint(top.texturePoint.x + (((mid.texturePoint.y - top.texturePoint.y) / (btm.texturePoint.y - top.texturePoint.y)) * (btm.texturePoint.x - top.texturePoint.x)), mid.texturePoint.y);

  //Then fill these two triangles that are produced
  fillBottomFlatTriangleTex(top, mid, midpoint, btm, colour);
  // fillTopFlatTriangle(mid, midpoint, btm, colour);

}

void fillBottomFlatTriangleTex(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, CanvasPoint btm, Colour colour) {
  //TOP, MID, MIDPOINT
  std::vector<float> startXs = interpolate(point2.x, point1.x, point2.y - point1.y);
  std::vector<float> endXs = interpolate(point3.x, point1.x, point3.y - point1.y);

  std::vector<float> startTXs = interpolate(point2.texturePoint.x, point1.texturePoint.x, point2.y - point1.y);
  std::vector<float> endTXs = interpolate(point3.texturePoint.x, point1.texturePoint.x, point3.y - point1.y);

  std::vector<float> startTYs = interpolate(point2.texturePoint.y, point1.texturePoint.y, point2.y - point1.y);
  std::vector<float> endTYs = interpolate(point3.texturePoint.y, point1.texturePoint.y, point3.y - point1.y);

  for (float y = point1.y; y < point2.y; y+=1.0) {
    float xStart = startXs.back();
    startXs.pop_back();
    float xEnd = endXs.back();
    endXs.pop_back();

    float xTStart = startTXs.back();
    startTXs.pop_back();
    float xTEnd = endTXs.back();
    endTXs.pop_back();

    float yTStart = startTYs.back();
    startTYs.pop_back();
    float yTEnd = endTYs.back();
    endTYs.pop_back();


    CanvasPoint start = CanvasPoint(xStart, y);
    CanvasPoint end = CanvasPoint(xEnd, y);

    start.texturePoint = TexturePoint(xTStart, yTStart);
    end.texturePoint = TexturePoint(xTEnd, yTEnd);
    drawLineTex(start, end);
  }
}

void fillTopFlatTriangleTex(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour) {
  //MID, MIDPOINT, BTM
  //Draw from bottom to each point along the flat top
  for (int curX = point1.x; curX <= point2.x; curX++) {
    CanvasPoint end = CanvasPoint(curX, point2.y);
    //Figure out what the texture point is for the end points
    //point3 will remain the same
    float proportion = (curX - point1.x) / (point2.x - point1.x);
    float newTx = point1.texturePoint.x + proportion * (point2.texturePoint.x - point1.texturePoint.x);
    float newTy = point1.texturePoint.y + proportion * (point2.texturePoint.y - point1.texturePoint.y);
    end.texturePoint = TexturePoint(newTx, newTy);
    drawLineTex(point3, end);
  }
}

void drawLineTex(CanvasPoint point1, CanvasPoint point2) {
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

    int xT = xTsteps.back();
    xTsteps.pop_back();
    int yT = yTsteps.back();
    yTsteps.pop_back();

    Colour tex = texture[xT*height + yT];

    uint32_t colour32 = (255<<24) + (tex.red<<16) + (tex.green<<8) + tex.blue;
    window.setPixelColour(x, y, colour32);
  }
}
