close all;%小波特征提取
ga=imread('C:\Users\正爱上方\Desktop\9.jpg');

[c,s]=wavefast(rgb2gray(ga),1,'haar');

wave2gray(c,s,0);
% [Wc,Wk]=size(W);
% Wcut=W(Wc/2+1:Wc,1:Wk/2);
% thr=graythresh(Wcut);
% Wcutbw=im2bw(Wcut,thr);
% figure;
% imshow(Wcutbw);
[nc,y]=wavecut('a',c,s);

[nnc,y]=wavecut('d',nc,s);
figure;
wave2gray(nnc,s,0);
edges=abs(waveback(nnc,s,'haar'));
% imshow(edges)
figure;
fin=mat2gray(edges);
% %尝试加入PCA
% imshow(fin)
% [mm,nn]=size(fin)
% X=reshape(fin,1,mm*nn);
% [X_norm, mu, sigma] = featureNormalize(X);
% [U, S] = pca(X_norm);
% fin=reshape(U,mm,nn);
% figure;
fin=3.*fin;
imshow(fin);
fin=medfilt2(fin);
fin=medfilt2(fin);
fin=medfilt2(fin);
figure;
imshow(fin);
% thresh = graythresh(fin);     %自动确定二值化阈值
% I2 = im2bw(fin,thresh);       %对图像二值化
% I2=imfill(I2,'holes');        %填充空洞
% figure;
% imshow(I2);



