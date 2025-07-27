#ifndef QOPENGLRENDERWIDGET_H
#define QOPENGLRENDERWIDGET_H

#include <QOpenGLWidget>  // 相当于GLFW
#include <QOpenGLFunctions_4_5_Core>  // 相当于 GLAD
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QOpenGLTexture>


class QOpenGLRenderWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    struct VertexAttributeData
    {
        // Postion
        float pos[3];
        float coord[2];
        // float normal[3];
    };

    struct QuareData
    {
        VertexAttributeData p[4];
    };

public:
    explicit QOpenGLRenderWidget(QWidget *parent = nullptr);
    ~QOpenGLRenderWidget();

    // 设置/获取线框模式
    // void setFillStatus(bool isFill);
    // bool isFill(void);

    // // 混色参数
    // void setMixNumber(float number);
    // float getMixNumber(void);

    // // 设置旋转方向
    // void setRotationDirection(const QVector3D& vec);
    // QVector3D getRotationDirection(void);

    // // 获取/设置速度
    // void setSpeed(int speed);
    // int getSpeed(void);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    bool initShaderProgram(void);
    //    void createVertexAttributeData(VertexAttributeData* pVetAttr);

    // 添加纹理
    void createTexture(const QImage& image, GLuint& textureid);

    GLuint m_shaderProgramId;
    QOpenGLShaderProgram* m_pShaderProgram = nullptr;
    QOpenGLShader* m_pVertexShader = nullptr;
    QOpenGLShader* m_pFragmentShader = nullptr;

    GLuint m_nVAOId;
    GLuint m_nVBOId;
    GLuint m_nEBOId;

    GLuint m_nTextureId;
    GLuint m_nTextureId2;

    // Attribute Location
    GLint m_nPosAttrLocationId;
    GLint m_nCoordAttrLocationId;
    GLint m_nNormalAttrLocationId;

    // Uniform Location
    GLint m_nCoordLocationId;
    GLint m_nCoordLocationId2;
    GLint m_nMixNumLocationId;

    // MVP
    GLint m_nMLocationId;
    GLint m_nVLocationId;
    GLint m_nPLocationId;

    QMatrix4x4 m_MMat;
    QMatrix4x4 m_VMat;
    QMatrix4x4 m_PMat;

    bool m_isFill = true;
    float val_alpha = 0.5f;
    QVector3D m_rotateVec;
    int m_nSpeed = 8;

private:
    QVector<QVector3D> m_points;                // 顶点数组
    QVector<QVector2D> m_textureCoords;         // 纹理坐标
    QVector<QVector3D> m_normalVNs;              // 法线
    QVector<VertexAttributeData> m_cube;

    QOpenGLTexture* texture_wall_;  // 存储砖墙那张图片数据
    QOpenGLTexture* texture_nekosilverfox_;
    QOpenGLTexture* texture_nekosilverfox_bk_;

    // 初始化模型数据【立方体】
    void initModelData(void);

    // 旋转
    QTimer* m_pTimer = nullptr;
    void initTimer(void);
    void onTimeout(void);
    int m_angle = 0;
};

#endif // QOPENGLRENDERWIDGET_H
