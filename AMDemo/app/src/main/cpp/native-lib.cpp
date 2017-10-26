#include <jni.h>
#include <string>

#include "gif_lib.h"
#include <android/bitmap.h>
#define  argb(a,r,g,b) ( ((a) & 0xff) << 24 ) | ( ((b) & 0xff) << 16 ) | ( ((g) & 0xff) << 8 ) | ((r) & 0xff)

typedef  struct GifBean
{
//正在播放的当前帧
    int currentFrame;
//记录所有帧的时长
    int *dealys;
//记录总的帧数
    int totalFrame;
} GifBean;

void  drawFrame(GifFileType *gifFileType,GifBean *gifBean,AndroidBitmapInfo info,void *pixels)
{
    //获取当前帧
    SavedImage savedImage=gifFileType->SavedImages[gifBean->currentFrame];
    GifImageDesc imageDesc=savedImage.ImageDesc;

    //某个像素位置
    int pointPixel;
    int *px= (int *) pixels;
    int *line;
    GifColorType gifColorType;
    ColorMapObject *colorMapObject=imageDesc.ColorMap;
    if(colorMapObject==NULL)
    {
       colorMapObject=gifFileType->SColorMap;
    }
    GifByteType  gifByteType;
    px = (int *)((char *)px + info.stride * imageDesc.Top);
    for(int y=imageDesc.Top;y<imageDesc.Top+imageDesc.Height;y++)
    {
        line=px;
        for(int x=imageDesc.Left;x<imageDesc.Left+imageDesc.Width;x++)
        {
            pointPixel=(y-imageDesc.Top)*imageDesc.Width+(x-imageDesc.Left);
            gifByteType=savedImage.RasterBits[pointPixel];
            //需要给每个像素赋颜色
            if(colorMapObject!=NULL)
            {

                gifColorType=colorMapObject->Colors[gifByteType];
                line[x]=argb(255,gifColorType.Red,gifColorType.Green,gifColorType.Blue);
            }

        }
        px = (int *)((char *)px + info.stride);

    }


}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_huozhenpeng_amdemo_MainActivity_updateFrame(JNIEnv *env, jobject instance,
                                                             jobject bitmap,jlong gifPoint) {

    GifFileType * gifFileType= (GifFileType *) gifPoint;

    GifBean *gifBean= (GifBean *) gifFileType->UserData;

    AndroidBitmapInfo info;

    AndroidBitmap_getInfo(env,bitmap,&info);

    void *pixels;
    //锁定画布
    AndroidBitmap_lockPixels(env,bitmap,&pixels);
    //绘制图形
    drawFrame(gifFileType,gifBean,info,pixels);
    //控制当前播放量
    gifBean->currentFrame+=1;
    if(gifBean->currentFrame>=gifBean->totalFrame-1)
    {
        gifBean->currentFrame=0;
    }
    //解锁画布
    AndroidBitmap_unlockPixels(env,bitmap);
    return  gifBean->dealys[gifBean->currentFrame];

}



extern "C"
JNIEXPORT jint JNICALL
Java_com_example_huozhenpeng_amdemo_MainActivity_getWidth(JNIEnv *env, jobject instance,
                                                          jlong gifPoint) {
    GifFileType *gifFileType= (GifFileType *) gifPoint;

    return gifFileType->SWidth;


}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_huozhenpeng_amdemo_MainActivity_getHeight(JNIEnv *env, jobject instance,
                                                           jlong gifPoint) {

    GifFileType *gifFileType= (GifFileType *) gifPoint;

    return gifFileType->SHeight;

}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_huozhenpeng_amdemo_MainActivity_loadGif(JNIEnv *env, jobject instance,
                                                         jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    int error;
    //Open a new GIF file for read, given by its name.
    GifFileType *gifFileType = DGifOpenFileName(path, &error);
    //This routine reads an entire GIF into core, hanging all its state info off the GifFileType pointer
    DGifSlurp(gifFileType);

    GifBean *gifBean= (GifBean *) malloc(sizeof(GifBean));
    memset(gifBean,0, sizeof(GifBean));

    gifBean->dealys= (int *) malloc(sizeof(int) * gifFileType->ImageCount);
    memset(gifBean->dealys,0, sizeof(int)*gifFileType->ImageCount);

    //在图形控制扩展块中的第5个字节和第6个字节存放的是每帧的延迟时间，单位是
    // 1/100秒，而唯一能标识这是一个图形扩展块的是第2个字节，固定值0xF9
    ExtensionBlock *extensionBlock;
    //给gibBean的delays赋值，它将存放所有的帧的延迟时间
    for(int i=0;i<gifFileType->ImageCount;i++)
    {
        //取出每一帧
        SavedImage frame=gifFileType->SavedImages[i];
        // Count of extensions before imag
        //遍历扩展块，取出图形扩展块的表示的时间
        for(int j=0;j<frame.ExtensionBlockCount;j++)
        {
            //#define GRAPHICS_EXT_FUNC_CODE    0xf9    /* graphics control (GIF89) */
            if(GRAPHICS_EXT_FUNC_CODE==frame.ExtensionBlocks[j].Function)
            {
               extensionBlock= &frame.ExtensionBlocks[j];
                break;
            }
        }
        if(extensionBlock)
        {
            int delay=10*((extensionBlock->Bytes[2]<<8)|extensionBlock->Bytes[1]);
            gifBean->dealys[i]=delay;
        }


    }
    gifBean->totalFrame=gifFileType->ImageCount;
    gifFileType->UserData=gifBean;
    env->ReleaseStringUTFChars(path_, path);
    return (jlong) gifFileType;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_huozhenpeng_amdemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";


    return env->NewStringUTF(hello.c_str());
}
