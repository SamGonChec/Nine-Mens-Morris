#include "include/splash.h"

Splash::Splash(QGraphicsScene *scene){
    this->scene = scene;
    splashScreen();
}

void Splash::splashScreen(){
    // Declaring background image and text.
    QGraphicsTextItem* teamName = new QGraphicsTextItem(QString("The Gifted Chimpanzees"));
    QGraphicsTextItem* secondLine = new QGraphicsTextItem(QString("present:"));
    QFont nameFont("comic sans MS", 50);
    splashBackground = QPixmap(":/images/media/chimp.jpg");
    int height = scene->height()/2 + 300;
    int width = scene->width()/2 - teamName->boundingRect().width()/2 + 600;

    // sets the font, style, and color of the text
    teamName->setFont(nameFont);
    teamName->setDefaultTextColor(QColor("brown"));
    secondLine->setFont(nameFont);
    secondLine->setDefaultTextColor(QColor("brown"));


    // sets up the proper spacing and format for the image and lines
    splashBackground = splashBackground.scaled(1920,1080, Qt::IgnoreAspectRatio);
    teamName->setPos(width,height);
    secondLine->setPos(width+250, height+100);


    // adds the items to the scene
    scene->addPixmap(splashBackground);
    scene->addItem(teamName);
    scene->addItem(secondLine);
}
