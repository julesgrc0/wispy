import os
import sys
import time
import zlib
import json

ASSETS_FOLDER_NAME = "assets"
TMP_FILE = "resource.tmp"
PACK_FILE = "resource.pack"
HEADER_FILE = "resource.pack.h"


def write_pack_file(folder_path: str, redirect_files: dict[str, str]) -> int:
    total_size = 0
    null_byte = (0).to_bytes(1, "little")
    tmp_file_path = TMP_FILE

    with open(tmp_file_path, "wb") as out_file:
        for root, _, files in os.walk(folder_path):
            for file in files:
                file_path = os.path.join(root, file)
                relative_path = os.path.relpath(file_path, folder_path)

                if file in redirect_files:
                    target = redirect_files[file]
                    if target == "none":
                        print(f"[+] SKIP {file} (ignored)")
                        continue
                    relative_path = os.path.relpath(
                        os.path.join(root, target), folder_path)
                    file_path = os.path.join(root, target)
                    print(f"[+] REDIRECT {file} => {target}")
                elif file in redirect_files.values():
                    print(f"[+] SKIP {file} (ignored)")
                    continue

                file_size = os.path.getsize(file_path)
                total_size += file_size

                # Write: path\0 + size(4 bytes) + file content
                out_file.write(relative_path.encode("utf-8") + null_byte)
                out_file.write(file_size.to_bytes(4, "little"))
                with open(file_path, "rb") as f:
                    out_file.write(f.read())

                print(
                    f"[+] {file_path:100} {file_size} bytes ({relative_path})")

    return total_size


def create_header_file(pack_path: str):
    with open(pack_path, "rb") as f:
        data = f.read()

    hex_bytes = [f"0x{b:02X}" for b in data]

    lines = []
    for i in range(0, len(hex_bytes), 16):
        lines.append(", ".join(hex_bytes[i:i+16]))

    array_content = ",\n    ".join(lines)
    header_content = f"""#pragma once
unsigned char resource_pack[] = {{
    {array_content}
}};
unsigned int resource_pack_len = {len(data)};
"""

    with open(HEADER_FILE, "w") as f:
        f.write(header_content)
    print(f"[+] Header file generated: {HEADER_FILE}")


def main(args: list[str]):
    redirect_files = json.load(open(args[0])) if args else {}
    base_dir = os.path.dirname(os.path.abspath(__file__))
    assets_dir = os.path.join(os.path.dirname(base_dir), ASSETS_FOLDER_NAME)
    out_dir = base_dir if len(args) < 2 else args[1]
    pack_path = os.path.join(out_dir, PACK_FILE)

    print("[PACK]: Start packing...")
    print(f"[I] {'filename':100} size {' ' * 5} id\n")
    start_time = time.time()

    total_size = write_pack_file(assets_dir, redirect_files)

    with open(TMP_FILE, "rb") as f_in, open(pack_path, "wb") as f_out:
        f_out.write(zlib.compress(f_in.read()))

    os.remove(TMP_FILE)
    gz_size = os.path.getsize(pack_path)

    print(f"\n[PACK]: Packed in {int((time.time() - start_time) * 1000)} ms")
    print(f"[PACK]: Compression {round(100 - (gz_size / total_size) * 100, 2)}% "
          f"(from {total_size} to {gz_size} bytes)")
    print(f"[PACK]: Generated file: {pack_path}")

    create_header_file(pack_path)
    print("[PACK]: End")


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
