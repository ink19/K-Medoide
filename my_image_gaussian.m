I = imread('goddess.jpg');
Ig = imnoise(I,'gaussian',0.1,0.004);
imwrite(Ig, 'goddess_gaussian.jpg');