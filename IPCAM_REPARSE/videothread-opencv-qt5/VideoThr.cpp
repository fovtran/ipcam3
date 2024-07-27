
Viewer::Viewer(QWidget *parent) : QWidget(parent)
{
	this->setFixedSize(640, 480);
	this->show();
	//connect(quit,SIGNAL(triggered()),qApp,SLOT(quit()));
}

Mat Viewer::qImage2Mat(const QImage &src)
{
    Mat res;
    return res;
}

void Viewer::setImage(QByteArray frame)
{
	//const int w=source.width(), h=source.height(), depth = 4, stride = w*depth, W = w, H = h;
	//QImage image(640,480,QImage::Format_RGB32);
	QImage copy = QImage::fromData(frame,"MJPG");  //QImage(frame,W,H,QImage::Format_RGB32);
}

void Viewer::paintEvent(QPaintEvent *event)
{
 QPainter painter;
 painter.begin(this);
 painter.drawImage(QPoint(0, 0), source);
 painter.end();
}

void Viewer::update() { repaint(); }
Viewer::~Viewer() { delete this; }

void createAlphaMat(cv::Mat &mat)
{
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            Vec4b& rgba = mat.at<Vec4b>(i, j);
            //rgba[0] = 200; //UCHAR_MAX;
            rgba[1] = saturate_cast<uchar>((float (mat.cols - j)) / ((float)mat.cols) * UCHAR_MAX);
            //rgba[2] = saturate_cast<uchar>((float (mat.rows - i)) / ((float)mat.rows) * UCHAR_MAX);
            rgba[3] = saturate_cast<uchar>(0.1 * (rgba[1] + rgba[2]));
        }
    }
}


class Viewer : public QWidget {
    Q_OBJECT

public:
    Viewer(QWidget *parent = 0);
	void setImage(QByteArray frame);
	~Viewer();
	void paintEvent(QPaintEvent *event);
	void update();
	cv::Mat qImage2Mat(const QImage &src);

public slots:

private:
	QWidget scene;
    QString path;
	QImage source;
	QLabel view;
    //vec2 translate;
};
