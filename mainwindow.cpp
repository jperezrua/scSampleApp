#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QString>

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/shape.hpp"

#include "iostream"

using namespace cv;

static std::vector<Point> simpleContour( const Mat& currentQuery, int n=300 )
{
    std::vector< std::vector<Point> > _contoursQuery;
    std::vector <Point> contoursQuery;
    findContours(currentQuery, _contoursQuery, RETR_LIST, CHAIN_APPROX_NONE);
    for (size_t border=0; border<_contoursQuery.size(); border++)
    {
        for (size_t p=0; p<_contoursQuery[border].size(); p++)
        {
            contoursQuery.push_back( _contoursQuery[border][p] );
        }
    }

    // In case actual number of points is less than n
    for (int add=contoursQuery.size()-1; add<n; add++)
        contoursQuery.push_back(contoursQuery[contoursQuery.size()-add+1]); //adding dummy values

    // Uniformly sampling
    random_shuffle(contoursQuery.begin(), contoursQuery.end());
    std::vector<Point> cont;
    for (int i=0; i<n; i++)
        cont.push_back(contoursQuery[i]);

    return cont;
}

static QImage Mat2QImage(Mat const& src)
{
     Mat temp;
     cvtColor(src, temp, COLOR_BGR2RGB);
     QImage dest((uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     QImage dest2(dest);
     dest2.detach();
     return dest2;
}

static void imShow(QLabel *win, Mat const& src)
{
    win->setPixmap(QPixmap::fromImage( Mat2QImage(src) ));
    win->show();
    QApplication::processEvents();
}

static void imShowShiny(QLabel *win, Mat const& src)
{
    for (int ii=0; ii<250; ii++){
        if (ii%2!=0){
            win->setPixmap(QPixmap::fromImage( Mat2QImage(src) ));
            win->show();
        }
        else
        {
            Mat colored = Mat::zeros(src.rows, src.cols, src.type());
            colored = Scalar(0,255,0);
            win->setPixmap(QPixmap::fromImage( Mat2QImage(colored) ));
            win->show();
        }
        QApplication::processEvents();
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open image"), "./", tr("Image (*.jpg *.png)"));

    if (fileName.begin()!=fileName.end())
    {
        QStringList partedList = fileName.split(".", QString::SkipEmptyParts);
        QString extension = partedList.at(partedList.size()-1);

        if ( extension.compare(extension,"jpg",Qt::CaseInsensitive)==0 ||
             extension.compare(extension,"png",Qt::CaseInsensitive)==0 ){
            Mat sshape1 = imread(fileName.toStdString(), IMREAD_COLOR);
            imShow(ui->label_1, sshape1);
            cvtColor(sshape1, shape1, COLOR_BGR2GRAY);
            ui->searchButton->setEnabled(true);
        }
    }
}

void MainWindow::on_searchButton_clicked()
{
    Ptr<ShapeContextDistanceExtractor> mysc = createShapeContextDistanceExtractor();
    mysc->setIterations(1);
    std::vector<Point> s1 = simpleContour(shape1,100);

    QString base = "./shape_sample/";
    float minDis = FLT_MAX;
    int sind=1;
    for (int ii=1; ii<=20; ii++){
        QString name = base + QString::number(ii) + ".png";
        std::cout<<name.toStdString()<<std::endl;
        Mat sshape2 = imread(name.toStdString(), IMREAD_COLOR);
        cvtColor(sshape2, shape2, COLOR_BGR2GRAY);

        std::vector<Point> s2 = simpleContour(shape2);
        float val=mysc->computeDistance(s1, s2);
        if (val<minDis){
            minDis=val;
            sind=ii;
        }
        ui->label->setText(QString("Distance is: ")+QString::number(val));
        imShow(ui->label_2, sshape2);
    }

    Mat win = imread((base + QString::number(sind) + ".png").toStdString(), IMREAD_COLOR);
    ui->label->setText(QString("Distance is: ")+QString::number(minDis));
    imShowShiny(ui->label_2, win);
}
