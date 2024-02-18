import os
import sys
import time
import zlib

from io import TextIOWrapper


def write_to_pack(out_file: TextIOWrapper, folder_path: str, redirect_files: dict[str, str]) -> int:
    total_size = 0
    null = 0
    
    for root, _, files in os.walk(folder_path):
        for file in files:
            file_path = os.path.join(root, file)
            relative_path = os.path.relpath(file_path, folder_path)

            if file in redirect_files.keys():
                if redirect_files[file] == "none":
                    print(f"[+] SKIP {file} (ignored)")
                    continue

                file_path = os.path.join(root, file)
                relative_path = os.path.relpath(os.path.join(root, redirect_files[file]), folder_path)

                print(f"[+] REDIRECT {file} => {redirect_files[file]}")
            elif file in redirect_files.values():
                print(f"[+] SKIP {file} (ignored)")
                continue

            file_size = os.path.getsize(file_path)
            total_size += file_size

            out_file.write(relative_path.encode("utf-8"))
            out_file.write(null.to_bytes(
                length=1, byteorder="little", signed=False))  # end of string
            out_file.write(file_size.to_bytes(
                length=4, byteorder="little", signed=False))  # sizeof(unsigned int) = 4
            print(f"[+] {file_path:100}{file_size} o{' ' * 10}({relative_path})")

            with open(file_path, "rb") as file:
                out_file.write(file.read())

    return total_size


def main(args: list[str]) -> int:
    redirect_files = {}
    for arg in args:
        parts = arg.split("=")
        if len(parts) == 2:
            redirect_files[parts[0]] = parts[1]
            
    base_dir = os.path.dirname(os.path.abspath(__file__))
    assets_dir = os.path.join(os.path.dirname(base_dir), "assets")

    out_tmp = os.path.join(base_dir, "resource.tmp")
    out_gz = os.path.join(base_dir, "resource.pack")

    print("[PACK]: Start packing...")
    print(f"[I] {'filename':100}size{' '*10}id\n")

    start_t = time.time()
    total_size = 0
    with open(out_tmp, "wb+") as fp:
        total_size = write_to_pack(fp, assets_dir, redirect_files)
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
