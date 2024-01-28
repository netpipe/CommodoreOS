from PIL import Image
import numpy as np

def image_to_c64_bytecode(image_path, output_file):
    # Open the image
    with Image.open(image_path) as img:
        # Convert to black & white and resize to 24x21 (C64 sprite size)
        img = img.convert('1').resize((24, 21), Image.NEAREST)

        # Convert image to numpy array
        data = np.array(img)

        # Initialize sprite data
        sprite_data = []

        # Process each row
        for y in range(21):  # Sprite height
            row_data = []
            for x in range(0, 24, 8):  # Sprite width, 8 pixels per byte
                byte = 0
                for bit in range(8):
                    if x + bit < img.width:
                        byte |= (data[y, x + bit] < 128) << (7 - bit)
                row_data.append(byte)
            sprite_data.extend(row_data)

        # Write to output file
        with open(output_file, 'w') as f:
            for byte in sprite_data:
                f.write(f"{byte:02x} ")
            f.write("\n")

# Usage
image_to_c64_bytecode('path/to/your/image.png', 'output.txt')

