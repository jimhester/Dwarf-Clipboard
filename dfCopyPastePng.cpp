#include "dfCopyPastePng.h"

#include "dfCopyObj.h"
#include "dfLocationIterator.h"

#include <QImage>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QDebug>

dfCopyPastePng::dfCopyPastePng(DFHack::Context * DFPtr)
{
    DF = DFPtr;
    QString Path("C:/NCF/DF/versions/windows/df_31_12_win/data/art/curses_square_16x16.png");
    tileSet = new QImage(Path);
    tileWidth = tileSet->width()/16;
    tileHeight = tileSet->height()/16;
    QString Path2("C:/NCF/DF/versions/windows/df_31_12_win/data/init/colors.txt");
    readConfig(Path2);
}
QVector<QImage> dfCopyPastePng::getPicture(QVector<cursorIdx> range)
{
    QVector<QImage> retImages;
    DFHack::Position * Pos = DF->getPosition();
    DFHack::WindowIO * Win = DF->getWindowIO();
    dfLocationIterator itr(range[0],range[1]);
    cursorIdx begin = itr.begin();
    cursorIdx end = itr.end();
    cursorIdx size;
    size.x = (end.x-begin.x)+1;
    size.y = (end.y-begin.y)+1;
    size.z = begin.z-end.z+1;
    cursorIdx oldCursor,oldPosition;
    Pos->getCursorCoords(oldCursor.x,oldCursor.y,oldCursor.z);
    Pos->getViewCoords(oldPosition.x,oldPosition.y,oldPosition.z);
    Pos->setCursorCoords(-30000,oldCursor.y,oldCursor.z);
    int32_t screenWidth,screenHeight;
    Pos->getWindowSize(screenWidth,screenHeight);
    int32_t effectiveWidth = screenWidth-2-55; // 55 is the width of the map and the menus
    int32_t effectiveHeight = screenHeight-2;

    DFHack::t_screen *screen;
    screen = new DFHack::t_screen[screenWidth*screenHeight];
    for(int zitr = 0;zitr < size.z;zitr++){
        QImage currentImg((size.x+1)*tileWidth,(size.y+1)*tileHeight,QImage::Format_ARGB32);//(end.x-begin.x)*tileWidth,(end.y-begin.y)*tileHeight,QImage::Format_ARGB32);
        QPainter painter(&currentImg);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        QVector<QVector<QString> > rawNumbers(size.x,QVector<QString>(size.y));
        for(int xIdx = 0;xIdx < int(size.x/effectiveWidth)+1;xIdx++){
            for(int yIdx=0;yIdx < int(size.y/effectiveHeight)+1;yIdx++){
                Pos->setViewCoords(begin.x+xIdx*effectiveWidth,begin.y+yIdx*effectiveHeight,begin.z);
                Win->TypeSpecial(DFHack::WAIT,1,50);
                Pos->getScreenTiles(screenWidth,screenHeight,screen);
                int ylim = screenHeight-1;
                int xlim = effectiveWidth+1;
                if(yIdx == int(size.y/effectiveHeight)){
                    ylim = size.y % effectiveHeight;
                }
                if(xIdx == int(size.x/effectiveWidth)){
                    xlim = size.x % effectiveWidth;
                }
                for(int yitr = 1;yitr < ylim+1;yitr++){
                    for(int xitr = 1;xitr < xlim+1;xitr++){
                        QImage tile = getTileFromScreen(screen[yitr*screenWidth+xitr]);
                        QString test = stringFromScreen(screen[yitr*screenWidth+xitr]);
                        rawNumbers[effectiveWidth*xIdx + xitr-1][effectiveHeight*yIdx + yitr-1] = test;
                        painter.drawImage((effectiveWidth*xIdx + xitr-1) * tileWidth, (effectiveHeight*yIdx + yitr-1)* tileHeight, tile);
                    }
                }
            }
        }
        painter.end();
        QString rawComplete;
        for(int yitr = 0;yitr < rawNumbers[0].size();yitr++){
            for(int xitr = 0;xitr < rawNumbers.size();xitr++){
                rawComplete.append(rawNumbers[xitr][yitr]);
            }
            rawComplete.append(':');
        }
        currentImg.setText("rawNumbers",rawComplete);
        retImages.append(currentImg);
    }
    Pos->setViewCoords(oldPosition.x,oldPosition.y,oldPosition.z);
    Pos->setCursorCoords(oldCursor.x,oldCursor.y,oldCursor.z);
    delete screen;
    return(retImages);
}
QString dfCopyPastePng::stringFromScreen(DFHack::t_screen tile)
{
    return (QString("%1,%2,%3,%4;").arg(tile.symbol).arg(tile.foreground).arg(tile.background).arg(tile.bright));
}
void dfCopyPastePng::waitTillDigMenuClear()
{
    DFHack::Position * Pos = DF->getPosition();
    DFHack::WindowIO * Win = DF->getWindowIO();

    int32_t screenWidth,screenHeight;
    Pos->getWindowSize(screenWidth,screenHeight);
    DFHack::t_screen *screen;
    screen = new DFHack::t_screen[screenWidth*screenHeight];
    Pos->getScreenTiles(screenWidth,screenHeight,screen);
    while(screen[3*screenWidth+50].symbol == 'M'){
        DF->Resume();
        Win->TypeSpecial(DFHack::WAIT,1,100);
        DF->Resume();
        Pos->getScreenTiles(screenWidth,screenHeight,screen);
    }
}

void dfCopyPastePng::waitTillScreenPosition(cursorIdx check)
{
    DFHack::Position * p = DF->getPosition();
    DFHack::WindowIO * w = DF->getWindowIO();
    cursorIdx state;
    p->getViewCoords(state.x,state.y,state.z);
    while(state.x != check.x && state.y != check.y && state.z != check.z){
       DF->Resume();
       w->TypeSpecial(DFHack::WAIT,1,100);
       p->getViewCoords(state.x,state.y,state.z);
    }
    w->TypeSpecial(DFHack::WAIT,1,100);
}
void dfCopyPastePng::setTransparency(QImage* toSet)
{
    toSet->setAlphaChannel(toSet->createMaskFromColor(QColor(255,0,255).rgba(),Qt::MaskOutColor));
    }
void dfCopyPastePng::readConfig(QString FileName)
{
    QFile file(FileName);
    file.open(QFile::ReadOnly);
    QTextStream t(&file);
    QString currLine;
    while(!t.atEnd()){
        processLine(t.readLine());
    }
    file.close();
}
QImage dfCopyPastePng::getTileFromScreen(DFHack::t_screen screen)
{
    QImage foreground(tileWidth,tileHeight,QImage::Format_ARGB32);
    QImage background(tileWidth,tileHeight,QImage::Format_ARGB32);
    foreground.fill(getForegroundColorFromScreen(screen).rgb());
    background.fill(getBackgroundColorFromScreen(screen).rgb());
    QImage tile = tileSet->copy((screen.symbol % 16)*tileWidth,int(screen.symbol/16)*tileHeight,tileWidth,tileHeight);
    QImage out = tile.createMaskFromColor(QColor(255,0,255).rgba(),Qt::MaskOutColor);
    QImage in = tile.createMaskFromColor(QColor(255,0,255).rgba(),Qt::MaskInColor);
    foreground.setAlphaChannel(out);
    background.setAlphaChannel(in);
    QImage resultImage(tileWidth,tileHeight,QImage::Format_ARGB32);
    QPainter painter(&resultImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(resultImage.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, background);
    painter.setCompositionMode(QPainter::CompositionMode_Plus); 
    painter.drawImage(0,0,foreground);
    painter.end();
    return(resultImage);
}
void dfCopyPastePng::processLine(QString line)
{
    if(line.startsWith("[")){
        QString Color, Channel,Value;
        QStringList list;
        list = line.split(QRegExp("[\\[:_\\]]"),QString::SkipEmptyParts);
        Color = list[0];
        Channel = list[1];
        Value = list[2];
        if(Channel == "R"){
            colorMap[Color].setRed(Value.toInt());
        }
        else if(Channel == "G"){
            colorMap[Color].setGreen(Value.toInt());
        }
        else if(Channel == "B"){
            colorMap[Color].setBlue(Value.toInt());
        }
    }
}
QColor dfCopyPastePng::getForegroundColorFromScreen(DFHack::t_screen tile)
{
    if(tile.bright == 0){
        switch (tile.foreground){
            case 0:
                return colorMap["BLACK"];
            case 1:
                return colorMap["BLUE"];
            case 2:
                return colorMap["GREEN"];
            case 3:
                return colorMap["CYAN"];
            case 4:
                return colorMap["RED"];
            case 5:
                return colorMap["MAGENTA"];
            case 6:
                return colorMap["BROWN"];
            case 7:
                return colorMap["LGRAY"];
        }
    }
    else{
        switch(tile.foreground){
            case 0:
                return colorMap["DGRAY"];
            case 1:
                return colorMap["LBLUE"];
            case 2:
                return colorMap["LGREEN"];
            case 3:
                return colorMap["LCYAN"];
            case 4:
                return colorMap["LRED"];
            case 5:
                return colorMap["LMAGENTA"];
            case 6:
                return colorMap["YELLOW"];
            case 7:
                return colorMap["WHITE"];
        }
    }
    return QColor();
}
QColor dfCopyPastePng::getBackgroundColorFromScreen(DFHack::t_screen tile)
{
    //if(tile.bright == 0){
        switch (tile.background){
            case 0:
                return colorMap["BLACK"];
            case 1:
                return colorMap["BLUE"];
            case 2:
                return colorMap["GREEN"];
            case 3:
                return colorMap["CYAN"];
            case 4:
                return colorMap["RED"];
            case 5:
                return colorMap["MAGENTA"];
            case 6:
                return colorMap["BROWN"];
            case 7:
                return colorMap["LGRAY"];
        }
        return QColor();
//    }
/*    else{
        switch(tile.foreground){
            case 0:
                return colorMap["DGRAY"];
            case 1:
                return colorMap["LBLUE"];
            case 2:
                return colorMap["LGREEN"];
            case 3:
                return colorMap["LCYAN"];
            case 4:
                return colorMap["LRED"];
            case 5:
                return colorMap["LMAGENTA"];
            case 6:
                return colorMap["YELLOW"];
            case 7:
                return colorMap["WHITE"];
        }
    }*/
}