#include "glwidget.hh"
#include "window.hh"
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>

Window::Window() {
  glWidget = new GLWidget;

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(glWidget);

  setLayout(mainLayout);

  setWindowTitle(tr("Hello GL"));
}

void Window::keyPressEvent(QKeyEvent *e) {
  if (e->key() == Qt::Key_Escape)
    close();
  else
    QWidget::keyPressEvent(e);
}
