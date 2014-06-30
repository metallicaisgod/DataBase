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
      int xRot;
      int yRot;
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

      void getImplVertexArray();
      void getImplNormalArray();
      void getImplColorArray();
      void getImplIndexArray();
      void drawImplant();

      void getAbutVertexArray();
      void getAbutNormalArray();
      void getAbutColorArray();
      void getAbutIndexArray();
      void drawAbutment();

      db::DbImplant * m_curImpl;
      db::DbAbutment * m_curAbut;

      int m_faceCount;
      int m_implVertexCount;
      int m_implTrianglesCount;
      int m_abutVertexCount;
      int m_abutTrianglesCount;

      GLFloatTriplet *  m_implVertexArray;
      GLFloatTriplet *  m_implNormalArray;
      GLFloatTriplet *  m_implColorArray;
      GLUShortTriplet * m_implIndexArray;

      GLFloatTriplet *  m_abutVertexArray;
      GLFloatTriplet *  m_abutNormalArray;
      GLFloatTriplet *  m_abutColorArray;
      GLUShortTriplet * m_abutIndexArray;


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
