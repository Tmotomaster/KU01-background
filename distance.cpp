#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <queue>
#include <cmath>
#include <cstring>

#include "lookup_colors.h"

typedef unsigned int uint;

using namespace std;

// const float proximity_max = 250.f;

uint width, height;

void createppm(const string path, uint8_t* data) {
  ofstream f(path, ios::binary);
  f << "P6\n" << width << ' ' << height << "\n255\n";
  for (uint i = 0; i < width * height * 3; i++) {
    f << data[i];
  }
  f.close();
  cout << "Generated " << path << endl;
}

void creategrayscale(const string path, uint8_t* data) {
  ofstream f(path, ios::binary);
  f << "P6\n" << width << ' ' << height << "\n255\n";
  for (uint i = 0; i < width * height; i++) {
    f << data[i];
    f << data[i];
    f << data[i];
  }
  f.close();
  cout << "Generated " << path << endl;
}

#ifndef _WIN32
#include <sys/wait.h>
#endif

bool system_succeeded(int value) {
  if (value == -1) return false;

  #ifdef _WIN32
  return (value == 0);
  #else
  return (WIFEXITED(value) && WEXITSTATUS(value) == 0);
  #endif
}

struct Distance {
  int y;
  int x;
  int originY;
  int originX;

  Distance(int _y, int _x, int _oy, int _ox) {
    y = _y;
    x = _x;
    originY = _oy;
    originX = _ox;
  }
};

queue<Distance*> to_check;

double proximity_max = 250.;

int distance_algorithm = 0;

double calc_distance(int y1, int x1, int y2, int x2) {
  switch (distance_algorithm) {
    case 1: // Manhattan
      return (double)(abs(y2 - y1) + abs(x2 - x1));
    case 2: // Square
      return (double)max(abs(y2 - y1), abs(x2 - x1));
    case 3: // Flatter
      return sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1)) + abs(y2 - y1);
    case 4: // No bottom left
      return (y2 - y1 < 0 || x2 - x1 > 0) ? proximity_max : sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));
    default: // Euclidean
      return sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));
  }
}

int main(int argc, char** argv) {
  bool asking = true;
  bool dopng = true;
  string outputfile = "output/distance_output";
  bool reading1 = false;
  string inputppm;
  bool reading_prox = false;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--input") == 0 || strcmp(argv[i], "-i") == 0) {
      asking = false;
      reading1 = true;
    } else if (reading1) {
      inputppm = argv[i];
      reading1 = false;
    } else if (strcmp(argv[i], "--proximity_max") == 0 || strcmp(argv[i], "-p") == 0) {
      reading_prox = true;
    } else if (reading_prox) {
      proximity_max = atof(argv[i]);
      reading_prox = false;
    } else if (strcmp(argv[i], "--noask") == 0 || strcmp(argv[i], "-n") == 0) {
      asking = false;
    } else if (strcmp(argv[i], "--dontpng") == 0 || strcmp(argv[i], "-d") == 0) {
      dopng = false;
    } else if (strcmp(argv[i], "--manhattan") == 0) {
      distance_algorithm = 1;
    } else if (strcmp(argv[i], "--square") == 0) {
      distance_algorithm = 2;
    } else if (strcmp(argv[i], "--flatter") == 0) {
      distance_algorithm = 3;
    } else if (strcmp(argv[i], "--nobottomleft") == 0) {
      distance_algorithm = 4;
    } else {
      outputfile = argv[i];
    }
  }
  if (asking && inputppm == "") {
    int exitcode = system("python3 imgtodata.py");
    if (!system_succeeded(exitcode)) {
      cout << "Python program terminated early" << endl;
      return 1;
    }
  }

  ifstream raw(inputppm != "" ? inputppm : "_srcdata.ppm", ios::binary);
  string __magic;
  int __maxval;
  raw >> __magic >> width >> height >> __maxval;
  raw.ignore(1);
  uint8_t* original = new uint8_t[width * height * 3];
  raw.read((char*)original, width * height * 3);
  cout << "Data initialized." << endl;
  raw.close();

  double* proximity = new double[width * height] {};
  for (uint i = 0; i < width * height; i++) {
    // cout << original[3*i  ] << endl;
    if (original[3*i  ] > 127) {
      // cout << original[0] << endl;
      to_check.push(new Distance(i / width, i % width, i / width, i % width));
      // to_check.push(pair<uint, uint> {i / width, i % width});
      proximity[i] = 255.;
    }
  }

  while (!to_check.empty()) {
    Distance* coords = to_check.front();
    to_check.pop();
    
    double current_proximity = proximity[coords->y * width + coords->x];
    // if (current_proximity == 0) continue;

    double distance_right = calc_distance(coords->y, coords->x + 1, coords->originY, coords->originX);
    double distance_left = calc_distance(coords->y, coords->x - 1, coords->originY, coords->originX);
    double distance_up = calc_distance(coords->y - 1, coords->x, coords->originY, coords->originX);
    double distance_down = calc_distance(coords->y + 1, coords->x, coords->originY, coords->originX);

    double proximity_right = max(proximity_max - distance_right, 0.) / proximity_max * 255.;
    double proximity_left = max(proximity_max - distance_left, 0.) / proximity_max * 255.;
    double proximity_up = max(proximity_max - distance_up, 0.) / proximity_max * 255.;
    double proximity_down = max(proximity_max - distance_down, 0.) / proximity_max * 255.;

    if (coords->x + 1 < width && proximity[coords->y * width + coords->x + 1] < proximity_right) {
      proximity[coords->y * width + coords->x + 1] = proximity_right;
      to_check.push(new Distance(coords->y, coords->x + 1, coords->originY, coords->originX));
    }
    if (coords->x > 0 && proximity[coords->y * width + coords->x - 1] < proximity_left) {
      proximity[coords->y * width + coords->x - 1] = proximity_left;
      to_check.push(new Distance(coords->y, coords->x - 1, coords->originY, coords->originX));
    }
    if (coords->y > 0 && proximity[(coords->y - 1) * width + coords->x] < proximity_up) {
      proximity[(coords->y - 1) * width + coords->x] = proximity_up;
      to_check.push(new Distance(coords->y - 1, coords->x, coords->originY, coords->originX));
    }
    if (coords->y + 1 < height && proximity[(coords->y + 1) * width + coords->x] < proximity_down) {
      proximity[(coords->y + 1) * width + coords->x] = proximity_down;
      to_check.push(new Distance(coords->y + 1, coords->x, coords->originY, coords->originX));
    }
    delete coords;
  }

  uint8_t* proximity_data = new uint8_t[width * height * 3];
  cout << "Converting proximity to colors" << endl;
  for (int i = 0; i < width * height; i++) {
    // proximity_data[i] = (uint8_t)(proximity[i] + .5f);
    uint8_t* result = grayscale_colors(pow((double)proximity[i] / 255, 1) * 255.);
    proximity_data[i*3  ] = result[0];
    proximity_data[i*3+1] = result[1];
    proximity_data[i*3+2] = result[2];
    delete[] result;
  }

  createppm(outputfile + ".ppm", proximity_data);
  if (dopng) {
    int exitcode = system(("pnmtopng " + outputfile + ".ppm > " + outputfile + ".png").c_str());
    cout << (system_succeeded(exitcode) ? "Generated the PNG version." : "Error: The PNG was NOT generated due to an error.") << endl;
  }

  delete[] proximity;
  delete[] proximity_data;

  return 0;
}