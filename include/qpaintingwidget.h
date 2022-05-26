#ifndef QPAINTINGWIDGET_H
#define QPAINTINGWIDGET_H

#include <QWidget>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <QOpenGLWidget>
class QPaintingWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit QPaintingWidget(QWidget *parent = nullptr);
    ~QPaintingWidget();

signals:

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    GLuint shaderProgram ;
    GLuint VBO, Color, VAO;

};

#endif // QPAINTINGWIDGET_H



