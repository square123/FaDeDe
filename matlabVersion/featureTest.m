close all;%С��������ȡ
ga=imread('C:\Users\�����Ϸ�\Desktop\9.jpg');

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
% %���Լ���PCA
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
% thresh = graythresh(fin);     %�Զ�ȷ����ֵ����ֵ
% I2 = im2bw(fin,thresh);       %��ͼ���ֵ��
% I2=imfill(I2,'holes');        %���ն�
% figure;
% imshow(I2);



