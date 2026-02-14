#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdlib>
using namespace std;

typedef unsigned int uint;

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

// Arg order: maskable, mask, output (optional)
int main(int argc, char** argv) {
  if (argc != 3 && argc != 4) {
    cout << "You need to enter 2 or 3 extra arguments! Terminating." << endl;
    return 1;
  }

  string maskable_path;
  string mask_path;
  string output_path = "output/mask_output";
  for (int i = 1; i < argc; i++) {
    if (i == 1) {
      maskable_path = argv[i];
    } else if (i == 2) {
      mask_path = argv[i];
    } else {
      output_path = argv[i];
    }
  }

  ifstream maskable(maskable_path, ios::binary);
  string __magic;
  int __maxval;
  maskable >> __magic >> width >> height >> __maxval;
  maskable.ignore(1);
  uint8_t* maskable_data = new uint8_t[width * height * 3];
  maskable.read((char*)maskable_data, width * height * 3);
  cout << "Maskable initialized." << endl;
  maskable.close();

  ifstream mask(mask_path, ios::binary);
  __magic;
  __maxval;
  mask >> __magic >> width >> height >> __maxval;
  mask.ignore(1);
  uint8_t* mask_data = new uint8_t[width * height];
  for (uint i = 0; i < width * height; i++) {
    mask.read((char*)mask_data + i, 1);
    mask.ignore(2);
  }
  cout << "Mask initialized." << endl;
  mask.close();

  uint8_t* output = new uint8_t[width * height * 3];
  for (uint i = 0; i < width * height; i++) {
    output[i * 3    ] = (uint)((double)mask_data[i] / 255 * maskable_data[i * 3    ] + .5);
    output[i * 3 + 1] = (uint)((double)mask_data[i] / 255 * maskable_data[i * 3 + 1] + .5);
    output[i * 3 + 2] = (uint)((double)mask_data[i] / 255 * maskable_data[i * 3 + 2] + .5);
  }

  createppm(output_path + ".ppm", output);
  int exitcode = system(("pnmtopng " + output_path + ".ppm > " + output_path + ".png").c_str());
  cout << (system_succeeded(exitcode) ? "Generated the PNG version." : "Error: The PNG was NOT generated due to an error.") << endl;

  return 0;
}