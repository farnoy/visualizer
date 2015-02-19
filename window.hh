#include <QWidget>

class QSlider;
class QPushButton;

class GLWidget;

class Window : public QWidget {
  Q_OBJECT

public:
  Window();

protected:
  void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
  GLWidget *glWidget;
};
