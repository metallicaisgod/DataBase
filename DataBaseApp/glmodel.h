#ifndef GLMODEL_H
#define GLMODEL_H

#include <QGLWidget>

#include "..\iadatabase\headers\IADataBase.h"

struct GLFloatTriplet
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

struct GLUShortTriplet
{
    GLushort x;
    GLushort y;
    GLushort z;
};


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

      db::DbImplant * m_curImpl;
      db::DbAbutment * m_curAbut;

      int m_faceCount;
      int m_vertexCount;
      int m_trianglesCount;

      GLFloatTriplet *  m_vertexArray;
      GLFloatTriplet *  m_normalArray;
      GLFloatTriplet *  m_colorArray;
      GLUShortTriplet * m_indexArray;


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

      void setCurrentImplant(db::DbImplant * impl);
      void setCurrentAbutment(db::DbAbutment * abut);

};

#endif // GLMODEL_H
