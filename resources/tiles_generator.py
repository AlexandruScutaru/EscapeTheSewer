"""
Auto-generate C++ array of tile_t based on .bmp image tileset
"""

import sys
from PIL import Image
import numpy as np

TILE_SIZE = 0

rgb_colorIndex_map = {
    "[21 15 10]"    :  0, # brown darker
    "[42 29 13]"    :  1, # brown dark
    "[57 42 28]"    :  2, # brown
    "[104  76  60]" :  3, # brown light
    "[146 126 106]" :  4, # brown lighter
    "[216 158 101]" :  5, # beige dark
    "[239 183 117]" :  6, # beige
    "[31 36 10]"    :  7, # green dark
    "[57 87 28]"    :  8, # green light
    "[165 140  39]" :  9, # mustard
    "[24 63 57]"    : 10, # blue dark
    "[ 60 159 156]" : 11, # cyan
    "[155  26  10]" : 12, # red
    "[239 105  47]" : 13, # orange
    "[239 172  40]" : 14, # yellow
    "[239 216 161]" : 15, # whitish
}


def bitmap2pixelList(image):
    img = Image.open(image)
    w, h = img.size
    """ in format: [ row_0 = [pixel_00, pixel_01, ...], row_1 = [pixel_10, pixel_11, ...], ... ] """
    img_data = np.array(Image.open(image)).reshape(h, w, 3)
    return (w, h, img_data)


def getTilesetAsIndexMatrix(pixelList):
    tileset = []
    for row in img_data:
        indicesList = []
        for pixel in row:
            indicesList.append(rgb_colorIndex_map[str(pixel)])
        tileset.append(indicesList)
    return tileset


def getTileByIndex(i, j, indexMatrix):
    tile = []
    for row in range(TILE_SIZE):
        tile.append(list(indexMatrix[i * TILE_SIZE + row][j * TILE_SIZE:j * TILE_SIZE + TILE_SIZE]))
    return tile


def isEmptyTile(tile):
    for row in tile:
        for col in row:
            if col != 0:
                return False
    return True


def tile2cArrayStr(tile):
    #god this line of code is crazy, not even sure if I can come up with a worse one, damn
    return "    {{\n{}\n    }}".format(str(["        {{{}}}".format(str(tile[i])[1:-1]) for i in range(TILE_SIZE)])[1:-1].replace('\'', '').replace('}, ', '},\n'))


def indexMatrix2tilesArrayHeaderStr(rows, cols, indexMatrix):
    tile_row = "        {" + "0, " * (TILE_SIZE-1) + "0},\n"
    tilesArrStr = "#ifndef TILES_H\n" \
                  "#define TILES_H\n\n" \
                  "/*** NOTE: file auto-generated based on " + str(sys.argv[1]) + " tileset ***/\n\n" \
                  "//define a list of 8x8 tiles of colors from the above array -> a tile will have a size of 32 bytes -> 6bits indexable -> max 2048 bytes\n" \
                  "const Data::tile_t Data::tiles[] PROGMEM = {\n" #\
                  #"    {\n" + tile_row * TILE_SIZE + "    },\n"
    
    for row in range(rows):
        for col in range(cols):
            tile = getTileByIndex(row, col, indexMatrix)
            #if not isEmptyTile(tile):
            tilesArrStr += (tile2cArrayStr(tile)) + ",\n"
    tilesArrStr += "};\n\n#endif // TILES_H"
    
    return tilesArrStr


def writeToFile(filepath, contents):
    with open(filepath, 'w') as file:
        file.write(contents)


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: {} tileset tile_size".format(sys.argv[0]))
        sys.exit(1)

    TILE_SIZE = int(sys.argv[2])
    w, h, img_data = bitmap2pixelList(sys.argv[1])
    indexMatrix = getTilesetAsIndexMatrix(img_data)
    writeToFile('../include/tiles.h', indexMatrix2tilesArrayHeaderStr(h // TILE_SIZE, w // TILE_SIZE, indexMatrix))
