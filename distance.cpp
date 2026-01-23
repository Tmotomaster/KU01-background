#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <queue>
#include <cmath>

#include "lookup_colors.h"

typedef unsigned int uint;

using namespace std;

const float proximity_max = 250.f;

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

float calc_distance(int y1, int x1, int y2, int x2) {
  return (float)(abs(y2 - y1) + abs(x2 - x1));
  // return sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));
}

int main() {
  ifstream raw("_srcdata.txt");
  raw >> width >> height;
  uint8_t* original = new uint8_t[width * height * 3];
  for (uint i = 0; i < width * height * 3; i++) {
    uint value;
    raw >> value;
    original[i] = value;
  }
  cout << "Data initialized.\n";
  raw.close();

  float* proximity = new float[width * height] {};
  for (uint i = 0; i < width * height; i++) {
    // cout << original[3*i  ] << endl;
    if (original[3*i  ] > 0) {
      // cout << original[0] << endl;
      to_check.push(new Distance(i / width, i % width, i / width, i % width));
      // to_check.push(pair<uint, uint> {i / width, i % width});
      proximity[i] = 255.f;
    }
  }

  while (!to_check.empty()) {
    Distance* coords = to_check.front();
    to_check.pop();
    
    float current_proximity = proximity[coords->y * width + coords->x];
    // if (current_proximity == 0) continue;

    float distance_right = calc_distance(coords->y, coords->x + 1, coords->originY, coords->originX);
    float distance_left = calc_distance(coords->y, coords->x - 1, coords->originY, coords->originX);
    float distance_up = calc_distance(coords->y - 1, coords->x, coords->originY, coords->originX);
    float distance_down = calc_distance(coords->y + 1, coords->x, coords->originY, coords->originX);

    float proximity_right = max(proximity_max - distance_right, 0.f) / proximity_max * 255.f;
    float proximity_left = max(proximity_max - distance_left, 0.f) / proximity_max * 255.f;
    float proximity_up = max(proximity_max - distance_up, 0.f) / proximity_max * 255.f;
    float proximity_down = max(proximity_max - distance_down, 0.f) / proximity_max * 255.f;

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
    uint8_t* result = grayscale_colors(pow((double)proximity[i] / 255, 1) * 255.f);
    proximity_data[i*3  ] = result[0];
    proximity_data[i*3+1] = result[1];
    proximity_data[i*3+2] = result[2];
    delete[] result;
  }

  createppm("output/output_test.ppm", proximity_data);

  delete[] proximity;
  delete[] proximity_data;

  return 0;
}