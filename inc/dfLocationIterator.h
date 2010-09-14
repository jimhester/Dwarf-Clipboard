#pragma once

#include "common.h"

class dfLocationIterator
{
    private: 
        DFHack::Maps *Maps;
        cursorIdx pos[2];
        tileIdx tile[2];
        cursorIdx currentPos;
        tileIdx currentTile;
        DFHack::mapblock40d currentBlock;
        DFHack::designations40d currentDesignation;
        cursorIdx currentBlockPos;
        bool updateDesignationsBlock;
        void updateBlock();
        void writeDesigations();
        void getTiles();
        void sort();
        tileIdx getTileFromCursor(cursorIdx in);
    public:
        dfLocationIterator(cursorIdx start, cursorIdx end,DFHack::Maps* map=NULL);
        void setUpdateDesignations(bool state = true){ updateDesignationsBlock = state; };
        cursorIdx getCurrentPosition(){ return currentPos; };
        tileIdx getCurrentTile(){ return currentTile; };
        DFHack::mapblock40d* getBlockHandle(){ return &currentBlock; };
        DFHack::designations40d* getDesignationHandle(){ return &currentDesignation; };
        cursorIdx getCurrentBlockPos(){ return currentBlockPos; };
        int16_t getCurrentTileType(){ return currentBlock.tiletypes[currentBlockPos.x][currentBlockPos.y]; };
        bool next();
        cursorIdx begin();
        cursorIdx end();
};
