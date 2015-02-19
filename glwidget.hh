#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <fftw3.h>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
struct pa_simple;
struct pa_sample_spec;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

public:
  GLWidget(QWidget *parent = 0);
  ~GLWidget();

  QSize minimumSizeHint() const Q_DECL_OVERRIDE;
  QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
  void cleanup();

signals:
  void xRotationChanged(int angle);
  void yRotationChanged(int angle);
  void zRotationChanged(int angle);

protected:
  void initializeGL() Q_DECL_OVERRIDE;
  void paintGL() Q_DECL_OVERRIDE;
  void resizeGL(int width, int height) Q_DECL_OVERRIDE;

private:
  void setupVertexAttribs();

  QOpenGLVertexArrayObject m_vao;
  QOpenGLBuffer m_fsTriangleVbo;
  QOpenGLBuffer m_audioDataVbo;
  QOpenGLShaderProgram *m_program;
  bool m_transparent;
  pa_simple* m_pa;
  pa_sample_spec* m_paSpec;
  QSurfaceFormat* m_surfaceFormat;
  fftw_plan* m_plan;
  fftw_complex* m_fftwIn, *m_fftwOut;
};
