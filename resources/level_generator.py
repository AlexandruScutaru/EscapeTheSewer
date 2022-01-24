"""
Generate a binary file with level data based on Tiled json map export.

The format is described in the levels_kaitai_format.ksy.
To visualize it, navitage to https://ide.kaitai.io/ and drag and drop the levels.bin and levels_kaitai_format.ksy in the ide's Local Storage to explore it.
"""

import json
import sys


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


def writeSpecialObjectsGroup(file, positions, writeCount = True):
    if writeCount:
        file.write(len(positions).to_bytes(1, 'little'))

    for pos in positions:
        file.write(pos[0].to_bytes(2, 'little'))
        file.write(pos[1].to_bytes(2, 'little'))


def writeToBinFile(filepath, levels):
    with open(filepath, 'wb') as file:
        ## current level
        file.write(int(0).to_bytes(1, 'little'))

        ## number of levels
        file.write(len(levels).to_bytes(1, 'little'))

        levelDatas = []
        for level in levels:
            levelDatas.append(getLevelData(parseJson(level)))
            ## for each level write the level size in bytes for level seeking functionalities
            file.write(levelDatas[-1]['size'].to_bytes(4, 'little'))

        for levelData in levelDatas:
            ## write level info
            # start by writing the actual size as w and h
            levelW = levelData["w"]
            levelH = levelData["h"]
            file.write(levelW.to_bytes(1, 'little'))
            file.write(levelH.to_bytes(1, 'little'))

            # for every w * h tileIndex write a byte as 6 bits = index, 2 bits flipping
            indexBytes = bytearray(levelW * levelH)

            indices = levelData['indices']
            for i in range(len(indices)):
                flip = 0
                index = indices[i]
                if index & (1 << 31):
                    flip |= 1 << 0 #flipped on X axis
                if index & (1 << 30):
                    flip |= 1 << 1 #flipped on Y axis

                indexBytes[i] = (flip << 6) + ((index - 1) & 0x3F)
            file.write(indexBytes)

            ## write special objects
            specialObjects = levelData['specialObjs']

            # write start and end coords and special objects
            writeSpecialObjectsGroup(file, [levelData['start']], writeCount = False)
            writeSpecialObjectsGroup(file, [levelData['end']], writeCount = False)
            writeSpecialObjectsGroup(file, specialObjects.get('Slime', []))
            writeSpecialObjectsGroup(file, specialObjects.get('Bug', []))
            writeSpecialObjectsGroup(file, specialObjects.get('Health', []))

        file.close()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} level.json. The script will parse all files of the form level*.json.".format(sys.argv[0]))
        sys.exit(1)

    levels = ['level1.json', 'level2.json']
    writeToBinFile("levels.bin", levels)
