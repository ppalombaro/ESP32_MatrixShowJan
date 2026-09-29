import struct
data = open('ffat.bin', 'rb').read()
off = 4
n = struct.unpack_from('<I', data, 0)[0]
for i in range(n):
    plen = struct.unpack_from('<H', data, off)[0]; off += 2
    path = data[off:off+plen].decode(); off += plen
    clen = struct.unpack_from('<I', data, off)[0]; off += 4
    if 'spooky_message' in path or 'trick_or_treat' in path:
        content = data[off:off+clen]
        print(path, 'declared_clen=', clen, 'actual_len=', len(content), 'bytes=', content)
    off += clen
    pad = (512 - (off % 512)) % 512
    off += pad
