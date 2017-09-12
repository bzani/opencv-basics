#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

int main()
{

	//----------------------------------------------------------------------------
	// INICIALIZA WEBCAM
	//----------------------------------------------------------------------------
	VideoCapture cap(0);
	if ( !cap.isOpened() )
    {
         cout << "[ERRO] nao eh possivel abrir a camera" << endl;
         return -1;
    }


	//----------------------------------------------------------------------------
	// INICIO DE CAPTURA
	//----------------------------------------------------------------------------
	Mat imgTmp;
	cap.read(imgTmp); // captura frame primario

	//----------------------------------------------------------------------------
	// LACO DE CAPTURA / TRATAMENTO
	//----------------------------------------------------------------------------
    while (true)
    {

		// matriz de trabalho
		Mat imgOriginal;

		// le novo frame da webcam
		if (!cap.read(imgOriginal))
		{
			 cout << "[ERRO] nao eh possivel ler frame da camera." << endl;
			 break;
		}

		 // converte matriz em imagem Ipl
         IplImage* img = new IplImage(imgOriginal);

		 // mostra imagem original
		 cvNamedWindow("Original");
		 cvShowImage("Original",img);

		 // converte imagem original em escalas de cinza
		 IplImage* imgGrayScale = cvCreateImage(cvGetSize(img), 8, 1);
		 cvCvtColor(img,imgGrayScale,CV_BGR2GRAY);

		 // trata imagem em escalas de cinza para melhorar resultado
		 cvThreshold(imgGrayScale,imgGrayScale,128,255,CV_THRESH_BINARY);

		 // variaveis para definicao de contorno dos objetos
         CvSeq* contours;
		 CvSeq* result;
		 // para armazenar todos os contornos
         CvMemStorage *storage = cvCreateMemStorage(0); 

		 // varre todos os contornos na imagem
		 cvFindContours(imgGrayScale, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

		 // mostra imagem em cinza original
		 cvNamedWindow("Cinza");
		 cvShowImage("Cinza",imgGrayScale);

		 // iterando cada contorno
		 while(contours)
		 {
			 // obtem sequencias de pontos de contorno
			 result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);

			 // caso 3 pontos: triangulo
			 if(result->total==3 )
			  {
				  // iterando por cada ponto
				  CvPoint *pt[3];
				  for(int i=0;i<3;i++){
					  pt[i] = (CvPoint*)cvGetSeqElem(result, i);
				  }

				  // desenhando linhas do objeto
				  cvLine(img, *pt[0], *pt[1], cvScalar(255,0,0),4);
				  cvLine(img, *pt[1], *pt[2], cvScalar(255,0,0),4);
				  cvLine(img, *pt[2], *pt[0], cvScalar(255,0,0),4);

			  }

              // caso tenha 4 vertices: retangulo
			  else if(result->total==4 )
			  {
				  // iterando cada ponto
				  CvPoint *pt[4];
				  for(int i=0;i<4;i++){
					  pt[i] = (CvPoint*)cvGetSeqElem(result, i);
				  }

				  // desenhando linhas do objeto
				  cvLine(img, *pt[0], *pt[1], cvScalar(0,255,0),4);
				  cvLine(img, *pt[1], *pt[2], cvScalar(0,255,0),4);
				  cvLine(img, *pt[2], *pt[3], cvScalar(0,255,0),4);
				  cvLine(img, *pt[3], *pt[0], cvScalar(0,255,0),4);
			  }

			  // se possui 7 vertices: heptagono
			  else if(result->total ==7  )
			  {
				  // iterando por cada ponto
				  CvPoint *pt[7];
				  for(int i=0;i<7;i++){
					  pt[i] = (CvPoint*)cvGetSeqElem(result, i);
				  }

				  // desenhando linhas do heptagono
				  cvLine(img, *pt[0], *pt[1], cvScalar(0,0,255),4);
				  cvLine(img, *pt[1], *pt[2], cvScalar(0,0,255),4);
				  cvLine(img, *pt[2], *pt[3], cvScalar(0,0,255),4);
				  cvLine(img, *pt[3], *pt[4], cvScalar(0,0,255),4);
				  cvLine(img, *pt[4], *pt[5], cvScalar(0,0,255),4);
				  cvLine(img, *pt[5], *pt[6], cvScalar(0,0,255),4);
				  cvLine(img, *pt[6], *pt[0], cvScalar(0,0,255),4);
			  }
			 
             // obtem proximo contorno
			 contours = contours->h_next;
		 }

		 // mostra imagens com as formas marcadas
		 cvNamedWindow("Formas");
		 cvShowImage("Formas",img);

		// aguarda por 'esc' para finalizar programa
 		if (waitKey(30) == 27)
		{
			cout << "[ESC] programa finalizado." << endl;
			break;
		}
    }
    return 0;
}
