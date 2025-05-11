#ifndef TEXTUREWIDGET_H
#define TEXTUREWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLTexture>



class TextureWidget : public QOpenGLWidget, public QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    struct VertexAttributeData
    {
        // Postion
        float pos[3];
        float color[3];
        float coord[2];
    };

public:
    TextureWidget(QWidget* parent = nullptr);
    ~TextureWidget();

    // 设置/获取线框模式
    void setFillStatus(bool isFill);
    bool isFill(void);

    // 混色参数
    void setMixNumber(float number);
    float getMixNumber(void);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    bool initShaderProgram(void);
    void createVertexAttributeData(VertexAttributeData* pVetAttr);

    // 添加纹理
    void createTexture(const QImage& image, GLuint& textureid);

    GLuint m_shaderProgramId;
    QOpenGLShaderProgram m_pShaderProgram;
    QOpenGLShader* m_pVertexShader = nullptr;
    QOpenGLShader* m_pFragmentShader = nullptr;

    GLuint m_nVBOId;
    GLuint m_nVAOId;
    GLuint m_nEBOId;

    GLuint m_nTextureId;
    GLuint m_nTextureId2;

    QOpenGLTexture *m_Texture;
    QOpenGLTexture *texture_wall_;
    QOpenGLTexture *texture_nekosilverfox_;

    // Attribute Location
    GLint m_nPosAttrLocationId;
    GLint m_nColorAttrLocationId;
    GLint m_nCoordAttrLocationId;

    // Uniform Location
    GLint m_nCoordLocationId;
    GLint m_nCoordLocationId2;
    GLint m_nMixNumLocationId;

    bool m_isFill = true;
    float m_mixNumber = 0.5f;
};

#endif // TEXTUREWIDGET_H
