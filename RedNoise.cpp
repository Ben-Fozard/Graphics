#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>

using namespace std;
using namespace glm;

#define WIDTH 320
#define HEIGHT 240

struct Camera{
  float focalLength;
  vec3 position;
  float movementSpeed;
};

//STILL NEED TO EXPERIMENT WITH THESE VALUES
Camera camera = {
  200,
  vec3(0, 2.5, 6),
  0.001
};

int outfile = 1;

void readImage(string filename);
void writeImage(string filename);
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
void drawLineTex(CanvasPoint point1, CanvasPoint point2);
void filledTriangleTex(CanvasTriangle points, Colour colour);
void fillBottomFlatTriangleTex(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, CanvasPoint btm, Colour colour);
void fillTopFlatTriangleTex(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour);

Colour texture[480*395]; //THIS SHOULD BE DONE PROGRAMMATICALLY
int width = 0;
int height = 0;

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
float depthBuf[HEIGHT][WIDTH] = {};

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
    else if (type == "map_Kd") { //Load texture map
      //Load texture ppm
      readImage(filename);
    }
  }


  return cMap;
}

//OBJ LOADER
//MAYBE THIS HOULD RETURN CANVAS TRIANGLE???????????????????????
std::vector<ModelTriangle> load_obj(string filename) {
  std::ifstream ifs (filename, std::ifstream::in);
  string line;
  string gap = " ";
  string seperator = "/";

  vector<vec3> vertices;

  vector<ModelTriangle> triangles;

  vector<TexturePoint> tPoints;

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

      cMap = readMtl(materialLib);
    }
    else if (type == "usemtl") { //We have changed what material we are using
      curMtl = values;
    }
    else if (type == "v") {
      stringstream ss(values);
      float x, y, z;
      ss >> x >> y >> z;
      vec3 newV = vec3(x, y, z);
      vertices.push_back(newV);
    }
    else if (type == "f") {
      //THIS NEEDS TO BE ADAPTED TO TAKE INTO ACCOUNT v/vt
      //GET THE TEXTURE POINT ASSOCIATED ITH THE VERTEX
      int v1, v2, v3;
      int t1, t2, t3;

      //GET THE VERTEX TO BE USED
      found = values.find(seperator);
      string vertex = values.substr(0, found);
      stringstream ss1(vertex);
      ss1 >> v1;
      CanvasPoint cp1 = CanvasPoint(vertices[v1-1].x, vertices[v1-1].y, vertices[v1-1].z);


      //GET THE TEXTURE POINT TO USE
      values = values.substr(found+1);
      found = values.find(gap);
      if (found != 0) { //If there is texture point associated with the vertex
        string vt = values.substr(0, found);
        stringstream tt1(vt);
        tt1 >> t1;
        // cp1.texturePoint = tPoints[t1 - 1];
      }
      // values = values.substr(found + 1);

      // FIND THE NEXT VALUE
      // OF THE FORM v/vt
      found = values.find(seperator);
      vertex = values.substr(0, found);
      stringstream ss2(vertex);
      ss2 >> v2;
      CanvasPoint cp2 = CanvasPoint(vertices[v2-1].x, vertices[v2-1].y, vertices[v2-1].z);

      values = values.substr(found+1);
      found = values.find(gap);
      if (found != 0) { //If there is texture point associated with the vertex
        string vt = values.substr(0, found);
        stringstream tt2(vt);
        tt2 >> t2;
        // cp2.texturePoint = tPoints[t2 - 1];
      }

      // FIND THE NEXT VALUE
      found = values.find(seperator);
      vertex = values.substr(0, found);
      stringstream ss3(vertex);
      ss3 >> v3;
      CanvasPoint cp3 = CanvasPoint(vertices[v3-1].x, vertices[v3-1].y, vertices[v3-1].z);

      values = values.substr(found+1);
      found = values.find(gap);
      if (found != 0) { //If there is texture point associated with the vertex
        string vt = values.substr(0, found);
        stringstream tt3(vt);
        tt3 >> t3;
        // cp3.texturePoint = tPoints[t3 - 1];
      }

      //SHOULD ADD A CHECK AND THEN DEFINE THE COLOUR
      CanvasTriangle cTriangle = CanvasTriangle(cp1, cp2, cp3);

      ModelTriangle triangle = ModelTriangle(vertices[v1-1], vertices[v2-1], vertices[v3-1], cMap[curMtl]);
      triangles.push_back(triangle);
    }
    else if (type == "vt") {
      stringstream ss(values);
      float x, y;
      ss >> x >> y;
      TexturePoint tP = TexturePoint(x, y);
      tPoints.push_back(tP);
      //Access element at a point using :
      // TexturePoint & element = tPoints[x];
      // OR use at()
    }
  }
  return triangles;
}

int main(int argc, char* argv[])
{
  //FOR TEXTURES
  readImage("texture.ppm");

  // writeImage("output.ppm");

  // vector<ModelTriangle> triangles = load_obj("cornell-box.obj");

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

void readImage(string filename) {
  std::ifstream ifs (filename, std::ifstream::in);
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

void writeImage(string filename) {
  ofstream myfile;
  myfile.open(filename);

  //File type
  myfile << "P6\n";
  //Whitespace
  myfile << "\n";
  //Dimensions
  myfile << WIDTH << " " << HEIGHT << "\n";
  //MaxVal
  myfile << 255 << "\n"; //We will say that this is our maximal value as we are using just 1 byte per colour channel

  //Then the actual pixel values
  for (int i = 0; i < HEIGHT; i++) { //FOR EACH ROW
    for (int j = 0; j < WIDTH; j++) { //FOR EACH PIXEL
      //WRITE IN THE COLOURS FOR THE PIXEL
      uint32_t pixColour = window.getPixelColour(j, i);
      //Get each byte for the colour channels
      unsigned char red = (pixColour >> (8 * 2)) & 0xff;
      unsigned char green = (pixColour >> (8 * 1)) & 0xff;
      unsigned char blue = (pixColour >> (8 * 0)) & 0xff;
      myfile << red << green << blue;
    }
  }
  printf("Finished writing to file\n");
}

void draw()
{
  window.clearPixels();

  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      depthBuf[i][j] = std::numeric_limits<float>::infinity();
    }
  }

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
  CanvasPoint cp1 = CanvasPoint(160.f, 10.f, 10.f);
  CanvasPoint cp2 = CanvasPoint(300.f, 230.f, 10.f);
  CanvasPoint cp3 = CanvasPoint(10.f, 150.f, 10.f);
  CanvasTriangle points = CanvasTriangle(cp1, cp2, cp3);
  // stroked(points, drawColour);
  filledTriangle(points, drawColour);

  //HAVE AN OVERLAPPING TRIANGLE WHICH SHOULD BE BEHIND
  drawColour = Colour(0, 255, 0);
  cp1 = CanvasPoint(250.f, 10.f, 30.f);
  cp2 = CanvasPoint(150.f, 240.f, 30.f);
  cp3 = CanvasPoint(20.f, 120.f, 30.f);
  points = CanvasTriangle(cp1, cp2, cp3);
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
  // stroked(points, drawColour);
  //Now fill the triangle
  // filledTriangleTex(points, drawColour);
*/

  //LAB 3 DEPTH BUFFER
  // float depthBuf[HEIGHT][WIDTH] = {};
  // for (int i = 0; i < HEIGHT; i++) {
  //   for (int j = 0; j < WIDTH; j++) {
  //     depthBuf[i][j] = std::numeric_limits<float>::infinity();
  //   }
  // }
  // cout << depthBuf[0][0] << "\n";

  //FOR THE HACKSPACE LOGO
  // vector<ModelTriangle> triangles = load_obj("logo.obj");

  //LAB 3 WIREFRAMES

  vector<ModelTriangle> triangles = load_obj("cornell-box.obj");
  // cout << triangles.size() << "\n";
  int len = int(triangles.size()); //GOT TO DECLARE HERE OTHERWISE WE POP THE LIST WHILST IT'S SIZE IS BEING USED AS A LOOP CONDITION
  // float xi = 0.f;
  // float yi = 0.f;

  for (int i = 0; i < len; i++) {
    ModelTriangle t = triangles.back();

    //GETTING DEPTH
    vec3 vertex1 = t.vertices[0] - camera.position;
    vec3 vertex2 = t.vertices[1] - camera.position;
    vec3 vertex3 = t.vertices[2] - camera.position;

    //GETTING PROJECTION ONTO IMAGE PLANE
    //For the first canvas point
    vertex1.x = (camera.focalLength * (vertex1.x / vertex1.z)) + WIDTH/2;
    vertex1.y = (camera.focalLength * (vertex1.y / vertex1.z)) + HEIGHT/2;
    CanvasPoint cp1 = CanvasPoint(vertex1.x, vertex1.y, vertex1.z);
    //For the second canvas point
    vertex2.x = (camera.focalLength * (vertex2.x / vertex2.z)) + WIDTH/2;
    vertex2.y = (camera.focalLength * (vertex2.y / vertex2.z)) + HEIGHT/2;
    CanvasPoint cp2 = CanvasPoint(vertex2.x, vertex2.y, vertex2.z);
    //For the third canvas point
    vertex3.x = (camera.focalLength * (vertex3.x / vertex3.z)) + WIDTH/2;
    vertex3.y = (camera.focalLength * (vertex3.y / vertex3.z)) + HEIGHT/2;
    CanvasPoint cp3 = CanvasPoint(vertex3.x, vertex3.y, vertex3.z);

    //THEN DRAW THE TRIANGLE
    CanvasTriangle points = CanvasTriangle(cp1, cp2, cp3);
    // stroked(points, t.colour);
    filledTriangle(points, t.colour);

    triangles.pop_back();
  }

  // vector<ModelTriangle> logo = load_obj("logo.obj");

  //Write current screen to file
  // std::stringstream ss;
  // ss << std::setw(5) << std::setfill('0') << outfile << ".ppm";
  // std::string s = ss.str();
  // cout << s << endl;

  //UNCOMMENT THIS LINE TO SAVE ALL FRAMES
  // writeImage(s);

  //Increment outfile name
  // outfile++;

}

void update()
{
  // Function for performing animation (shifting artifacts or moving the camera)
}

void handleEvent(SDL_Event event)
{
  //Calculate frame rate
  static int t = SDL_GetTicks();
  int t2 = SDL_GetTicks();
  float dt = float(t2 - t);
  t = t2;

  if(event.type == SDL_KEYDOWN) {
    if(event.key.keysym.sym == SDLK_LEFT) cout << "LEFT" << endl;
    else if(event.key.keysym.sym == SDLK_RIGHT) cout << "RIGHT" << endl;
    else if(event.key.keysym.sym == SDLK_UP) cout << "UP" << endl;
    else if(event.key.keysym.sym == SDLK_DOWN) cout << "DOWN" << endl;
    else if(event.key.keysym.sym == SDLK_w) {
      cout << "W" << endl;
      camera.position.z += camera.movementSpeed * dt;
    }
    else if(event.key.keysym.sym == SDLK_s) {
      cout << "S" << endl;
      camera.position.z -= camera.movementSpeed * dt;
    }
    else if(event.key.keysym.sym == SDLK_a) {
      cout << "A" << endl;
      camera.position.x -= camera.movementSpeed * dt;
    }
    else if(event.key.keysym.sym == SDLK_d) {
      cout << "D" << endl;
      camera.position.x += camera.movementSpeed * dt;
    }
    else if(event.key.keysym.sym == SDLK_q) {
      cout << "Q" << endl;
      camera.position.y -= camera.movementSpeed * dt;
    }
    else if(event.key.keysym.sym == SDLK_e) {
      cout << "E" << endl;
      camera.position.y += camera.movementSpeed * dt;
    }
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

  float stepX = (to.x - from.x)/float(glm::max(numberOfValues - 1, 1));
  float stepY = (to.y - from.y)/float(glm::max(numberOfValues - 1, 1));
  float stepZ = (to.z - from.z)/float(glm::max(numberOfValues - 1, 1));

  for (int i = 0; i < numberOfValues; i++){
    vec3 v = vec3(from.x + (i*stepX), from.y + (i*stepY), from.z + (i*stepZ));
    results.push_back(v);
  }

  return results;
}

void drawLine(CanvasPoint point1, CanvasPoint point2, Colour colour) {
  float xDiff = point2.x - point1.x;
  float yDiff = point2.y - point1.y;
  float zDiff = point2.depth - point1.depth;
  float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
  float xStepSize = xDiff/numberOfSteps;
  float yStepSize = yDiff/numberOfSteps;
  float zStepSize = zDiff/numberOfSteps;
  for (float i=0.0; i<numberOfSteps; i++) {
    float x = point1.x + (xStepSize*i);
    float y = point1.y + (yStepSize*i);
    float z = point1.depth + (zStepSize*i);

    float invZ = 1 / z;
    //SHOULD PROBABLY CHECK THAT THE DEPTH IS NOT NEGATIVE

    //BOUNDS CHECKING
    if ((x < 0) || (WIDTH - 1 < x)) {
      continue;
    }
    if ((y < 0) || (HEIGHT - 1 < y)) {
      continue;
    }

    float curDepth = depthBuf[int(y)][int(x)];
    //WE FLIPPED THE SIGN OF THE COMPARISON HERE BECAUSE THE COORDINATES FOR THE CORNELL BOX ARE NEGATIVE
    if ((curDepth == numeric_limits<float>::infinity()) || (curDepth > invZ)) {
      uint32_t colour32 = (255<<24) + (colour.red<<16) + (colour.green<<8) + colour.blue;
      window.setPixelColour(x, y, colour32);

      depthBuf[int(y)][int(x)] = invZ;
    }
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
  //DEPTH OF midpoint
  float proportion = (mid.y - top.y) / (btm.y - top.y);
  float midpointDepth = (proportion * (btm.depth - top.depth)) + top.depth;

  CanvasPoint midpoint = CanvasPoint(top.x + (((mid.y - top.y) / (btm.y - top.y)) * (btm.x - top.x)), mid.y, midpointDepth);

  //Then fill these two triangles that are produced
  fillBottomFlatTriangle(top, mid, midpoint, colour);
  fillTopFlatTriangle(mid, midpoint, btm, colour);

}

void fillBottomFlatTriangle(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour) {
  //TOP, MID, MIDPOINT
  vec3 vertex1 = vec3(point1.x, point1.y, float(point1.depth));
  vec3 vertex2 = vec3(point2.x, point2.y, float(point2.depth));
  vec3 vertex3 = vec3(point3.x, point3.y, float(point3.depth));

  std::vector<vec3> start = newInterpolate(vertex2, vertex1, point2.y - point1.y);
  std::vector<vec3> end = newInterpolate(vertex3, vertex1, point3.y - point1.y);

  int len = int(start.size());

  for (int i = 0; i < len; i++) {
    vec3 ls = start.back();
    start.pop_back();
    vec3 le = end.back();
    end.pop_back();

    CanvasPoint lineStart = CanvasPoint(ls.x, ls.y, ls.z);
    CanvasPoint lineEnd = CanvasPoint(le.x, le.y, le.z);

    drawLine(lineStart, lineEnd, colour);
  }


}

void fillTopFlatTriangle(CanvasPoint point1, CanvasPoint point2, CanvasPoint point3, Colour colour) {
  //MID, MIDPOINT, BTM
  vec3 vertex1 = vec3(point1.x, point1.y, float(point1.depth));
  vec3 vertex2 = vec3(point2.x, point2.y, float(point2.depth));
  vec3 vertex3 = vec3(point3.x, point3.y, float(point3.depth));

  std::vector<vec3> start = newInterpolate(vertex3, vertex1, point3.y - point1.y);
  std::vector<vec3> end = newInterpolate(vertex3, vertex2, point3.y - point2.y);

  int len = int(start.size());

  for (int i = 0; i < len; i++) {
    vec3 ls = start.back();
    start.pop_back();
    vec3 le = end.back();
    end.pop_back();

    CanvasPoint lineStart = CanvasPoint(ls.x, ls.y, ls.z);
    CanvasPoint lineEnd = CanvasPoint(le.x, le.y, le.z);

    drawLine(lineStart, lineEnd, colour);
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
