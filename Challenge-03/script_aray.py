import struct
from zlib import crc32
from hashlib import md5, sha256

def brute_hash(hash_func, target):
    for i in range(256):
        for j in range(256):
            data = bytes([i, j])
            if hash_func(data).hexdigest() == target:
                return data

def brute_crc32(target):
    target = int(target, 16)
    for i in range(256):
        for j in range(256):
            data = bytes([i, j])
            if crc32(data) & 0xffffffff == target:
                return data

def main():
    
    file_data = bytearray(85)
    
   
    file_data[58] = 122 - 25  # uint8(58) + 25 == 122
    file_data[36] = 72 - 4    # uint8(36) + 4 == 72
    file_data[74] = 116 - 11  # uint8(74) + 11 == 116
    file_data[2] = 119 - 11   # uint8(2) + 11 == 119
    file_data[84] = 128 - 3   # uint8(84) + 3 == 128
    file_data[65] = 70 + 29   # uint8(65) - 29 == 70
    file_data[75] = 86 + 30   # uint8(75) - 30 == 86
    file_data[7] = 82 + 15    # uint8(7) - 15 == 82
    file_data[21] = 94 + 21   # uint8(21) - 21 == 94
    file_data[26] = 25 + 7    # uint8(26) - 7 == 25
    file_data[27] = 40 ^ 21   # uint8(27) ^ 21 == 40
    file_data[45] = 104 ^ 9   # uint8(45) ^ 9 == 104
    file_data[16] = 115 ^ 7   # uint8(16) ^ 7 == 115
    
    # 32-bit integers (little-endian)
    file_data[10:14] = struct.pack('<I', 2448764514 - 383041523)  # uint32(10) + 383041523 == 2448764514
    file_data[17:21] = struct.pack('<I', 1412131772 + 323157430)  # uint32(17) - 323157430 == 1412131772
    file_data[22:26] = struct.pack('<I', 1879700858 ^ 372102464)  # uint32(22) ^ 372102464 == 1879700858
    file_data[28:32] = struct.pack('<I', 959764852 + 419186860)   # uint32(28) - 419186860 == 959764852
    file_data[3:7] = struct.pack('<I', 2108416586 ^ 298697263)    # uint32(3) ^ 298697263 == 2108416586
    file_data[37:41] = struct.pack('<I', 1228527996 - 367943707)  # uint32(37) + 367943707 == 1228527996
    file_data[41:45] = struct.pack('<I', 1699114335 - 404880684)  # uint32(41) + 404880684 == 1699114335
    file_data[46:50] = struct.pack('<I', 1503714457 + 412326611)  # uint32(46) - 412326611 == 1503714457
    file_data[52:56] = struct.pack('<I', 1495724241 ^ 425706662)  # uint32(52) ^ 425706662 == 1495724241
    file_data[59:63] = struct.pack('<I', 1908304943 ^ 512952669)  # uint32(59) ^ 512952669 == 1908304943
    file_data[66:70] = struct.pack('<I', 849718389 ^ 310886682)   # uint32(66) ^ 310886682 == 849718389
    file_data[70:74] = struct.pack('<I', 2034162376 - 349203301)  # uint32(70) + 349203301 == 2034162376
    file_data[80:84] = struct.pack('<I', 69677856 + 473886976)    # uint32(80) - 473886976 == 69677856
    
    # Hash brute-forcing (2 bytes each)
    file_data[0:2] = brute_hash(md5, "89484b14b36a8d5329426a3d944d2983")
    file_data[32:34] = brute_hash(md5, "738a656e8e8ec272ca17cd51e12f558b")
    file_data[50:52] = brute_hash(md5, "657dae0913ee12be6fb2a6f687aae1c7")
    file_data[76:78] = brute_hash(md5, "f98ed07a4d5f50f7de1410d905f1477f")
    file_data[14:16] = brute_hash(sha256, "403d5f23d149670348b147a15eeb7010914701a7e99aad2e43f90cfa0325c76f")
    file_data[56:58] = brute_hash(sha256, "593f2d04aab251f60c9e4b8bbc1e05a34e920980ec08351a18459b2bc7dbf2f6")
    file_data[8:10] = brute_crc32("0x61089c5c")
    file_data[34:36] = brute_crc32("0x5888fc1b")
    file_data[63:65] = brute_crc32("0x66715919")
    file_data[78:80] = brute_crc32("0x7cab8d64")
    
    # Verify
    target_md5 = "b7dc94ca98aa58dabb5404541c812db2"
    actual_md5 = md5(file_data).hexdigest()
    
    if actual_md5 == target_md5:
        print("Worked")
        print(file_data)
        with open('flag.txt', 'wb') as f:
            f.write(file_data)
            
    else:
        print("Fail")
        print("Hashes don't match")

if __name__ == "__main__":
    main()