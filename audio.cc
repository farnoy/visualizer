#include <vector>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <iostream>
#include <fftw3.h>
#include <cmath>
#include "audio.hh"


std::vector<double> process(fftw_plan* p, fftw_complex* in, fftw_complex* out, const std::vector<uint16_t>& buffer, const size_t bufsize) {
  double rate = 10*2*M_PI/bufsize;
  double rate2 = 4*2*M_PI/bufsize;
  for (unsigned i = 0; i < bufsize; i++) {
    double multiplier = 0.5 * (1 - cos(2*M_PI*i/(bufsize-1)));
    in[i][0] = static_cast<double>(buffer[i]) * multiplier;// (buffer[i]);// * multiplier;
    printf("%u\t%f\t%f\n", i, static_cast<double>(buffer[i]), sin(i * rate2));
    in[i][1] = 0.0;
  }

  fftw_execute(*p);

  std::vector<double> extracted;
  extracted.reserve(bufsize);

  for (unsigned i = 1; i < bufsize; i++) {
    double mag = sqrt(out[i][0] * out[i][0] +
                      out[i][1] * out[i][1]);
    mag = 20 * log10(mag);
    extracted.push_back(mag);
    // printf("%u\t%f\t%f\t%f\t%f\n", i, in[i][0], out[i][0], out[i][1], mag);
  }

  auto reduced = extracted; //reduce(reduce(extracted));

  /*
  double max = 0;
  for (auto const& n : reduced) {
    if (n > max)
      max = n;
  }
  */

  for (unsigned i = 0; i < reduced.size(); i++) {
    // reduced[i] /= max;

    // printf("%u\t%f\n", i, reduced[i]);
  }

  return reduced;
}
