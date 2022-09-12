import os
import sys
import zlib
import json

kioskROM = "rom/dk64_kiosk.z64"

class FileData:
    def __init__(self, table, entry, return_entry, mod_function = ""):
        self.table = table
        self.entry = entry
        self.return_entry = return_entry
        self.mod = mod_function

def modifyArmyModel(file_name):
    write_data = [
        0x217C,
        0x222C,
        0x2334,
        0x244C,
        0x2544,
        0x26BC,
        0x27D4,
        0x287C,
        0x28EC,
        0x29AC,
        0x2A5C,
        0x2ABC,
        0x2B2C,
        0x2C44,
        0x2CFC,
        0x2DAC,
        0x2E9C,
        0x2FB4,
        0x30AC,
        0x31FC,
        0x335C,
        0x347C
    ]
    with open(file_name,"r+b") as fg:
        for location in write_data:
            fg.seek(location)
            old = int.from_bytes(fg.read(4),"big")
            fg.seek(location)
            fg.write((old + 0x82C).to_bytes(4,"big"))

data = [
    {
        "folder": "army",
        "data": [
            FileData(4, 0x22, 0xEC, "modifyArmyModel"),
            FileData(24, 0xF53, 6015),
            FileData(24, 0xF54, 6016),
            FileData(24, 0xF55, 6017),
            FileData(24, 0xF56, 6018),
            FileData(24, 0xF57, 6019),
            FileData(24, 0xF58, 6020),
            FileData(24, 0xF59, 6021),
            FileData(24, 0xF5A, 6022),
            FileData(24, 0xF5B, 6023),
        ]
    }
]

kiosk_data = []
if os.path.exists(kioskROM):
    pointer_start = 0x1A7C20
    if not os.path.exists("assets/Non-Code/kiosk"):
        os.mkdir("assets/Non-Code/kiosk")
    with open(kioskROM,"rb") as kiosk:
        for enemy in data:
            folder = f"assets/Non-Code/kiosk/{enemy['folder']}"
            if not os.path.exists(folder):
                os.mkdir(folder)
            for file in enemy["data"]:
                kiosk.seek(pointer_start + (file.table * 4))
                table_start = pointer_start + int.from_bytes(kiosk.read(4),"big")
                kiosk.seek(table_start + (file.entry * 4))
                file_start = pointer_start + (int.from_bytes(kiosk.read(4),"big") & 0x7FFFFFFF)
                file_end = pointer_start + (int.from_bytes(kiosk.read(4),"big") & 0x7FFFFFFF)
                file_size = file_end - file_start
                kiosk.seek(file_start)
                compress = kiosk.read(file_size)
                kiosk.seek(file_start)
                is_compressed = int.from_bytes(kiosk.read(2),"big") == 0x1F8B
                if is_compressed:
                    decompress = zlib.decompress(compress, (15 + 32))
                else:
                    decompress = compress
                data_file = f"{folder}/t{file.table}_f{file.entry}.bin"
                with open(data_file,"wb") as fh:
                    fh.write(decompress)
                if file.mod != "":
                    mod_func = globals()[file.mod]
                    mod_func(data_file)
                file_dict_entry = {
                    "name": f"Kiosk File t{file.table}_f{file.entry}",
                    "pointer_table_index": file.table + 1,
                    "file_index": file.return_entry,
                    "source_file": data_file,
                    "do_not_extract": True,
                    "do_not_delete_source": True
                }
                kiosk_data.append(file_dict_entry)
else:
    print("Kiosk ROM doesn't exist. Place dk64_kiosk.z64 ROM into ./base-hack/rom/")
    sys.exit()

with open("kiosk_data.json","w") as json_f:
    json.dump(kiosk_data,json_f)