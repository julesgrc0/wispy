import os
import sys
import time
import zlib

from io import TextIOWrapper


def write_to_pack(out_file: TextIOWrapper, folder_path: str) -> int:
    total_size = 0
    null = 0
    for root, _, files in os.walk(folder_path):
        for file in files:
            file_path = os.path.join(root, file)
            relative_path = os.path.relpath(file_path, folder_path)

            file_size = os.path.getsize(file_path)
            total_size += file_size

            out_file.write(relative_path.encode("utf-8"))
            out_file.write(null.to_bytes(
                length=1, byteorder="little", signed=False))  # end of string
            out_file.write(file_size.to_bytes(
                length=4, byteorder="little", signed=False))  # sizeof(unsigned int) = 4
            print(f"[+] {file_path:100}{file_size} o")

            with open(file_path, "rb") as file:
                out_file.write(file.read())

    return total_size


def main(args: list[str]) -> int:
    base_dir = os.path.dirname(os.path.abspath(__file__))

    assets_dir = os.path.join(os.path.dirname(base_dir), "assets")

    out_tmp = os.path.join(base_dir, "resource.tmp")
    out_gz = os.path.join(base_dir, "resource.pack")

    print("[PACK]: Start packing...")
    print(f"[-] {'filename':100}{'size (octet)'}\n")

    start_t = time.time()
    total_size = 0
    with open(out_tmp, "wb+") as fp:
        total_size = write_to_pack(fp, assets_dir)
        fp.close()

    with open(out_tmp, 'rb') as fp_in:
        with open(out_gz, 'wb') as fp_out:
            fp_out.write(zlib.compress(fp_in.read()))
            fp_out.close()
        fp_in.close()

    if os.path.exists(out_tmp):
        os.remove(out_tmp)

    print(f"\n[PACK]: packed in {round((time.time() - start_t) * 1000)} ms")
    gz_size = os.path.getsize(out_gz)
    print(
        f"[PACK]: compression {round(100 - (gz_size/total_size)*100, 2)}% (from {total_size} to {gz_size})")
    print("[PACK]: End")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
