from PIL import Image
import numpy as np

# C64 color palette in RGB
c64_colors = [
    (0, 0, 0), (255, 255, 255), (136, 0, 0), (170, 255, 238),
    (204, 68, 204), (0, 204, 85), (0, 0, 170), (238, 238, 119),
    (221, 136, 85), (102, 68, 0), (255, 119, 119), (51, 51, 51),
    (119, 119, 119), (170, 255, 102), (0, 136, 255), (187, 187, 187)
]

def closest_c64_color(rgb):
    # Find the closest C64 color to the given RGB color
    return min(c64_colors, key=lambda color: sum((s - q) ** 2 for s, q in zip(color, rgb)))

def image_to_c64_bytecode(image_path, output_file):
    # Open the image
    with Image.open(image_path) as img:
        # Resize to 24x21 (C64 sprite size)
        img = img.resize((24, 21), Image.NEAREST)

        # Convert image to C64 color palette
        c64_img = Image.new('P', img.size)
        c64_img.putpalette([c for color in c64_colors for c in color])
        np_img = np.array(img)
        quantized = np.apply_along_axis(closest_c64_color, 2, np_img)
        c64_img.putdata([c64_colors.index(tuple(color)) for color in quantized.reshape(-1, 3)])

        # Convert to bytecode
        sprite_data = []
        for y in range(21):  # Sprite height
            for x in range(0, 24, 8):  # Sprite width, 8 pixels per byte
                byte = 0
                for bit in range(8):
                    if x + bit < img.width:
                        color_index = c64_img.getpixel((x + bit, y))
                        byte |= (color_index != 0) << (7 - bit)  # Non-zero color index
                sprite_data.append(byte)

        # Write to output file
        with open(output_file, 'w') as f:
            for byte in sprite_data:
                f.write(f"{byte:02x} ")
            f.write("\n")

# Usage
image_to_c64_bytecode('path/to/your/image.png', 'output.txt')

