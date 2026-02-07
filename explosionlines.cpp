#include <iostream>
#include <fstream>
#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <cmath>

typedef unsigned int uint;

using namespace std;

const uint width = 3840;
const uint height = 2160;

const uint centerX = width >> 1;
const uint centerY = height >> 1;

const uint particle_count = 10000;
const uint plength_min = 50;
const uint plength_var = 250;

void createppm(const string path, uint8_t* data) {
  ofstream f(path, ios::binary);
  f << "P6\n" << width << ' ' << height << "\n255\n";
  for (uint i = 0; i < width * height * 3; i++) {
    f << data[i];
  }
  f.close();
  cout << "Generated " << path << endl;
}

int main() {
  cout << "ueaaaa" << endl;
  srand(time(0));

  uint8_t* original = new uint8_t[width * height * 3];

  for (uint t = 0; t < particle_count; t++) {
    int x = rand() % width;
    int y = rand() % height;
    int length = plength_min + rand() % plength_var;

    int fromcenterX = x - centerX;
    int fromcenterY = y - centerY;

    float fromc_length = sqrt(fromcenterX*fromcenterX + fromcenterY*fromcenterY);

    float normX = (float)fromcenterX / fromc_length;
    float normY = (float)fromcenterY / fromc_length;

    if (abs(normX) > abs(normY)) {
      for (int i = 0; i < abs(normX * length); i++) {
        int actualX = x + (normX < 0 ? -i : i);
        int actualY = y + (int)(i / abs(normX * length) * normY * length + .5f);

        if (actualX < 0 || actualX >= width || actualY < 0 || actualY >= height) break;

        original[3 * (width * actualY + actualX)    ] = 255;
        original[3 * (width * actualY + actualX) + 1] = 255;
        original[3 * (width * actualY + actualX) + 2] = 255;
      }
    } else {
      for (int i = 0; i < abs(normY * length); i++) {
        int actualX = x + (int)(i / abs(normY * length) * normX * length + .5f);
        int actualY = y + (normY < 0 ? -i : i);

        if (actualX < 0 || actualX >= width || actualY < 0 || actualY >= height) break;

        original[3 * (width * actualY + actualX)    ] = 255;
        original[3 * (width * actualY + actualX) + 1] = 255;
        original[3 * (width * actualY + actualX) + 2] = 255;
      }
    }
  }

  createppm("output/explosionlines_output.ppm", original);


  return 0;
}
