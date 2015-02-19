#pragma once
#include <vector>

template <typename T> static std::vector<T> reduce(std::vector<T> in) {
  if (in.size() % 2 > 0)
    in.pop_back();
  std::vector<T> reduced(in.size() / 2);
  for (unsigned i = 0; i < in.size(); i += 2) {
    reduced.at(i / 2) = (in.at(i) + in.at(i + 1)) / 2.f;
  }
  return reduced;
}

std::vector<double> process(fftw_plan* p, fftw_complex* in, fftw_complex* out, const std::vector<uint16_t>& buffer, const size_t bufsize);
