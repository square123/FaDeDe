close all
clear
fileform = 'C:\Users\正爱上方\Desktop\renlian\chu\c'; %生成模板%考虑到做图像运算的模板都比较小，为了近似匹配可能不需要很大的图像模板，所以将尺寸缩小一半试试
I =cell(1,69);
for i=1:69% 读取图片
    imgname=strcat(strcat(fileform,num2str(i)),'.jpg');
    I{i}=imread(imgname);
end
delI=cell(1,69);

for i=1:9 %得到小波图像
[c,s]=wavefast(rgb2gray(I{i}),1,'haar');

[nc,y]=wavecut('a',c,s);

[nnc,y]=wavecut('d',nc,s);

edges=abs(waveback(nnc,s,'haar'));

delI{i}=mat2gray(edges);

figure;
imshow(delI{i})
% fin=medfilt2(fin);%听取宾宾的意见，用中值滤波来模糊
% thresh = graythresh(fin);     %自动确定二值化阈值
% fin= im2bw(fin,thresh);       %对图像二值化
% delI{i}=imfill(fin,'holes') ;%填充空洞

end

% Numsum=zeros(80,60);
% for i=1:69
%     Numsum=Numsum+delI{i};
% end
% 
% 
% Numsum=imresize(Numsum,0.5);
% imshow(Numsum);
% Numsum=medfilt2(Numsum);%中值滤波
% Numsum=medfilt2(Numsum);
% Numsum=medfilt2(Numsum);
% 
% NorNumsum=Numsum/max(max(Numsum));
% surf(NorNumsum);
% 
% %缩小模板试试
% 
% % [chang,kuan,gong]=find(NorNumsum>0);
% % p=sum(gong)/(80*60)
% % test=ones(80,60);
% % testsum1=sum(sum(test.*NorNumsum))%略微修改模板，将眉毛和眼睛分开试试
% % NorNumsum(find(NorNumsum>0.3))=1;%制造一个很大的惩罚项，参数需要多次调试
% %  NorNumsum(find(NorNumsum<=0.1))=0;%制造一个很大的惩罚项，参数需要多次调试
% % testsum2=sum(sum(test.*NorNumsum))
% % figure;
% % %试试方形模板
% % % Nor=NorNumsum(6:35,:);
% % imshow(NorNumsum);
% % contourf(flipud(NorNumsum));
