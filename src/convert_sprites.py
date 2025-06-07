import os
import subprocess
from PIL import Image
import struct

def convert_png_to_bmp(png_path, bmp_path):
    # Convert PNG to 1-bit BMP with white background, inverted colors, and flipped vertically
    cmd = f"convert {png_path} -background white -alpha remove -alpha off -negate -flip -threshold 50% -type bilevel BMP3:{bmp_path}"
    subprocess.run(cmd, shell=True, check=True)

def read_bmp_to_array(bmp_path):
    with open(bmp_path, 'rb') as f:
        # Read BMP header
        header = f.read(54)
        width = struct.unpack('<I', header[18:22])[0]
        height = struct.unpack('<I', header[22:26])[0]
        
        # Calculate row padding
        row_size = (width + 7) // 8
        padding = (4 - (row_size % 4)) % 4
        
        # Read pixel data
        pixels = []
        for _ in range(height):
            row = []
            for _ in range(row_size):
                byte = f.read(1)
                if not byte:
                    break
                row.append(ord(byte))
            pixels.append(row)
            f.seek(padding, 1)  # Skip padding
        
        return width, height, pixels

def generate_cpp_array(name, width, height, pixels):
    array_str = f"const uint8_t {name}[] = {{\n"
    array_str += f"    {width}, {height},  // Width and height\n"
    
    # Convert pixels to hex
    for row in pixels:
        hex_values = [f"0x{byte:02x}" for byte in row]
        array_str += "    " + ", ".join(hex_values) + ",\n"
    
    array_str += "};\n"
    return array_str

def main():
    # Use absolute paths
    current_dir = os.path.dirname(os.path.abspath(__file__))
    png_dir = os.path.join(current_dir, "png")
    bmp_dir = os.path.join(current_dir, "bmp")
    output_file = os.path.join(current_dir, "sprites.h")
    
    # Create BMP directory if it doesn't exist
    os.makedirs(bmp_dir, exist_ok=True)
    
    # Get all PNG files
    png_files = [f for f in os.listdir(png_dir) if f.endswith('.png')]
    
    # Generate header file content
    header_content = """#pragma once
#include <stdint.h>

// Sprite data arrays
"""
    
    # Process each PNG file
    for png_file in sorted(png_files):
        png_path = os.path.join(png_dir, png_file)
        bmp_path = os.path.join(bmp_dir, png_file.replace('.png', '.bmp'))
        
        print(f"Processing {png_file}...")
        
        # Convert PNG to BMP
        convert_png_to_bmp(png_path, bmp_path)
        
        # Read BMP and generate array
        width, height, pixels = read_bmp_to_array(bmp_path)
        array_name = png_file.replace('.png', '').replace('-', '_')
        array_data = generate_cpp_array(array_name, width, height, pixels)
        
        header_content += f"\n// {png_file}\n{array_data}\n"
    
    # Write header file
    with open(output_file, 'w') as f:
        f.write(header_content)
    
    print(f"Generated {output_file}")

if __name__ == "__main__":
    main() 