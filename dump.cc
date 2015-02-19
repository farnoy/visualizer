#include <vector>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <iostream>
#include <fftw3.h>
#include <cmath>
#include "audio.hh"

int main()
{
  pa_sample_spec ss;
  ss.format = PA_SAMPLE_S16LE;
  ss.channels = 1;
  ss.rate = 44100;
  int error;
  auto pa = pa_simple_new(NULL,     // Use the default server.
                    "Fooapp", // Our application's name.
                    PA_STREAM_RECORD,
                    NULL,    // Use the default device.
                    "Music", // Description of our stream.
                    &ss,     // Our sample format.
                    NULL,    // Use default channel map
                    NULL,    // Use default buffering attributes.
                    &error);

  auto lat = pa_simple_get_latency(pa, NULL);
  // std::cout << "latency " << lat << std::endl;

  const auto bufsize = 256; //pa_usec_to_bytes(16000, &ss);
  // std::cout << "buffsize " << bufsize << std::endl;

  std::vector<uint16_t> buffer(bufsize);

  pa_simple_read(pa, buffer.data(), sizeof(uint16_t) * bufsize, &error);

  fftw_complex *in, *out;
  fftw_plan p;
  in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * bufsize);
  out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * bufsize);

  p = fftw_plan_dft_1d(bufsize, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

  auto result = process(&p, in, out, buffer, bufsize);

  // std::cout << "# Frequency\tPower" << std::endl;
  /*
  for (unsigned i = 0; i < result.size(); i++) {
    printf("%u\t%f\n", i, result[i]);
  }
  */


  fftw_free(in);
  fftw_free(out);
  fftw_destroy_plan(p);
  pa_simple_free(pa);

  return 0;
}
