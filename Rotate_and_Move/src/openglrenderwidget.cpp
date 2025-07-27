#include "openglrenderwidget.h"
#include <QDebug>
#include <QTime>

OpenGLRenderWidget::OpenGLRenderWidget(QWidget* parent)
    :QOpenGLWidget(parent)
{
    this->timer_.start(100);
    connect(&this->timer_, SIGNAL(timeout()),
            this, SLOT(rotate()));
}

OpenGLRenderWidget::~OpenGLRenderWidget()
{
    qDebug() << __FUNCTION__;
    if (!isValid()) return;  // 如果 paintGL 没有执行，下面的代码不存在（着色器 VAO VBO之类的），所以避免出错。如果他们没有执行就直接 return

    makeCurrent();

    /* 对象的回收 */
    glDeleteVertexArrays(1, &m_nVAOId);
    glDeleteBuffers(1, &m_nVBOId);
    glDeleteBuffers(1, &m_nEBOId);

    doneCurrent();
    update();
}

void OpenGLRenderWidget::initializeGL()
{
    this->initializeOpenGLFunctions();

    // 初始化GPU程序
    bool result = initShaderProgram();
    if (!result)
        return;

    // 创建顶点属性数据
    VertexAttributeData vAttrData[4];
    createVertexAttributeData(vAttrData);
    // 创建EBO数据
    GLuint indices[] = {0, 1, 3, 1, 2, 3};
    // 创建VAO
    glGenVertexArrays(1, &m_nVAOId);
    // 创建VBO
    glGenBuffers(1, &m_nVBOId);
    // 创建EBO
    glGenBuffers(1, &m_nEBOId);

    glBindVertexArray(m_nVAOId);

    // 初始化VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_nVBOId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vAttrData), vAttrData, GL_STATIC_DRAW);

    // 初始化EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nEBOId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /* 告知显卡如何解析缓冲区里面的属性值
        void glVertexAttribPointer(
                                    GLuint index,  // VAO 中的第几个属性（VAO 属性的索引）
                                    GLint size,  // VAO 中的第几个属性中对应的位置放几份数据
                                    GLEnum type,  // 存放数据的数据类型
                                    GLboolean normalized,  // 是否标准化
                                    GLsizei stride,  // 步长
                                    const void* offset  // 偏移量
        )
    */

    // 设置顶点信息属性指针
    this->m_pShaderProgram.bind();  // 如果使用 QShaderProgram，那么最好在获取顶点属性位置前，先 bind()
    m_nPosAttrLocationId = this->m_pShaderProgram.attributeLocation("aPos");  // 获取顶点着色器中顶点属性 aPos 的位置
    glVertexAttribPointer(m_nPosAttrLocationId, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributeData), (void*)0);
    glEnableVertexAttribArray(m_nPosAttrLocationId);
    // 设置颜色信息属性指针
    this->m_pShaderProgram.bind();
    m_nColorAttrLocationId = this->m_pShaderProgram.attributeLocation("aColor");
    glVertexAttribPointer(m_nColorAttrLocationId, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributeData), (void*)(sizeof (float) * 3));
    glEnableVertexAttribArray(m_nColorAttrLocationId);
    // 设置纹理坐标
    this->m_pShaderProgram.bind();
    m_nCoordAttrLocationId = this->m_pShaderProgram.attributeLocation("aTexel");
    glVertexAttribPointer(m_nCoordAttrLocationId, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttributeData), (void*)(sizeof (float) * 6));
    glEnableVertexAttribArray(m_nCoordAttrLocationId);

    // 创建第一个纹理
    this->m_pShaderProgram.bind();
    this->m_pShaderProgram.setUniformValue("texture0", 0);  // 【重点】当涉及到多个纹理使，一定要为 uniform 设置纹理单元的编号
    this->texture_wall_ = new QOpenGLTexture(QImage(":/image/wall.jpg").mirrored());  // 因为QOpenGL的y轴是反的（镜像），所以需要mirrored翻转一下

    // 创建第二个纹理
    this->m_pShaderProgram.bind();
    this->m_pShaderProgram.setUniformValue("texture1", 1);
    this->texture_nekosilverfox_ = new QOpenGLTexture(QImage(":/image/nekosilverfox.png").mirrored());
    // 纹理环绕方式
    this->texture_nekosilverfox_->bind(1);  // 【重点】注意！再修改纹理之前要先绑定到对应的纹理单元上！
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);// T轴纹理【环绕】方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);// S轴纹理【环绕】方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // 放大时纹理【过滤】方式

    this->m_pShaderProgram.bind();
    this->m_pShaderProgram.setUniformValue("val_alpha", m_mixNumber);


    // ===================== 解绑 =====================
    // 解绑 VAO 和 VBO，注意先解绑 VAO再解绑EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);  // 注意 VAO 不参与管理 VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLRenderWidget::resizeGL(int w, int h)
{
    this->glViewport(0, 0, w, h);

    return QOpenGLWidget::resizeGL(w, h);
}

void OpenGLRenderWidget::paintGL()
{
    glClearColor(51.0f / 255.0f, 76.0f / 255.0f, 76.0f / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_isFill)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /* 重新绑定 VAO */
    glBindVertexArray(m_nVAOId);

    // 使用shader
    this->m_pShaderProgram.bind();

    QMatrix4x4 matrix;  // QMatrix 默认生成的是一个单位矩阵（对角线上的元素为1）
    // 【重点】这里是先旋转再位移！！ 因为矩阵运算 T*R*v 顺序是从右向左的！！不然转轴不是图片中心！！
    // 3. 绕 Z 轴旋转（假设旋转角度随时间变化）
    float angle = 90.0f; // 假设旋转 45 度（实际可以用 QTime 或动画控制）
    unsigned int time_ms = QTime::currentTime().msec();
    matrix.rotate(angle, 0.0f, 0.0f, 1.0f);  // 旋转
    // matrix.translate(0.4f, -0.4f, 0.0f);  // 位移
    // matrix.rotate(time_ms, 0.0f, 0.0f, 1.0f);  // 旋转


    this->m_pShaderProgram.setUniformValue("matrix", matrix);

    this->texture_wall_->bind(0);  // 绑定纹理单元0的数据，并激活对应区域
    this->texture_nekosilverfox_->bind(1);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool OpenGLRenderWidget::initShaderProgram(void)
{
    initializeOpenGLFunctions();  // 【重点】初始化OpenGL函数，将 Qt 里的函数指针指向显卡的函数（头文件 QOpenGLFunctions_X_X_Core）
    // 顶点着色器
    if (!m_pShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/vertex.vsh"))
    {
        qDebug() << __FILE__ << __FUNCTION__ << " add vertex shader file failed.";
        // close();
        return false;
    }
    // 片段着色器
    if (!m_pShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/fragment.fsh"))
    {
        qDebug() << __FILE__ << __FUNCTION__ << " add fragment shader file failed.";
        // close();
        return false;
    }
    // 链接着色器
    if (!m_pShaderProgram.link())
    {
        qDebug() << __FILE__ << __LINE__ << "program link failed";
        // close();
        return false;
    }

    return true;
}

void OpenGLRenderWidget::createVertexAttributeData(VertexAttributeData* pVetAttr)
{
    // 第一个点位置信息
    pVetAttr[0].pos[0] = -0.5f;
    pVetAttr[0].pos[1] = 0.5f;
    pVetAttr[0].pos[2] = 0.0f;
    // 第一个点颜色信息
    pVetAttr[0].color[0] = 1.0f;
    pVetAttr[0].color[1] = 0.0f;
    pVetAttr[0].color[2] = 0.0f;
    // 第一个点的纹理坐标
    pVetAttr[0].coord[0] = 0.0f;
    pVetAttr[0].coord[1] = 1.0f;

    // 第二个点位置信息
    pVetAttr[1].pos[0] = -0.5f;
    pVetAttr[1].pos[1] = -0.5f;
    pVetAttr[1].pos[2] = 0.0f;
    // 第二个点颜色信息
    pVetAttr[1].color[0] = 0.0f;
    pVetAttr[1].color[1] = 1.0f;
    pVetAttr[1].color[2] = 0.0f;
    // 第二个点的纹理坐标
    pVetAttr[1].coord[0] = 0.0f;
    pVetAttr[1].coord[1] = 0.0f;

    // 第三个点位置信息
    pVetAttr[2].pos[0] = 0.5f;
    pVetAttr[2].pos[1] = -0.5f;
    pVetAttr[2].pos[2] = 0.0f;
    // 第三个点颜色信息
    pVetAttr[2].color[0] = 0.0f;
    pVetAttr[2].color[1] = 0.0f;
    pVetAttr[2].color[2] = 1.0f;
    // 第三个点的纹理坐标
    pVetAttr[2].coord[0] = 1.0f;
    pVetAttr[2].coord[1] = 0.0f;

    // 第四个点位置信息
    pVetAttr[3].pos[0] = 0.5f;
    pVetAttr[3].pos[1] = 0.5f;
    pVetAttr[3].pos[2] = 0.0f;
    // 第四个点颜色信息
    pVetAttr[3].color[0] = 0.0f;
    pVetAttr[3].color[1] = 1.0f;
    pVetAttr[3].color[2] = 1.0f;
    // 第四个点的纹理坐标
    pVetAttr[3].coord[0] = 1.0f;
    pVetAttr[3].coord[1] = 1.0f;
}

void OpenGLRenderWidget::setFillStatus(bool isFill)
{
    m_isFill = isFill;
    this->update();
}

bool OpenGLRenderWidget::isFill(void)
{
    return m_isFill;
}

void OpenGLRenderWidget::setMixNumber(float number)
{
    m_mixNumber = number;
    this->update();
}

float OpenGLRenderWidget::getMixNumber(void)
{
    return m_mixNumber;
}

// 添加纹理, 如果不适用Qt封装则使用以下代码创建纹理
void OpenGLRenderWidget::createTexture(const QImage& image, GLuint& textureid)
{
    glGenTextures(1, &textureid);
    glBindTexture(GL_TEXTURE_2D, textureid);
    // 环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 内存对齐方式
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); // 指定内存中图像的行长度
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0); // 指定跳过多少像素
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0); // 指定跳过多少行

    // 设置纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLRenderWidget::rotate()
{
    // QMatrix4x4 matrix;  // QMatrix 默认生成的是一个单位矩阵（对角线上的元素为1）

    // // 【重点】这里是先旋转再位移！！ 因为矩阵运算 T*R*v 顺序是从右向左的！！不然转轴不是图片中心！！
    // // 3. 绕 Z 轴旋转（假设旋转角度随时间变化）
    // float angle = 45.0f; // 假设旋转 45 度（实际可以用 QTime 或动画控制）
    // matrix.rotate(angle, 0.0f, 0.0f, 1.0f);  // 旋转
    // // matrix.translate(0.4f, 0.0f, 0.0f);  // 位移
    // this->m_pShaderProgram.bind();
    // this->m_pShaderProgram.setUniformValue("matrix", matrix);
    update();
}
