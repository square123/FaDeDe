clear;%�ܵĳ���
close all;
clc;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%designed by Hao Kai-feng
%%%%%%%%%%%%%%%%%%%%%%%%%%%

% %��Χͳ��
% fileform = 'C:\Users\�����Ϸ�\Desktop\WiFi̽��\lfw\000\renlian'; 
% I =cell(1,20);
% for i=981:1000% ��ȡͼƬ
%     imgname=strcat(strcat(fileform,num2str(i)),'.jpg');
%     I{i}=imread(imgname);
% end
% %��ɫ�ָ�
% for iii=981:1000
%     img=I{iii};
    
    
img=imread('C:\Users\�����Ϸ�\Desktop\WiFi̽��\lfw\000\renlian360.jpg');
% %ɫ��ƽ�⣬�Ҷȿռ����� ��������
% R=img(:,:,1);
% G=img(:,:,2);
% B=img(:,:,3);
% avrR=mean(R(:));
% avrG=mean(G(:));
% avrB=mean(B(:));
% K=(avrR+avrG+avrB);
% nR=R*(avrR/K);
% nG=G*(avrG/K);
% nB=B*(avrB/K);
% img(:,:,1)=nR;
% img(:,:,2)=nG;
% img(:,:,3)=nB;
%����ǰ�ٷ�֮�������ֵ ��������Ӱ���Լ����㷨
% imshow(img);
% figure;
Yimg=sort(img(:),'descend');     %����ֵ����
avrYimg=mean(Yimg(1:round(length(Yimg)*0.05))); %����ǰ%5������ƽ��ֵ
img=img*(255/avrYimg);                %��(255/v)ϵ������
% imshow(img);
oriImg=img;%��ԭʼͼ�񱣴�
% �Ӹ��Ͻ�ͼ��ĳߴ����̼򻯼����������ͼ���800����Ϊ��׼
[mm,nn,hh]=size(img);%ͼ����ϢԽ�ḻԽ���׼��������������������Ϊ���Ż�����Ŀ�ѡ����
if mm>600||nn>600
    Temp=min(mm,nn);
    Tempxishu=round(Temp/600);
    img=imresize(img,1/Tempxishu);%�Ѵ�ͼ��任��Сͼ�񣬷������
else
    Tempxishu=1;
end
%400*400�ߴ����ҵ�ͼƬ������ȡС����ȡ������Ҫ�� ���ܽ�ͼƬת����̫С


%С��������Ҫ��ͼ�������Ҫ�����ϸ�ڣ�����������ٶ�ͼ����б任�ȽϺ�
%%%%%%%%%%%%%%%%%%%%%%%
% hsvImg=rgb2hsv(img);
% sImg=hsvImg(:,:,2);
yccImg=rgb2ycbcr(img);
crImg=yccImg(:,:,3);
cbImg=yccImg(:,:,2);
% sel=zeros(size(img,1),size(img,2),'single');sel=cbImg>=98&cbImg<=127&crImg>=140&crImg<=160;
sel=cbImg>=98&cbImg<=127&crImg>=140&crImg<=160;
%                                                             figure;
%                                                             imshow(sel);
%��ɫȥ��
sel=imfill(sel,'holes');%���ն�
%                                                             figure;
%                                                             imshow(sel);
se=strel('square',5);%������ ȥ��һЩ���
sel=imopen(sel,se);
sel=imfill(sel,'holes');%���ն�
se=strel('square',9);%������ ȥ����͹�Խϴ�����򣬽���ϸС���ӵ���������ֿ�
sel=imopen(sel,se);
%                                                             figure;
%                                                             imshow(sel);
%��ͨ����
[seled,num]=bwlabel(sel,8);
%ȥ������Ƚϴ������
for i=1:num
    [hang,lie,shu]=find(seled==i);
    if (max(hang)-min(hang))>3*(max(lie)-min(lie))%ȥ������ȹ��������
        seled(seled==i)=0;
    end
    if 3*(max(hang)-min(hang))<(max(lie)-min(lie))%ȥ������ȹ��������
        seled(seled==i)=0;
    end
end
%                                                             figure;
%                                                             imshow(seled);
%���¹���
seled(seled>0)=1;
%ȥ��С������
%ͳ����ͨ�����ƽ��
mianJi=sum(seled(:));
[seleded,numed]=bwlabel(seled,8);
mianJiAvr=mianJi/numed;%ƽ�����
for i=1:numed
    [hang,lie,shu]=find(seleded==i);
    chang=max(hang)-min(hang);
    kuan=max(lie)-min(lie);
    may=chang*kuan;
    if sum(shu)<mianJiAvr*0.3 %С��ƽ����������㣬ȥ��һЩС������
        seleded(seleded==i)=0;
    end
    %ȥ��һЩ���νṹ���������ռ�ȣ���������Ӧ�÷ֲ����������ȵĴ󲿷�
    if sum(shu)<may*0.5 %ȥ���ṹ��̫����ķ�ɫ����
        seleded(seleded==i)=0;
    end
end
%��ȥ��С����ĵ�ȥ�������½�����ͨ����ı�ǣ���Ҫ����ģ��ƥ��Ľ��з���
seleded(seleded>0)=1;
%                                                             figure;
%                                                             imshow(seleded);
[reseled,renum]=bwlabel(seleded,8);
%���ָ�õķ�ɫ��С�������ں�
%��ȡͼ���С��������
[c,s]=wavefast(rgb2gray(img),1,'haar');
[nc,y]=wavecut('a',c,s);
% [nnc,y]=wavecut('d',nc,s);
edges=abs(waveback(nc,s,'haar'));
fin=mat2gray(edges);
% thresh = graythresh(fin);     %�Զ�ȷ����ֵ����ֵ
% I2 = im2bw(fin,thresh);       %��ͼ���ֵ��
% I2=imfill(I2,'holes');%���ն�
% I2=medfilt2(I2);%���ݱ����Ľ��飬����ת���ɴ����������������ֵ�˲�
% % %�����ֵ�˲�%����ͼ����С�����Բ���Ҫ̫��ε���ֵ�˲�
% %����ʹ�������㷨��
% se=strel('square',3);%���ϸС�㣬ʹ���ϵ�����
% I2=imdilate(I2,se);
% I2=imfill(I2,'holes');%���ն�
%                                                                 figure
%                                                                 imshow(I2);
% figure;
% imshow(fin)
fin=medfilt2(fin);
% % fin=medfilt2(fin);
fin=medfilt2(fin);
% fin=fin*3;
% figure;
% imshow(fin)
rongImg=fin.*seleded;%ȡ������ ������ǽ�û��ͨ����ͨ�����ǵ������ٺ�ͼ������
%                                                                 figure;
%                                                                 imshow(rongImg);
%ͻȻ�뵽��ͼ����С�ߴ����ԣ���Ϊ����Ҫ�ܸ����ص�ͼ��������������⣬������ý�ͼ������СһЩ
%��ؼ���ģ��ƥ��,���ǵ�ȫ�׵���������ǰ�ģ���޸����£�����Ĭ��һ�����ֻ��һ������
% figure;
% imshow(rongImg)
muban=load('C:\Users\�����Ϸ�\Desktop\mubanwavelet.mat');%����ģ��
%�����Ż����򣬰ѳ����޸�Ϊֻ�����������飬�������Ҫ��취�ѵó����ر�С�Ŀ�ȥ�������������������
panInf=zeros(renum,6,'single');%������Ϣ����洢 x��y,���ߣ��о���Ϣ����� ��ǰ������Լ�����
for i=1:renum %�����Сͼ���ϵ�ƥ�书��
    [hang,lie,shu]=find(reseled==i);
    temmat=zeros(size(reseled));
    temmat(reseled==i)=1;
    %����ʧ����д��һ��ܹؼ���,����˴���ʱ����˷ѣ��Ժ�������������Ҫ�ȼ�������û�д���
    temmat=temmat.*rongImg;
    temmat(reseled~=i)=-100;%���߽����Ϣ���⣬��Ҫ���Ѿ��õ�����Ϣ�˷� %find�����������߼������������������
    %%%%%%%%%%%%%%%%%%%%%%%
    indzuo=min(lie);%ȷ������������
    indyou=max(lie);
    indsha=min(hang);
    indxia=max(hang);
    indW=indyou-indzuo;%��
    indL=indxia-indsha;%��
    %�Ľ�������Ĵ�С��ʹ������ɴ�С�ı仯
    if indW>indL%ֱ��ȷ��5����ͬ������Ĵ�С ��������Ĵ�С�ĳ�����
        soL=round([indL*0.4 indL*0.5 indL*0.6 indL*0.7 indL*0.8]);%Ϊ�˷���������Ҫ�������������
        soW=round(3*soL/4);%LҪ��W��
        %         soW=soL;
    else
        soW=round([indW*0.4 indW*0.5 indW*0.6 indW*0.7 indW*0.8]); %
        soL=round(soW*4/3);
        %         soL=soW;
    end
    soUnit=[2 2 2 3 3];%
    panInf(i,5)=0;%�趨��ʼֵ
    %     panInf(i,1)=0;%�趨������������ϵ��¼ֵ
    %     panInf(i,2)=0;
    for ui=1:5
        W=soW(ui);%�ı�������Ĵ�С
        L=soL(ui);
        unit=soUnit(ui);
        remuban=imresize(muban.NorNumsum,[L+1 W+1]);
        %�ı䲽������
        for nj=indsha:unit:indxia-L %ѡ����������ֵ%
            for ni=indzuo:unit:indyou-W
                searchArea=temmat(nj:nj+L,ni:ni+W);%������
                %                 reArea=imresize(searchArea,[40 30]);%������ �޸�ģ���С����������������õ�ģ��Ҳ��Сһ����36*36�����Խ�80*60���40*30�����ģ���ٶȸ���
                %��Ҫ�Գ�����һ��С�Ż�������resize������ʹ�ã���߳�����ٶȣ����ö�ģ�����resize�任�������ظ�����
                %��Ϊmatlab�ƺ��Լ���������غܿ죬��imresizeʹ��ʱ��ܳ�������������Ҫ�޸���
                %       figure; ���Գ���
                %       imshow(reArea);
                %       pan=sum(sum(reArea.*muban.NorNumsum));
                %       %������������м��㣬Ҫ����ѡ��ģ����ƥ��
                %������غ������о�
                pan=corrcoef(searchArea,remuban);
                pan=pan(1,2);
                if pan>panInf(i,5)
                    %                     areamax=pan;
                    %                     ptw=ni;
                    %                     ptl=nj;
                    %                     sW=W;
                    %                     sL=L;
                    panInf(i,5)=pan;
                    panInf(i,1)=ni;
                    panInf(i,2)=nj;
                    panInf(i,3)=W;
                    panInf(i,4)=L;
                end
            end
        end
    end
end
%Ҫ��취����С�ߴ��ͼƬ�ٱ任����
%������ó���ȵı��������ͼ���ڷŴ��Ļ�����ʾ
panInf(:,6)=(panInf(:,3).*panInf(:,4));%�������Ϣ�������
panSqu=median(panInf(:,6));
panCor=median(panInf(:,5));
panInf(:,6)=panInf(:,6)/panSqu;
panInf(:,5)=panInf(:,5)/panCor;
figure;
imshow(oriImg);
hold on;%par
for i=1:renum %����ڴ�ͼ���ϻ����Լ������о���ɸѡ
        if panInf(i,6)>0.75&&panInf(i,6)<13%.6826%*panSqu %ȥ����Щ�о������С��ֵ��0.3����̬�ֲ���һ�ֽ���
            if panInf(i,5)>0.8%.6826%*panCor%��������������ֵ�����ж�
%     if 0.532*panInf(i,5)+3.503>=panInf(i,6)
        %             rectangle('Position',[Tempxishu*panInf(i,1),Tempxishu*panInf(i,2),Tempxishu*panInf(i,3),Tempxishu*panInf(i,4)],'Curvature',[0,0],'LineWidth',2,'LineStyle','--','EdgeColor','r');
        rectangle('Position',[Tempxishu*(panInf(i,1)-0.3*panInf(i,3)),Tempxishu*(panInf(i,2)-0.3*panInf(i,4)),Tempxishu*panInf(i,3)*1.6,Tempxishu*panInf(i,4)*1.6],'Curvature',[0,0],'LineWidth',2,'LineStyle','--','EdgeColor','r');
        %x,y,����,�����¿�ʼ��
%     end
            end
        end
end
% end