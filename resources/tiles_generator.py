"""
Auto-generate C++ array of tile_t based on .bmp image tileset
"""

import sys
from PIL import Image
import numpy as np


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
    "[24 62 56]"    : 10, # blue dark
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
    for row in range(0, 8):
        tile.append(list(indexMatrix[i*8+row][j*8:j*8+8]))
    return tile


def tile2cArrayStr(tile):
    return "    {{\n{}\n    }}".format(str(["        {{{}}}".format(str(tile[i])[1:-1]) for i in range(8)])[1:-1].replace('\'', '').replace('}, ', '},\n'))


def indexMatrix2tilesArrayHeaderStr(rows, cols, indexMatrix):
    tilesArrStr = "#ifndef TILES_H\n" \
                  "#define TILES_H\n\n" \
                  "/*** NOTE: file auto-generated based on " + str(sys.argv[1]) + " tileset ***/\n\n" \
                  "//define a list of 8x8 tiles of colors from the above array -> a tile will have a size of 32 bytes -> 6bits indexable -> max 2048 bytes\n" \
                  "const Data::tile_t Data::tiles[] PROGMEM = {\n"
    
    for row in range(rows):
        for col in range(cols):
            tilesArrStr += (tile2cArrayStr(getTileByIndex(row, col, indexMatrix))) + ",\n"
    tilesArrStr += "};\n\n#endif // TILES_H"
    
    return tilesArrStr


def writeToFile(filepath, contents):
    with open(filepath, 'w') as file:
        file.write(contents)


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: {} tileset tile_size".format(sys.argv[0]))
        sys.exit(1)

    w, h, img_data = bitmap2pixelList(sys.argv[1])
    indexMatrix = getTilesetAsIndexMatrix(img_data)
    writeToFile('../include/tiles.h', indexMatrix2tilesArrayHeaderStr(h // 8, w // 8, indexMatrix))
