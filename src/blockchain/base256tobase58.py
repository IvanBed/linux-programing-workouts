import base58
import hashlib
import collections

def make_sha256(hex_address):
    second_hash = hashlib.sha256(bytes.fromhex(hashlib.sha256(bytes.fromhex(hex_address)).hexdigest())).hexdigest()
    return  hex_address + second_hash[:8]

def bin_to_hex(n):
    num = int(n, 2)
    hex_num = hex(num)
    return(hex_num)

def num_to_base58_addr(num):
    return base58.b58encode_check(num)

def keywithmaxval(d):
     v = list(d.values())
     k = list(d.keys())
     return k[v.index(max(v))]

def symb_cnt(addr, d):

    for c in  addr:
        if c == '1':
            continue
        if c in d:
            d[c] = d[c] + 1
        else:
            d[c] = 1

def main(file_path):
    d = {}
    with open(file_path, "r") as file:
        for binary_string in file:
            hex_address = str(bin_to_hex(binary_string))
            addr = num_to_base58_addr(bytes.fromhex('00' + hex_address[2:]))
            addr_str = str(addr)
            addr_str = addr_str[2:-1]
            print(addr_str)
            symb_cnt(addr_str, d)

    print(d)
    print(keywithmaxval(d))

main('test1.txt')