close all
clear
fileform = 'C:\Users\�����Ϸ�\Desktop\renlian\chu\c'; %����ģ��%���ǵ���ͼ�������ģ�嶼�Ƚ�С��Ϊ�˽���ƥ����ܲ���Ҫ�ܴ��ͼ��ģ�壬���Խ��ߴ���Сһ������
I =cell(1,69);
for i=1:69% ��ȡͼƬ
    imgname=strcat(strcat(fileform,num2str(i)),'.jpg');
    I{i}=imread(imgname);
end
delI=cell(1,69);

for i=1:9 %�õ�С��ͼ��
[c,s]=wavefast(rgb2gray(I{i}),1,'haar');

[nc,y]=wavecut('a',c,s);

[nnc,y]=wavecut('d',nc,s);

edges=abs(waveback(nnc,s,'haar'));

delI{i}=mat2gray(edges);

figure;
imshow(delI{i})
% fin=medfilt2(fin);%��ȡ���������������ֵ�˲���ģ��
% thresh = graythresh(fin);     %�Զ�ȷ����ֵ����ֵ
% fin= im2bw(fin,thresh);       %��ͼ���ֵ��
% delI{i}=imfill(fin,'holes') ;%���ն�

end

% Numsum=zeros(80,60);
% for i=1:69
%     Numsum=Numsum+delI{i};
% end
% 
% 
% Numsum=imresize(Numsum,0.5);
% imshow(Numsum);
% Numsum=medfilt2(Numsum);%��ֵ�˲�
% Numsum=medfilt2(Numsum);
% Numsum=medfilt2(Numsum);
% 
% NorNumsum=Numsum/max(max(Numsum));
% surf(NorNumsum);
% 
% %��Сģ������
% 
% % [chang,kuan,gong]=find(NorNumsum>0);
% % p=sum(gong)/(80*60)
% % test=ones(80,60);
% % testsum1=sum(sum(test.*NorNumsum))%��΢�޸�ģ�壬��üë���۾��ֿ�����
% % NorNumsum(find(NorNumsum>0.3))=1;%����һ���ܴ�ĳͷ��������Ҫ��ε���
% %  NorNumsum(find(NorNumsum<=0.1))=0;%����һ���ܴ�ĳͷ��������Ҫ��ε���
% % testsum2=sum(sum(test.*NorNumsum))
% % figure;
% % %���Է���ģ��
% % % Nor=NorNumsum(6:35,:);
% % imshow(NorNumsum);
% % contourf(flipud(NorNumsum));
