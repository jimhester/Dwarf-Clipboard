#include "inc\DwarfClipboardCopyObj.h"
#include "inc\common.h"
#include <QPainter>
#include <QDebug>

bool DwarfClipboardCopyObj::useOriginal = false;
DFHack::Context* DwarfClipboardCopyObj::DF=0;
QMap<QString,QString> DwarfClipboardCopyObj::buildCommands= QMap<QString,QString>();
int DwarfClipboardCopyObj::delay = 100;

DwarfClipboardCopyObj::DwarfClipboardCopyObj(DwarfClipboardCopyObj *parent)
{
    cursorIdx temp;
    temp.x = temp.y = temp.z = -30000;
    pos << temp << temp;
    idx = 0;
    defaultIndex = 0;
	parentItem = parent;
    prevPaste.x = prevPaste.y = prevPaste.z = -30000;
}

DwarfClipboardCopyObj::DwarfClipboardCopyObj(cursorIdx c1, cursorIdx c2, DwarfClipboardCopyObj *parent)
{
    pos << c1 << c2;
    setupVectors();
    getDataFromDF();
    writeImages();
    idx = 0;
    defaultIndex = 0;
	parentItem = parent;
    prevPaste.x = prevPaste.y = prevPaste.z = -30000;
}
DwarfClipboardCopyObj::DwarfClipboardCopyObj(QImage img,DwarfClipboardCopyObj *parent)
{
    DwarfClipboardPng png;
    dig = png.ThreeDVectorFromString(img.text("dig"));
    build = png.ThreeDVectorFromString(img.text("build"));
    images = png.ImagesFromString(img.text("rawNumbers"));
    Name = img.text("name");
    Comment = img.text("comment");
    setTextForImages();
	parentItem = parent;
	originalImages = imageListFromTiledImages(img);
    defaultIndex = 0;
    prevPaste.x = prevPaste.y = prevPaste.z = -30000;
}
void DwarfClipboardCopyObj::recalcImages()
{   
    if(images.empty()) 
        return;
    DwarfClipboardPng png;
    images = png.ImagesFromString(images.at(0).text("rawNumbers"));
}
QList<QImage> DwarfClipboardCopyObj::imageListFromTiledImages(const QImage &image)
{
    QList<QImage> retList;
    QStringList dimensions;      
    dimensions = image.text("rawNumbers").left(image.text("rawNumbers").indexOf("|")).split(',',QString::SkipEmptyParts);
    int tileWidth = image.width()/dimensions[0].toInt();
    int tileHeight = image.height()/dimensions[1].toInt()/dimensions[2].toInt();
    for(int zIdx = 0;zIdx < dimensions[2].toInt();++zIdx)
    {
        QImage test = image.copy(QRect(0,dimensions[1].toInt()*tileHeight*zIdx,tileWidth*dimensions[0].toInt(),tileHeight*dimensions[1].toInt()));
        QString key;
        foreach(key,image.textKeys()){
            test.setText(key,image.text(key));
        }
        retList.append(test);
    }
    return retList;
}
QImage DwarfClipboardCopyObj::getImage(int z) const
{ 
    if(z == -1)
    { 
        z = defaultIndex; 
    } 
    if(useOriginal && !originalImages.empty())
    { 
        return originalImages.at(z); 
    } 
    else 
    { 
        if(images.empty())
        {
            return QImage();
        } 
        return images.at(z); 
    } 
}
QImage DwarfClipboardCopyObj::getTiledImages() const
{
    if(originalImages.empty())
    {
        return QImage();
    }
    QImage result(originalImages.at(0).width(),originalImages.at(0).height()*originalImages.size(),QImage::Format_ARGB32);
    QString key;
    foreach(key,originalImages.at(0).textKeys()){
        result.setText(key,originalImages.at(0).text(key));
    }
    QPainter paint(&result);
    for(int i = 0;i<originalImages.size();i++)
    {
        paint.drawImage(0,i*originalImages.at(0).height(),originalImages.at(i));
    }
    paint.end();
    return result;
}
DwarfClipboardCopyObj::~DwarfClipboardCopyObj()
{
	qDeleteAll(childItems);
}
void DwarfClipboardCopyObj::setOriginalState(int state)
{ 
	if(state)
	{
		useOriginal = true; 
	}
	else
	{ 
		useOriginal = false;
	}
}
void DwarfClipboardCopyObj::appendChild(DwarfClipboardCopyObj *item)
{
	childItems.append(item);
}
void DwarfClipboardCopyObj::prependChild(DwarfClipboardCopyObj *item)
{
	childItems.prepend(item);
}
void DwarfClipboardCopyObj::insertChild(int position, DwarfClipboardCopyObj *item)
{
    //if(position > childItems.size()){
    //    childItems.append(item);
    //}
	childItems.insert(position,item);
}
void DwarfClipboardCopyObj::removeChildAt(int position)
{
	childItems.removeAt(position);
}
DwarfClipboardCopyObj* DwarfClipboardCopyObj::child(int row)
{
	return childItems.value(row);
}
int DwarfClipboardCopyObj::childCount() const
{
	return childItems.count();
}
int DwarfClipboardCopyObj::row() const
{
	if(parentItem)
		return parentItem->childItems.indexOf(const_cast<DwarfClipboardCopyObj*>(this));
	return 0;
}
DwarfClipboardCopyObj* DwarfClipboardCopyObj::parent()
{
	return parentItem;
}
void DwarfClipboardCopyObj::writeImages()
{
    DwarfClipboardPng png;
    images = png.getImagesForRange(pos);
    setTextForImages();
	originalImages = images;
}
void DwarfClipboardCopyObj::setTextForImages()
{    
    DwarfClipboardPng png;
    for(int itr = 0;itr < images.size();itr++)
    {
        images[itr].setText("dig",png.stringFrom3dVector(dig));
        images[itr].setText("build",png.stringFrom3dVector(build));
        images[itr].setText("name", Name);
        images[itr].setText("comment", Comment);
    }
}
        
void DwarfClipboardCopyObj::setName(QString n)
{
    Name = n;
    for(int itr = 0;itr < images.size();itr++){
        images[itr].setText("name",Name);
        originalImages[itr].setText("name",Name);
    }
}
void DwarfClipboardCopyObj::setComment(QString c)
{
    Comment = c;
    for(int itr = 0;itr < images.size();itr++){
        images[itr].setText("comment",Comment);
        originalImages[itr].setText("comment",Comment);
    }
}
void DwarfClipboardCopyObj::getDataFromDF()
{
    Maps = DF->getMaps();
    Maps->Start();
    dfLocationIterator itr(pos[0],pos[1],Maps);
    cursorIdx end = itr.end();
    cursorIdx begin = itr.begin();
    checkDig(itr.getCurrentTileType(),itr.getCurrentPosition(),begin);
    while(itr.next()){
        checkDig(itr.getCurrentTileType(),itr.getCurrentPosition(),begin);
    }
    DFHack::Buildings * Bld = DF->getBuildings();
    DFHack::VersionInfo* mem = DF->getMemoryInfo();
    uint32_t numBuildings;
    if(Bld->Start(numBuildings)){
        for(uint32_t i = 0; i < numBuildings; i++){
            DFHack::t_building temp;
            Bld->Read(i, temp);
            if(temp.type != 0xFFFFFFFF){
                std::string typestr;
                mem->resolveClassIDToClassname(temp.type, typestr);
                if(temp.z >= end.z && temp.z <= begin.z && temp.x1 >= begin.x && temp.x2 <= end.x && temp.y1 >= begin.y && temp.y2 <= end.y){
                    build[begin.z - temp.z][temp.y1-begin.y][temp.x1-begin.x] = buildCommands[QString(typestr.c_str())];
                    qDebug() << buildCommands[QString(typestr.c_str())];
                }
            }
        }
    }
}
void DwarfClipboardCopyObj::checkDig(int32_t tileType, cursorIdx current, cursorIdx begin)
{
    if(tileType < 0 || tileType > 519)return;
    if(DFHack::isOpenTerrain(tileType) || DFHack::isFloorTerrain(tileType)) {dig[begin.z - current.z][current.y - begin.y][current.x - begin.x] = "d";} 
    else if(DFHack::STAIR_DOWN == DFHack::tileTypeTable[tileType].c){ dig [begin.z - current.z][current.y - begin.y][current.x - begin.x] = "j"; }
    else if(DFHack::STAIR_UP == DFHack::tileTypeTable[tileType].c){ dig [begin.z - current.z][current.y - begin.y][current.x - begin.x] = "u"; }
    else if(DFHack::STAIR_UPDOWN == DFHack::tileTypeTable[tileType].c){ dig [begin.z - current.z][current.y - begin.y][current.x - begin.x] = "i" ;}
    else if(DFHack::isRampTerrain(tileType)){dig [begin.z - current.z][current.y - begin.y][current.x - begin.x] = "r" ;}
   // else if(DFHack::isWallTerrain(tileType)){build [begin.z - current.z][current.y - begin.y][current.x - begin.x] = "u" ;}
    //build [currentPos.z - pos[0].z][currentPos.y - pos[0].y][currentPos.x - pos[0].x] = "w";}
}
void DwarfClipboardCopyObj::pasteDesignations(cursorIdx location)
{
    Maps = DF->getMaps();
    Maps->Start();
    cursorIdx end;
    end.z = location.z-dig.size()+1;
    end.y = location.y+dig[0].size()-1;
    end.x = location.x+dig[0][0].size()-1;
    dfLocationIterator itr(location,end,Maps);
    cursorIdx beginning = itr.begin();
    itr.setUpdateDesignations(true);
    DFHack::designations40d *designationPtr = itr.getDesignationHandle();
    cursorIdx cur = itr.getCurrentPosition();
    cursorIdx blockPos = itr.getCurrentBlockPos();
    QChar digValue = dig[beginning.z - cur.z][cur.y-beginning.y][cur.x-beginning.x].at(0);
    changeDesignation(designationPtr,digValue,blockPos);
    while(itr.next()){
        cur = itr.getCurrentPosition();
        blockPos = itr.getCurrentBlockPos();
        digValue = dig[beginning.z - cur.z][cur.y-beginning.y][cur.x-beginning.x].at(0);
        changeDesignation(designationPtr,digValue,blockPos);
    }
    prevPaste = location;
}
void DwarfClipboardCopyObj::pasteBuildings(cursorIdx location)
{
    DFHack::WindowIO* IO = DF->getWindowIO();
    IO->TypeSpecial(DFHack::WAIT,3,delay);
    IO->TypeSpecial(DFHack::ESCAPE,1,delay);
    IO->TypeStr("b",delay); // enter build menu

    for(int z = 0;z < build.size();z++){
        for(int y = 0;y < build[z].size();y++){
            for(int x = 0;x < build[z][y].size();x++){
                if(build[z][y][x] != ""){
                    IO->TypeStr(build[z][y][x].toLatin1(),delay);
                    cursorIdx temp;
                    moveToPoint(location.x+x,location.y+y,location.z-z);
                    IO->TypeSpecial(DFHack::ENTER,2);
                }
            }
        }
    }
    IO->TypeSpecial(DFHack::ESCAPE);
}
void DwarfClipboardCopyObj::moveToPoint(int x, int y,int z)
{
    cursorIdx newLoc;
    newLoc.x = x;
    newLoc.y = y;
    newLoc.z = z;
    moveToPoint(newLoc);
}

void DwarfClipboardCopyObj::moveToPoint(cursorIdx location)
{
    DFHack::WindowIO* IO = DF->getWindowIO();
    DFHack::Position* Pos = DF->getPosition();
    cursorIdx temp;
    Pos->getCursorCoords(temp.x,temp.y,temp.z);
    if(temp.x == -30000)
        return;
    if(location.x-temp.x < 0){
        IO->TypeSpecial(DFHack::LEFT,temp.x-location.x);
    }
    if(location.x-temp.x > 0){
        IO->TypeSpecial(DFHack::RIGHT,location.x-temp.x);
    }
    if(location.y-temp.y < 0){
        IO->TypeSpecial(DFHack::UP,temp.y-location.y);
    }
    if(location.y-temp.y > 0){
        IO->TypeSpecial(DFHack::DOWN,location.y-temp.y);
    }
    if(location.z-temp.z < 0){
        IO->TypeStr(">",temp.z-location.z);
    }
    if(location.z-temp.z > 0){
        IO->TypeStr("<",location.z-temp.z);
    }
}
                    
void DwarfClipboardCopyObj::changeDesignation(DFHack::designations40d *ptr, QChar desig, cursorIdx blockIdx)
{
    switch(desig.toAscii())
    {
        case 'd':
            (*ptr)[blockIdx.x][blockIdx.y].bits.dig = DFHack::designation_default;
            break;
        case 'i':
            (*ptr)[blockIdx.x][blockIdx.y].bits.dig = DFHack::designation_ud_stair;
            break;
        case 'u':
            (*ptr)[blockIdx.x][blockIdx.y].bits.dig = DFHack::designation_u_stair;
            break;
        case 'j':
            (*ptr)[blockIdx.x][blockIdx.y].bits.dig = DFHack::designation_d_stair;
            break;
        case 'r':
            (*ptr)[blockIdx.x][blockIdx.y].bits.dig = DFHack::designation_ramp;
            break;
            
    }
}
QString DwarfClipboardCopyObj::printDig(QChar sep) const
{
    QString ret;
    for(int z = 0;z < dig.size();z++){
        for(int y = 0;y < dig[z].size();y++){
            for(int x = 0;x < dig[z][y].size();x++){
                ret.append(dig[z][y][x] + sep);
            }
            ret.append("\n");
        }
        ret.append("#>");
    }
    return(ret);
}
void DwarfClipboardCopyObj::setupVectors()
{
    dig.clear();
    build.clear();
    dig.resize(qAbs(pos[1].z-pos[0].z)+1);
    for(int z = 0;z < dig.size();z++){
        dig[z].resize(qAbs(pos[1].y-pos[0].y)+1);
        for(int y = 0;y < dig[z].size();y++){
            dig[z][y].resize(qAbs(pos[1].x-pos[0].x)+1);
            dig[z][y].fill(" ");
        }
    }
    build.resize(qAbs(pos[1].z-pos[0].z)+1);
    for(int z = 0;z < build.size();z++){
        build[z].resize(qAbs(pos[1].y-pos[0].y)+1);
        for(int y = 0;y < build[z].size();y++){
            build[z][y].resize(qAbs(pos[1].x-pos[0].x)+1);
        }
    }
}
/*DwarfClipboardCopyObj(const DwarfClipboardCopyObj & oldObj){
    DF = oldObj.DF;
    pos = oldObj.pos;
    tile = oldObj.tile;
    currentPos = oldObj.currentPos;
    currentTile = oldObj.currentTile;
    idx = oldObj.idx;
}*/
void DwarfClipboardCopyObj::setDF(DFHack::Context *tDF)
{
    DF = tDF;
}

void DwarfClipboardCopyObj::addPos(cursorIdx newPos)
{
    pos[idx] = newPos;
    idx = (idx+1) % 2;
    if(getValid() == 2){
        setupVectors();
        getDataFromDF();
        writeImages();
    }
}
void DwarfClipboardCopyObj::clear()
{
    pos[0].x = -30000;
    pos[1].x = -30000;
    childItems.clear();
}
int DwarfClipboardCopyObj::getValid() const
{
    int retVal = 0;
    if(pos[0].x != -30000){
        retVal++;
    }
    if(pos[1].x != -30000){
        retVal++;
    }
    return(retVal);
}
void DwarfClipboardCopyObj::setBuildCommands(QMap<QString,QString> commands)
{
    buildCommands = commands;
}
cursorIdx DwarfClipboardCopyObj::getPrevPastePoint()
{
    return prevPaste;
}
void DwarfClipboardCopyObj::setDelay(int newDelay)
{ 
	delay = newDelay;
}
void DwarfClipboardCopyObj::setOrignialToCurrent()
{
    originalImages = images;
}