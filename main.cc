#include <iostream>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <vector>
#include <QApplication>
#include "window.hh"

#define BUFSIZE 1024

template <typename T> std::vector<T> reduce(const std::vector<T> &in) {
  std::vector<T> reduced(in.size() / 2);
  for (unsigned i = 0; i < in.size(); i += 2) {
    reduced[i / 2] = in[i] + in[i + 1] / 2;
  }
  return reduced;
}

int main(int argc, char **argv) {
  /*
  int error = 0;
  pa_simple *s;
  pa_sample_spec ss;
  ss.format = PA_SAMPLE_S16LE;
  ss.channels = 2;
  ss.rate = 44100;
  s = pa_simple_new(NULL,     // Use the default server.
                    "Fooapp", // Our application's name.
                    PA_STREAM_RECORD,
                    NULL,    // Use the default device.
                    "Music", // Description of our stream.
                    &ss,     // Our sample format.
                    NULL,    // Use default channel map
                    NULL,    // Use default buffering attributes.
                    &error);

  const auto bufsize = pa_usec_to_bytes(16000, &ss);

  std::vector<uint16_t> buffer(bufsize);

  auto ret =
      pa_simple_read(s, buffer.data(), sizeof(uint16_t) * bufsize, &error);
  std::cout << "RET " << ret << std::endl;
  // std::cout << "BUFSIZE " << bufsize << std::endl;

  // fwrite(buffer.data(), sizeof(uint8_t), bufsize, stdout);

  pa_simple_free(s);

  auto reduced = reduce(reduce(reduce(reduce(reduce(buffer)))));
  // std::cout << reduced.size() << std::endl;

  // fwrite(reduced.data(), sizeof(uint16_t), reduced.size(), stdout);

  for (auto &e : reduced) {
    printf("%u\n", e);
  }
  */

  QApplication app(argc, argv);

  Window win;
  win.show();

  return app.exec();
}
