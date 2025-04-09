import numpy as np
import pycocotools
from PIL import Image

LEFT_LEG_COLOR = (64 * 256 + 24) * 256 + 126
RIGHT_LEG_COLOR = (42 * 256 + 126) * 256 + 24
BUTT_COLOR = (255 * 256 + 24) * 256 + 255
BOT_ARM_COLOR = (62 * 256 + 21) * 256 + 211
LIP_COLOR = (21 * 256 + 109) * 256 + 211
TOP_ARM_COLOR = (255 * 256 + 0) * 256 + 0
BODY_COLOR = (21 * 256 + 211) * 256 + 91
HEAD_COLOR = (211 * 256 + 167) * 256 + 21

def rle_encode(a: np.ndarray)->[int]:
    i = 0
    a = a.reshape(-1)
    s = a.size
    prev = False
    len = 0
    out = []
    while i<s:
        if a[i] == prev:
            len += 1
        else:
            prev = not prev
            out.append(len)
            len = 0
    if len > 0:
        out.append(len)
    return out


def rlencode(x:np.ndarray, dropna=False):
    """
    Run length encoding.
    Based on http://stackoverflow.com/a/32681075, which is based on the rle
    function from R.

    Parameters
    ----------
    x : 1D array_like
        Input array to encode
    dropna: bool, optional
        Drop all runs of NaNs.

    Returns
    -------
    start positions, run lengths, run values

    """
    where = np.flatnonzero
    x = x.reshape(-1)
    n = len(x)
    if n == 0:
        return (np.array([], dtype=int),
                np.array([], dtype=int),
                np.array([], dtype=x.dtype))

    starts = np.r_[0, where(~np.isclose(x[1:], x[:-1], equal_nan=True)) + 1]
    lengths = np.diff(np.r_[starts, n])
    values = x[starts]

    if dropna:
        mask = ~np.isnan(values)
        starts, lengths, values = starts[mask], lengths[mask], values[mask]

    return lengths


def rle_decode(rle: [int], shape)->np.ndarray:
    mat = np.empty(shape, dtype=bool)
    value = False
    offset = 0
    mat1d = mat.reshape(-1)
    for l in rle:
        end = offset+l
        mat1d[offset:end] = value
        value = not value
        offset = end
    return mat

def get_uv_mask(filename):
    uv_region_mask = np.array(Image.open(filename), dtype=np.uint32)
    uv_region_mask = (uv_region_mask[:, :, 0] * 256 + uv_region_mask[:, :, 1]) * 256 + uv_region_mask[:, :, 2]
    return uv_region_mask

def disp(rle, shape):
    from matplotlib import pyplot as plt
    m = rle_decode(rle, shape)
    plt.imshow(m)
    plt.show()

def main(filename, colors, display):
    img = get_uv_mask(filename)
    for name, color, region in colors:
        if region is None:
            tile = img
        else:
            from_x,to_x,from_y,to_y = region
            tile = img[from_y:to_y,from_x:to_x]
        if isinstance(color, tuple):
            bools = tile==color[0]
            for c in color[1:]:
                bools = np.logical_or(bools, tile == c)
        else:
            bools = tile==color
        rle:np.ndarray = rlencode(bools)
        print(name, "= np.array(", rle.tolist(),", dtype=np.uint32)")
        if display:
            disp(rle, tile.shape)


def body_main(display=True):
    a = 1024*4
    b = 1024*8
    main("uv_region_mask.png",[
        ('HEAD_RLE', (HEAD_COLOR, LIP_COLOR), (0,a,a,b)),
        ('BODY_RLE', (BODY_COLOR, BUTT_COLOR), (a,b,a,b)),
        ('LEFT_LEG_RLE', LEFT_LEG_COLOR, (0,a,0,a)),
        ('RIGHT_LEG_RLE', RIGHT_LEG_COLOR, (0,a,0,a)),
        ('BUTT_RLE', BUTT_COLOR, (a,b,a,b)),
        ('BOT_ARM_RLE', BOT_ARM_COLOR, (a,b,0,a)),
        ('LIP_RLE', LIP_COLOR,  (0,a,a,b)),
        ('TOP_ARM_RLE', TOP_ARM_COLOR, (a,b,0,a)),
    ],display)

def body_eyelashes(display=True):
    main("Tara9_Eyelashes_map.png",[
        ('EYELASHES_RLE', BUTT_COLOR, None),
    ],display)


if __name__ == '__main__':
    # disp(, (4048,4048))
    body_main(display=False)