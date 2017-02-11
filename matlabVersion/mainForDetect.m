clear;%总的程序
close all;
clc;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%designed by Hao Kai-feng
%%%%%%%%%%%%%%%%%%%%%%%%%%%

% %大范围统计
% fileform = 'C:\Users\正爱上方\Desktop\WiFi探针\lfw\000\renlian'; 
% I =cell(1,20);
% for i=981:1000% 读取图片
%     imgname=strcat(strcat(fileform,num2str(i)),'.jpg');
%     I{i}=imread(imgname);
% end
% %肤色分割
% for iii=981:1000
%     img=I{iii};
    
    
img=imread('C:\Users\正爱上方\Desktop\WiFi探针\lfw\000\renlian360.jpg');
% %色彩平衡，灰度空间试试 真是垃圾
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
%试试前百分之五的像素值 这个不会很影响自己的算法
% imshow(img);
% figure;
Yimg=sort(img(:),'descend');     %像素值排序
avrYimg=mean(Yimg(1:round(length(Yimg)*0.05))); %最亮前%5的像素平均值
img=img*(255/avrYimg);                %按(255/v)系数补偿
% imshow(img);
oriImg=img;%将原始图像保存
% 从根上将图像的尺寸缩短简化计算这里就以图像的800像素为基准
[mm,nn,hh]=size(img);%图像信息越丰富越容易检测出人脸，这个部分算是为了优化运算的可选部分
if mm>600||nn>600
    Temp=min(mm,nn);
    Tempxishu=round(Temp/600);
    img=imresize(img,1/Tempxishu);%把大图像变换成小图像，方便计算
else
    Tempxishu=1;
end
%400*400尺寸左右的图片满足提取小波提取特征的要求 不能将图片转换的太小


%小波特征需要的图像可能需要更多的细节，所以在最后再对图像进行变换比较好
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
%肤色去噪
sel=imfill(sel,'holes');%填充空洞
%                                                             figure;
%                                                             imshow(sel);
se=strel('square',5);%开运算 去除一些噪点
sel=imopen(sel,se);
sel=imfill(sel,'holes');%填充空洞
se=strel('square',9);%开运算 去除凹凸性较大的区域，将有细小连接的两个区域分开
sel=imopen(sel,se);
%                                                             figure;
%                                                             imshow(sel);
%连通域标记
[seled,num]=bwlabel(sel,8);
%去除长宽比较大的区域
for i=1:num
    [hang,lie,shu]=find(seled==i);
    if (max(hang)-min(hang))>3*(max(lie)-min(lie))%去掉长宽比过大的区域
        seled(seled==i)=0;
    end
    if 3*(max(hang)-min(hang))<(max(lie)-min(lie))%去掉长宽比过大的区域
        seled(seled==i)=0;
    end
end
%                                                             figure;
%                                                             imshow(seled);
%重新归零
seled(seled>0)=1;
%去除小的区域
%统计连通区域的平均
mianJi=sum(seled(:));
[seleded,numed]=bwlabel(seled,8);
mianJiAvr=mianJi/numed;%平均面积
for i=1:numed
    [hang,lie,shu]=find(seleded==i);
    chang=max(hang)-min(hang);
    kuan=max(lie)-min(lie);
    may=chang*kuan;
    if sum(shu)<mianJiAvr*0.3 %小于平均面积的置零，去除一些小点区域
        seleded(seleded==i)=0;
    end
    %去除一些畸形结构，利用面积占比，人脸区域应该分布在真个长宽比的大部分
    if sum(shu)<may*0.5 %去掉结构不太合理的肤色区域
        seleded(seleded==i)=0;
    end
end
%将去除小区域的点去除后重新进行连通区域的标记，将要进行模板匹配的进行分类
seleded(seleded>0)=1;
%                                                             figure;
%                                                             imshow(seleded);
[reseled,renum]=bwlabel(seleded,8);
%将分割好的肤色和小波特征融合
%提取图像的小波的特征
[c,s]=wavefast(rgb2gray(img),1,'haar');
[nc,y]=wavecut('a',c,s);
% [nnc,y]=wavecut('d',nc,s);
edges=abs(waveback(nc,s,'haar'));
fin=mat2gray(edges);
% thresh = graythresh(fin);     %自动确定二值化阈值
% I2 = im2bw(fin,thresh);       %对图像二值化
% I2=imfill(I2,'holes');%填充空洞
% I2=medfilt2(I2);%根据宾宾的建议，将点转换成大块的区域，这里采用中值滤波
% % %多次中值滤波%由于图像缩小，所以不需要太多次的中值滤波
% %还是使用膨胀算法吧
% se=strel('square',3);%填充细小点，使相关系数变大
% I2=imdilate(I2,se);
% I2=imfill(I2,'holes');%填充空洞
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
rongImg=fin.*seleded;%取与运算 这个就是将没有通过连通区域标记的区域再和图像相与
%                                                                 figure;
%                                                                 imshow(rongImg);
%突然想到将图像缩小尺寸试试，因为不需要很高像素的图像就能完成人脸检测，所以最好将图像做的小一些
%最关键的模板匹配,考虑到全白的情况，我们把模板修改了下，这里默认一类最多只有一个人脸
% figure;
% imshow(rongImg)
muban=load('C:\Users\正爱上方\Desktop\mubanwavelet.mat');%导入模板
%重新优化程序，把程序修改为只建立画框数组，并且最后还要想办法把得出的特别小的框去除掉，利用面积比例。
panInf=zeros(renum,6,'single');%建立信息数组存储 x，y,宽，高，判决信息，面积 提前分配可以简化运算
for i=1:renum %完成在小图像上的匹配功能
    [hang,lie,shu]=find(reseled==i);
    temmat=zeros(size(reseled));
    temmat(reseled==i)=1;
    %真是失误，少写了一句很关键的,造成了大量时间的浪费，以后如果程序出不来要先检查程序有没有错误
    temmat=temmat.*rongImg;
    temmat(reseled~=i)=-100;%将边界的信息排外，不要将已经得到的信息浪费 %find函数可以用逻辑符来替代来加速运算
    %%%%%%%%%%%%%%%%%%%%%%%
    indzuo=min(lie);%确定搜索的区间
    indyou=max(lie);
    indsha=min(hang);
    indxia=max(hang);
    indW=indyou-indzuo;%宽
    indL=indxia-indsha;%高
    %改进搜索域的大小，使可以完成大小的变化
    if indW>indL%直接确定5个不同搜索域的大小 将搜索域的大小改成数组
        soL=round([indL*0.4 indL*0.5 indL*0.6 indL*0.7 indL*0.8]);%为了方便索引，要求加入四舍五入
        soW=round(3*soL/4);%L要比W长
        %         soW=soL;
    else
        soW=round([indW*0.4 indW*0.5 indW*0.6 indW*0.7 indW*0.8]); %
        soL=round(soW*4/3);
        %         soL=soW;
    end
    soUnit=[2 2 2 3 3];%
    panInf(i,5)=0;%设定初始值
    %     panInf(i,1)=0;%设定搜索区域的左上点记录值
    %     panInf(i,2)=0;
    for ui=1:5
        W=soW(ui);%改变搜索域的大小
        L=soL(ui);
        unit=soUnit(ui);
        remuban=imresize(muban.NorNumsum,[L+1 W+1]);
        %改变步进长度
        for nj=indsha:unit:indxia-L %选择出区域最大值%
            for ni=indzuo:unit:indyou-W
                searchArea=temmat(nj:nj+L,ni:ni+W);%长，宽
                %                 reArea=imresize(searchArea,[40 30]);%长，宽 修改模板大小，其他人脸检测所用的模板也很小一般是36*36，所以将80*60变成40*30，这个模板速度更快
                %需要对程序做一个小优化，减少resize函数的使用，提高程序的速度，采用对模板进行resize变换，避免重复工作
                %因为matlab似乎对计算矩阵的相关很快，但imresize使用时间很长，所以这里需要修改下
                %       figure; 测试程序
                %       imshow(reArea);
                %       pan=sum(sum(reArea.*muban.NorNumsum));
                %       %利用面积来进行计算，要与所选的模板来匹配
                %利用相关函数做判决
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
%要想办法把缩小尺寸的图片再变换回来
%这里采用长宽比的比例来完成图像在放大后的画框显示
panInf(:,6)=(panInf(:,3).*panInf(:,4));%把面积信息计算出来
panSqu=median(panInf(:,6));
panCor=median(panInf(:,5));
panInf(:,6)=panInf(:,6)/panSqu;
panInf(:,5)=panInf(:,5)/panCor;
figure;
imshow(oriImg);
hold on;%par
for i=1:renum %完成在大图像上画框以及最后的判决框筛选
        if panInf(i,6)>0.75&&panInf(i,6)<13%.6826%*panSqu %去除那些判决面积很小的值，0.3是正态分布的一种近似
            if panInf(i,5)>0.8%.6826%*panCor%设置门限来对阈值进行判断
%     if 0.532*panInf(i,5)+3.503>=panInf(i,6)
        %             rectangle('Position',[Tempxishu*panInf(i,1),Tempxishu*panInf(i,2),Tempxishu*panInf(i,3),Tempxishu*panInf(i,4)],'Curvature',[0,0],'LineWidth',2,'LineStyle','--','EdgeColor','r');
        rectangle('Position',[Tempxishu*(panInf(i,1)-0.3*panInf(i,3)),Tempxishu*(panInf(i,2)-0.3*panInf(i,4)),Tempxishu*panInf(i,3)*1.6,Tempxishu*panInf(i,4)*1.6],'Curvature',[0,0],'LineWidth',2,'LineStyle','--','EdgeColor','r');
        %x,y,宽，高,从左下开始画
%     end
            end
        end
end
% end