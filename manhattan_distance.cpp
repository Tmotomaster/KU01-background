#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <queue>
#include <cmath>

typedef unsigned int uint;

using namespace std;

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

  for (int t = 0; t < 100; t++) {

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
    
    float current_proximity = proximity[coords->x * width + coords->y];
    // if (current_proximity == 0) continue;
    float proximity_max = 250.f;

    cout << "aa" << endl;
    float distance_right = sqrt((coords->x + 1 - coords->originX) * (coords->x + 1 - coords->originX) + (coords->y - coords->originY) * (coords->y - coords->originY));
    float distance_left = sqrt((coords->x - 1 - coords->originX) * (coords->x - 1 - coords->originX) + (coords->y - coords->originY) * (coords->y - coords->originY));
    float distance_up = sqrt((coords->x - coords->originX) * (coords->x - coords->originX) + (coords->y - 1 - coords->originY) * (coords->y - 1 - coords->originY));
    float distance_down = sqrt((coords->x - coords->originX) * (coords->x - coords->originX) + (coords->y + 1 - coords->originY) * (coords->y + 1 - coords->originY));

    float proximity_right = max(proximity_max - distance_right, 0.f) / proximity_max * 255.f;
    float proximity_left = max(proximity_max - distance_left, 0.f) / proximity_max * 255.f;
    float proximity_up = max(proximity_max - distance_up, 0.f) / proximity_max * 255.f;
    float proximity_down = max(proximity_max - distance_down, 0.f) / proximity_max * 255.f;

    cout << "bb" << endl;
    cout << "x " << coords->x << " y " << coords->y << endl;
    if (coords->x + 1 < width && proximity[coords->y * width + coords->x + 1] < proximity_right) {
    cout << "x " << coords->x << endl;
      proximity[coords->y * width + coords->x + 1] = proximity_right;
      to_check.push(new Distance(coords->x + 1, coords->y, coords->originX, coords->originY));
    }
    cout << "cc" << endl;
    if (coords->x > 0 && proximity[coords->y * width + coords->x - 1] < proximity_left) {
    cout << "x " << coords->x << endl;
      proximity[coords->y * width + coords->x - 1] = proximity_left;
      to_check.push(new Distance(coords->x - 1, coords->y, coords->originX, coords->originY));
    }
    cout << "dd" << endl;
    if (coords->y > 0 && proximity[(coords->y - 1) * width + coords->x] < proximity_up) {
    cout << "y " << coords->y << endl;
      proximity[(coords->y - 1) * width + coords->x] = proximity_up;
      to_check.push(new Distance(coords->x, coords->y - 1, coords->originX, coords->originY));
    }
    cout << "ee" << endl;
    if (coords->y + 1 < height && proximity[(coords->y + 1) * width + coords->x] < proximity_down) {
    cout << "y " << coords->y << endl;
      proximity[(coords->y + 1) * width + coords->x] = proximity_down;
      to_check.push(new Distance(coords->x, coords->y + 1, coords->originX, coords->originY));
    }
    delete coords;

    // if (coords.second + 1 < width && proximity[coords.first * width + coords.second + 1] < current_proximity - proximity_mod) {
    //   proximity[coords.first * width + coords.second + 1] = current_proximity - proximity_mod;
    //   to_check.push(pair<uint, uint>(coords.first, coords.second + 1));
    // }
    // if (coords.second > 0 && proximity[coords.first * width + coords.second - 1] < current_proximity - proximity_mod) {
    //   proximity[coords.first * width + coords.second - 1] = current_proximity - proximity_mod;
    //   to_check.push(pair<uint, uint>(coords.first, coords.second - 1));
    // }
    // if (coords.first + 1 < height && proximity[(coords.first + 1) * width + coords.second] < current_proximity - proximity_mod) {
    //   proximity[(coords.first + 1) * width + coords.second] = current_proximity - proximity_mod;
    //   to_check.push(pair<uint, uint>(coords.first + 1, coords.second));
    // }
    // if (coords.first > 0 && proximity[(coords.first - 1) * width + coords.second] < current_proximity - proximity_mod) {
    //   proximity[(coords.first - 1) * width + coords.second] = current_proximity - proximity_mod;
    //   to_check.push(pair<uint, uint>(coords.first - 1, coords.second));
    // }
  }

  uint8_t* proximity_data = new uint8_t[width * height];
  for (int i = 0; i < width * height; i++) {
    proximity_data[i] = (uint8_t)proximity[i];
  }

  creategrayscale("output_test" + to_string(t) + ".ppm", proximity_data);

  delete[] proximity;
  delete[] proximity_data;

  }

  return 0;
}