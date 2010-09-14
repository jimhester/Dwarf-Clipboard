#include "inc\dfLocationIterator.h"

dfLocationIterator::dfLocationIterator(cursorIdx start, cursorIdx end,DFHack::Maps* map)
{
    Maps = map;
    pos[0] = start;
    pos[1] = end;
    sort();
    getTiles();
    begin();
    updateBlock();
    updateDesignationsBlock = false;
}

void dfLocationIterator::sort()
{
    int32_t temp1;
    if(pos[0].x > pos[1].x){
        temp1 = pos[0].x;
        pos[0].x = pos[1].x;
        pos[1].x = temp1;
    }
    if(pos[0].y > pos[1].y){
        temp1 = pos[0].y;
        pos[0].y = pos[1].y;
        pos[1].y = temp1;
    }
    if(pos[0].z > pos[1].z){
        temp1 = pos[0].z;
        pos[0].z = pos[1].z;
        pos[1].z = temp1;
    }
}
void dfLocationIterator::getTiles()
{
    tile[0] = getTileFromCursor(pos[0]);
    tile[1] = getTileFromCursor(pos[1]);
}
tileIdx dfLocationIterator::getTileFromCursor(cursorIdx in)
{
    tileIdx ret;
    ret.x = in.x/16;
    ret.y = in.y/16;
    ret.z = in.z;
    return(ret);
}
cursorIdx dfLocationIterator::begin()
{
    currentPos = pos[0];
    currentPos.z = pos[1].z;
    currentTile = tile[0];
    currentTile.z = tile[1].z;
    currentBlockPos.x = pos[0].x % 16;
    currentBlockPos.y = pos[0].y % 16;
    currentBlockPos.z = pos[1].z;
    return currentPos;
}
cursorIdx dfLocationIterator::end()
{
    currentPos = pos[1];
    currentPos.z = pos[0].z;
    currentTile = tile[1];
    currentBlockPos.x = pos[1].x % 16;
    currentBlockPos.y = pos[1].y % 16;
    currentBlockPos.z = pos[0].z;
    return currentPos;
}
void dfLocationIterator::updateBlock()
{
    if(!Maps || !Maps->isValidBlock(currentTile.x,currentTile.y,currentTile.z)) return;
    Maps->Start();
    Maps->ReadBlock40d(currentTile.x,currentTile.y,currentTile.z,&currentBlock);
    Maps->ReadDesignations(currentTile.x,currentTile.y,currentTile.z,&currentDesignation);
}
void dfLocationIterator::writeDesigations()
{
    if(!Maps || !Maps->isValidBlock(currentTile.x,currentTile.y,currentTile.z)) return;
    if(updateDesignationsBlock){
        Maps->Start();
        Maps->WriteDesignations(currentTile.x,currentTile.y,currentTile.z, &currentDesignation);
    }
}
bool dfLocationIterator::next()
{
    currentBlockPos.x++;
    if(currentPos.x == pos[1].x && currentPos.y == pos[1].y && currentPos.z == pos[0].z){
        writeDesigations();
        return false;
    }
    if(currentBlockPos.x >= 16 || currentTile.x == tile[1].x && currentPos.x >= pos[1].x){
        currentBlockPos.y++;
        if(currentTile.x == tile[0].x){
            currentBlockPos.x = pos[0].x % 16;
            currentPos.x = pos[0].x;
        }
        else{
            currentBlockPos.x = 0;
            currentPos.x = currentTile.x * 16;
        }
    }
    else{
        currentPos.x++;
        return true;
    }
    if(currentBlockPos.y >= 16 || currentTile.y == tile[1].y && currentPos.y >= pos[1].y){
        if(currentTile.x < tile[1].x){
            writeDesigations();
            currentTile.x++;
            updateBlock();
            currentBlockPos.x = 0;
            currentPos.x = currentTile.x * 16;
        }
        else{
            if(currentTile.y < tile[1].y){
                writeDesigations();
                currentTile.x = tile[0].x;
                currentTile.y++;
                updateBlock();
                currentBlockPos.x = pos[0].x % 16;
                currentPos.x = pos[0].x;
            }
            else{
                writeDesigations();
                currentTile.z--;
                currentPos.z--;
                currentBlockPos.z--;
                currentTile.y = tile[0].y;
                currentTile.x = tile[0].x;
                currentBlockPos.x = pos[0].x % 16;
                currentPos.x = pos[0].x;
                currentBlockPos.y = pos[0].y % 16;
                currentPos.y = pos[0].y;
                updateBlock();
                return true;
            }
        }
        if(currentTile.y == tile[0].y){
            currentBlockPos.y = pos[0].y % 16;
            currentPos.y = pos[0].y;
        }
        else{
            currentBlockPos.y = 0;
            currentPos.y = currentTile.y * 16;
        }
    }
    else{
        currentPos.y++;
        return true;
    }
    return true;
//    return false;
}
