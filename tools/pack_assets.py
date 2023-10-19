import os
import sys
import gzip

from io import TextIOWrapper


def write_to_pack(out_file: TextIOWrapper, folder_path: str) -> None:
    for root, _, files in os.walk(folder_path):
        for file in files:
            file_path = os.path.join(root, file)
            relative_path = os.path.relpath(file_path, folder_path)

            out_file.write(relative_path.encode("utf-8") + b"\n")

            with open(file_path, "rb") as file:
                out_file.write(file.read())

def main(args: list[str]) -> int:
    base_dir = os.path.dirname(os.path.abspath(__file__)) 
    
    assets_dir = os.path.join(os.path.dirname(base_dir), "assets")
    
    out_tmp = os.path.join(base_dir, "resource.tmp")
    out_gz = os.path.join(base_dir, "resource.pack")
    
    
    with open(out_tmp, "wb+") as fp:
        write_to_pack(fp, assets_dir)
        fp.close()
        
    with open(out_tmp, 'rb') as fp_in, gzip.open(out_gz, 'wb') as fp_out:
        fp_out.writelines(fp_in)

    if os.path.exists(out_tmp):
        os.remove(out_tmp)
    
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))