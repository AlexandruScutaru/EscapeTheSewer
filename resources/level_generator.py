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


def tiledArray2CarrayStr(tiles):
    tilesArrStr = "#ifndef LEVEL_H\n" \
                  "#define LEVEL_H\n\n" \
                  "/*** NOTE: file auto-generated based on " + str(sys.argv[1]) + " file ***/\n\n" \
                  "const Data::tile_index_t Data::level[levelH][levelW] /*PROGMEM*/ =  {\n"
    
    for row in range(LEVEL_H):
        arrStr = "    { "
        for col in range(LEVEL_W):
            if LEVEL_W * row + col == 53:
                print(tiles[LEVEL_W * row + col])
            tileStr = getTileStr(tiles[LEVEL_W * row + col])
            arrStr += tileStr
        arrStr += " },\n"
        tilesArrStr += arrStr
    tilesArrStr += "};\n\n#endif // LEVEL_H"
    
    return tilesArrStr


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

    writeToFile("../include/level.h", tiledArray2CarrayStr(levelJson["layers"][0]["data"]))
