#include <iostream>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <vector>
#include "glwidget.hh"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QTimer>
#include <cmath>
#include "audio.hh"

static const auto msec = 16;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_program(0)
{
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    m_transparent = QCoreApplication::arguments().contains(QStringLiteral("--transparent"));
    if (m_transparent)
        setAttribute(Qt::WA_TranslucentBackground);

    int error = 0;
    m_paSpec = new pa_sample_spec;
    m_paSpec->format = PA_SAMPLE_S16LE;
    m_paSpec->channels = 2;
    m_paSpec->rate = 44100;
    pa_buffer_attr ba;
    ba.maxlength = 1000000;
    ba.fragsize = 1;
    m_pa = pa_simple_new(NULL,     // Use the default server.
                      "Fooapp", // Our application's name.
                      PA_STREAM_RECORD,
                      NULL,    // Use the default device.
                      "Music", // Description of our stream.
                      m_paSpec,     // Our sample format.
                      NULL,    // Use default channel map
                      &ba,    // Use default buffering attributes.
                      &error);

    auto lat = pa_simple_get_latency(m_pa, NULL);
    std::cout << "latency " << lat << std::endl;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(10);

    const auto bufsize = pa_usec_to_bytes(msec * 1000, m_paSpec);
    std::cout << "BUFSIZE " << bufsize << std::endl;

    m_plan = new fftw_plan;
    m_fftwIn  = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * bufsize);
    m_fftwOut = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * bufsize);
    *m_plan = fftw_plan_dft_1d(bufsize, m_fftwIn, m_fftwOut, FFTW_FORWARD, FFTW_ESTIMATE);
}

GLWidget::~GLWidget()
{
    cleanup();
    fftw_free(m_fftwIn);
    fftw_free(m_fftwOut);
    fftw_destroy_plan(*m_plan);
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void GLWidget::cleanup()
{
    makeCurrent();
    m_fsTriangleVbo.destroy();
    delete m_program;
    m_program = 0;
    doneCurrent();

    pa_simple_free(m_pa);
    delete m_paSpec;
}

static const char *vertexShaderSourceCore =
R"SHADEREND(#version 130
in vec2 vertex;
out vec2 ss;

void main() {
  gl_Position = vec4(vertex.xy, 0.0, 1.0);
  ss = vertex.xy * 0.5 + 0.5;
}
)SHADEREND";

static const char *fragmentShaderSourceCore =
R"END(#version 130
// const float densityMultiplier = 20.;
const float samplesCount = 87.;
const float sampleWidth  = 1. / samplesCount;

const vec4 sampleColor     = vec4(0.0, 0.0, 1.0, 1.0);
const vec4 backgroundColor = vec4(0.0, 0.0, 0.3, 1.0);

const float spacing  = .002;
// const float barWidth = (1. - 2.*spacing*samplesCount)/samplesCount;

uniform float samples[256];

in vec2 ss;

bool shouldDisplaySample(float x) {
  float rem = mod(x, sampleWidth);

  return rem > spacing;
}

float sampleIndex(float x) {
  return floor(x / sampleWidth);
}

float sampleFunc(float x) {
  float i = sampleIndex(x);
  float a = samples[int(i)];

  // a *= 1./4.;
  // a += .5;

  // a = a*a;

  return a;
}

void main(void)
{
  vec2 uv = ss;
  // uv.x *= densityMultiplier;

  if (shouldDisplaySample(uv.x) && uv.y < sampleFunc(uv.x)) {
    gl_FragColor = sampleColor;
  } else {
    gl_FragColor = backgroundColor;
  }
}
)END";

void GLWidget::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSourceCore);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSourceCore);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->link();

    m_program->bind();

    // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
    // implementations this is optional and support may not be present
    // at all. Nonetheless the below code works in all cases and makes
    // sure there is a VAO when one is needed.
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    QVector<GLfloat> data{-1, -1, 3, -1, -1, 3};
    // Setup our vertex buffer object.
    m_fsTriangleVbo.create();
    m_fsTriangleVbo.bind();
    m_fsTriangleVbo.allocate(data.constData(), data.count() * sizeof(GLfloat));

    m_audioDataVbo.create();
    m_audioDataVbo.bind();
    m_audioDataVbo.allocate(100 * sizeof(GLuint));

    // Store the vertex attribute bindings for the program.
    setupVertexAttribs();

    m_program->release();
}

void GLWidget::setupVertexAttribs()
{
    m_fsTriangleVbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    m_fsTriangleVbo.release();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();

    const auto bufsize = pa_usec_to_bytes(msec * 1000, m_paSpec);

    std::vector<uint16_t> buffer(bufsize);

    int error;
    pa_simple_read(m_pa, buffer.data(), sizeof(uint16_t) * bufsize, &error);
    pa_simple_flush(m_pa, &error);

    auto result = process(m_plan, m_fftwIn, m_fftwOut, buffer, bufsize);

    std::vector<GLfloat> converted;
    converted.reserve(result.size());

    for (int i = 1; i < result.size(); i++) {
      converted.push_back(static_cast<GLfloat>(result[i]));
    }

    m_program->setUniformValueArray("samples", converted.data(), converted.size(), 1);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    m_program->release();
}

void GLWidget::resizeGL(int w, int h)
{
}

