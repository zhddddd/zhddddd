#include "widget.h"
#include "ui_widget.h"
#include<QDebug>
#include <QFile>
#include<QNetworkAccessManager>
#include<QNetworkRequest>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->lineEdit->setPlaceholderText("城市");
    setWindowTitle("天气预报");
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);//禁言最大化按钮
    setFixedSize(this->width(), this->height());//防止窗口变大变小
    this->move(1000,50);//初始化位置为屏幕右侧
    //右键点击出现菜单功能,无实际意义
    // menuQuit=new QMenu(this);
    // QAction *exitAct=new QAction(QIcon(":/new/prefix1/png/18.png"),tr("退出"),this);
    // menuQuit->addAction(exitAct);
    // connect(menuQuit,&QMenu::triggered,this,[=]
    //        {
    //     menuQuit->close();
    //  });

    manager=new QNetworkAccessManager(this);
    connect(manager,&QNetworkAccessManager::finished,this,[]()
            {
        qDebug()<<"manager finished!";
    });
    strurl="http://v1.yiketianqi.com/api?unescape=1&version=v91&appid=47394236&appsecret=CgAIiCa2&ext=&cityid=";
    QUrl url(strurl);
    QNetworkRequest res(url);
    reply=manager->get(res);



    initMyTypeMap();//初始化天气图标数据

    connect(manager,&QNetworkAccessManager::finished,this,&Widget::readHttpReply);
    connect(ui->lineEdit,&QLineEdit::returnPressed,this,&Widget::on_pushButton_clicked);


}

Widget::~Widget()
{
    delete ui;
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==ui->widget_0401&&event->type()==QEvent::Paint)
    {
        drawTmpLineHigh();
        return true;
    }
    if(watched==ui->widget_0402&&event->type()==QEvent::Paint)
    {
        drawTmpLineLow();
        return true;
    }
    return QWidget::eventFilter(watched,event);
}


//重写鼠标事件
// void Widget::keyPressEvent(QKeyEvent *event)
// {
//     if(event->key()==Qt::Key_2)
//     {

//         QMessageBox::information(this,"this","nh");
//     }



// }
// void Widget::parseWeatherJsonData(QByteArray rawData)
// {
//     QJsonDocument jsoDoc=QJsonDocument::fromJson(rawData);
//     if(!jsoDoc.isNull()&&jsoDoc.isObject())
//     {
//         QJsonObject jsoObj=jsoDoc.object();
//         QString date=jsoObj["date"].toString();
//         QString week=jsoObj["week"].toString();
//         ui->label_data->setText("  "+date+" "+week);
//         QString city=jsoObj["city"].toString();
//         ui->label_address->setText(city);
//         QString currentTem=jsoObj["tem"].toString();
//         ui->label_temperature->setText(currentTem+"°c");
//         ui->label_weather->setText(jsoObj["wea"].toString());//解析天气类型

//         ui->label_weathericon->setPixmap(myTypeMap[jsoObj["wea"].toString()]);

//         ui->label_range->setText(jsoObj["tem2"].toString()+"°c"+"~"+jsoObj["tem1"].toString()+"°c");
//         ui->label->setText(jsoObj["air_tips"].toString());
//         ui->label_windtype->setText(jsoObj["win"].toString());
//         ui->label_windrating->setText("风力"+jsoObj["win_speed"].toString());
//         ui->label_PMvalues->setText("风力"+jsoObj["air_pm25"].toString());
//         ui->label_airquality->setText(jsoObj["air_level"].toString());
//         ui->label_humidity->setText(jsoObj["humidity"].toString());



//     }
// }

void Widget::readHttpReply(QNetworkReply *reply)
{
    int resCode=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug()<<resCode;

    if(reply->error()==QNetworkReply::NoError&&resCode==200)
    {
        QByteArray data=reply->readAll();
        parseWeatherJsonDataNew(data);
        qDebug()<<QString::fromUtf8(data);
    }
    else
    {
        QString str=reply->errorString();
        QMessageBox::warning(this,"提示",str,QMessageBox::Cancel);
    }
}
void Widget::initcityMap()
{
    QFile file(":/citycode.json");
    file.open(QFile::ReadOnly);
    QByteArray rawData1=file.readAll();
    file.close();

    QJsonDocument jsoDoc=QJsonDocument::fromJson(rawData1);
    if(jsoDoc.isArray())
    {
        QJsonArray citys=jsoDoc.array();
        for(QJsonValue value:citys)
        {
            if(value.isObject())
            {
                QString cityname=value["city_name"].toString();
                QString citycode=value["city_code"].toString();
                cityMap.insert(cityname,citycode);
            }
        }
    }

}

QString Widget::getCityCodeFromName(QString cityName)//通过搜索框获取城市代码
{
    if(cityMap.isEmpty())
    {
        initcityMap();
    }
    QMap<QString,QString>::iterator it=cityMap.find(cityName);
    if(it==cityMap.end())
    {
        it=cityMap.find(cityName+"市");
        if(it==cityMap.end())
        {
            it=cityMap.find(cityName+"县");
        }
        if(it==cityMap.end())
        {
            it=cityMap.find(cityName+"区");
        }
        if(it==cityMap.end())
        {
            return "";
        }
    }
    return it.value();
}

void Widget::initMyTypeMap()
{
    myTypeMap.insert("晴",":/new/prefix1/png/11.png");
    myTypeMap.insert("多云",":/new/prefix1/png/14.png");
    myTypeMap.insert("多云",":/new/prefix1/png/14.png");
    myTypeMap.insert("阴",":/new/prefix1/png/17.png");
    myTypeMap.insert("雨",":/new/prefix1/png/13.png");
    myTypeMap.insert("大雨",":/new/prefix1/png/4.png");
    myTypeMap.insert("震雨转多云",":/new/prefix1/png/13.png");
    myTypeMap.insert("雷阵雨",":/new/prefix1/png/7.png");
    myTypeMap.insert("雪",":/new/prefix1/png/15.png");
    myTypeMap.insert("暴雨",":/new/prefix1/png/4.png");
    myTypeMap.insert("雾",":/new/prefix1/png/wu.png");
    myTypeMap.insert("小雨",":/new/prefix1/png/xiaoyu.png");
    myTypeMap.insert("中到大雨",":/new/prefix1/png/zhongdaodayu.png");
    myTypeMap.insert("暴雨",":/new/prefix1/png/baoyu.png");
    myTypeMap.insert("小雨转多云",":/new/prefix1/png/14.png");
    myTypeMap.insert("阴转晴",":/new/prefix1/png/17.png");
    myTypeMap.insert("中雨",":/new/prefix1/png/zhongdaodayu.png");
    ui->widget_0401->installEventFilter(this);
    ui->widget_0402->installEventFilter(this);
}



void Widget::parseWeatherJsonDataNew(QByteArray rawData)
{
    QJsonDocument jsoDoc=QJsonDocument::fromJson(rawData);
    if(!jsoDoc.isNull()&&jsoDoc.isObject())
    {
        QJsonObject jsoObj=jsoDoc.object();
        days[0].mCity=jsoObj["city"].toString();
        days[0].mPm=jsoObj["aqi"].toObject()["pm25"].toString();
        if(jsoObj.contains("data")&&jsoObj["data"].isArray())
        {
            QJsonArray weaArray=jsoObj["data"].toArray();
            for(int i=0;i<weaArray.size();i++)
            {
                QJsonObject obj=weaArray[i].toObject();
                //qDebug()<<obj["date"].toString()<<obj["wea"].toString();
                days[i].mData=obj["date"].toString();
                days[i].mWeek=obj["week"].toString();
                days[i].mTmp=obj["tem"].toString();
                days[i].mWeaType=obj["wea"].toString();
                days[i].mTmpLow=obj["tem2"].toString();
                days[i].mTmpHigh=obj["tem1"].toString();

                days[i].mFx=obj["win"].toArray()[1].toString();
                days[i].mFl=obj["win_speed"].toString();
                days[i].mAirQ=obj["air_level"].toString();
                days[i].mTips=obj["air_tips"].toString();
                days[i].mHu=obj["humidity"].toString();
            }
        }
    }
    update();
    upDateUi();

}

void Widget::upDateUi()
{
    QString date=days[0].mData;
    QString week=days[0].mWeek;
    ui->label_data->setText("  "+date+" "+week);
    QString currentTem=days[0].mTmp;
    ui->label_temperature->setText(currentTem+"°c");
    ui->label_weather->setText(days[0].mWeaType);//解析天气类型

    ui->label_weathericon->setPixmap(myTypeMap[days[0].mWeaType]);

    ui->label_range->setText(days[0].mTmpLow+"°c"+"~"+days[0].mTmpHigh+"°c");
    ui->label_address->setText(days[0].mCity);
    ui->label_windtype->setText(days[0].mFx);
    ui->label_windrating->setText("风力"+days[0].mFl);
    ui->label_PMvalues->setText(days[0].mPm);
    ui->label_airquality->setText(days[0].mAirQ);
    ui->label_humidity->setText(days[0].mHu);
    initdata();//更新后几日天气数据




}

void Widget::initdata()//更新后几日天气数据
{
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::white);
    //第一天
    ui->label01->setText(days[0].mData.right(5));
    ui->label02->setPixmap(myTypeMap[days[0].mWeaType]);
    ui->label03->setText(days[0].mWeaType);
    ui->label04->setText(days[0].mAirQ);
    ui->label04->setPalette(pe);
    ui->label05->setText(days[0].mFx);
    ui->label06->setText(days[0].mFl);
    //第二天
    ui->label11->setText(days[1].mData.right(5));
    ui->label12->setPixmap(myTypeMap[days[1].mWeaType]);
    ui->label13->setText(days[1].mWeaType);
    ui->label14->setText(days[1].mAirQ);
    ui->label14->setPalette(pe);
    ui->label15->setText(days[1].mFx);
    ui->label16->setText(days[1].mFl);
    //第三天
    ui->label20->setText(days[2].mWeek);
    ui->label21->setText(days[2].mData.right(5));
    ui->label22->setPixmap(myTypeMap[days[2].mWeaType]);
    ui->label23->setText(days[2].mWeaType);
    ui->label24->setText(days[2].mAirQ);
    ui->label24->setPalette(pe);
    ui->label25->setText(days[2].mFx);
    ui->label26->setText(days[2].mFl);
    //第四天
    ui->label30->setText(days[3].mWeek);
    ui->label31->setText(days[3].mData.right(5));
    ui->label32->setPixmap(myTypeMap[days[3].mWeaType]);
    ui->label33->setText(days[3].mWeaType);
    ui->label34->setText(days[3].mAirQ);
    ui->label34->setPalette(pe);
    ui->label35->setText(days[3].mFx);
    ui->label36->setText(days[3].mFl);
    //第五天
    ui->label40->setText(days[4].mWeek);
    ui->label41->setText(days[4].mData.right(5));
    ui->label42->setPixmap(myTypeMap[days[4].mWeaType]);
    ui->label43->setText(days[4].mWeaType);
    ui->label44->setText(days[4].mAirQ);
    ui->label44->setPalette(pe);
    ui->label45->setText(days[4].mFx);
    ui->label46->setText(days[4].mFl);
    //第六天
    ui->label50->setText(days[5].mWeek);
    ui->label51->setText(days[5].mData.right(5));
    ui->label52->setPixmap(myTypeMap[days[5].mWeaType]);
    ui->label53->setText(days[5].mWeaType);
    ui->label54->setText(days[5].mAirQ);
    ui->label54->setPalette(pe);
    ui->label55->setText(days[5].mFx);
    ui->label56->setText(days[5].mFl);
}

void Widget::drawTmpLineHigh()
{

    QPainter painter(ui->widget_0401);
    painter.setBrush(Qt::red);
    painter.setPen(Qt::red);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QPoint points[6];
    //将点坐标对齐
    int totalTmp=0;
    int tmp1;
    int middle=ui->widget_0401->height()/2;
    qDebug()<<middle;
    for(int i=0;i<6;i++)
    {
        totalTmp+=days[i].mTmpHigh.toInt();
    }
    tmp1=totalTmp/6;

    points[0].setX(ui->label00->x()+ui->label00->width()/2);
    points[0].setY(middle-(days[0].mTmpHigh.toInt()-tmp1)*3);

    qDebug()<<days[0].mTmpHigh.toInt();
    points[1].setX(ui->label10->x()+ui->label10->width()/2);
    points[1].setY(middle-(days[1].mTmpHigh.toInt()-tmp1)*3);

    points[2].setX(ui->label20->x()+ui->label20->width()/2);
    points[2].setY(middle-(days[2].mTmpHigh.toInt()-tmp1)*3);
    points[3].setX(ui->label30->x()+ui->label30->width()/2);
    points[3].setY(middle-(days[3].mTmpHigh.toInt()-tmp1)*3);

    points[4].setX(ui->label40->x()+ui->label40->width()/2);
    points[4].setY(middle-(days[4].mTmpHigh.toInt()-tmp1)*3);

    points[5].setX(ui->label50->x()+ui->label50->width()/2);
    points[5].setY(middle-(days[5].mTmpHigh.toInt()-tmp1)*3);

    for(int i=0;i<6;i++)
    {
        painter.drawEllipse(QPoint(points[i]),3,3);
        painter.drawText(points[i].x()-15,points[i].y()-10,days[i].mTmpHigh+"°c");

    }
    for(int i=0;i<5;i++)
    {
        painter.drawLine(points[i],points[i+1]);

    }


}

void Widget::drawTmpLineLow()
{
    QPainter painter(ui->widget_0402);
    painter.setBrush(Qt::blue);
    painter.setPen(Qt::blue);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QPoint points[6];
    //将点坐标对齐
    int totalTmp=0;
    int tmp1;
    int middle=ui->widget_0402->height()/2;
    qDebug()<<middle;
    for(int i=0;i<6;i++)
    {
        totalTmp+=days[i].mTmpLow.toInt();
    }
    tmp1=totalTmp/6;

    points[0].setX(ui->label00->x()+ui->label00->width()/2);
    points[0].setY(middle-(days[0].mTmpLow.toInt()-tmp1)*3);

    qDebug()<<days[0].mTmpLow.toInt();
    points[1].setX(ui->label10->x()+ui->label10->width()/2);
    points[1].setY(middle-(days[1].mTmpLow.toInt()-tmp1)*3);

    points[2].setX(ui->label20->x()+ui->label20->width()/2);
    points[2].setY(middle-(days[2].mTmpLow.toInt()-tmp1)*3);
    points[3].setX(ui->label30->x()+ui->label30->width()/2);
    points[3].setY(middle-(days[3].mTmpLow.toInt()-tmp1)*3);

    points[4].setX(ui->label40->x()+ui->label40->width()/2);
    points[4].setY(middle-(days[4].mTmpLow.toInt()-tmp1)*3);

    points[5].setX(ui->label50->x()+ui->label50->width()/2);
    points[5].setY(middle-(days[5].mTmpLow.toInt()-tmp1)*3);

    for(int i=0;i<6;i++)
    {
        painter.drawEllipse(QPoint(points[i]),3,3);
        painter.drawText(points[i].x()-15,points[i].y()-10,days[i].mTmpLow+"°c");

    }
    for(int i=0;i<5;i++)
    {
        painter.drawLine(points[i],points[i+1]);

    }
}
void Widget::on_pushButton_clicked()
{
    QString cityNameFromUser=ui->lineEdit->text();
    QString cityCode=getCityCodeFromName(cityNameFromUser);
    if(cityCode.isEmpty()&&!ui->lineEdit->text().isEmpty())
        QMessageBox::information(this,"提示","你搜索的城市不存在!",QMessageBox::Ok);
    else if(ui->lineEdit->text().isEmpty())
    {
        QMessageBox::information(this,"提示","输入为空",QMessageBox::Ok);
    }
    else
    {
        strurl+="&cityid="+cityCode;
        qDebug()<<strurl;
        manager->get(QNetworkRequest(QUrl(strurl)));
    }
}

