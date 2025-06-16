import sys

obj_path = sys.argv[1]
output_path = obj_path + ".h"

with open(obj_path, 'rb') as f:
    content = f.read()

text_content = content.decode('latin-1')

with open(output_path, 'w', encoding='utf-8') as out:
    out.write(f'R"(\n{text_content}\n)"')