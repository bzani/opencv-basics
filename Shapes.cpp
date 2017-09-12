#include <opencv/cv.h>
#include <opencv/highgui.h>
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

		// converte frame de matriz para imagem Ipl
		IplImage* img = new IplImage(imgOriginal);

		// mostra janela com imagem original
		cvNamedWindow("Original");
		cvShowImage("Original",img);

		// suaviza imagem original utilizando funcao Gaussiana para remover ruido
		cvSmooth(img, img, CV_GAUSSIAN,3,3);

		// converte imagem em tons de cinza
		IplImage* imgGrayScale = cvCreateImage(cvGetSize(img), 8, 1);
		cvCvtColor(img,imgGrayScale,CV_BGR2GRAY);

		// abre janela com imagem em escala de cinza
		cvNamedWindow("Escala de Cinza");
		cvShowImage("Escala de Cinza",imgGrayScale);

		// trata imagem em escala de cinza para melhores resultados
		cvThreshold(imgGrayScale,imgGrayScale,100,255,CV_THRESH_BINARY_INV);

		// abre janela da imagem tratada
		cvNamedWindow("Thresholded");
		cvShowImage("Thresholded",imgGrayScale);

		// varre area da imagem em busca de contornos
		CvSeq* contour;
		CvSeq* result;
		CvMemStorage *storage = cvCreateMemStorage(0);
		cvFindContours(imgGrayScale, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

		// itera por cada contorno encontrado
		while(contour)
		{
			// obtem uma sequencia de pontos do contorno
			result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.02, 0);

			// se objeto possui 3 vertices e area do triangulo for maior que 100 pixels
			if(result->total==3 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>100 )
			{
				// iterando cada ponto do triangulo
				CvPoint *pt[3];
				for(int i=0;i<3;i++){
				pt[i] = (CvPoint*)cvGetSeqElem(result, i);
			}

			// desenhando triangulo pela ligacao de seus pontos
			cvLine(img, *pt[0], *pt[1], cvScalar(255,0,0),4);
			cvLine(img, *pt[1], *pt[2], cvScalar(255,0,0),4);
			cvLine(img, *pt[2], *pt[0], cvScalar(255,0,0),4);

			}

			// se objeto possui 4 vertices e area do retangulo for maior que 100 pixels
			if(result->total==4 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>100 )
			{
				// iterando cada ponto do retangulo
				CvPoint *pt[4];
				for(int i=0;i<4;i++){
				pt[i] = (CvPoint*)cvGetSeqElem(result, i);
			}

			// desenhando retangulo pela ligacao de seus pontos
			cvLine(img, *pt[0], *pt[1], cvScalar(255,0,0),4);
			cvLine(img, *pt[1], *pt[2], cvScalar(255,0,0),4);
			cvLine(img, *pt[2], *pt[3], cvScalar(255,0,0),4);
			cvLine(img, *pt[3], *pt[0], cvScalar(255,0,0),4);

			}


			// obtem proximo contorno
			contour = contour->h_next;
		}

		// mostra imagens com contornos marcados
		cvNamedWindow("Contornos");
		cvShowImage("Contornos",img);

		// aguarda por 'esc' para finalizar programa
 		if (waitKey(30) == 27)
		{
			cout << "[ESC] programa finalizado." << endl;
			break;
		}
    }
}
