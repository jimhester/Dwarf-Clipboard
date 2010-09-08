#include "inc\dfCopyObj.h"
#include "inc\common.h"

dfCopyObj::dfCopyObj(DFHack::Context *tDF,dfCopyObj *parent)
{
    setDF(tDF);
    cursorIdx temp;
    temp.x = temp.y = temp.z = -30000;
    pos << temp << temp;
    idx = 0;
	parentItem = parent;
}

dfCopyObj::dfCopyObj(DFHack::Context *tDF,cursorIdx c1, cursorIdx c2, dfCopyObj *parent)
{
    setDF(tDF);
    pos << c1 << c2;
    setupVectors();
    getDataFromDF();
    writeImages();
    idx = 0;
	parentItem = parent;
}
dfCopyObj::dfCopyObj(DFHack::Context *tDF,QImage img,dfCopyObj *parent)
{
    setDF(tDF);
    dfCopyPastePng png(DF);
    dig = png.ThreeDVectorFromString(img.text("dig"));
    build = png.ThreeDVectorFromString(img.text("build"));
    images = png.ImagesFromString(img.text("rawNumbers"));
    Name = img.text("name");
    Comment = img.text("comment");
    setTextForImages();
	parentItem = parent;
}
dfCopyObj::~dfCopyObj()
{
	qDeleteAll(childItems);
}
void dfCopyObj::appendChild(dfCopyObj *item)
{
	childItems.append(item);
}
void dfCopyObj::prependChild(dfCopyObj *item)
{
	childItems.prepend(item);
}
void dfCopyObj::insertChild(int position, dfCopyObj *item)
{
	childItems.insert(position,item);
}
void dfCopyObj::removeChildAt(int position)
{
	childItems.removeAt(position);
}
dfCopyObj* dfCopyObj::child(int row)
{
	return childItems.value(row);
}
int dfCopyObj::childCount() const
{
	return childItems.count();
}
int dfCopyObj::row() const
{
	if(parentItem)
		return parentItem->childItems.indexOf(const_cast<dfCopyObj*>(this));
	return 0;
}
dfCopyObj* dfCopyObj::parent()
{
	return parentItem;
}
void dfCopyObj::writeImages()
{
    dfCopyPastePng png(DF);
    images = png.getImagesForRange(pos);
    setTextForImages();
}
void dfCopyObj::setTextForImages()
{    
    dfCopyPastePng png(DF);
    for(int itr = 0;itr < images.size();itr++)
    {
        images[itr].setText("dig",png.stringFrom3dVector(dig));
        images[itr].setText("build",png.stringFrom3dVector(build));
        images[itr].setText("name", Name);
        images[itr].setText("comment", Comment);
    }
}
        
void dfCopyObj::setName(QString n)
{
    Name = n;
    for(int itr = 0;itr < images.size();itr++){
        images[itr].setText("name",Name);
    }
}
void dfCopyObj::setComment(QString c)
{
    Comment = c;
    for(int itr = 0;itr < images.size();itr++){
        images[itr].setText("comment",Comment);
    }
}
void dfCopyObj::getDataFromDF()
{
    dfLocationIterator itr(pos[0],pos[1],Maps);
    cursorIdx beginning = itr.begin();
    checkDig(itr.getCurrentTileType(),itr.getCurrentPosition(),beginning);
    while(itr.next()){
        checkDig(itr.getCurrentTileType(),itr.getCurrentPosition(),beginning);
    }
}
void dfCopyObj::checkDig(int32_t tileType, cursorIdx current, cursorIdx begin)
{
    if(DFHack::isOpenTerrain(tileType) || DFHack::isFloorTerrain(tileType)) {dig[begin.z - current.z][current.y - begin.y][current.x - begin.x] = "d";} 
    else if(DFHack::STAIR_DOWN == DFHack::tileTypeTable[tileType].c){ dig [begin.z - current.z][current.y - begin.y][current.x - begin.x] = "j"; }
    else if(DFHack::STAIR_UP == DFHack::tileTypeTable[tileType].c){ dig [begin.z - current.z][current.y - begin.y][current.x - begin.x] = "u"; }
    else if(DFHack::STAIR_UPDOWN == DFHack::tileTypeTable[tileType].c){ dig [begin.z - current.z][current.y - begin.y][current.x - begin.x] = "i" ;}
    else if(DFHack::isRampTerrain(tileType)){dig [begin.z - current.z][current.y - begin.y][current.x - begin.x] = "r" ;}
   // else if(DFHack::isWallTerrain(tileType)){build [begin.z - current.z][current.y - begin.y][current.x - begin.x] = "u" ;}
    //build [currentPos.z - pos[0].z][currentPos.y - pos[0].y][currentPos.x - pos[0].x] = "w";}
}
void dfCopyObj::paste(cursorIdx location)
{
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
}
void dfCopyObj::changeDesignation(DFHack::designations40d *ptr, QChar desig, cursorIdx blockIdx)
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
QString dfCopyObj::printDig(QChar sep) const
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
void dfCopyObj::setupVectors()
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
/*dfCopyObj(const dfCopyObj & oldObj)
{
    DF = oldObj.DF;
    pos = oldObj.pos;
    tile = oldObj.tile;
    currentPos = oldObj.currentPos;
    currentTile = oldObj.currentTile;
    idx = oldObj.idx;
}*/
void dfCopyObj::setDF(DFHack::Context *tDF)
{
    if(tDF)
    {
        DF = tDF;
        Maps = DF->getMaps();
        Maps->Start();
    }
}

void dfCopyObj::addPos(cursorIdx newPos)
{
    pos[idx] = newPos;
    idx = (idx+1) % 2;
    if(getValid() == 2){
        setupVectors();
        getDataFromDF();
        writeImages();
    }
}
void dfCopyObj::clear(){
    pos[0].x = -30000;
    pos[1].x = -30000;
}
int dfCopyObj::getValid() const{
    int retVal = 0;
    if(pos[0].x != -30000){
        retVal++;
    }
    if(pos[1].x != -30000){
        retVal++;
    }
    return(retVal);
}
