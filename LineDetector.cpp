#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

/*
 * funcao auxiliar para converter numero em string
 */
string intToString(int number){
	char Result[16];
	sprintf ( Result, "%d", number );
	return Result;
}

/*
 * Desenhar forma em objeto
 */
void drawObj(Mat &img, int x, int y) {

	rectangle(img, Rect(x - 40,y,80,50), Scalar(0,0,255), 3);
	string objeto = "Linha (" + intToString(x) + "," + intToString(y) + ")";
	putText(img,objeto,Point(x, y),1,1,Scalar(255,255,255));

}

/*
 * janela para tratamento manual de imagens
 */
void thresholdImg(Mat imgOriginal, Scalar scLow, Scalar scHigh) {

	// converte frame RGB -> HSV
    Mat imgHSV;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

	// aloca matriz para tratar frame
	Mat imgThresholded;
	inRange(imgHSV, scLow, scHigh, imgThresholded);

	// morphological opening: remove objetos pequenos do ambiente
	erode (imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	// morphological closing: remove pequeno buracos do ambiente
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	erode (imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	// display imagem tratada
	imshow("Thresholded", imgThresholded);

}

/*
 * detecta linha
 */
void detectaLinha(Mat &imgOriginal, Mat imgOper, int &iLastX, int &iLastY) {

	// converte frame capturado RGB -> HSV
    Mat imgHSV;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

	// aloca matriz para tratar frame
	Mat imgThresholded;
	inRange(imgHSV, Scalar(0,0,0), Scalar(179,255,5), imgThresholded);

	// morphological opening
	erode (imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	// morphological closing
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	erode (imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	// mostra e calcula momentos da imagem tratada
	Moments oMoments = oMoments = moments(imgThresholded);
	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	// se area eh menor que 10000 consideramos que nao ha objetos na imagen, apenas ruido
	if (dArea > 10000)
	{
		// calcula posicao central do objeto
		int posX = dM10 / dArea;
		int posY = dM01 / dArea;

		// desenha forma
		if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
		{
			drawObj(imgOper, posX, posY);
		}

		// atualiza indices
		iLastX = posX;
		iLastY = posY;
	}

	// desenha em cima de imagem limpa
	imgOriginal = imgOriginal + imgOper;
}


/*
 * programa principal
 */
int main( int argc, char** argv )
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
	// TRACKBARS
	//----------------------------------------------------------------------------

	// inicializa janela de controle da imagem
	namedWindow("Control", CV_WINDOW_AUTOSIZE);

	int iLowH=0, iHighH=179, iLowS=0, iHighS=255, iLowV=0, iHighV=5;

    // cria os trackbars para controle HSV
	createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);

	int iLastX = -1;
	int iLastY = -1;

	//----------------------------------------------------------------------------
	// INICIO DE CAPTURA
	//----------------------------------------------------------------------------
	Mat imgTmp;
	cap.read(imgTmp); // captura frame primario

    // inicializa arquivo para gravacao de video
    VideoWriter oVideoOrig ("Resultado.avi", CV_FOURCC('D','I','V','3'), 20, imgTmp.size(), true);
    if ( !oVideoOrig.isOpened())
    {
        cout << "[ERRO] falha ao abrir saída de vídeo." << endl;
        return -1;
    }

	//----------------------------------------------------------------------------
	// LACO DE CAPTURA / TRATAMENTO
	//----------------------------------------------------------------------------
    while (true)
    {

    	// reseta config camera para nova captura
    	Mat imgOper = Mat::zeros( imgTmp.size(), CV_8UC3 );

    	// matriz de trabalho
    	Mat imgOriginal;

        // le novo frame da webcam
        if (!cap.read(imgOriginal))
        {
             cout << "[ERRO] nao eh possivel ler frame da camera." << endl;
             break;
        }

    	// abre janela de tratamento auxiliar
        thresholdImg(imgOriginal, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV));

    	// chama funcao para detectar linha
        detectaLinha(imgOriginal, imgOper, iLastX, iLastY);

    	//----------------------------------------------------------------------------
    	// RESULTADO EM DISPLAY / VIDEO
    	//----------------------------------------------------------------------------
    	// display do resultado
        imshow("Resultado",imgOriginal);

    	// grava frame do video
		oVideoOrig.write(imgOriginal); //writer the frame into the file

    	// aguarda por 'esc' para finalizar programa
		if (waitKey(30) == 27)
		{
			cout << "[ESC] programa finalizado." << endl;
			break;
		}
	}

    return 0;
}
