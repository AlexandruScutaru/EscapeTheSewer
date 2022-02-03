"""
Generate a C++ byte array to be written to the EEPROM storing the levels.

The format is described in the levels_kaitai_format.ksy.
To visualize it, navigate to https://ide.kaitai.io/ and drag and drop the levels.bin and levels_kaitai_format.ksy in the ide's Local Storage to explore it.
"""

import json
import sys
from tkinter.tix import COLUMN
from turtle import position

from numpy import array_str


COLUMN_COUNT = 0
ARRAY_SIZE = 0

def parseJson(filename):
    json_file = open(filename, 'r')
    data = json.load(json_file)
    json_file.close()
    return data


def getTiledSpecialObjects(tiledObjects):
    specialObjects = {}
    start = ()
    end = ()

    for obj in tiledObjects:
        if obj['name'] == 'Start':
            start = (obj['x'], obj['y'] - 8)
            continue
        elif obj['name'] == 'End':
            end = (obj['x'], obj['y'] - 8)
            continue

        if obj['name'] not in specialObjects:
            specialObjects[obj['name']] = []
        specialObjects[obj['name']].append((obj['x'], obj['y'] - 8))

    return (specialObjects, start, end)


def getLevelData(levelJson):
    levelData = {}

    levelData['w'] = levelJson["width"]
    levelData['h'] = levelJson["height"]

    levelData['indices'] = levelJson["layers"][0]["data"]
    levelData['specialObjs'], levelData['start'], levelData['end'] = getTiledSpecialObjects(levelJson["layers"][1]["objects"])

    # level size in bytes
    levelWAndH = 2
    countSize = 1
    bytesPerCoord = 2
    specialObjsSize = 0
    for coords in  levelData['specialObjs'].values():
        specialObjsSize += countSize + 2 * len(coords) * bytesPerCoord
    levelData['size'] = levelWAndH + levelData['w'] * levelData['h'] + specialObjsSize + 2 * 2 * bytesPerCoord

    return levelData


def writeSpecialObjectsGroup(arrStr, positions, writeCount = True):
    if writeCount:
        arrStr = writeByte(arrStr, len(positions))

    for pos in positions:
        arrStr = writeWord(arrStr, pos[0])
        arrStr = writeWord(arrStr, pos[1])
    return arrStr


def writeByte(arrStr, val):
    arrStr += str.format('0x{:02X}, ', val)

    global COLUMN_COUNT
    global ARRAY_SIZE
    COLUMN_COUNT += 1
    if COLUMN_COUNT == 16:
        arrStr += "\n    "
        COLUMN_COUNT = 0
    ARRAY_SIZE += 1

    return arrStr


def writeWord(arrStr, val):
    arrStr = writeByte(arrStr, (val >> 0) & 0xFF)
    arrStr = writeByte(arrStr, (val >> 8) & 0xFF)
    return arrStr


def writeDWord(arrStr, val):
    arrStr = writeByte(arrStr, (val >>  0) & 0xFF)
    arrStr = writeByte(arrStr, (val >>  8) & 0xFF)
    arrStr = writeByte(arrStr, (val >> 16) & 0xFF)
    arrStr = writeByte(arrStr, (val >> 24) & 0xFF)
    return arrStr


def getLevelsArrayStr(levels):
    arrStr = "#ifndef LEVELS_H\n" \
                    "#define LEVELS_H\n\n" \
                    "/*** NOTE: file auto-generated based on Tiled exported json levels ***/\n\n" \
                    "//define an array of bytes to be written to the EEPROM\n" \
                    "static const uint8_t levels[] PROGMEM = {\n    "

    ## current level
    arrStr = writeByte(arrStr, 0)

    ## number of levels
    arrStr = writeByte(arrStr, len(levels))

    levelDatas = []
    for level in levels:
        levelDatas.append(getLevelData(parseJson(level)))
        ## for each level write the level size in bytes for level seeking functionalities
        arrStr = writeDWord(arrStr, levelDatas[-1]['size'])

    for levelData in levelDatas:
        ## write level info
        # start by writing the actual size as w and h
        levelW = levelData["w"]
        levelH = levelData["h"]
        arrStr = writeByte(arrStr, levelW)
        arrStr = writeByte(arrStr, levelH)

        # for every w * h tileIndex write a byte as 6 bits = index, 2 bits flipping
        indices = levelData['indices']
        for i in range(len(indices)):
            flip = 0
            index = indices[i]
            if index & (1 << 31):
                flip |= 1 << 0 #flipped on X axis
            if index & (1 << 30):
                flip |= 1 << 1 #flipped on Y axis

            indexValue = (flip << 6) + ((index - 1) & 0x3F)
            arrStr = writeByte(arrStr, indexValue)

        ## write special objects
        specialObjects = levelData['specialObjs']

        # write start and end coords and special objects
        arrStr = writeSpecialObjectsGroup(arrStr, [levelData['start']], writeCount = False)
        arrStr = writeSpecialObjectsGroup(arrStr, [levelData['end']], writeCount = False)
        arrStr = writeSpecialObjectsGroup(arrStr, specialObjects.get('Slime', []))
        arrStr = writeSpecialObjectsGroup(arrStr, specialObjects.get('Bug', []))
        arrStr = writeSpecialObjectsGroup(arrStr, specialObjects.get('Health', []))

    arrStr += "\n};\n\n#define ARRAY_SIZE " + str(ARRAY_SIZE) + "\n\n"

    arrStr += "#endif // LEVELS_H\n"
    return arrStr


def writeToFile(filepath, arrayStr):
    with open(filepath, 'w') as file:
        file.write(arrayStr)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} level.json. The script will parse all files of the form level*.json.".format(sys.argv[0]))
        #sys.exit(1)

    levels = ['level1.json', 'level2.json']
    writeToFile("../include/levels.h", getLevelsArrayStr(levels))
