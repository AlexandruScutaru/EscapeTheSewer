"""
Auto-generate C++ file with level data based on Tiled json map export.
"""


import json
import sys

LEVEL_W = 0
LEVEL_H = 0


def parseJson(filename):
    json_file = open(filename, 'r')
    data = json.load(json_file)
    json_file.close()
    return data


def getTileStr(tiledGid):
    flip = 0
    if tiledGid & (1 << 31):
        flip |= 1 << 0 #flipped on X axis
    if tiledGid & (1 << 30):
        flip |= 1 << 1 #flipped on Y axis
    index = (tiledGid & 0x7F) - 1 #indexes are 1 based...
    return "{{{}, {}}},".format(index, flip)


def getTilesAsStr(tiles):
    dataStr = "const Level::tile_index_t Level::level[levelH][levelW] PROGMEM =  {\n"
    
    for row in range(LEVEL_H):
        arrStr = "    { "
        for col in range(LEVEL_W):
            tileStr = getTileStr(tiles[LEVEL_W * row + col])
            arrStr += tileStr
        arrStr += " },\n"
        dataStr += arrStr
    dataStr += "};\n\n"
    return dataStr


def getTiledSpecialObjects(tiledObjects):
    specialObjects = {}
    for obj in tiledObjects:
        if obj["name"] not in specialObjects:
            specialObjects[obj["name"]] = []
        specialObjects[obj["name"]].append((obj["x"], obj["y"]-8))
    return specialObjects


def getSpecialObjectsAsStr(specialObjects):
    specialsStr = "void Level::populateSpecialObjects() {\n"
    for name, positions in specialObjects.items():
        if name == "Start":
            specialsStr += "    mStartCoords = vec2({}, {});\n".format(positions[0][0], positions[0][1])
        elif name == "End":
            specialsStr += "    mEndCoords = vec2({}, {});\n".format(positions[0][0], positions[0][1])
        elif name == "Slime":
            for pos in positions:
                specialsStr += "    mEnemies.push_back(Enemy(vec2({}, {}), EnemyType::SLIME));\n".format(pos[0], pos[1])
        elif name == "Bug":
            for pos in positions:
                specialsStr += "    mEnemies.push_back(Enemy(vec2({}, {}), EnemyType::BUG));\n".format(pos[0], pos[1])

    specialsStr += "}\n\n"
    return specialsStr


def tiled2cFileStr(tiledData):
    tilesArrStr = "#ifndef LEVEL_DATA_H\n" \
                  "#define LEVEL_DATA_H\n\n" \
                  "/*** NOTE: file auto-generated based on " + str(sys.argv[1]) + " file ***/\n\n"

    tilesArrStr += getTilesAsStr(tiledData["layers"][0]["data"])

    tilesArrStr += getSpecialObjectsAsStr(getTiledSpecialObjects(tiledData["layers"][1]["objects"]))
    
    tilesArrStr += "#endif // LEVEL_DATA_H"
    return tilesArrStr


def mapSize2cFileStr():
    sizeStr = "#ifndef LEVEL_SIZE_H\n" \
                  "#define LEVEL_SIZE_H\n\n" \
                  "/*** NOTE: file auto-generated based on " + str(sys.argv[1]) + " file ***/\n\n"

    sizeStr += "#define LEVEL_WIDTH  {}\n".format(LEVEL_W)
    sizeStr += "#define LEVEL_HEIGHT {}\n\n".format(LEVEL_H)

    sizeStr += "#endif // LEVEL_SIZE_H"
    return sizeStr


def writeToFile(filepath, contents):
    with open(filepath, 'w') as file:
        file.write(contents)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} level-name.json ".format(sys.argv[0]))
        sys.exit(1)

    levelJson = parseJson(sys.argv[1])
    LEVEL_W = levelJson["width"]
    LEVEL_H = levelJson["height"]

    writeToFile("../include/level_data.h", tiled2cFileStr(levelJson))
    writeToFile("../include/level_size.h", mapSize2cFileStr())
