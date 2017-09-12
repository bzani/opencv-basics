#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

enum TIPO {
	OBJ_VERMELHO = 1,
	OBJ_AZUL,
	OBJ_VERDE,
	OBJ_AMARELO,
	OBJ_VIOLETA,
	OBJ_LARANJA,
};

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
void drawObj(Mat &img, int x, int y, int tipo) {

	Scalar sc;
	string nomeCor;
	switch(tipo) {
		case OBJ_VERMELHO:
			sc = Scalar(0,0,255);
			nomeCor = "Vermelho";
			break;
		case OBJ_AZUL:
			sc = Scalar(255,0,0);
			nomeCor = "Azul";
			break;
		case OBJ_VERDE:
			sc = Scalar(0,255,0);
			nomeCor = "Verde";
			break;
		case OBJ_AMARELO:
			sc = Scalar(0,255,255);
			nomeCor = "Amarelo";
			break;
		case OBJ_VIOLETA:
			sc = Scalar(255,255,0);
			nomeCor = "Violeta";
			break;
		case OBJ_LARANJA:
			sc = Scalar(0,150,150);
			nomeCor = "Laranja";
			break;
	}

	// desenho de deteccao
	circle(img, Point(x, y), 50, sc, 2);
	line(img,Point(x,y),Point(x,y-25),sc,2);
	line(img,Point(x,y),Point(x,y+25),sc,2);
	line(img,Point(x,y),Point(x-25,y),sc,2);
	line(img,Point(x,y),Point(x+25,y),sc,2);

	// texto do desenho
	string objeto = nomeCor + " (" + intToString(x) + "," + intToString(y) + ")";
    putText(img,nomeCor,Point(x + 5, y),1,1,Scalar(255,255,255));  // texto apenas com cor
    //putText(img,objeto,Point(x, y),1,1,Scalar(255,255,255));     // texto com posicoes matriciais
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
 * detecta objetos pela cor
 */
void detectaCor(Mat &imgOriginal, Mat imgOper, int &iLastX, int &iLastY, int tipo) {

	// verifica cor a ser tratada
	Scalar scLow, scHigh;
	string nomeCor;
	switch (tipo) {
    	case OBJ_VERMELHO:
    		scLow = Scalar(120,150,75);
    		scHigh = Scalar(179,255,255);
    		nomeCor = "Vermelho";
    		break;

    	case OBJ_AZUL:
    		scLow = Scalar(75,180,80);
    		scHigh = Scalar(130,255,255);
    		nomeCor = "Azul";
			break;

    	case OBJ_VERDE:
    		scLow = Scalar(35,80,60);
    		scHigh = Scalar(75,255,255);
    		nomeCor = "Verde";
			break;

    	case OBJ_AMARELO:
    		scLow = Scalar(20,0,0);
    		scHigh = Scalar(25,255,255);
    		nomeCor = "Amarelo";
			break;

    	case OBJ_VIOLETA:
    		scLow = Scalar(120,30,125);
    		scHigh = Scalar(140,255,255);
    		nomeCor = "Violeta";
			break;

    	case OBJ_LARANJA:
    		scLow = Scalar(0,110,138);
    		scHigh = Scalar(22,255,255);
    		nomeCor = "Laranja";
			break;
	}

	// converte frame capturado RGB -> HSV
    Mat imgHSV;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

	// aloca matriz para tratar frame
	Mat imgThresholded;
	inRange(imgHSV, scLow, scHigh, imgThresholded);

	// morphological opening
	erode (imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	// morphological closing
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	erode (imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	// abre uma janela para cada cor
    //imshow(nomeCor, imgThresholded);

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
			drawObj(imgOper, posX, posY, tipo);
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
	namedWindow("Control", CV_WINDOW_NORMAL);

    int iLowH=0, iHighH=50, iLowS=50, iHighS=255, iLowV=50, iHighV=255;

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

    	// chama funcao para tratar cores
        detectaCor(imgOriginal, imgOper, iLastX, iLastY, OBJ_VERMELHO);
        detectaCor(imgOriginal, imgOper, iLastX, iLastY, OBJ_AZUL);
        detectaCor(imgOriginal, imgOper, iLastX, iLastY, OBJ_VERDE);
        detectaCor(imgOriginal, imgOper, iLastX, iLastY, OBJ_AMARELO);
        detectaCor(imgOriginal, imgOper, iLastX, iLastY, OBJ_VIOLETA);
        //detectaCor(imgOriginal, imgOper, iLastX, iLastY, OBJ_LARANJA);

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
