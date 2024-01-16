//
// Created by lukemartinlogan on 1/16/24.
//
#include <vector>
#include <fstream>
#include "allvars.h"

int main() {
  int N = 16;
  long Ntot = N * N * N;
  double BoxSize = 50000;

  struct io_header_1 header = {0};
  header.num_files = 1;
  header.BoxSize = BoxSize;
  header.npartTotal[1] = Ntot;
  header.npart[1] = Ntot;

  std::vector<float> glass(N * N * N * 3);
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      for (int k = 0; k < N; ++k) {
        size_t idx = i * N * N * 3 + j * N * 3 + k * 3;
        glass[idx + 0] = (i + 0.0)/N * header.BoxSize;
        glass[idx + 1] = (j + 0.0)/N * header.BoxSize;
        glass[idx + 2] = (k + 0.0)/N * header.BoxSize;
      }
    }
  }

  std::ofstream file("cppglass.dat", std::ios::binary);
  // writeu,1, npart,massarr,time,redshift,flag_sfr,flag_feedback,npartall,flag_cooling,num_files,BoxSize,la
  int hdr_size = sizeof(header);
  file.write(reinterpret_cast<char*>(&hdr_size), sizeof(int));
  file.write(reinterpret_cast<char*>(&header), sizeof(header));
  file.write(reinterpret_cast<char*>(&hdr_size), sizeof(int));
  // writeu,1, pos
  int pos_size = sizeof(float) * glass.size();\
  file.write(reinterpret_cast<char*>(&pos_size), sizeof(int));
  file.write(reinterpret_cast<char*>(&glass[0]), sizeof(float) * glass.size());
  file.write(reinterpret_cast<char*>(&pos_size), sizeof(int));
  file.close();
}