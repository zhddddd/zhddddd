#ifndef WIDGET_H
#define WIDGET_H
#include <QMouseEvent>
#include<QKeyEvent>
#include <QWidget>
#include<QMenu>
#include <QNetworkReply>
#include<QMessageBox>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QPainter>
#include"day.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    day days[7];
protected:
    bool eventFilter(QObject *watched,QEvent *event);
public slots:
    void readHttpReply(QNetworkReply *reply);

private slots:
    void on_pushButton_clicked();

private:
    Ui::Widget *ui;
    QMenu *menuQuit;
    QNetworkReply *reply;
    void parseWeatherJsonData(QByteArray rawData);
    QString strurl;
    QNetworkAccessManager *manager;
    void initcityMap();
    QString getCityCodeFromName(QString cityName);
    QMap<QString,QString> cityMap;
    QMap<QString,QString> myTypeMap;
    void initMyTypeMap();
    void parseWeatherJsonDataNew(QByteArray rawData);
    void upDateUi();
    void initdata();//后几日天气数据；
    void drawTmpLineHigh();
    void drawTmpLineLow();

};
#endif // WIDGET_H
