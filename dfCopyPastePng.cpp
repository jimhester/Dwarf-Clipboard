#include "dfCopyPastePng.h"

#include "dfCopyObj.h"
#include "dfLocationIterator.h"

#include <QImage>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QDebug>

int dfCopyPastePng::delay = 100;

dfCopyPastePng::dfCopyPastePng(DFHack::Context * DFPtr)
{
    DF = DFPtr;
    QString Path("C:/DF/versions/windows/df_31_12_win/data/art/curses_square_16x16.png");
    tileSet = new QImage(Path);
    tileWidth = tileSet->width()/16;
    tileHeight = tileSet->height()/16;
    QString Path2("C:/DF/versions/windows/df_31_12_win/data/init/colors.txt");
    readConfig(Path2);
}
QList<QImage> dfCopyPastePng::getImagesForRange(QList<cursorIdx> range)
{
    QList<QImage> retImages;
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
    QString rawComplete = QString("%1,%2,%3|").arg(size.x).arg(size.y).arg(size.z);
    for(int zitr = 0;zitr < size.z;zitr++){
        QVector<QVector<QString> > rawNumbers(size.x,QVector<QString>(size.y));
        for(int xIdx = 0;xIdx < int(size.x/effectiveWidth)+1;xIdx++){
            for(int yIdx=0;yIdx < int(size.y/effectiveHeight)+1;yIdx++){
                Pos->setViewCoords(begin.x+xIdx*effectiveWidth,begin.y+yIdx*effectiveHeight,begin.z);
                Win->TypeSpecial(DFHack::WAIT,1,delay);
                Pos->getScreenTiles(screenWidth,screenHeight,screen);
                int ylim = effectiveHeight;
                int xlim = effectiveWidth;
                if(yIdx == int(size.y/effectiveHeight)){
                    ylim = size.y % effectiveHeight;
                }
                if(xIdx == int(size.x/effectiveWidth)){
                    xlim = size.x % effectiveWidth;
                }
                for(int yitr = 1;yitr < ylim+1;yitr++){
                    for(int xitr = 1;xitr < xlim+1;xitr++){
                        QString test = stringFromScreen(screen[yitr*screenWidth+xitr]);
                        rawNumbers[effectiveWidth*xIdx + xitr-1][effectiveHeight*yIdx + yitr-1] = test;
                    }
                }
            }
        }
        for(int yitr = 0;yitr < rawNumbers[0].size();yitr++){
            for(int xitr = 0;xitr < rawNumbers.size();xitr++){
                rawComplete.append(rawNumbers[xitr][yitr]);
            }
            rawComplete.append(':');
        }
        rawComplete.append("|");
    }
    retImages = ImagesFromString(rawComplete); // this is a little weird, but it will be easier to only have to debug one way to get the images
    for(int itr = 0;itr<retImages.size();itr++){
        retImages[itr].setText("rawNumbers",rawComplete);
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
QList<QImage> dfCopyPastePng::ImagesFromString(QString str)
{
    if(str.isEmpty()){
        return QList<QImage>();
    }
    QStringList zStr,yStr,xStr,screenStr,dimensions;
    
    zStr = str.split('|',QString::SkipEmptyParts);
    dimensions = zStr.takeFirst().split(',',QString::SkipEmptyParts);

    QList<QImage> Images;
//    qDebug() << dimensions[0] << dimensions[1] << dimensions[2];
    for(int z = 0;z<zStr.size();z++){
        QImage image(dimensions[0].toInt()*tileWidth,dimensions[1].toInt()*tileHeight,QImage::Format_ARGB32);
        QPainter painter(&image);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        yStr = zStr[z].split(':',QString::SkipEmptyParts);
        for(int y = 0;y<yStr.size();y++){
            xStr = yStr[y].split(';',QString::SkipEmptyParts);
            for(int x = 0;x < xStr.size();x++){
                DFHack::t_screen temp;
                screenStr = xStr[x].split(",",QString::SkipEmptyParts);
                temp.symbol = screenStr[0].toInt();
                temp.foreground = screenStr[1].toInt();
                temp.background = screenStr[2].toInt();
                temp.bright = screenStr[3].toInt();
                QImage tile = getTileFromScreen(temp);
                painter.drawImage(x*tileWidth,y*tileHeight,tile);
            }
        }
        painter.end();
        image.setText("rawNumbers",str);
        Images.append(image);
    }
    return Images;
}
QString dfCopyPastePng::stringFrom3dVector(const QVector<QVector<QVector<QString> > > &vec)
{
    QString ret;
    ret = QString("%1,%2,%3|").arg(vec.size()).arg(vec[0].size()).arg(vec[0][0].size());
    for(int z = 0; z < vec.size();z++){
        for(int y = 0; y < vec[z].size();y++){
            for(int x = 0; x < vec[z][y].size();x++){
                ret.append(vec[z][y][x] + ',');
            }
            ret.append(';');
        }
        ret.append('|');
    }
    return(ret);
}
QVector<QVector<QVector<QString> > > dfCopyPastePng::ThreeDVectorFromString(QString str)
{
    if(str.isEmpty()){
        return QVector<QVector<QVector<QString> > >();
    }
    QStringList zStr,yStr,xStr,dimensions;
    
    zStr = str.split('|',QString::SkipEmptyParts);
    dimensions = zStr.takeFirst().split(',',QString::SkipEmptyParts);

    QVector<QVector<QVector<QString> > > vec(dimensions[0].toInt(),QVector<QVector<QString> >(dimensions[1].toInt(),QVector<QString>(dimensions[2].toInt())));

    
    for(int z = 0;z<zStr.size();z++){
        yStr = zStr[z].split(';',QString::SkipEmptyParts);
        for(int y = 0;y<yStr.size();y++){
            xStr = yStr[y].split(',',QString::SkipEmptyParts);
            for(int x = 0;x < xStr.size();x++){
                vec[z][y][x] = xStr[x];
            }
        }
    }
    return vec;
}
QList< QImage> dfCopyPastePng::regenerateImages(dfCopyObj & obj)
{
    QList<QImage> retImages;
    QVector<QVector<QVector<QString > > > dig = obj.getDig();
    QVector<QVector<QVector<QString > > > build = obj.getBuild();
    DFHack::t_screen temp;
    temp.background = 0;
    temp.foreground = 7;
    temp.bright = 1;
    cursorIdx current;
    for(current.z=dig.size()-1;current.z>=0;current.z--){
        QImage currentImg(dig[current.z][0].size()*tileWidth,dig[current.z].size()*tileHeight,QImage::Format_ARGB32);
        QPainter painter(&currentImg);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        QVector<QVector<QString> > rawNumbers(dig[current.z][0].size(),QVector<QString>(dig[current.z].size()));
        for(current.y = 0;current.y<dig[current.z].size();current.y++){
            for(current.x =0;current.x<dig[current.z][current.y].size();current.x++){
                QList< int > borders = getBorders(dig,current);
                temp.symbol = 0;
                if(dig[current.z][current.y][current.x] == " " && borders.size() >= 1 ){
                    temp.symbol = 176;
                }
                if(dig[current.z][current.y][current.x] == "i"){
                    temp.symbol = 88;
                }
                QImage tile = getTileFromScreen(temp);
                QString test = stringFromScreen(temp);
                rawNumbers[current.x][current.y] = test;
                painter.drawImage(current.x * tileWidth, current.y * tileHeight, tile);
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
    return(retImages);
}
QList< int > dfCopyPastePng::getBorders(const QVector<QVector<QVector<QString > > > &dig,cursorIdx cur)
{
    int pos = 9;
    QList<int> ret;
    for(int y = cur.y -1;y<=cur.y+1;y++){
        for(int x = cur.x+1;x>=cur.x-1;x--){
            if(x != cur.x && y != cur.y){
//                if(y < 0 || x < 0 || y == dig[cur.z].size() || x == dig[cur.z][y].size()){
  //              }
                if(y >= 0 && y < dig[cur.z].size() && x >= 0 && x < dig[cur.z][y].size() && dig[cur.z][y][x] != " "){
                    ret.push_back(pos);
                }
            }
            pos--;
        }
    }
    return(ret);
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
        Win->TypeSpecial(DFHack::WAIT,1,delay);
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
       w->TypeSpecial(DFHack::WAIT,1,delay);
       p->getViewCoords(state.x,state.y,state.z);
    }
    w->TypeSpecial(DFHack::WAIT,1,delay);
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
}