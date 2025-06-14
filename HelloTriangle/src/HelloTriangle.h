#ifndef HELLOTRIANGLEWIDGET_H
#define HELLOTRIANGLEWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions_3_3_Core>
// 1234
class HelloTriangleWidget : public QOpenGLWidget, public QOpenGLFunctions_2_0
{
    Q_OBJECT
public:
    struct VertexAttributeData
    {
        // Postion
        float pos[3];
        float color[3];
    };

public:
    HelloTriangleWidget(QWidget* parent = nullptr);
    ~HelloTriangleWidget();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    bool initShaderProgram(void);
    void createVertexAttributeData(VertexAttributeData* pVetAttr);

    GLuint m_shaderProgramId;
    QOpenGLShaderProgram* m_pShaderProgram = nullptr;
    QOpenGLShader* m_pVertexShader = nullptr;
    QOpenGLShader* m_pFragmentShader = nullptr;

    GLuint m_nVBOId;

    // Attribute Location
    GLint m_nPosAttrLocationId;
    GLint m_nColorAttrLocationId;
};
#endif
