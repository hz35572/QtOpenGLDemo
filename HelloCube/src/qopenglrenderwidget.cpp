#include "qopenglrenderwidget.h"
#include <QTime>



/* 实现绘制10个立方体在不同位置 */
QVector<QVector3D> cubePositions = {
    QVector3D( 0.0f,  0.0f,  0.0f),
    QVector3D( 2.0f,  5.0f, -15.0f),
    QVector3D(-1.5f, -2.2f, -2.5f),
    QVector3D(-3.8f, -2.0f, -12.3f),
    QVector3D( 2.4f, -0.4f, -3.5f),
    QVector3D(-1.7f,  3.0f, -7.5f),
    QVector3D( 1.3f, -2.0f, -2.5f),
    QVector3D( 1.5f,  2.0f, -2.5f),
    QVector3D( 1.5f,  0.2f, -1.5f),
    QVector3D(-1.3f,  1.0f, -1.5f)
};

QOpenGLRenderWidget::QOpenGLRenderWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{}

QOpenGLRenderWidget::~QOpenGLRenderWidget()
{

}

void QOpenGLRenderWidget::initializeGL()
{
    this->initializeOpenGLFunctions();

    // 初始化GPU程序
    bool result = initShaderProgram();
    if (!result)
        return;
    // glEnable(GL_DEPTH_TEST);

    // 创建顶点数据
    initModelData();

    // ===================== VAO | VBO =====================
    // VAO 和 VBO 对象赋予 ID
    glGenVertexArrays(1, &m_nVAOId);
    glGenBuffers(1, &m_nVBOId);

    // 绑定 VAO、VBO 对象
    glBindVertexArray(m_nVAOId);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVBOId);

    glBufferData(GL_ARRAY_BUFFER, m_cube.size() * sizeof(VertexAttributeData), m_cube.data(), GL_STATIC_DRAW);

    this->m_pShaderProgram->bind();  // 如果使用 QShaderProgram，那么最好在获取顶点属性位置前，先 bind()
    m_nPosAttrLocationId = this->m_pShaderProgram->attributeLocation("aPos");  // 获取顶点着色器中顶点属性 aPos 的位置
    // glVertexAttribPointer(m_nPosAttrLocationId, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // 手动传入第几个属性
    glVertexAttribPointer(m_nPosAttrLocationId, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributeData), (void*)0);  // 手动传入第几个属性
    glEnableVertexAttribArray(m_nPosAttrLocationId); // 开始 VAO 管理的第一个属性值

    this->m_pShaderProgram->bind();
    GLint aTexelLocation = this->m_pShaderProgram->attributeLocation("aTexel");
    glVertexAttribPointer(aTexelLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttributeData), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(aTexelLocation);

    // ===================== EBO =====================
  /*  glGenBuffers(1, &m_nEBOId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nEBOId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); */ // EBO/IBO 是储存顶点【索引】的

    // ===================== 纹理 =====================
    // 开启透明度
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->m_pShaderProgram->bind();
    this->m_pShaderProgram->setUniformValue("texture0", 0);  // 【重点】当涉及到多个纹理使，一定要为 uniform 设置纹理单元的编号
    this->texture_wall_ = new QOpenGLTexture(QImage(":/images/wall.jpg").mirrored());  // 因为QOpenGL的y轴是反的（镜像），所以需要mirrored翻转一下

    this->m_pShaderProgram->bind();
    this->m_pShaderProgram->setUniformValue("texture1", 1);
    this->texture_nekosilverfox_ = new QOpenGLTexture(QImage(":/images/nekosilverfox.png").mirrored());
    this->texture_nekosilverfox_->bind(1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    this->m_pShaderProgram->bind();
    this->m_pShaderProgram->setUniformValue("texture2", 2);
    this->texture_nekosilverfox_bk_ = new QOpenGLTexture(QImage(":/images/nekosilverfox_bk.jpg").mirrored());
    // 纹理环绕方式
    this->texture_nekosilverfox_bk_->bind(2);  // 【重点】注意！再修改纹理之前要先绑定到对应的纹理单元上！
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);  // T轴纹理【环绕】方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);  // S轴纹理【环绕】方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // 缩小时轴纹理【过滤】方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // 放大时纹理【过滤】方式
    {  /* 【重点】如果在 Switch 里定义变量要放在花括号里，如果是颜色填充要先设置，再传入颜色*/ }
    // float bord_color[] = {1.0, 1.0, 0.0, 1.0};
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bord_color);

    this->m_pShaderProgram->bind();
    this->m_pShaderProgram->setUniformValue("val_alpha", val_alpha);

    /* 透视（焦距）一般设置一次就好了，之后不变。如果放在PaintGL() 里会导致每次重绘都调用，增加资源消耗 */
    QMatrix4x4 mat_projection;
    mat_projection.perspective(45, (float)width()/(float)height(), 0.1f, 100.0f);  // 透视
    this->m_pShaderProgram->setUniformValue("mat_projection", mat_projection);


    // ===================== 解绑 =====================
    // 解绑 VAO 和 VBO，注意先解绑 VAO再解绑EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);  // 注意 VAO 不参与管理 VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void QOpenGLRenderWidget::resizeGL(int w, int h)
{

}

void QOpenGLRenderWidget::paintGL()
{
    /* 设置 OpenGLWidget 控件背景颜色为深青色，并且设置深度信息（Z-缓冲） */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // set方法【重点】如果没有 initializeGL，目前是一个空指针状态，没有指向显卡里面的函数，会报错
    glEnable(GL_DEPTH_TEST);  // 深度信息，如果不设置立方体就像没有盖子
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // use方法

    /* 重新绑定 VAO */
    glBindVertexArray(m_nVAOId);

    /* 【重点】使用 QOpenGLShaderProgram 进行着色器绑定 */
    this->m_pShaderProgram->bind();

    /* 绘制三角形 */
    //    glDrawArrays(GL_TRIANGLES, 0, 6);
    //    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  // 6 代表6个点，因为一个矩形是2个三角形构成的，一个三角形有3个点
    //    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &indices);  // 直接到索引数组里去绘制，如果VAO没有绑定EBO的话

    unsigned int time_ms = QTime::currentTime().msec();

    QMatrix4x4 mat_model; // QMatrix 默认生成的是一个单位矩阵（对角线上的元素为1）
    QMatrix4x4 mat_view;

    mat_view.translate(0.0f, 0.0f, -3.0f);  // 移动世界，【重点】这个位置是世界原点相对于摄像机而言的！！所以这里相当于世界沿着 z 轴对于摄像机向后退 3 个单位



    // ===================== 绑定纹理 =====================
    this->texture_wall_->bind(0);  // 绑定纹理单元0的数据，并激活对应区域
    this->texture_nekosilverfox_->bind(1);
    this->texture_nekosilverfox_bk_->bind(2);

    this->m_pShaderProgram->setUniformValue("mat_view", mat_view);  // 摄像机矩阵
    //        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    foreach (auto item, cubePositions)
    {
        /* 【重点】一定要先旋转再位移！！！！ */
        mat_model.setToIdentity();  // 注意重置为单位矩阵！！
        mat_model.translate(item);  // 将每个立方体都移动到对应的不同位置
        mat_model.rotate(time_ms / 10 , 1.0f, 3.0f, 0.5f);  // 沿着转轴旋转图形
        this->m_pShaderProgram->setUniformValue("mat_model", mat_model);  // 图形矩阵
        glDrawArrays(GL_TRIANGLES, 0, 36);  // 一共绘制 36 个顶点
    }

}

bool QOpenGLRenderWidget::initShaderProgram()
{
    m_pShaderProgram = new QOpenGLShaderProgram(this);

    // 加载顶点着色器
    QString vertexShaderStr(":/shaders/vertex.vert");
    m_pVertexShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    bool result = m_pVertexShader->compileSourceFile(vertexShaderStr);
    if (!result)
    {
        qDebug() << m_pVertexShader->log();
        return false;
    }

    // 加载片段着色器
    QString fragmentShaderStr(":/shaders/fragment.frag");
    m_pFragmentShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    result = m_pFragmentShader->compileSourceFile(fragmentShaderStr);
    if (!result)
    {
        qDebug() << m_pFragmentShader->log();
        return false;
    }

    // 创建ShaderProgram
    m_pShaderProgram = new QOpenGLShaderProgram(this);
    m_pShaderProgram->addShader(m_pVertexShader);
    m_pShaderProgram->addShader(m_pFragmentShader);
    return m_pShaderProgram->link();
}

void QOpenGLRenderWidget::initModelData()
{
    m_points.clear();
    m_textureCoords.clear();
    m_normalVNs.clear();

    // 顶点数据
    m_points.push_back(QVector3D(-0.500000f, -0.500000f, -0.500000f));
    m_points.push_back(QVector3D(0.500000f, -0.500000f, -0.500000f));
    m_points.push_back(QVector3D(0.500000f, 0.500000f, -0.500000f));
    m_points.push_back(QVector3D(-0.500000f, 0.500000f, -0.500000f));

    m_points.push_back(QVector3D(-0.500000f, -0.500000f, 0.500000f));
    m_points.push_back(QVector3D(0.500000f, -0.500000f, 0.500000f));
    m_points.push_back(QVector3D(0.500000f, 0.500000f, 0.500000f));
    m_points.push_back(QVector3D(-0.500000f, 0.500000f, 0.500000f));

    m_points.push_back(QVector3D( -0.500000f, -0.500000f, -0.500000f ));
    m_points.push_back(QVector3D( -0.500000f, 0.500000f,  -0.500000f ));
    m_points.push_back(QVector3D( -0.500000f, 0.500000f,  0.500000f ));
    m_points.push_back(QVector3D( -0.500000f, -0.500000f, 0.500000f ));

    m_points.push_back(QVector3D( 0.500000f, -0.500000f, -0.500000f ));
    m_points.push_back(QVector3D( 0.500000f, 0.500000f,  -0.500000f ));
    m_points.push_back(QVector3D( 0.500000f, 0.500000f,  0.500000f ));
    m_points.push_back(QVector3D( 0.500000f, -0.500000f, 0.500000f ));

    m_points.push_back(QVector3D(-0.500000f,-0.500000f, -0.500000f ));
    m_points.push_back(QVector3D(0.500000f, -0.500000f, -0.500000f ));
    m_points.push_back(QVector3D(0.500000f, -0.500000f, 0.500000f ));
    m_points.push_back(QVector3D(-0.500000f,-0.500000f, 0.500000f ));

    m_points.push_back(QVector3D(-0.500000f, 0.500000f, -0.500000f ));
    m_points.push_back(QVector3D(0.500000f,  0.500000f, -0.500000f ));
    m_points.push_back(QVector3D(0.500000f,  0.500000f, 0.500000f ));
    m_points.push_back(QVector3D(-0.500000f, 0.500000f, 0.500000f ));


    // 纹理坐标
    m_textureCoords.push_back(QVector2D(0.0f, 0.0f));
    m_textureCoords.push_back(QVector2D(1.0f, 0.0f));
    m_textureCoords.push_back(QVector2D(1.0f, 1.0f));
    m_textureCoords.push_back(QVector2D(0.0f, 1.0f));


    // 法线
    // m_normalVNs.push_back(QVector3D(0.0000f, 1.0000f, 0.0000f));
    // m_normalVNs.push_back(QVector3D(0.0000f, 0.0000f, 1.0000f));
    // m_normalVNs.push_back(QVector3D(-1.0000f, 0.0000f, 0.0000f));
    // m_normalVNs.push_back(QVector3D(0.0000f, -1.0000f, 0.0000f));
    // m_normalVNs.push_back(QVector3D(1.0000f, 0.0000f, 0.0000f));
    // m_normalVNs.push_back(QVector3D(0.0000f, 0.0000f, -1.0000f));

    int facePoint[] = {1, 2, 3, 3, 4, 1, \
                       5, 6, 7, 7, 8, 5, \
                       9, 10, 11, 11, 12, 9, \
                       13, 14, 15, 15, 16, 13, \
                       17, 18, 19, 19, 20, 17, \
                       21, 22, 23, 23, 24, 21};

    int faceCoord[] = {1, 2, 3, 3, 4, 1, \
                       1, 2, 3, 3, 4, 1, \
                       1, 2, 3, 3, 4, 1, \
                       1, 2, 3, 3, 4, 1, \
                       1, 2, 3, 3, 4, 1, \
                       1, 2, 3, 3, 4, 1,};

    // int faceNoemal[] = {1, 1, 1, 1, \
    //                     2, 2, 2, 2, \
    //                     3, 3, 3, 3, \
    //                     4, 4, 4, 4, \
    //                     5, 5, 5, 5, \
    //                     6, 6, 6, 6};

    int count = sizeof(facePoint) / sizeof(int);
    for (int i=0; i<count; ++i)
    {
        VertexAttributeData vertexAttr;

        // 点坐标
        int index = facePoint[i] - 1;
        vertexAttr.pos[0] = m_points[index].x();
        vertexAttr.pos[1] = m_points[index].y();
        vertexAttr.pos[2] = m_points[index].z();

        // 纹理坐标
        index = faceCoord[i] - 1;
        vertexAttr.coord[0] = m_textureCoords[index].x();
        vertexAttr.coord[1] = m_textureCoords[index].y();

        // 法线
        // index = faceNoemal[i] - 1;
        // vertexAttr.normal[0] = m_normalVNs[index].x();
        // vertexAttr.normal[1] = m_normalVNs[index].y();
        // vertexAttr.normal[2] = m_normalVNs[index].z();

        m_cube.push_back(vertexAttr);
    }
}
