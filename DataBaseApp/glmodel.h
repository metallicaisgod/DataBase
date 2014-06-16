#ifndef GLMODEL_H
#define GLMODEL_H

#include <QGLWidget>

class GLModel : public QGLWidget
{
    Q_OBJECT
private:
      GLfloat xRot;
      GLfloat yRot;
      GLfloat zRot;
      GLfloat zTra;
      GLfloat nSca;

      GLfloat lightPos[4];

      QPoint ptrMousePosition;

      void scale_plus();
      void scale_minus();
      void rotate_up();
      void rotate_down();
      void rotate_left();
      void rotate_right();
      void translate_down();
      void translate_up();
      void defaultScene();

      void drawAxis();

      void getVertexArray();
      void getNormalArray();
      void getColorArray();
      void getIndexArray();
      void drawFigure();

   protected:
      void initializeGL();
      void resizeGL(int nWidth, int nHeight);
      void paintGL();
      void mousePressEvent(QMouseEvent* pe);
      void mouseMoveEvent(QMouseEvent* pe);
      void mouseReleaseEvent(QMouseEvent* pe);
      void wheelEvent(QWheelEvent* pe);
      void keyPressEvent(QKeyEvent* pe);

   public:
      GLModel(QWidget* parent = 0);

};

#endif // GLMODEL_H
