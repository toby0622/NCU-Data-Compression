import numpy as np
from PIL import Image
from skimage.metrics import peak_signal_noise_ratio as psnr

image1 = np.array(Image.open('Data/pic.jpg'))
image2 = np.array(Image.open('Data/pic2.jpg'))

image3 = np.array(Image.open('Data/astro.jpg'))
image4 = np.array(Image.open('Data/astro2.jpg'))

if __name__ == "__main__":
    print(round(psnr(image1, image2), 6))
    print(round(psnr(image3, image4), 6))
